/*
 * vectorized (requires SSSE3) implementation of binary to decimal conversion.
 * This was mainly an intellectual exercise, any performance gain over methods
 * that do 2-digit conversions with lookup table are not worth the effort.
 *
 * Currently works with clang but can be adapted to gcc.
 */

#include <stdint.h>
#include <stdbool.h>

static inline int
leftmost_one_pos64(uint64_t word)
{
    return 63 - __builtin_clzll(word);
}

/*
 * Note: value must not be 0.
 */
static int
decimalLength(uint64_t value)
{
    const uint64_t powers10[19] = {
        1U,                 10U,                100U,
        1000U,              10000U,             100000U,
        1000000U,           10000000U,          100000000U,
        1000000000U,        10000000000U,       100000000000U,
        1000000000000U,     10000000000000U,    100000000000000U,
        1000000000000000U,  10000000000000000U, 100000000000000000U,
        1000000000000000000U };
    const int t = (leftmost_one_pos64(value) + 1)*1233/4096;
    return t + (value >= powers10[t]);
}

/*
 * 32-bit platforms may need to optimize this.
 */
static inline uint64_t
div1e8(uint64_t v)
{
    return v / 100000000;
}

#define VEC128BITS __attribute__((vector_size (16)))
#define UNALIGNED __attribute__((aligned (1)))

typedef uint8_t     v16ub   VEC128BITS;
typedef uint8_t     v16ubu  VEC128BITS UNALIGNED;
typedef int16_t     v8sw    VEC128BITS;
typedef uint16_t    v8uw    VEC128BITS;
typedef uint32_t    v4ul    VEC128BITS;
typedef uint64_t    v2uq    VEC128BITS;
typedef int16_t     v8w     VEC128BITS;
typedef int32_t     v4l     VEC128BITS;
typedef long long   v2q     VEC128BITS;

#define PMULUDQ(a_,b_) __builtin_ia32_pmuludq128(a_,b_)
#define PMADDWD(a_,b_) __builtin_ia32_pmaddwd128(a_,b_)
#define PMULHUW(a_,b_) __builtin_ia32_pmulhuw128(a_,b_)
#define PCMPEQW(a_,b_) __builtin_ia32_pcmpeqw128(a_,b_)
#define PUNPCKWD(a_,b_) __builtin_ia32_punpckwd128(a_,b_)

/* These need different definitions on GCC */
#ifdef __clang__
#define SHUFFLE(a_, ...)        __builtin_shufflevector(a_, a_, __VA_ARGS__)
#define SHUFFLE2(a_, b_, ...)   __builtin_shufflevector(a_, b_, __VA_ARGS__)
#define PSLLDQ(a_, c_)          __builtin_ia32_pslldqi128_byteshift(a_, c_)
#define PALIGNR(a_, b_, c_)     __builtin_ia32_palignr128(a_, b_, c_)
#endif

/*
 make_4x4digits

 Input: two uint32_t each representing 8 decimal digits.
 Call these D15_8 and D7_0.
 The task: convert them to groups of 4 digits in uint16_t; each result
 group has both the raw digit value and 2x that.

  127                                                                   0
 +-----------------+-----------------+-----------------+-----------------+
 |                 |         D7_0    |                 |        D15_8    |
 +-----------------+-----------------+-----------------+-----------------+
 |        -        | ce(2^45/10000)  |        -        | ce(2^45/10000)  |
 +-----------------+-----------------+-----------------+-----------------+
 PMULUDQ
 PSRLQ #13
 +-----------------+-----------------+-----------------+-----------------+
 |      D7_0/10000 |       xx        |  D15_8/10000    |       xx        |
 |       | = D7_4  |                 |       | =D15_12 |                 |
 +-----------------+-----------------+-----------------+-----------------+
 PSHUFD
 +-----------------+-----------------+-----------------+-----------------+
 |    0   |   D7_4 |     0  |   D7_4 |    0  |  D15_12 |    0   | D15_12 |
 +-----------------+-----------------+-----------------+-----------------+
 |    0   |  10000 |     0  |     -1 |    0  |   10000 |    0   |     -1 |
 +-----------------+-----------------+-----------------+-----------------+
 PMADDWD
 +-----------------+-----------------+-----------------+-----------------+
 |  10000 * D7_4   |           -D7_4 |   10000*D15_12  |         -D15_12 |
 +-----------------+-----------------+-----------------+-----------------+
 shuffle original input:
 +-----------------+-----------------+-----------------+-----------------+
 |         D7_0    |        0        |         D15_8   |        0        |
 +-----------------+-----------------+-----------------+-----------------+
 |  10000 * D7_4   |           -D7_4 |   10000*D15_12  |         -D15_12 |
 +-----------------+-----------------+-----------------+-----------------+
 PSUBD
 +-----------------+-----------------+-----------------+-----------------+
 |      0 |  D3_0  |      0 |  D7_4  |     0  | D11_8  |     0  | D15_12 |
 +-----------------+-----------------+-----------------+-----------------+
 PSLLD #17
 POR
 +-----------------+-----------------+-----------------+-----------------+
 | 2xD3_0 |   D3_0 | 2xD7_4 |   D7_4 | 2xD11_8|  D11_8 |2xD15_12| D15_12 |
 +-----------------+-----------------+-----------------+-----------------+
*/

