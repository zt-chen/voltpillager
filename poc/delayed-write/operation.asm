	.file	"operation.c"
	.text
	.globl	verbose
	.data
	.align 4
	.type	verbose, @object
	.size	verbose, 4
verbose:
	.long	1
	.globl	display_calculation
	.bss
	.align 4
	.type	display_calculation, @object
	.size	display_calculation, 4
display_calculation:
	.zero	4
	.globl	run_compare
	.align 4
	.type	run_compare, @object
	.size	run_compare, 4
run_compare:
	.zero	4
	.comm	fd,4,4
	.globl	faulty_result_found
	.align 4
	.type	faulty_result_found, @object
	.size	faulty_result_found, 4
faulty_result_found:
	.zero	4
	.globl	undervolting_finished
	.align 4
	.type	undervolting_finished, @object
	.size	undervolting_finished, 4
undervolting_finished:
	.zero	4
	.comm	plane0_zero,8,8
	.comm	plane2_zero,8,8
	.globl	pp_y
	.data
	.align 4
	.type	pp_y, @object
	.size	pp_y, 4
pp_y:
	.long	6
	.globl	pp_x
	.align 4
	.type	pp_x, @object
	.size	pp_x, 4
pp_x:
	.long	1
	.comm	printme,100,32
	.comm	mainwin,8,8
	.text
	.globl	wrmsr_value
	.type	wrmsr_value, @function
wrmsr_value:
.LFB3923:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	cvtsi2sdq	-8(%rbp), %xmm0
	movsd	.LC0(%rip), %xmm1
	mulsd	%xmm1, %xmm0
	movsd	.LC1(%rip), %xmm1
	subsd	%xmm1, %xmm0
	cvttsd2siq	%xmm0, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	salq	$21, %rax
	andl	$4294967295, %eax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rdx
	movabsq	$-9223371963840331776, %rax
	orq	%rdx, %rax
	movq	%rax, -8(%rbp)
	movq	-16(%rbp), %rax
	salq	$40, %rax
	movq	%rax, %rdx
	movq	-8(%rbp), %rax
	orq	%rdx, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3923:
	.size	wrmsr_value, .-wrmsr_value
	.globl	voltage_change
	.type	voltage_change, @function
voltage_change:
.LFB3924:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	leaq	-16(%rbp), %rsi
	movl	-4(%rbp), %eax
	movl	$336, %ecx
	movl	$8, %edx
	movl	%eax, %edi
	call	pwrite@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3924:
	.size	voltage_change, .-voltage_change
	.section	.rodata
.LC2:
	.string	"\nVoltage reset"
	.text
	.globl	reset_voltage
	.type	reset_voltage, @function
reset_voltage:
.LFB3925:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	plane0_zero(%rip), %rdx
	movl	fd(%rip), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	voltage_change
	movq	plane2_zero(%rip), %rdx
	movl	fd(%rip), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	voltage_change
	movl	verbose(%rip), %eax
	testl	%eax, %eax
	je	.L6
	leaq	.LC2(%rip), %rdi
	call	puts@PLT
.L6:
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3925:
	.size	reset_voltage, .-reset_voltage
	.section	.rodata
.LC3:
	.string	"pthread set affinity error\r"
.LC4:
	.string	"\r\nUndervolting: %i\r\n"
	.text
	.globl	undervolt_it
	.type	undervolt_it, @function
