
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



/*
 * A table of all two-digit numbers. This is used to speed up decimal digit
 * generation by copying pairs of digits into the final output.
 */
static const char DIGIT_TABLE[200] = {
	'0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
	'1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
	'2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
	'3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
	'4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
	'5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
	'6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
	'7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
	'8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
	'9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'
};

static inline int
pg_leftmost_one_pos64(uint64_t word)
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
		1U,
		10U,
		100U,
		1000U,
		10000U,
		100000U,
		1000000U,
		10000000U,
		100000000U,
		1000000000U,
		10000000000U,
		100000000000U,
		1000000000000U,
		10000000000000U,
		100000000000000U,
		1000000000000000U,
		10000000000000000U,
		100000000000000000U,
		1000000000000000000U };
	const int t = (pg_leftmost_one_pos64(value) + 1)*1233/4096;
	return t + (value >= powers10[t]);
}

static inline uint64_t
div1e8(uint64_t v)
{
	return v / 100000000;
}

int
my_ulltoa_n_reference(uint64_t value, char *a)
{
	uint32_t	olength;
	uint32_t	i = 0;
	uint32_t	value2;

	/*
	 * 0 is too much of a special case below, so fastpath out.
	 */
	if (value == 0)
	{
		*a = '0';
		return 1;
	}

	olength = decimalLength(value);

	/* Compute the result string. */
	while (value >= 100000000)
	{
		const uint64_t q = div1e8(value);
		const uint32_t value2 = (uint32_t) (value - 100000000 * q);

		const uint32_t c = value2 % 10000;
		const uint32_t d = value2 / 10000;
		const uint32_t c0 = (c % 100) << 1;
		const uint32_t c1 = (c / 100) << 1;
		const uint32_t d0 = (d % 100) << 1;
		const uint32_t d1 = (d / 100) << 1;

		value = q;

		memcpy(a + olength - i - 2, DIGIT_TABLE + c0, 2);
		memcpy(a + olength - i - 4, DIGIT_TABLE + c1, 2);
		memcpy(a + olength - i - 6, DIGIT_TABLE + d0, 2);
		memcpy(a + olength - i - 8, DIGIT_TABLE + d1, 2);
		i += 8;
	}

	value2 = (uint32_t) value;

	if (value2 >= 10000)
	{
		const	uint32_t c = value2 - 10000 * (value2 / 10000);
		const	uint32_t c0 = (c % 100) << 1;
		const	uint32_t c1 = (c / 100) << 1;

		value2 /= 10000;

		memcpy(a + olength - i - 2, DIGIT_TABLE + c0, 2);
		memcpy(a + olength - i - 4, DIGIT_TABLE + c1, 2);
		i += 4;
	}
	if (value2 >= 100)
	{
		const uint32_t c = (value2 % 100) << 1;

		value2 /= 100;
		memcpy(a + olength - i - 2, DIGIT_TABLE + c, 2);
		i += 2;
	}
	if (value2 >= 10)
	{
		const uint32_t c = value2 << 1;

		memcpy(a + olength - i - 2, DIGIT_TABLE + c, 2);
		i += 2;
	}
	else
	{
		*a = (char) ('0' + value2);
	}

	return olength;
}

/*
 * Convert signed 64-bit to null-terminated string.
 *
 * Caller must ensure buffer is long enough.
 */
void
my_lltoa_reference(int64_t value, char *a)
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
    len = my_ulltoa_n_reference(uvalue, a);
    a[len] = 0;
}


char outbuf[64];
char numbuf[256][64];

__attribute__((noinline))
void outfunc(const char *foo)
{
	static volatile char c;
	c = foo[0];
}

extern void my_lltoa(int64_t value, char *a);

int main(void)
{
	int64_t n = 0;
	for (int j = 0; j < 10000000; ++j)
	{
		for (int i = 0; i < 256; ++i)
		{
			my_lltoa(n, numbuf[i]);
			strcpy(outbuf, numbuf[i]);
#if 0
			my_lltoa_reference(n, numbuf[i]);
			if (strcmp(numbuf[i], outbuf) != 0)
				abort();
#endif
			outfunc(outbuf);
			n = (int64_t)((uint64_t) n + (uint64_t) 0x000001010010011d);
		}
	}
	return 0;
}