static inline v4ul c_div1e4const(void)
{
	uint64_t v = 3518437209;
	v4ul v2;
	__asm__("vmovq %1,%0" : "=x" (v2) : "r" (v));
	return (v4ul) SHUFFLE(v2, 0, 1, 0, 1);
}

static inline v8uw c_mul1e4const(void)
{
	uint64_t v = ((uint64_t)10000U << 32) | ((uint16_t) -1);
	v4ul v2;
	__asm__("vmovq %1,%0" : "=x" (v2) : "r" (v));
	return (v8uw) SHUFFLE(v2, 0, 1, 0, 1);
}

static inline v8uw
make_4x4digits(uint64_t hi8digits, uint64_t lo8digits,
			   const v4ul div1e4const,
			   const v8uw mul1e4const)
{
    v4ul eightdigits;

    eightdigits[0] = hi8digits;
    eightdigits[1] = 0;
    eightdigits[2] = lo8digits;
    eightdigits[3] = 0;

    const v4ul qx           = (v4ul) ((v2uq) PMULUDQ(eightdigits, div1e4const) >> 13);
    const v4ul q0           = SHUFFLE(qx, 1, 1, 3, 3);

    const v4ul r0           = ((v4ul) ((v2uq)eightdigits << 32)
							   - (v4ul) PMADDWD((v8uw) q0, mul1e4const));
    v8uw r1 = (v8uw)(r0 | (r0 << 17));

    return (v8uw) r1;
}


/*
 cvt_4digits

 Input: uint16_t D3_0 representing a 4-digit decimal number.
 The task: convert to 4 individual byte values 0-9 (binary).

 The trick with starting with 2x the input is so that the input value can
 be preserved through a MULHUW operation. This works because we know that
 9999 * 2 won't overflow an int16_t.

  127                                                                   0
 +-----------------+-----------------+-----------------+-----------------+
 |     0  | D3_0*2 |     0  | D3_0*2 |     0  | D3_0*2 |     0  | D3_0*2 |
 +-----------------+-----------------+-----------------+-----------------+
 |     0  | 0x8000 |     0  |2^16/10 |     0  |2^20/100|     0  |2^23/1e3|
 +-----------------+-----------------+-----------------+-----------------+
 PMULHUW
 +-----------------+-----------------+-----------------+-----------------+
 |     0  |  D3_0  |     0  | D3_1*2 |     0  |D3_2<<5 |     0  |D3_1<<8 |
 +-----------------+-----------------+-----------------+-----------------+
 |     0  |   256  |     0  |    128 |     0  |      8 |     0  |      1 |
 +-----------------+-----------------+-----------------+-----------------+
 PMADDWD
 PSHRLD #8
 +-----------------+-----------------+-----------------+-----------------+
 |     0  |  D3_0  |     0  |  D3_1  |     0  |   D3_2 |     0  |    D3  |
 +-----------------+-----------------+-----------------+-----------------+
 PSLLDQ #6
 +-----------------+-----------------+-----------------+-----------------+
 |  D3_1  |     0  |  D3_2  |     0  |    D3  |     0  |     0  |     0  |
 +-----------------+-----------------+-----------------+-----------------+
 POR
 +-----------------+-----------------+-----------------+-----------------+
 |  D3_1  |  D3_0  |  D3_2  |   D3_1 |  D3    |  D3_2  |     0  |    D3  |
 +-----------------+-----------------+-----------------+-----------------+
 |   -10  |     1  |    -10 |      1 |    -10 |     1  |    -10 |     1  |
 +-----------------+-----------------+-----------------+-----------------+
 PMADDWD
 +-----------------+-----------------+-----------------+-----------------+
 |             D0  |             D1  |             D2  |             D3  |
 +-----------------+-----------------+-----------------+-----------------+
*/

