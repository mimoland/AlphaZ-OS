#include <alphaz/linkage.h>

.section .bss
.align 4096
idle_stack:
    .fill 4096,8,0
stack_top:

.section .text

ENTRY(_start)
    /* 切换栈 */
	movl	$stack_top, %esp
    jmp     kernel_main
END(_start)
