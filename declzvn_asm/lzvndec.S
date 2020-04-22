/*
 * LZVN decompression function
 *
 * Intel 64 bit (X86_64)
 *
 * int lzvn_decode (unsigned char *dst, unsigned long long dstlen, unsigned char *src, unsigned long long srclen);
 */

#if defined(_WIN32) || defined(_WIN64)
.macro LZVN_ENTRY
.globl lzvn_decode
lzvn_decode:
.endm
#else /* OTHER_OS */
.macro LZVN_ENTRY
.globl _lzvn_decode
_lzvn_decode:
.endm
#endif /* SUYMBOL_DEF */

/* 64 Bit */
.text
.align 8

LZVN_ENTRY
	push %rbp
	mov %rsp,%rbp
#if defined(_WIN32) || defined(_WIN64)
	push   %rdi
	push   %rsi
	mov    %rcx,%rdi
	mov    %rdx,%rsi
	mov    %r8,%rdx
	mov    %r9,%rcx
#endif
	push %rbx
	push %r12
	lea Llzvn_tableref(%rip),%rbx
	xor %rax,%rax
	xor %r12,%r12
	sub $0x8,%rsi
	jb Llzvn_exit2
	lea -0x8(%rdx,%rcx,1),%rcx
	cmp %rcx,%rdx
	ja Llzvn_exit2
	movzbq (%rdx),%r9
	mov (%rdx),%r8
	jmp *(%rbx,%r9,8)

Llzvn_table4:
	add $0x1,%rdx
	cmp %rcx,%rdx
	ja Llzvn_exit2
	movzbq (%rdx),%r9
	mov (%rdx),%r8
	jmp *(%rbx,%r9,8)
	nopl (%rax)

Llzvn_table1:
	shr $0x6,%r9
	lea 0x2(%rdx,%r9,1),%rdx
	cmp %rcx,%rdx
	ja Llzvn_exit2
	mov %r8,%r12
	bswap %r12
	mov %r12,%r10
	shl $0x5,%r12
	shl $0x2,%r10
	shr $0x35,%r12
	shr $0x3d,%r10
	shr $0x10,%r8
	add $0x3,%r10

Llzvn_l10:
	lea (%rax,%r9,1),%r11
	add %r10,%r11
	cmp %rsi,%r11
	jae Llzvn_l8
	mov %r8,(%rdi,%rax,1)
	add %r9,%rax
	mov %rax,%r8
	sub %r12,%r8
	jb Llzvn_exit2
	cmp $0x8,%r12
	jb Llzvn_l4

Llzvn_l5:
	mov (%rdi,%r8,1),%r9
	add $0x8,%r8
	mov %r9,(%rdi,%rax,1)
	add $0x8,%rax
	sub $0x8,%r10
	ja Llzvn_l5
	add %r10,%rax
	movzbq (%rdx),%r9
	mov (%rdx),%r8
	jmp *(%rbx,%r9,8)

Llzvn_l8:
	test %r9,%r9
	je Llzvn_l7
	lea 0x8(%rsi),%r11

Llzvn_l6:
	mov %r8b,(%rdi,%rax,1)
	add $0x1,%rax
	cmp %rax,%r11
	je Llzvn_exit1
	shr $0x8,%r8
	sub $0x1,%r9
	jne Llzvn_l6

Llzvn_l7:
	mov %rax,%r8
	sub %r12,%r8
	jb Llzvn_exit2

Llzvn_l4:
	lea 0x8(%rsi),%r11

Llzvn_l9:
	movzbq (%rdi,%r8,1),%r9
	add $0x1,%r8
	mov %r9b,(%rdi,%rax,1)
	add $0x1,%rax
	cmp %rax,%r11
	je Llzvn_exit1
	sub $0x1,%r10
	jne Llzvn_l9
	movzbq (%rdx),%r9
	mov (%rdx),%r8
	jmp *(%rbx,%r9,8)

