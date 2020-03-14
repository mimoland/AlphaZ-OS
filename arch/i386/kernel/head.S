#include <alphaz/linkage.h>
#include <alphaz/mm.h>

.section .bss
.align 4096
idle_stack:
    .fill 4096,1,0
stack_top:

.section ".head.text"

ENTRY(_start)

    	# 初始化页表
	movl	$(_PTE - __KERNEL_OFFSET), %edi		# 加载页目录的地址
    	movl    $2048, %ecx         			# 初始化2048个页表项
	movl    $7, %eax            			# 页表项的低12位的属性
initpages:
	movl    %eax, (%edi)
	addl    $4096, %eax
	addl    $4, %edi
	loop    initpages

	# 切换页表
	movl	$(_PDE - __KERNEL_OFFSET), %eax
	movl    %eax, %cr3
	movl	%cr0, %eax
	orl 	$0x80000000, %eax
	movl	%eax, %cr0
	movl 	$(flushtlb), %eax
	jmp	*%eax
flushtlb:
	movl	$stack_top, %esp
    	movl	$stack_top, %ebp
	jmp     kernel_main				# 跳转到内核


.align 8
.org 0x1000

_PDE:
	.long   0x00102007
	.long   0x00103007
	.fill   766,4,0             			# 填充中间的766的页目录
	.long   0x00102007
	.long   0x00103007
	.fill	254,4,0

.org 0x2000
_PTE:
	.fill   2048,4,0            			# 开辟2048个页表空间