//const v8sw mul10constb  = { 1, -10, 1, -10, 1, -10, 1, -10 };

#if 0
static inline v8sw c_mul10constb(void)
{
	uint32_t v = (uint32_t)(((uint16_t) -10) << 16) | (uint32_t)1;
	v4ul v2;
	__asm__("vmovd %1,%0" : "=x" (v2) : "r" (v));
	return (v8sw) SHUFFLE(v2, 0, 0, 0, 0);
}

static inline v8sw c_mul10const(void)
{
	uint32_t v = (uint32_t)(((uint16_t) 1) << 16) | (uint16_t)-10;
	v4ul v2;
	__asm__("vmovd %1,%0" : "=x" (v2) : "r" (v));
	return (v8sw) SHUFFLE(v2, 0, 0, 0, 0);
}
#endif

// FFF6 0001 FFF6 0001 FFF6 0001 FFF6 0001
// 0001 FFF6 0001 FFF6 0001 FFF6 0001 FFF6
// 0000 8000 0000 199A 0000 28F6 0000 20C5
// A3D8 20C5 A3D8 20C5 A3D8 20C5 A3D8 20C5
// CCCD 0000 CCCD 0000 CCCD 0000 CCCD 0000
// FFFF 0000 FFFF 0000 FFFF 0000 FFFF 0000
//
//
// FFF6 : 1111 1111 1111 0110
// 0001 : 0000 0000 0000 0001
// 20C5 : 0010 0000 1100 0101
// 28F6 : 0010 1000 1111 0110
// 199A : 0001 1001 1001 1010
// A3D8 : 1010 0011 1101 1000
// CCCD : 1100 1100 1100 1101
// 3030 : 0011 0000 0011 0000

//  0000
//  FFFF
//
//
//



static inline v8uw
cvt_4digits(uint16_t digits, const v8sw mul10constb, const v8uw tmulconst, const v8uw tadj)
{
    const v4ul topq0        = { digits*2, digits*2, digits*2, digits*2 };
    const v8uw tq0          = PMULHUW((v8uw) topq0, tmulconst);
    const v8uw tq1          = (v8uw) ((v4ul) PMADDWD(tq0, tadj) >> 8);
    const v8uw tq2          = tq1 | (v8uw) PSLLDQ((v2uq) tq1, 6);
    const v8uw res4         = (v8uw) PMADDWD(tq2, mul10constb);
    return res4;
}