Llzvn_table0:
	shr $0x6,%r9
	lea 0x1(%rdx,%r9,1),%rdx
	cmp %rcx,%rdx
	ja Llzvn_exit2
	mov $0x38,%r10
	and %r8,%r10
	shr $0x8,%r8
	shr $0x3,%r10
	add $0x3,%r10
	jmp Llzvn_l10

Llzvn_table3:
	shr $0x6,%r9
	lea 0x3(%rdx,%r9,1),%rdx
	cmp %rcx,%rdx
	ja Llzvn_exit2
	mov $0x38,%r10
	mov $0xffff,%r12
	and %r8,%r10
	shr $0x8,%r8
	shr $0x3,%r10
	and %r8,%r12
	shr $0x10,%r8
	add $0x3,%r10
	jmp Llzvn_l10

Llzvn_table6:
	shr $0x3,%r9
	and $0x3,%r9
	lea 0x3(%rdx,%r9,1),%rdx
	cmp %rcx,%rdx
	ja Llzvn_exit2
	mov %r8,%r10
	and $0x307,%r10
	shr $0xa,%r8
	movzbq %r10b,%r12
	shr $0x8,%r10
	shl $0x2,%r12
	or %r12,%r10
	mov $0x3fff,%r12
	add $0x3,%r10
	and %r8,%r12
	shr $0xe,%r8
	jmp Llzvn_l10

Llzvn_table2:
	add $0x1,%rdx
	cmp %rcx,%rdx
	ja Llzvn_exit2
	mov %r8,%r10
	and $0xf,%r10
	jmp Llzvn_l11

Llzvn_table5:
	add $0x2,%rdx
	cmp %rcx,%rdx
	ja Llzvn_exit2
	mov %r8,%r10
	shr $0x8,%r10
	and $0xff,%r10
	add $0x10,%r10

Llzvn_l11:
	mov %rax,%r8
	sub %r12,%r8
	lea (%rax,%r10,1),%r11
	cmp %rsi,%r11
	jae Llzvn_l4
	cmp $0x8,%r12
	jae Llzvn_l5
	jmp Llzvn_l4

Llzvn_table8:
	and $0xf,%r8
	lea 0x1(%rdx,%r8,1),%rdx
	jmp Llzvn_l0

Llzvn_table7:
	shr $0x8,%r8
	and $0xff,%r8
	add $0x10,%r8
	lea 0x2(%rdx,%r8,1),%rdx

Llzvn_l0:
	cmp %rcx,%rdx
	ja Llzvn_exit2
	lea (%rax,%r8,1),%r11
	neg %r8
	cmp %rsi,%r11
	ja Llzvn_l2
	lea (%rdi,%r11,1),%r11

Llzvn_l1:
	mov (%rdx,%r8,1),%r9
	mov %r9,(%r11,%r8,1)
	add $0x8,%r8
	jae Llzvn_l1
	mov %r11,%rax
	sub %rdi,%rax
	movzbq (%rdx),%r9
	mov (%rdx),%r8
	jmp *(%rbx,%r9,8)

Llzvn_l2:
	lea 0x8(%rsi),%r11

Llzvn_l3:
	movzbq (%rdx,%r8,1),%r9
	mov %r9b,(%rdi,%rax,1)
	add $0x1,%rax
	cmp %rax,%r11
	je Llzvn_exit1
	add $0x1,%r8
	jne Llzvn_l3
	movzbq (%rdx),%r9
	mov (%rdx),%r8
	jmp *(%rbx,%r9,8)

Llzvn_exit2:
	xor %rax,%rax

Llzvn_exit1:
	pop %r12
	pop %rbx
#if defined(_WIN32) || defined(_WIN64)
	pop    %rsi
	pop    %rdi
#endif
	pop %rbp
	ret

.data
.align 8

Llzvn_tableref:
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_exit1
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table4
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table4
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_exit2
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_exit2
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_exit2
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_exit2
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_exit2
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table6
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table1
	.quad	 Llzvn_table0
	.quad	 Llzvn_table3
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_exit2
	.quad	 Llzvn_table7
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table8
	.quad	 Llzvn_table5
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
	.quad	 Llzvn_table2
