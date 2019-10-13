	.text
	.file	"decvec2.c"
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4               # -- Begin function my_ulltoa_n
.LCPI0_0:
	.short	5977                    # 0x1759
	.short	53687                   # 0xd1b7
	.short	0                       # 0x0
	.short	0                       # 0x0
	.short	5977                    # 0x1759
	.short	53687                   # 0xd1b7
	.short	0                       # 0x0
	.short	0                       # 0x0
.LCPI0_1:
	.short	65535                   # 0xffff
	.short	0                       # 0x0
	.short	10000                   # 0x2710
	.short	0                       # 0x0
	.short	65535                   # 0xffff
	.short	0                       # 0x0
	.short	10000                   # 0x2710
	.short	0                       # 0x0
.LCPI0_2:
	.zero	16,48
.LCPI0_3:
	.short	8389                    # 0x20c5
	.short	0                       # 0x0
	.short	10486                   # 0x28f6
	.short	0                       # 0x0
	.short	6554                    # 0x199a
	.short	0                       # 0x0
	.short	32768                   # 0x8000
	.short	0                       # 0x0
.LCPI0_4:
	.short	1                       # 0x1
	.short	0                       # 0x0
	.short	8                       # 0x8
	.short	0                       # 0x0
	.short	128                     # 0x80
	.short	0                       # 0x0
	.short	256                     # 0x100
	.short	0                       # 0x0
.LCPI0_6:
	.short	8389                    # 0x20c5
	.short	41944                   # 0xa3d8
	.short	8389                    # 0x20c5
	.short	41944                   # 0xa3d8
	.short	8389                    # 0x20c5
	.short	41944                   # 0xa3d8
	.short	8389                    # 0x20c5
	.short	41944                   # 0xa3d8
.LCPI0_7:
	.short	0                       # 0x0
	.short	52429                   # 0xcccd
	.short	0                       # 0x0
	.short	52429                   # 0xcccd
	.short	0                       # 0x0
	.short	52429                   # 0xcccd
	.short	0                       # 0x0
	.short	52429                   # 0xcccd
.LCPI0_8:
	.short	65526                   # 0xfff6
	.short	1                       # 0x1
	.short	65526                   # 0xfff6
	.short	1                       # 0x1
	.short	65526                   # 0xfff6
	.short	1                       # 0x1
	.short	65526                   # 0xfff6
	.short	1                       # 0x1
.LCPI0_9:
	.short	0                       # 0x0
	.short	65535                   # 0xffff
	.short	0                       # 0x0
	.short	65535                   # 0xffff
	.short	0                       # 0x0
	.short	65535                   # 0xffff
	.short	0                       # 0x0
	.short	65535                   # 0xffff
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2
.LCPI0_5:
	.long	4294311937              # 0xfff60001
	.text
	.globl	my_ulltoa_n
	.p2align	4, 0x90
	.type	my_ulltoa_n,@function
my_ulltoa_n:                            # @my_ulltoa_n
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	testq	%rdi, %rdi
	je	.LBB0_1
# %bb.2:
	lzcntq	%rdi, %rax
	xorl	$63, %eax
	imull	$1233, %eax, %eax       # imm = 0x4D1
	addl	$1233, %eax             # imm = 0x4D1
	shrl	$12, %eax
	xorl	%r8d, %r8d
	cmpq	%rdi, .L__const.decimalLength.powers10(,%rax,8)
	setbe	%r8b
	addl	%eax, %r8d
	cmpl	$17, %r8d
	jb	.LBB0_4
# %bb.3:
	movabsq	$-6067343680855748867, %rcx # imm = 0xABCC77118461CEFD
	movq	%rdi, %rax
	mulq	%rcx
	movq	%rdx, %rcx
	shrq	$26, %rcx
	movabsq	$4153837486827862103, %rdx # imm = 0x39A5652FB1137857
	movq	%rdi, %rax
	mulq	%rdx
	movq	%rdx, %r9
	shrq	$51, %r9
	imull	$-100000000, %ecx, %eax # imm = 0xFA0A1F00
	addl	%eax, %edi
	imull	$-100000000, %r9d, %edx # imm = 0xFA0A1F00
	addl	%ecx, %edx
	addl	%r9d, %r9d
	jmp	.LBB0_7