undervolt_it:
.LFB3926:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$224, %rsp
	movq	%rdi, -216(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	call	pthread_self@PLT
	movq	%rax, -176(%rbp)
	leaq	-144(%rbp), %rax
	movq	%rax, %rsi
	movl	$0, %eax
	movl	$16, %edx
	movq	%rsi, %rdi
	movq	%rdx, %rcx
	rep stosq
	movq	$0, -168(%rbp)
	cmpq	$1023, -168(%rbp)
	ja	.L9
	movq	-168(%rbp), %rax
	shrq	$6, %rax
	leaq	0(,%rax,8), %rdx
	leaq	-144(%rbp), %rcx
	addq	%rcx, %rdx
	movq	(%rdx), %rdx
	movq	-168(%rbp), %rcx
	andl	$63, %ecx
	movl	$1, %esi
	salq	%cl, %rsi
	movq	%rsi, %rcx
	leaq	0(,%rax,8), %rsi
	leaq	-144(%rbp), %rax
	addq	%rsi, %rax
	orq	%rcx, %rdx
	movq	%rdx, (%rax)
.L9:
	leaq	-144(%rbp), %rdx
	movq	-176(%rbp), %rax
	movl	$128, %esi
	movq	%rax, %rdi
	call	pthread_setaffinity_np@PLT
	movl	%eax, -196(%rbp)
	cmpl	$0, -196(%rbp)
	je	.L10
	leaq	.LC3(%rip), %rdi
	call	puts@PLT
.L10:
	movq	-216(%rbp), %rax
	movq	%rax, -160(%rbp)
	movq	-160(%rbp), %rax
	movl	(%rax), %edx
	movq	-160(%rbp), %rax
	movl	%edx, 16(%rax)
	movq	-160(%rbp), %rax
	movq	24(%rax), %rax
	imulq	$1000000, %rax, %rax
	shrq	$3, %rax
	movabsq	$2361183241434822607, %rdx
	mulq	%rdx
	movq	%rdx, %rax
	shrq	$4, %rax
	movq	%rax, -152(%rbp)
	movl	$0, -200(%rbp)
	jmp	.L11
.L17:
	movl	verbose(%rip), %eax
	testl	%eax, %eax
	je	.L12
	movq	-160(%rbp), %rax
	movl	16(%rax), %eax
	movl	%eax, %esi
	leaq	.LC4(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
.L12:
	addl	$1, -200(%rbp)
	movq	-160(%rbp), %rax
	movl	16(%rax), %eax
	cltq
	movl	$0, %esi
	movq	%rax, %rdi
	call	wrmsr_value
	movq	%rax, %rdx
	movl	fd(%rip), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	voltage_change
	movq	-160(%rbp), %rax
	movl	16(%rax), %eax
	cltq
	movl	$2, %esi
	movq	%rax, %rdi
	call	wrmsr_value
	movq	%rax, %rdx
	movl	fd(%rip), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	voltage_change
	call	clock@PLT
	movq	%rax, -192(%rbp)
	jmp	.L13
.L15:
	call	clock@PLT
	movq	%rax, -184(%rbp)
.L13:
	movq	-192(%rbp), %rdx
	movq	-152(%rbp), %rax
	addq	%rax, %rdx
	movq	-184(%rbp), %rax
	cmpq	%rax, %rdx
	jbe	.L14
	movl	faulty_result_found(%rip), %eax
	testl	%eax, %eax
	je	.L15
.L14:
	movq	-160(%rbp), %rax
	movl	16(%rax), %edx
	movq	-160(%rbp), %rax
	movl	12(%rax), %eax
	subl	%eax, %edx
	movq	-160(%rbp), %rax
	movl	%edx, 16(%rax)
	movq	-184(%rbp), %rax
	subq	-192(%rbp), %rax
	cvtsi2sdq	%rax, %xmm0
	movsd	.LC5(%rip), %xmm1
	divsd	%xmm1, %xmm0
	cvtsd2ss	%xmm0, %xmm0
	movq	-160(%rbp), %rax
	movss	%xmm0, 20(%rax)
.L11:
	movl	faulty_result_found(%rip), %eax
	testl	%eax, %eax
	jne	.L16
	movq	-160(%rbp), %rax
	movl	16(%rax), %edx
	movq	-160(%rbp), %rax
	movl	4(%rax), %eax
	cmpl	%eax, %edx
	jg	.L17
	movq	-160(%rbp), %rax
	movl	8(%rax), %eax
	cmpl	%eax, -200(%rbp)
	jl	.L17
.L16:
	movl	$0, %eax
	call	reset_voltage
	movq	-184(%rbp), %rax
	subq	-192(%rbp), %rax
	cvtsi2sdq	%rax, %xmm0
	movsd	.LC5(%rip), %xmm1
	divsd	%xmm1, %xmm0
	cvtsd2ss	%xmm0, %xmm0
	movq	-160(%rbp), %rax
	movss	%xmm0, 20(%rax)
	movl	$1, undervolting_finished(%rip)
	movl	$0, %eax
	movq	-8(%rbp), %rdi
	xorq	%fs:40, %rdi
	je	.L19
	call	__stack_chk_fail@PLT
.L19:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3926:
	.size	undervolt_it, .-undervolt_it
	.section	.rodata
.LC6:
	.string	"Error initialising ncurses.\n"
	.text
	.globl	screen_init
	.type	screen_init, @function
screen_init:
.LFB3927:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	call	initscr@PLT
	movq	%rax, mainwin(%rip)
	movq	mainwin(%rip), %rax
	testq	%rax, %rax
	jne	.L21
	movq	stderr(%rip), %rax
	movq	%rax, %rcx
	movl	$28, %edx
	movl	$1, %esi
	leaq	.LC6(%rip), %rdi
	call	fwrite@PLT
	movl	$1, %edi
	call	exit@PLT
.L21:
	call	start_color@PLT
	movl	$0, %edx
	movl	$2, %esi
	movl	$1, %edi
	call	init_pair@PLT
	movl	$2, %edx
	movl	$0, %esi
	movl	$2, %edi
	call	init_pair@PLT
	movl	$1, %edx
	movl	$0, %esi
	movl	$3, %edi
	call	init_pair@PLT
	movl	$1, %edx
	movl	$7, %esi
	movl	$4, %edi
	call	init_pair@PLT
	movq	stdscr(%rip), %rax
	movl	$0, %edx
	movl	$1, %esi
	movq	%rax, %rdi
	call	wcolor_set@PLT
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3927:
	.size	screen_init, .-screen_init
	.globl	screen_del
	.type	screen_del, @function
screen_del:
.LFB3928:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	mainwin(%rip), %rax
	movq	%rax, %rdi
	call	delwin@PLT
	call	endwin@PLT
	movq	stdscr(%rip), %rax
	movq	%rax, %rdi
	call	wrefresh@PLT
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3928:
	.size	screen_del, .-screen_del
	.section	.rodata
.LC7:
	.string	" ---- core %i ---- "
	.align 8
.LC8:
	.string	"0x%08lx * 0x%08lx = 0x%016lx    "
	.text
	.globl	print_calculation
	.type	print_calculation, @function
print_calculation:
.LFB3929:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	$0, %edi
	call	curs_set@PLT
	movq	stdscr(%rip), %rax
	movl	$0, %edx
	movl	$2, %esi
	movq	%rax, %rdi
	call	wcolor_set@PLT
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC7(%rip), %rsi
	leaq	printme(%rip), %rdi
	movl	$0, %eax
	call	sprintf@PLT
	movl	pp_x(%rip), %eax
	leal	10(%rax), %esi
	movl	-20(%rbp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	leal	2(%rax), %ecx
	movq	stdscr(%rip), %rax
	movl	%esi, %edx
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	wmove@PLT
	cmpl	$-1, %eax
	je	.L25
	movq	stdscr(%rip), %rax
	movl	$-1, %edx
	leaq	printme(%rip), %rsi
	movq	%rax, %rdi
	call	waddnstr@PLT
.L25:
	movq	stdscr(%rip), %rax
	movl	$0, %edx
	movl	$1, %esi
	movq	%rax, %rdi
	call	wcolor_set@PLT
	movq	-8(%rbp), %rax
	imulq	-16(%rbp), %rax
	movq	%rax, %rcx
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rax
	movq	%rcx, %r8
	movq	%rdx, %rcx
	movq	%rax, %rdx
	leaq	.LC8(%rip), %rsi
	leaq	printme(%rip), %rdi
	movl	$0, %eax
	call	sprintf@PLT
	movl	pp_x(%rip), %ecx
	movl	-20(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	leal	(%rax,%rdx), %esi
	movq	stdscr(%rip), %rax
	movl	%ecx, %edx
	movq	%rax, %rdi
	call	wmove@PLT
	cmpl	$-1, %eax
	je	.L27
	movq	stdscr(%rip), %rax
	movl	$-1, %edx
	leaq	printme(%rip), %rsi
	movq	%rax, %rdi
	call	waddnstr@PLT
.L27:
	movq	stdscr(%rip), %rax
	movq	%rax, %rdi
	call	wrefresh@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3929:
	.size	print_calculation, .-print_calculation
	.section	.rodata
.LC9:
	.string	"\r"
.LC10:
	.string	"[%02i] 0x%016lx \r\n"
	.text
	.globl	compare_it
	.type	compare_it, @function
compare_it:
.LFB3930:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$208, %rsp
	movq	%rdi, -200(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	-200(%rbp), %rax
	movq	%rax, -160(%rbp)
	movq	-160(%rbp), %rax
	movq	64(%rax), %rax
	movq	%rax, -152(%rbp)
	movl	$0, -184(%rbp)
	movq	$0, -168(%rbp)
	leaq	-144(%rbp), %rdx
	movl	$0, %eax
	movl	$16, %ecx
	movq	%rdx, %rdi
	rep stosq
	jmp	.L29
.L38:
	leaq	-144(%rbp), %rax
	movl	$128, %edx
	movl	$18, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	$0, -176(%rbp)
.L36:
	movq	$12, -168(%rbp)
	cmpq	$7, -168(%rbp)
	jle	.L31
	movq	$7, -168(%rbp)
	jmp	.L31
.L32:
	movq	-168(%rbp), %rax
	movabsq	$-6076574518398440533, %rcx
	movq	%rcx, -144(%rbp,%rax,8)
	subq	$1, -168(%rbp)
.L31:
	cmpq	$0, -168(%rbp)
	jg	.L32
	movq	-80(%rbp), %rdx
	movabsq	$1302123111085380114, %rax
	cmpq	%rax, %rdx
	jne	.L33
	movq	-144(%rbp), %rdx
	movabsq	$1302123111085380114, %rax
	cmpq	%rax, %rdx
	je	.L34
.L33:
	movl	$1, -184(%rbp)
.L34:
	addq	$1, -176(%rbp)
	movl	-184(%rbp), %eax
	testl	%eax, %eax
	jne	.L35
	movq	-176(%rbp), %rax
	cmpq	-152(%rbp), %rax
	jb	.L36
.L35:
	movl	-184(%rbp), %eax
	cmpl	$1, %eax
	jne	.L29
	movl	$1, faulty_result_found(%rip)
.L29:
	movl	faulty_result_found(%rip), %eax
	testl	%eax, %eax
	jne	.L37
	movl	undervolting_finished(%rip), %eax
	testl	%eax, %eax
	je	.L38
.L37:
	movl	-184(%rbp), %eax
	cmpl	$1, %eax
	jne	.L39
	movl	$0, %edi
	call	sleep@PLT
	movq	-160(%rbp), %rax
	movq	$1, 56(%rax)
	movq	-160(%rbp), %rax
	movq	-176(%rbp), %rdx
	movq	%rdx, 64(%rax)
	movq	-144(%rbp), %rdx
	movq	-160(%rbp), %rax
	movq	%rdx, 16(%rax)
	movq	-80(%rbp), %rdx
	movq	-160(%rbp), %rax
	movq	%rdx, 24(%rax)
	leaq	.LC9(%rip), %rdi
	call	puts@PLT
	movl	$0, -180(%rbp)
	jmp	.L40
.L41:
	movl	-180(%rbp), %eax
	cltq
	movq	-144(%rbp,%rax,8), %rdx
	movl	-180(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC10(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -180(%rbp)
.L40:
	cmpl	$15, -180(%rbp)
	jle	.L41
.L39:
	movl	$0, %eax
	movq	-8(%rbp), %rsi
	xorq	%fs:40, %rsi
	je	.L43
	call	__stack_chk_fail@PLT
.L43:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3930:
	.size	compare_it, .-compare_it
	.globl	multiply_it
	.type	multiply_it, @function
multiply_it:
.LFB3931:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
	movq	%rdi, -88(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	-88(%rbp), %rax
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rax
	movq	64(%rax), %rax
	movq	%rax, -48(%rbp)
	movq	-56(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, -40(%rbp)
	movq	-56(%rbp), %rax
	movq	24(%rax), %rax
	movq	%rax, -32(%rbp)
	movq	-56(%rbp), %rax
	movq	32(%rax), %rax
	movq	%rax, -24(%rbp)
	movq	-56(%rbp), %rax
	movq	40(%rax), %rax
	movq	%rax, -16(%rbp)
	jmp	.L45
.L52:
	movq	-56(%rbp), %rax
	movq	$0, 64(%rax)
	movq	-56(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$77, %al
	jne	.L46
	leaq	-72(%rbp), %rax
	rdrand	%rdx
	movq	%rdx, (%rax)
	movq	-72(%rbp), %rax
	movq	-40(%rbp), %rdx
	movq	%rdx, %rcx
	subq	-24(%rbp), %rcx
	movl	$0, %edx
	divq	%rcx
	movq	-24(%rbp), %rax
	addq	%rax, %rdx
	movq	-56(%rbp), %rax
	movq	%rdx, 16(%rax)
.L46:
	movq	-56(%rbp), %rax
	movzbl	1(%rax), %eax
	cmpb	$77, %al
	jne	.L47
	leaq	-64(%rbp), %rax
	rdrand	%rdx
	movq	%rdx, (%rax)
	movq	-64(%rbp), %rax
	movq	-32(%rbp), %rdx
	movq	%rdx, %rcx
	subq	-16(%rbp), %rcx
	movl	$0, %edx
	divq	%rcx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	leaq	1(%rax), %rdx
	movq	-56(%rbp), %rax
	movq	%rdx, 24(%rax)
.L47:
	movl	display_calculation(%rip), %eax
	testl	%eax, %eax
	je	.L50
	movq	-56(%rbp), %rax
	movl	72(%rax), %edx
	movq	-56(%rbp), %rax
	movq	24(%rax), %rcx
	movq	-56(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	print_calculation
.L50:
	movq	-56(%rbp), %rax
	movq	64(%rax), %rax
	leaq	1(%rax), %rdx
	movq	-56(%rbp), %rax
	movq	%rdx, 64(%rax)
	movq	-56(%rbp), %rax
	movq	16(%rax), %rdx
	movq	-56(%rbp), %rax
	movq	24(%rax), %rax
	imulq	%rax, %rdx
	movq	-56(%rbp), %rax
	movq	%rdx, 48(%rax)
	movq	-56(%rbp), %rax
	movq	16(%rax), %rdx
	movq	-56(%rbp), %rax
	movq	24(%rax), %rax
	imulq	%rax, %rdx
	movq	-56(%rbp), %rax
	movq	%rdx, 56(%rax)
	movq	-56(%rbp), %rax
	movq	48(%rax), %rdx
	movq	-56(%rbp), %rax
	movq	56(%rax), %rax
	cmpq	%rax, %rdx
	jne	.L49
	movq	-56(%rbp), %rax
	movq	64(%rax), %rax
	cmpq	%rax, -48(%rbp)
	jbe	.L49
	movl	faulty_result_found(%rip), %eax
	testl	%eax, %eax
	jne	.L49
	movl	undervolting_finished(%rip), %eax
	testl	%eax, %eax
	je	.L50
.L49:
	movq	-56(%rbp), %rax
	movq	48(%rax), %rdx
	movq	-56(%rbp), %rax
	movq	56(%rax), %rax
	cmpq	%rax, %rdx
	je	.L45
	movl	faulty_result_found(%rip), %eax
	testl	%eax, %eax
	jne	.L45
	movl	$1, faulty_result_found(%rip)
.L45:
	movl	faulty_result_found(%rip), %eax
	testl	%eax, %eax
	jne	.L51
	movl	undervolting_finished(%rip), %eax
	testl	%eax, %eax
	je	.L52
.L51:
	movl	$0, %eax
	movq	-8(%rbp), %rsi
	xorq	%fs:40, %rsi
	je	.L54
	call	__stack_chk_fail@PLT
.L54:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3931:
	.size	multiply_it, .-multiply_it
	.section	.rodata
	.align 8
.LC11:
	.string	"So many options - so little time:\n"
	.align 8
.LC12:
	.string	"\t -m #           \t millisecs between voltage drop"
.LC13:
	.string	"\t -i #           \t iterations"
	.align 8
.LC14:
	.string	"\t -s #           \t start voltage"
	.align 8
.LC15:
	.string	"\t -e #           \t end voltage"
	.align 8
.LC16:
	.string	"\t -X #           \t Stop after x drops"
.LC17:
	.string	"\t -1 0x#         \t operand1"
.LC18:
	.string	"\t -2 0x#         \t operand2"
	.align 8
.LC19:
	.string	"\t -t #           \t number of threads - default=1"
	.align 8
.LC20:
	.string	"\t -v #           \t voltage_steps"
	.align 8
.LC21:
	.string	"\t -z fixed | max \t fixed|max (what is operand 1 - default=fixed)"
	.align 8
.LC22:
	.string	"\t -x fixed | max \t fixed|max (what is operand 2 - default=fixed)"
	.align 8
.LC23:
	.string	"\t -q #           \t operand 1 minimum - default=0"
	.align 8
.LC24:
	.string	"\t -w #           \t operand 2 minimum - default=0"
	.align 8
.LC25:
	.string	"\t -S             \t Silent mode - default=verbose"
	.align 8
.LC26:
	.string	"\t -M             \t Multiply only (no undervolting)"
	.align 8
.LC27:
	.string	"\t -P             \t run compare (not multiplication)"
	.align 8
.LC28:
	.string	"\t -C             \t display Calculation "
	.align 8
.LC29:
	.string	"\t -h             \t display this Help"
	.text
	.globl	usage
	.type	usage, @function
usage:
.LFB3932:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	leaq	.LC11(%rip), %rdi
	call	puts@PLT
	leaq	.LC12(%rip), %rdi
	call	puts@PLT
	leaq	.LC13(%rip), %rdi
	call	puts@PLT
	leaq	.LC14(%rip), %rdi
	call	puts@PLT
	leaq	.LC15(%rip), %rdi
	call	puts@PLT
	leaq	.LC16(%rip), %rdi
	call	puts@PLT
	leaq	.LC17(%rip), %rdi
	call	puts@PLT
	leaq	.LC18(%rip), %rdi
	call	puts@PLT
	leaq	.LC19(%rip), %rdi
	call	puts@PLT
	leaq	.LC20(%rip), %rdi
	call	puts@PLT
	leaq	.LC21(%rip), %rdi
	call	puts@PLT
	leaq	.LC22(%rip), %rdi
	call	puts@PLT
	leaq	.LC23(%rip), %rdi
	call	puts@PLT
	leaq	.LC24(%rip), %rdi
	call	puts@PLT
	leaq	.LC25(%rip), %rdi
	call	puts@PLT
	leaq	.LC26(%rip), %rdi
	call	puts@PLT
	leaq	.LC27(%rip), %rdi
	call	puts@PLT
	leaq	.LC28(%rip), %rdi
	call	puts@PLT
	leaq	.LC29(%rip), %rdi
	call	puts@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3932:
	.size	usage, .-usage
	.section	.rodata
.LC30:
	.string	"/dev/cpu/0/msr"
.LC31:
	.string	"Could not open /dev/cpu/0/msr"
.LC32:
	.string	"fixed"
.LC33:
	.string	"max"
	.align 8
.LC34:
	.string	"Error setting operand1 to be fixed or max value"
	.align 8
.LC35:
	.string	"Error setting operand2 to be fixed or max value"
.LC36:
	.string	"option needs a value"
.LC37:
	.string	"unknown option: %c\n"
	.align 8
.LC38:
	.string	"m:e:i:s:v:t:1:2:q:w:X:z:x:SMChP"
.LC39:
	.string	"extra arguments: %s\n"
	.align 8
.LC40:
	.string	"Error: start voltage must be set"
	.align 8
.LC41:
	.string	"Error: end voltage or end after x drops must be set."
	.align 8
.LC42:
	.string	"Error: operand1 and operand2 must be set for multiplications."
.LC43:
	.string	"\nSummary"
	.align 8
.LC44:
	.string	"-------------------------------------------------"
.LC45:
	.string	"time (ms) interval:      %li\n"
.LC46:
	.string	"Iterations:         \t %li\n"
.LC47:
	.string	"Start Voltage:      \t %li\n"
.LC48:
	.string	"End Voltage:        \t %li\n"
.LC49:
	.string	"Stop after x drops: \t %i\n"
.LC50:
	.string	"Voltage steps:      \t %i\n"
.LC51:
	.string	"Threads:            \t %i\n"
	.align 8
.LC52:
	.string	"Operand1:           \t 0x%016lx\n"
	.align 8
.LC53:
	.string	"Operand2:           \t 0x%016lx\n"
.LC54:
	.string	"maximum"
.LC55:
	.string	"fixed value"
.LC56:
	.string	"Operand1 is:        \t %s\n"
.LC57:
	.string	"Operand2 is:        \t %s\n"
	.align 8
.LC58:
	.string	"Operand1 min is:    \t 0x%016lx\n"
	.align 8
.LC59:
	.string	"Operand2 min is:    \t 0x%016lx\n"
.LC60:
	.string	"Yes"
.LC61:
	.string	"No"
.LC62:
	.string	"Multiply only:\t    \t %s\n"
.LC63:
	.string	"Display calculation:\t %s\n"
.LC64:
	.string	"COMPARE"
.LC65:
	.string	"MULTIPLY"
.LC66:
	.string	"Mode:\t                 %s\n"
	.align 8
.LC67:
	.string	"------   CALCULATION ERROR DETECTED   ------"
	.align 8
.LC68:
	.string	" > Time spent     \t  : %f seconds\n"
.LC69:
	.string	" > Max Iterations \t  : %08li\n"
.LC70:
	.string	" > Milli_secs     \t  : %08li\n"
.LC71:
	.string	" > Faulted Thread \t  : %i/%i\n"
.LC72:
	.string	" > Iterations     \t  : %08li\n"
.LC73:
	.string	" > Operand 1   \t  : %016lx\n"
.LC74:
	.string	" > Operand 2   \t  : %016lx\n"
.LC75:
	.string	" > Correct     \t  : %016lx\n"
.LC76:
	.string	" > Result      \t  : %016lx\n"
.LC77:
	.string	" > array[0]      \t  : %016lx\n"
	.align 8
.LC78:
	.string	" > array[ARRAY_SIZE_TEST]: %016lx\n"
.LC79:
	.string	" > Starting op1  \t  : %016lx\n"
.LC80:
	.string	" > Mode           \t  : %s\n"
	.align 8
.LC81:
	.string	" > xor result     \t  : %016lx\n"
.LC82:
	.string	" > undervoltage   \t  : %i\n"
	.align 8
.LC83:
	.string	"sensors | grep \"Core 0\" |awk '{print $3}'i | tr -d '\\n'"
.LC84:
	.string	"r"
.LC85:
	.string	" > temperature \t  : %s\n"
	.align 8
.LC86:
	.string	"cat /proc/cpuinfo | grep MHz | head -n 1 | cut  -d ':' -f2| cut -d '.' -f1 | tr -d '\\n'"
.LC87:
	.string	" > Frequency   \t  :%sMHz\n"
.LC88:
	.string	" [Done] Press any key."
.LC89:
	.string	"[Done]"
	.text
	.globl	main
	.type	main, @function
main:
.LFB3933:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$256, %rsp
	movl	%edi, -244(%rbp)
	movq	%rsi, -256(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	-256(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -120(%rbp)
	cmpl	$4, -244(%rbp)
	jg	.L57
	movq	-120(%rbp), %rax
	movq	%rax, %rdi
	call	usage
	movl	$-1, %eax
	jmp	.L125
.L57:
	movl	$1, %esi
	leaq	.LC30(%rip), %rdi
	movl	$0, %eax
	call	open@PLT
	movl	%eax, fd(%rip)
	movl	fd(%rip), %eax
	cmpl	$-1, %eax
	jne	.L59
	leaq	.LC31(%rip), %rdi
	call	puts@PLT
	movl	$-1, %eax
	jmp	.L125
.L59:
	movq	$1000, -184(%rbp)
	movq	$0, -176(%rbp)
	movq	$0, -168(%rbp)
	movq	$0, -160(%rbp)
	movq	$0, -152(%rbp)
	movl	$0, -224(%rbp)
	movl	$1, -220(%rbp)
	movb	$70, -226(%rbp)
	movb	$70, -225(%rbp)
	movq	$0, -144(%rbp)
	movq	$0, -136(%rbp)
	movq	$1000, -128(%rbp)
	movl	$1, -216(%rbp)
	movl	$0, -212(%rbp)
	movl	$0, -196(%rbp)
	jmp	.L60
.L88:
	movl	-196(%rbp), %eax
	subl	$49, %eax
	cmpl	$73, %eax
	ja	.L60
	movl	%eax, %eax
	leaq	0(,%rax,4), %rdx
	leaq	.L62(%rip), %rax
	movl	(%rdx,%rax), %eax
	movslq	%eax, %rdx
	leaq	.L62(%rip), %rax
	addq	%rdx, %rax
	jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L62:
	.long	.L61-.L62
	.long	.L63-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L64-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L65-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L66-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L67-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L68-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L69-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L70-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L71-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L72-.L62
	.long	.L73-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L74-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L60-.L62
	.long	.L75-.L62
	.long	.L60-.L62
	.long	.L76-.L62
	.long	.L77-.L62
	.long	.L60-.L62
	.long	.L78-.L62
	.long	.L79-.L62
	.long	.L80-.L62
	.long	.L60-.L62
	.long	.L81-.L62
	.text
.L61:
	movq	optarg(%rip), %rax
	movl	$16, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	strtol@PLT
	movq	%rax, -176(%rbp)
	jmp	.L60
.L63:
	movq	optarg(%rip), %rax
	movl	$16, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	strtol@PLT
	movq	%rax, -168(%rbp)
	jmp	.L60
.L81:
	movq	optarg(%rip), %rax
	leaq	.LC32(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L82
	movb	$70, -226(%rbp)
	jmp	.L60
.L82:
	movq	optarg(%rip), %rax
	leaq	.LC33(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L84
	movb	$77, -226(%rbp)
	jmp	.L60
.L84:
	leaq	.LC34(%rip), %rdi
	call	puts@PLT
	movl	$-1, %eax
	jmp	.L125
.L80:
	movq	optarg(%rip), %rax
	leaq	.LC32(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L85
	movb	$70, -225(%rbp)
	jmp	.L60
.L85:
	movq	optarg(%rip), %rax
	leaq	.LC33(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L87
	movb	$77, -225(%rbp)
	jmp	.L60
.L87:
	leaq	.LC35(%rip), %rdi
	call	puts@PLT
	movl	$-1, %eax
	jmp	.L125
.L73:
	movq	optarg(%rip), %rax
	movl	$10, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	strtol@PLT
	movq	%rax, -184(%rbp)
	jmp	.L60
.L72:
	movq	-120(%rbp), %rax
	movq	%rax, %rdi
	call	usage
	movl	$-1, %eax
	jmp	.L125
.L76:
	movq	optarg(%rip), %rax
	movl	$10, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	strtol@PLT
	movq	%rax, -160(%rbp)
	jmp	.L60
.L71:
	movq	optarg(%rip), %rax
	movl	$10, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	strtol@PLT
	movq	%rax, -152(%rbp)
	jmp	.L60
.L75:
	movq	optarg(%rip), %rax
	movl	$16, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	strtol@PLT
	movq	%rax, -144(%rbp)
	jmp	.L60
.L79:
	movq	optarg(%rip), %rax
	movl	$16, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	strtol@PLT
	movq	%rax, -136(%rbp)
	jmp	.L60
.L78:
	movq	optarg(%rip), %rax
	movl	$10, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	strtol@PLT
	movl	%eax, -220(%rbp)
	jmp	.L60
.L77:
	movq	optarg(%rip), %rax
	movl	$10, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	strtol@PLT
	movl	%eax, -216(%rbp)
	jmp	.L60
.L74:
	movq	optarg(%rip), %rax
	movl	$10, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	strtol@PLT
	movq	%rax, -128(%rbp)
	jmp	.L60
.L70:
	movq	optarg(%rip), %rax
	movl	$10, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	strtol@PLT
	movl	%eax, -224(%rbp)
	jmp	.L60
.L69:
	movl	$0, verbose(%rip)
	jmp	.L60
.L67:
	movl	$1, -212(%rbp)
	jmp	.L60
.L66:
	movl	$1, display_calculation(%rip)
	jmp	.L60
.L68:
	movl	$1, run_compare(%rip)
	jmp	.L60
.L64:
	leaq	.LC36(%rip), %rdi
	call	puts@PLT
	jmp	.L60
.L65:
	movl	optopt(%rip), %eax
	movl	%eax, %esi
	leaq	.LC37(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
.L60:
	movq	-256(%rbp), %rcx
	movl	-244(%rbp), %eax
	leaq	.LC38(%rip), %rdx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	getopt@PLT
	movl	%eax, -196(%rbp)
	cmpl	$-1, -196(%rbp)
	jne	.L88
	jmp	.L127
.L90:
	movl	optind(%rip), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-256(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rsi
	leaq	.LC39(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-120(%rbp), %rax
	movq	%rax, %rdi
	call	usage
	movl	$-1, %eax
	jmp	.L125
.L127:
	movl	optind(%rip), %eax
	cmpl	%eax, -244(%rbp)
	jg	.L90
	cmpl	$0, -212(%rbp)
	jne	.L91
	cmpq	$0, -160(%rbp)
	jne	.L91
	leaq	.LC40(%rip), %rdi
	call	puts@PLT
	movl	$-1, %eax
	jmp	.L125
.L91:
	cmpl	$0, -212(%rbp)
	jne	.L92
	cmpq	$0, -152(%rbp)
	jne	.L92
	cmpl	$0, -224(%rbp)
	jne	.L92
	leaq	.LC41(%rip), %rdi
	call	puts@PLT
	movl	$-1, %eax
	jmp	.L125
.L92:
	cmpl	$0, -216(%rbp)
	jle	.L93
	cmpq	$0, -176(%rbp)
	je	.L94
	cmpq	$0, -168(%rbp)
	jne	.L93
.L94:
	movl	run_compare(%rip), %eax
	testl	%eax, %eax
	jne	.L93
	leaq	.LC42(%rip), %rdi
	call	puts@PLT
	movl	$-1, %eax
	jmp	.L125
.L93:
	cmpl	$0, -216(%rbp)
	jle	.L95
	cmpq	$0, -176(%rbp)
	jne	.L95
	movl	run_compare(%rip), %eax
	cmpl	$1, %eax
	jne	.L95
	movabsq	$1099511627775, %rax
	movq	%rax, -176(%rbp)
	movb	$77, -226(%rbp)
.L95:
	movl	verbose(%rip), %eax
	testl	%eax, %eax
	je	.L96
	leaq	.LC43(%rip), %rdi
	call	puts@PLT
	leaq	.LC44(%rip), %rdi
	call	puts@PLT
	movq	-128(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC45(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-184(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC46(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-160(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC47(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-152(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC48(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-224(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC49(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-220(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC50(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-216(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC51(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-176(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC52(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-168(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC53(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	cmpb	$77, -226(%rbp)
	jne	.L97
	leaq	.LC54(%rip), %rax
	jmp	.L98
.L97:
	leaq	.LC55(%rip), %rax
.L98:
	movq	%rax, %rsi
	leaq	.LC56(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	cmpb	$77, -225(%rbp)
	jne	.L99
	leaq	.LC54(%rip), %rax
	jmp	.L100
.L99:
	leaq	.LC55(%rip), %rax
.L100:
	movq	%rax, %rsi
	leaq	.LC57(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-144(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC58(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-136(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC59(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	cmpl	$1, -212(%rbp)
	jne	.L101
	leaq	.LC60(%rip), %rax
	jmp	.L102
.L101:
	leaq	.LC61(%rip), %rax
.L102:
	movq	%rax, %rsi
	leaq	.LC62(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	display_calculation(%rip), %eax
	cmpl	$1, %eax
	jne	.L103
	leaq	.LC60(%rip), %rax
	jmp	.L104
.L103:
	leaq	.LC61(%rip), %rax
.L104:
	movq	%rax, %rsi
	leaq	.LC63(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	run_compare(%rip), %eax
	cmpl	$1, %eax
	jne	.L105
	leaq	.LC64(%rip), %rax
	jmp	.L106
.L105:
	leaq	.LC65(%rip), %rax
.L106:
	movq	%rax, %rsi
	leaq	.LC66(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
.L96:
	movl	$0, %esi
	movl	$0, %edi
	call	wrmsr_value
	movq	%rax, plane0_zero(%rip)
	movl	$2, %esi
	movl	$0, %edi
	call	wrmsr_value
	movq	%rax, plane2_zero(%rip)
	movl	-216(%rbp), %eax
	cltq
	salq	$3, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, -112(%rbp)
	movl	-216(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, -104(%rbp)
	movl	$0, -208(%rbp)
	jmp	.L107
.L110:
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rax, %rdx
	movq	-176(%rbp), %rax
	movq	%rax, 8(%rdx)
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rax, %rdx
	movq	-176(%rbp), %rax
	movq	%rax, 16(%rdx)
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rax, %rdx
	movq	-168(%rbp), %rax
	movq	%rax, 24(%rdx)
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rax, %rdx
	movq	-144(%rbp), %rax
	movq	%rax, 32(%rdx)
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rax, %rdx
	movq	-136(%rbp), %rax
	movq	%rax, 40(%rdx)
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rax, %rdx
	movzbl	-226(%rbp), %eax
	movb	%al, (%rdx)
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rax, %rdx
	movzbl	-225(%rbp), %eax
	movb	%al, 1(%rdx)
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	$0, 48(%rax)
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	$0, 56(%rax)
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rax, %rdx
	movq	-184(%rbp), %rax
	movq	%rax, 64(%rdx)
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rax, %rdx
	movl	-208(%rbp), %eax
	movl	%eax, 72(%rdx)
	movl	run_compare(%rip), %eax
	cmpl	$1, %eax
	jne	.L108
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rax, %rdx
	movl	-208(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rcx
	movq	-112(%rbp), %rax
	addq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	compare_it(%rip), %rdx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create@PLT
	jmp	.L109
.L108:
	movl	-208(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rax, %rdx
	movl	-208(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rcx
	movq	-112(%rbp), %rax
	addq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	multiply_it(%rip), %rdx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create@PLT
.L109:
	addl	$1, -208(%rbp)
.L107:
	movl	-208(%rbp), %eax
	cmpl	-216(%rbp), %eax
	jl	.L110
	movq	-160(%rbp), %rax
	movl	%eax, -64(%rbp)
	movq	-152(%rbp), %rax
	movl	%eax, -60(%rbp)
	movl	-224(%rbp), %eax
	movl	%eax, -56(%rbp)
	movl	-220(%rbp), %eax
	movl	%eax, -52(%rbp)
	movq	-128(%rbp), %rax
	movq	%rax, -40(%rbp)
	movl	$0, %eax
	call	screen_init
	cmpl	$0, -212(%rbp)
	jne	.L111
	leaq	-64(%rbp), %rdx
	leaq	-192(%rbp), %rax
	movq	%rdx, %rcx
	leaq	undervolt_it(%rip), %rdx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create@PLT
	movq	-192(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join@PLT
.L111:
	movl	$0, -204(%rbp)
	jmp	.L112
.L113:
	movl	-204(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-112(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join@PLT
	addl	$1, -204(%rbp)
.L112:
	movl	-204(%rbp), %eax
	cmpl	-216(%rbp), %eax
	jl	.L113
	movl	$0, -200(%rbp)
	jmp	.L114
.L124:
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	48(%rax), %rcx
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	56(%rax), %rax
	cmpq	%rax, %rcx
	je	.L115
	movq	stdscr(%rip), %rax
	movq	%rax, %rdi
	call	wclear@PLT
	movl	$3, pp_y(%rip)
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	16(%rax), %rcx
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	24(%rax), %rax
	imulq	%rcx, %rax
	movq	%rax, -96(%rbp)
	movq	stdscr(%rip), %rax
	movl	$0, %edx
	movl	$4, %esi
	movq	%rax, %rdi
	call	wcolor_set@PLT
	movl	pp_y(%rip), %eax
	leal	1(%rax), %edx
	movl	%edx, pp_y(%rip)
	movq	stdscr(%rip), %rcx
	movl	$0, %edx
	movl	%eax, %esi
	movq	%rcx, %rdi
	call	wmove@PLT
	cmpl	$-1, %eax
	je	.L117
	movq	stdscr(%rip), %rax
	movl	$-1, %edx
	leaq	.LC67(%rip), %rsi
	movq	%rax, %rdi
	call	waddnstr@PLT
.L117:
	movq	stdscr(%rip), %rax
	movl	$0, %edx
	movl	$1, %esi
	movq	%rax, %rdi
	call	wcolor_set@PLT
	movss	-44(%rbp), %xmm0
	cvtss2sd	%xmm0, %xmm0
	movl	pp_x(%rip), %ecx
	movl	pp_y(%rip), %eax
	leal	1(%rax), %edx
	movl	%edx, pp_y(%rip)
	leaq	.LC68(%rip), %rdx
	movl	%ecx, %esi
	movl	%eax, %edi
	movl	$1, %eax
	call	mvprintw@PLT
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %edx
	movl	%edx, pp_y(%rip)
	movq	-184(%rbp), %rdx
	movq	%rdx, %rcx
	leaq	.LC69(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %edx
	movl	%edx, pp_y(%rip)
	movq	-128(%rbp), %rdx
	movq	%rdx, %rcx
	leaq	.LC70(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %edx
	movl	%edx, pp_y(%rip)
	movl	-216(%rbp), %ecx
	movl	-200(%rbp), %edx
	movl	%ecx, %r8d
	movl	%edx, %ecx
	leaq	.LC71(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	64(%rax), %rdx
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %ecx
	movl	%ecx, pp_y(%rip)
	movq	%rdx, %rcx
	leaq	.LC72(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movl	run_compare(%rip), %eax
	testl	%eax, %eax
	jne	.L118
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	16(%rax), %rdx
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %ecx
	movl	%ecx, pp_y(%rip)
	movq	%rdx, %rcx
	leaq	.LC73(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	24(%rax), %rdx
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %ecx
	movl	%ecx, pp_y(%rip)
	movq	%rdx, %rcx
	leaq	.LC74(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	16(%rax), %rcx
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	24(%rax), %rax
	imulq	%rax, %rcx
	movq	%rcx, %rdx
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %ecx
	movl	%ecx, pp_y(%rip)
	movq	%rdx, %rcx
	leaq	.LC75(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	48(%rax), %rax
	cmpq	%rax, -96(%rbp)
	je	.L119
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	48(%rax), %rdx
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %ecx
	movl	%ecx, pp_y(%rip)
	movq	%rdx, %rcx
	leaq	.LC76(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
.L119:
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	56(%rax), %rax
	cmpq	%rax, -96(%rbp)
	je	.L121
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	56(%rax), %rdx
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %ecx
	movl	%ecx, pp_y(%rip)
	movq	%rdx, %rcx
	leaq	.LC76(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	jmp	.L121
.L118:
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	16(%rax), %rdx
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %ecx
	movl	%ecx, pp_y(%rip)
	movq	%rdx, %rcx
	leaq	.LC77(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	24(%rax), %rdx
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %ecx
	movl	%ecx, pp_y(%rip)
	movq	%rdx, %rcx
	leaq	.LC78(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	8(%rax), %rdx
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %ecx
	movl	%ecx, pp_y(%rip)
	movq	%rdx, %rcx
	leaq	.LC79(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
.L121:
	movl	run_compare(%rip), %eax
	cmpl	$1, %eax
	jne	.L122
	leaq	.LC64(%rip), %rdx
	jmp	.L123
.L122:
	leaq	.LC65(%rip), %rdx
.L123:
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %ecx
	movl	%ecx, pp_y(%rip)
	movq	%rdx, %rcx
	leaq	.LC80(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	48(%rax), %rcx
	movl	-200(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	56(%rax), %rax
	xorq	%rax, %rcx
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %edx
	movl	%edx, pp_y(%rip)
	leaq	.LC81(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movl	-48(%rbp), %eax
	leal	1(%rax), %ecx
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %edx
	movl	%edx, pp_y(%rip)
	leaq	.LC82(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	leaq	.LC83(%rip), %rax
	movq	%rax, -88(%rbp)
	movq	-88(%rbp), %rax
	leaq	.LC84(%rip), %rsi
	movq	%rax, %rdi
	call	popen@PLT
	movq	%rax, -80(%rbp)
	movq	-80(%rbp), %rdx
	leaq	-28(%rbp), %rax
	movl	$10, %esi
	movq	%rax, %rdi
	call	fgets@PLT
	movq	-80(%rbp), %rax
	movq	%rax, %rdi
	call	fclose@PLT
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %edx
	movl	%edx, pp_y(%rip)
	leaq	-28(%rbp), %rdx
	movq	%rdx, %rcx
	leaq	.LC85(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	leaq	.LC86(%rip), %rax
	movq	%rax, -72(%rbp)
	movq	-72(%rbp), %rax
	leaq	.LC84(%rip), %rsi
	movq	%rax, %rdi
	call	popen@PLT
	movq	%rax, -80(%rbp)
	movq	-80(%rbp), %rdx
	leaq	-18(%rbp), %rax
	movl	$10, %esi
	movq	%rax, %rdi
	call	fgets@PLT
	movl	pp_x(%rip), %esi
	movl	pp_y(%rip), %eax
	leal	1(%rax), %edx
	movl	%edx, pp_y(%rip)
	leaq	-18(%rbp), %rdx
	movq	%rdx, %rcx
	leaq	.LC87(%rip), %rdx
	movl	%eax, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movq	-80(%rbp), %rax
	movq	%rax, %rdi
	call	fclose@PLT
	leaq	.LC88(%rip), %rdx
	movl	$1, %esi
	movl	$21, %edi
	movl	$0, %eax
	call	mvprintw@PLT
	movq	stdscr(%rip), %rax
	movq	%rax, %rdi
	call	wrefresh@PLT
	movq	stdscr(%rip), %rax
	movq	%rax, %rdi
	call	wgetch@PLT
	movl	$0, %eax
	call	screen_del
.L115:
	addl	$1, -200(%rbp)
.L114:
	movl	-200(%rbp), %eax
	cmpl	-216(%rbp), %eax
	jl	.L124
	movq	mainwin(%rip), %rax
	movq	%rax, %rdi
	call	delwin@PLT
	call	endwin@PLT
	leaq	.LC89(%rip), %rdi
	call	puts@PLT
	movl	$0, %eax
.L125:
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L126
	call	__stack_chk_fail@PLT
.L126:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3933:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC0:
	.long	3539053052
	.long	1072718413
	.align 8
.LC1:
	.long	0
	.long	1071644672
	.align 8
.LC5:
	.long	0
	.long	1093567616
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