/*
 cvt_16digits

 Beginning with a vector of 4 4-digit numbers, repeated in pairs of 16-bit
 lanes with the high lane doubled, (reversed order, high digits are in the low
 lanes), convert to 16 ASCII digits (most-significant in the low-order byte).

  127                                                                   0
 +-----------------+-----------------+-----------------+-----------------+
 | 2xD3_0 |   D3_0 | 2xD7_4 |   D7_4 | 2xD11_8|  D11_8 |2xD15_12| D15_12 |
 +-----------------+-----------------+-----------------+-----------------+
 | 41944  |  8389  | 41944  |  8389  | 41944  |  8389  | 41944  |  8389  |
 +-----------------+-----------------+-----------------+-----------------+
 PMULHUW  (note that 8389 = ceil(2^23/1000), 41944 = ceil(2^22/100))
 PSRLW #7
 +-----------------+-----------------+-----------------+-----------------+
 |D3_0/1e2|D3_0/1e3|D7_4/1e2|D7_4/1e3|D11_8/  |D11_8/  |D15_12/ |D15_12/ |
 |=D3_2   |=D3     |=D7_6   |=D7     |=D11_10 |=D11    |=D15_14 |=D15    |  Q1
 +-----------------+-----------------+-----------------+-----------------+
 |     1  |   -10  |     1  |   -10  |     1  |   -10  |     1  |   -10  |
 +-----------------+-----------------+-----------------+-----------------+
 PMADDWD
 +-----------------+-----------------+-----------------+-----------------+
 |  D3_2 - 10*D3   |  D7_6 - 10*D7   | D11_10 - 10*D11 | D15_14 - 10*D15 |
 |          = D2   |          = D6   |         = D10   |          = D14  |
 +-----------------+-----------------+-----------------+-----------------+
 PSLLD #8
 +-----------------+-----------------+-----------------+-----------------+
 |  0 | 0 | D2 | 0 |  0 | 0 | D6 | 0 |  0 | 0 |D10 | 0 |  0 | 0 |D14 | 0 |  P1
 +-----------------+-----------------+-----------------+-----------------+

 +-----------------+-----------------+-----------------+-----------------+
 | 2xD3_0 |  D3_0  | 2xD7_4 |  D7_4  | 2xD11_8|  D11_8 |2xD15_12| D15_12 |
 +-----------------+-----------------+-----------------+-----------------+
 | 52429  |     0  | 52429  |     0  | 52429  |     0  | 52429  |     0  |
 +-----------------+-----------------+-----------------+-----------------+
 PMULHUW   (note 52429 = ceil(2^19/10))
 PSHRLW #4
 +-----------------+-----------------+-----------------+-----------------+
 | D3_0/10|     0  | D7_4/10|     0  |D11_8/10|      0 |D15_12/ |      0 |
 |=D3_1   |        |=D7_5   |        |=D11_9  |        |=D15_13 |        |
 +-----------------+-----------------+-----------------+-----------------+
 merge alternate lanes with FOURDIGITS:
 +-----------------+-----------------+-----------------+-----------------+
 |  D3_1  |  D3_0  |  D7_5  |  D7_4  |  D11_9 | D11_8  | D15_13 | D15_12 |  Q2
 +-----------------+-----------------+-----------------+-----------------+
 |   -10  |     1  |    -10 |     1  |    -10 |     1  |    -10 |     1  |
 +-----------------+-----------------+-----------------+-----------------+
 PMADDWD
 +-----------------+-----------------+-----------------+-----------------+
 |  D3_0 - 10*D3_1 |  D7_4 - 10*D7_5 | D11_8 - 10*D11_9| D15_12-10*D15_13|
 |         =  D0   |         =  D4   |         =  D8   |         =  D12  |
 +-----------------+-----------------+-----------------+-----------------+
 PSLLD #24
 +-----------------+-----------------+-----------------+-----------------+
 | D0 | 0 |  0 | 0 | D4 | 0 |  0 | 0 | D8 | 0 |  0 | 0 | D12| 0 |  0 | 0 |  P2
 +-----------------+-----------------+-----------------+-----------------+

 +-----------------+-----------------+-----------------+-----------------+
 |  D3_2  |  D3    |  D7_6  |  D7    | D11_10 |  D11   | D15_14 |  D15   |  Q1
 +-----------------+-----------------+-----------------+-----------------+
 |    -10 |    1   |    -10 |    1   |    -10 |    1   |    -10 |    1   |
 +-----------------+-----------------+-----------------+-----------------+
 PMULLW
 +-----------------+-----------------+-----------------+-----------------+
 |  D3_1  |    0   |  D7_5  |    0   | D11_9  |    0   | D15_13 |    0   |  Q2&mask
 +-----------------+-----------------+-----------------+-----------------+
 PADDW
 +-----------------+-----------------+-----------------+-----------------+
 |    D1  |   D3   |   D5   |   D7   |   D9   |   D11  |   D13  |   D15  |  P3
 +-----------------+-----------------+-----------------+-----------------+
 switch to 16 lanes:
 +-----------------+-----------------+-----------------+-----------------+
 |  0 |D1 |  0 |D3 |  0 |D5 | 0 |D7  |  0 |D9 |  0 |D11|  0 |D13|  0 |D15|
 +-----------------+-----------------+-----------------+-----------------+
 |  0 | 0 | D2 | 0 |  0 | 0 | D6 | 0 |  0 | 0 |D10 | 0 |  0 | 0 |D14 | 0 |  P1
 +-----------------+-----------------+-----------------+-----------------+
 | D0 | 0 |  0 | 0 | D4 | 0 |  0 | 0 | D8 | 0 |  0 | 0 | D12| 0 |  0 | 0 |  P2
 +-----------------+-----------------+-----------------+-----------------+
 POR
 POR
 +-----------------+-----------------+-----------------+-----------------+
 | D0 |D1 | D2 |D3 | D4 | D5| D6 |D7 | D8 |D9 |D10 |D11| D12|D13| D14|D15|
 +-----------------+-----------------+-----------------+-----------------+
*/