.LBB0_1:
	movb	$48, (%rsi)
	movl	$1, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_4:
	.cfi_def_cfa %rbp, 16
	xorl	%r9d, %r9d
	cmpl	$9, %r8d
	jb	.LBB0_5
# %bb.6:
	movabsq	$-6067343680855748867, %rcx # imm = 0xABCC77118461CEFD
	movq	%rdi, %rax
	mulq	%rcx
	shrq	$26, %rdx
	imull	$-100000000, %edx, %eax # imm = 0xFA0A1F00
	addl	%eax, %edi
	jmp	.LBB0_7
.LBB0_5:
	xorl	%edx, %edx
.LBB0_7:
	#APP
	# LLVM-MCA-BEGIN foo
	#NO_APP
	vmovd	%edi, %xmm0
	vmovd	%edx, %xmm1
	vpunpcklqdq	%xmm0, %xmm1, %xmm0 # xmm0 = xmm1[0],xmm0[0]
	#APP
	vmovdqa	.LCPI0_0(%rip), %xmm1
	#NO_APP
	#APP
	vmovdqa	.LCPI0_1(%rip), %xmm2
	#NO_APP
	vpmuludq	%xmm0, %xmm1, %xmm1
	vpsrlq	$13, %xmm1, %xmm1
	vpshufd	$245, %xmm1, %xmm1      # xmm1 = xmm1[1,1,3,3]
	vpmaddwd	%xmm2, %xmm1, %xmm1
	vpslldq	$4, %xmm0, %xmm0        # xmm0 = zero,zero,zero,zero,xmm0[0,1,2,3,4,5,6,7,8,9,10,11]
	vpsubd	%xmm1, %xmm0, %xmm0
	vpslld	$17, %xmm0, %xmm1
	vpor	%xmm0, %xmm1, %xmm0
	#APP
	vmovdqa	.LCPI0_2(%rip), %xmm2
	#NO_APP
	vmovd	%r9d, %xmm1
	vpbroadcastd	%xmm1, %xmm1
	#APP
	vmovdqa	.LCPI0_3(%rip), %xmm3
	#NO_APP
	vpmulhuw	%xmm3, %xmm1, %xmm1
	#APP
	vmovdqa	.LCPI0_4(%rip), %xmm3
	#NO_APP
	vpmaddwd	%xmm3, %xmm1, %xmm1
	vpsrld	$8, %xmm1, %xmm1
	vpslldq	$6, %xmm1, %xmm3        # xmm3 = zero,zero,zero,zero,zero,zero,xmm1[0,1,2,3,4,5,6,7,8,9]
	vpor	%xmm1, %xmm3, %xmm1
	vpbroadcastd	.LCPI0_5(%rip), %xmm3 # xmm3 = [4294311937,4294311937,4294311937,4294311937]
	vpmaddwd	%xmm3, %xmm1, %xmm1
	vpor	%xmm1, %xmm2, %xmm1
	#APP
	vmovdqa	.LCPI0_6(%rip), %xmm4
	#NO_APP
	vpmulhuw	%xmm4, %xmm0, %xmm4
	vpsrlw	$7, %xmm4, %xmm4
	#APP
	vmovdqa	.LCPI0_7(%rip), %xmm5
	#NO_APP
	vpmulhuw	%xmm5, %xmm0, %xmm5
	vpsrlw	$4, %xmm5, %xmm5
	vpblendw	$170, %xmm5, %xmm0, %xmm0 # xmm0 = xmm0[0],xmm5[1],xmm0[2],xmm5[3],xmm0[4],xmm5[5],xmm0[6],xmm5[7]
	#APP
	vmovdqa	.LCPI0_8(%rip), %xmm5
	#NO_APP
	vpmaddwd	%xmm5, %xmm4, %xmm5
	vpslld	$8, %xmm5, %xmm5
	vpmaddwd	%xmm3, %xmm0, %xmm6
	vpslld	$24, %xmm6, %xmm6
	#APP
	vmovdqa	.LCPI0_9(%rip), %xmm7
	#NO_APP
	vpand	%xmm0, %xmm7, %xmm0
	vpmullw	%xmm3, %xmm4, %xmm3
	vpaddw	%xmm3, %xmm0, %xmm0
	vpor	%xmm2, %xmm5, %xmm2
	vpor	%xmm2, %xmm6, %xmm2
	vpextrw	$2, %xmm1, %edx
	vpextrw	$4, %xmm1, %ecx
	vpor	%xmm0, %xmm2, %xmm0
	vpextrw	$6, %xmm1, %eax
	#APP
	# LLVM-MCA-END
	#NO_APP
	leal	-1(%r8), %edi
	cmpl	$18, %edi
	ja	.LBB0_9
