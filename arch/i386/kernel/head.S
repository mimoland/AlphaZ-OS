#include <alphaz/linkage.h>

.section .bss
stack_space:
    .fill 2048,8,0
stack_top:

.section .text

ENTRY(_start)
    /* 切换栈 */
	movl	$stack_top, %esp
    jmp     kernel_main
END(_start)