static inline v16ub
cvt_16digits(const v8uw fourdigits,
			 const v8sw mul10const,
			 const v8sw mul10constb,
			 const v8uw div1e23,
			 const v8uw div1e1,
			 const v8uw mask3)
{
    const v8uw q1           = ((v8uw) PMULHUW((v8uw) fourdigits, div1e23)) >> 7;
    const v8uw q2           = SHUFFLE2( ((v8uw) PMULHUW((v8uw) fourdigits, div1e1) >> 4),
                                        (v8uw)fourdigits,
                                        8, 1, 10, 3, 12, 5, 14, 7 );

    const v4ul p1           = (v4ul) PMADDWD((v8sw) q1, mul10const) << 8;
    const v4ul p2           = (v4ul) PMADDWD((v8sw) q2, mul10constb) << 24;

    const v8sw px1          = (v8sw)(q2 & mask3);
    const v8sw px2          = ((v8sw)q1 * mul10constb);
    const v8uw p3           = (v8uw)(px1 + px2);

    const v16ub res         = (v16ub)(p1) | (v16ub)(p2) | (v16ub)(p3);
    return res;
}

static inline v16ub
c_ccconst(void)
{
	v4ul v;
	__asm__("vmovd %1,%0" : "=x" (v) : "r" (0xCCCCCCCC));
	return (v16ub) SHUFFLE(v, 0, 0, 0, 0);
}

static inline v8uw
c_3030const(v16ub c_cc)
{
	v16ub t1 = c_cc + c_cc;
	v16ub v;
	__asm__("vpaddb %1,%1,%0" : "=x" (v) : "x" (t1));
	return (v8uw) v;
}

static inline v8uw
c_div1e1(v16ub c_cc, v8uw mask3)
{
	v8uw v;
	__asm__("vpsubw %2,%1,%0" : "=x" (v) : "x" ((v8uw)c_cc & mask3), "x" (mask3));
	return v;
}

static inline v8uw
c_mask3(v8uw c_ff)
{
	v8uw v;
	__asm__("vpslld $16,%1,%0" : "=x" (v) : "x" ((v4ul) c_ff));
	return v;
}

static inline v8uw
c_div1e23(void)
{
	uint32_t v1 = (((uint32_t) 41944) << 16) | ((uint16_t) 8389);
	v4ul v;
	__asm__("vmovd %1,%0" : "=x" (v) : "r" (v1));
	return (v8uw) SHUFFLE(v, 0, 0, 0, 0);
}

static inline v8uw
c_4const(uint16_t c0, uint16_t c1, uint16_t c2, uint16_t c3)
{
	uint64_t v1 = (((uint64_t) c3 << 48)
				   | ((uint64_t) c2 << 32)
				   | ((uint32_t) c1 << 16)
				   | (uint16_t) c0);
	v8uw v0 = { 0 };
	v8uw v;
	__asm__("vmovq %1,%0" : "=x" (v) : "r" (v1));
	__asm__("vpunpcklwd %1,%2,%0" : "=x" (v) : "x" (v0), "x" (v));
	return v;
}

static inline v8uw
c_0001const(v8uw c_ff)
{
	v8uw v0 = { 0 };
	v8uw v;
	__asm__("vpsubw %2,%1,%0" : "=x" (v) : "x" (v0), "x" (c_ff));
	return v;
}

#if 0
static inline v8sw
c_mul10const(v8uw w0, v8uw w1)
{
	v8sw v;
	__asm__("vpunpcklwd %1,%2,%0" : "=x" (v) : "x" (w0), "x" (w1));
	return v;
}
#endif

/*
 * my_lltoa_n
 *
 * Given an unsigned 64-bit input, emit decimal digits into a buffer and
 * return the number of valid digits. No null terminator is written. The
 * function may write garbage after the valid digits, but will not use more
 * than 20 bytes.
 */