# %bb.8:
	jmpq	*.LJTI0_0(,%rdi,8)
.LBB0_28:
	vpalignr	$15, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14]
	vmovdqu	%xmm0, (%rsi)
	movl	$1, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_9:
	.cfi_def_cfa %rbp, 16
	vmovd	%xmm1, %edi
	movb	%dil, (%rsi)
	movb	%dl, 1(%rsi)
	movb	%cl, 2(%rsi)
	movb	%al, 3(%rsi)
	vmovdqu	%xmm0, 4(%rsi)
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_27:
	.cfi_def_cfa %rbp, 16
	vpalignr	$14, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13]
	vmovdqu	%xmm0, (%rsi)
	movl	$2, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_26:
	.cfi_def_cfa %rbp, 16
	vpalignr	$13, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12]
	vmovdqu	%xmm0, (%rsi)
	movl	$3, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_25:
	.cfi_def_cfa %rbp, 16
	vpshufd	$147, %xmm0, %xmm0      # xmm0 = xmm0[3,0,1,2]
	vmovdqu	%xmm0, (%rsi)
	movl	$4, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_24:
	.cfi_def_cfa %rbp, 16
	vpalignr	$11, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10]
	vmovdqu	%xmm0, (%rsi)
	movl	$5, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_23:
	.cfi_def_cfa %rbp, 16
	vpalignr	$10, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9]
	vmovdqu	%xmm0, (%rsi)
	movl	$6, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_22:
	.cfi_def_cfa %rbp, 16
	vpalignr	$9, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8]
	vmovdqu	%xmm0, (%rsi)
	movl	$7, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_21:
	.cfi_def_cfa %rbp, 16
	vpshufd	$78, %xmm0, %xmm0       # xmm0 = xmm0[2,3,0,1]
	vmovdqu	%xmm0, (%rsi)
	movl	$8, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_20:
	.cfi_def_cfa %rbp, 16
	vpalignr	$7, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6]
	vmovdqu	%xmm0, (%rsi)
	movl	$9, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_19:
	.cfi_def_cfa %rbp, 16
	vpalignr	$6, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5]
	vmovdqu	%xmm0, (%rsi)
	movl	$10, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_18:
	.cfi_def_cfa %rbp, 16
	vpalignr	$5, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4]
	vmovdqu	%xmm0, (%rsi)
	movl	$11, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_17:
	.cfi_def_cfa %rbp, 16
	vpshufd	$57, %xmm0, %xmm0       # xmm0 = xmm0[1,2,3,0]
	vmovdqu	%xmm0, (%rsi)
	movl	$12, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_16:
	.cfi_def_cfa %rbp, 16
	vpalignr	$3, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2]
	vmovdqu	%xmm0, (%rsi)
	movl	$13, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_15:
	.cfi_def_cfa %rbp, 16
	vpalignr	$2, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1]
	vmovdqu	%xmm0, (%rsi)
	movl	$14, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_14:
	.cfi_def_cfa %rbp, 16
	vpalignr	$1, %xmm0, %xmm0, %xmm0 # xmm0 = xmm0[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0]
	vmovdqu	%xmm0, (%rsi)
	movl	$15, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_13:
	.cfi_def_cfa %rbp, 16
	vmovdqu	%xmm0, (%rsi)
	movl	$16, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_12:
	.cfi_def_cfa %rbp, 16
	movb	%al, (%rsi)
	vmovdqu	%xmm0, 1(%rsi)
	movl	$17, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_11:
	.cfi_def_cfa %rbp, 16
	movb	%cl, (%rsi)
	movb	%al, 1(%rsi)
	vmovdqu	%xmm0, 2(%rsi)
	movl	$18, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_10:
	.cfi_def_cfa %rbp, 16
	movb	%dl, (%rsi)
	movb	%cl, 1(%rsi)
	movb	%al, 2(%rsi)
	vmovdqu	%xmm0, 3(%rsi)
	movl	$19, %r8d
	movl	%r8d, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	my_ulltoa_n, .Lfunc_end0-my_ulltoa_n
	.cfi_endproc
	.section	.rodata,"a",@progbits
	.p2align	3
