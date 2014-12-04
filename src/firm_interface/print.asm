	.file	"print.c"
	.section	.rodata
.LC0:
	.string	"%d\n"
	.text
	.globl	_COut_Mprintln
	.type	_COut_Mprintln, @function
_COut_Mprintln:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	_COut_Mprintln, .-_COut_Mprintln
	.ident	"GCC: (Debian 4.9.1-19) 4.9.1"
	.section	.note.GNU-stack,"",@progbits