int
my_ulltoa_n(uint64_t value, char *a)
{
    uint32_t    olength;

    /*
     * 0 is too much of a special case below, so fastpath out.
     */
    if (value == 0)
    {
        *a = '0';
        return 1;
    }

    olength = decimalLength(value);

    uint16_t topdigits = 0;
    uint64_t hi8digits;
    uint64_t lo8digits;

    if (olength > 16)
    {
        const uint64_t q1 = div1e8(value);
        const uint16_t q2 = div1e8(q1);
        lo8digits = (uint32_t) (value - 100000000 * q1);
        hi8digits = (uint32_t) (q1 - 100000000 * q2);
        topdigits = q2;
    }
    else if (olength > 8)
    {
        const uint64_t q1 = div1e8(value);
        lo8digits = (uint32_t) (value - 100000000 * q1);
        hi8digits = q1;
    }
    else
    {
        lo8digits = value;
        hi8digits = 0;
    }
	__asm__ volatile("# LLVM-MCA-BEGIN foo");

	const v4ul div1e4const  = c_div1e4const();
	const v8uw mul1e4const  = c_mul1e4const();

	const v8uw tmulconst = c_4const(8389, 10486, 6554, 0x8000);

	v16ub c_cc = c_ccconst();
	const v8uw c_ff = { -1, -1, -1, -1, -1, -1, -1, -1 };
	const v8uw c_0001 = c_0001const(c_ff);
	const v8uw c_fff7 = c_ff ^ (c_0001 << 3);
	const v8uw div1e23   = c_div1e23();
	const v8uw c_fff6 = c_fff7 ^ c_0001;

	const v8uw c_3030 = c_3030const(c_cc);
	v8uw mask3 = c_mask3(c_ff);
	v8uw div1e1 = c_div1e1(c_cc, mask3);

	const v8sw mul10const   = (c_fff6 & ~mask3) | (c_0001 & mask3);
	const v8sw mul10constb  = mul10const ^ c_fff7;
    const v8uw tadj      = c_4const(1, 8, 128, 256);

	const v8uw fourdigits   = make_4x4digits(hi8digits, lo8digits, div1e4const, mul1e4const);

    const v8uw res4         = cvt_4digits(topdigits, mul10constb, tmulconst, tadj) | c_3030;

    const v16ub res         = cvt_16digits(fourdigits, mul10const, mul10constb, div1e23, div1e1, mask3) | (v16ub) c_3030;

    // At this point:
    //   res = d0..d15
    //   res4 = 0,0,0,d16,0,0,0,d17,0,0,0,d18,0,0,0,d19

    uint8_t d19 = res4[0];
    uint8_t d18 = res4[2];
    uint8_t d17 = res4[4];
    uint8_t d16 = res4[6];

	__asm__ volatile("# LLVM-MCA-END");

    switch (olength)
    {
    default:
    case 20:
        a[0] = d19;
        a[1] = d18;
        a[2] = d17;
        a[3] = d16;
        *(v16ubu *)(a + 4) = res;
        return olength;

    case 19:
        a[0] = d18;
        a[1] = d17;
        a[2] = d16;
        *(v16ubu *)(a + 3) = res;
        return olength;

    case 18:
        a[0] = d17;
        a[1] = d16;
        *(v16ubu *)(a + 2) = res;
        return olength;

    case 17:
        a[0] = d16;
        *(v16ubu *)(a + 1) = res;
        return olength;

    case 16:
        *(v16ubu *)a = res;
        break;

    case 15:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 1); break;
    case 14:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 2); break;
    case 13:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 3); break;
    case 12:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 4); break;
    case 11:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 5); break;
    case 10:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 6); break;
    case  9:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 7); break;
    case  8:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 8); break;
    case  7:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 9); break;
    case  6:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 10); break;
    case  5:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 11); break;
    case  4:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 12); break;
    case  3:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 13); break;
    case  2:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 14); break;
    case  1:    *(v16ubu *)a = (v16ub) PALIGNR((v2q) res, (v2q) res, 15); break;
    }

    return olength;
}


/*
 * Convert signed 64-bit to null-terminated string.
 *
 * Caller must ensure buffer is long enough.
 */
void
my_lltoa(int64_t value, char *a)
{
    int len;
    uint64_t uvalue = value;

    if (value < 0)
    {
        *a++ = '-';
        /*
         * This works even for INT64_MIN:
         *
         * uvalue is required to be congruent to value mod UINT64_MAX+1 (the
         * conversion is defined behavior). (uint64_t)0 - uvalue is required to
         * be whatever value in [0,UINT64_MAX] results from adding or
         * subtracting multiples of UINT64_MAX+1 from the result.
         */
        uvalue = (uint64_t)0 - uvalue;
    }
    len = my_ulltoa_n(uvalue, a);
    a[len] = 0;
}