.LJTI0_0:
	.quad	.LBB0_28
	.quad	.LBB0_27
	.quad	.LBB0_26
	.quad	.LBB0_25
	.quad	.LBB0_24
	.quad	.LBB0_23
	.quad	.LBB0_22
	.quad	.LBB0_21
	.quad	.LBB0_20
	.quad	.LBB0_19
	.quad	.LBB0_18
	.quad	.LBB0_17
	.quad	.LBB0_16
	.quad	.LBB0_15
	.quad	.LBB0_14
	.quad	.LBB0_13
	.quad	.LBB0_12
	.quad	.LBB0_11
	.quad	.LBB0_10
                                        # -- End function
	.text
	.globl	my_lltoa                # -- Begin function my_lltoa
	.p2align	4, 0x90
	.type	my_lltoa,@function
my_lltoa:                               # @my_lltoa
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	pushq	%rax
	.cfi_offset %rbx, -24
	movq	%rsi, %rbx
	testq	%rdi, %rdi
	jns	.LBB1_2
# %bb.1:
	movb	$45, (%rbx)
	addq	$1, %rbx
	negq	%rdi
.LBB1_2:
	movq	%rbx, %rsi
	callq	my_ulltoa_n
	cltq
	movb	$0, (%rbx,%rax)
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end1:
	.size	my_lltoa, .Lfunc_end1-my_lltoa
	.cfi_endproc
                                        # -- End function
	.type	mul10constb,@object     # @mul10constb
	.section	.rodata,"a",@progbits
	.globl	mul10constb
	.p2align	4
mul10constb:
	.short	1                       # 0x1
	.short	65526                   # 0xfff6
	.short	1                       # 0x1
	.short	65526                   # 0xfff6
	.short	1                       # 0x1
	.short	65526                   # 0xfff6
	.short	1                       # 0x1
	.short	65526                   # 0xfff6
	.size	mul10constb, 16

	.type	.L__const.decimalLength.powers10,@object # @__const.decimalLength.powers10
	.p2align	4
.L__const.decimalLength.powers10:
	.quad	1                       # 0x1
	.quad	10                      # 0xa
	.quad	100                     # 0x64
	.quad	1000                    # 0x3e8
	.quad	10000                   # 0x2710
	.quad	100000                  # 0x186a0
	.quad	1000000                 # 0xf4240
	.quad	10000000                # 0x989680
	.quad	100000000               # 0x5f5e100
	.quad	1000000000              # 0x3b9aca00
	.quad	10000000000             # 0x2540be400
	.quad	100000000000            # 0x174876e800
	.quad	1000000000000           # 0xe8d4a51000
	.quad	10000000000000          # 0x9184e72a000
	.quad	100000000000000         # 0x5af3107a4000
	.quad	1000000000000000        # 0x38d7ea4c68000
	.quad	10000000000000000       # 0x2386f26fc10000
	.quad	100000000000000000      # 0x16345785d8a0000
	.quad	1000000000000000000     # 0xde0b6b3a7640000
	.size	.L__const.decimalLength.powers10, 152


	.ident	"clang version 8.0.1 (tags/RELEASE_801/final)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
