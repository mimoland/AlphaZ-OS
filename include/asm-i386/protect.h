#ifndef _ASM_PROTECT_H_
#define _ASM_PROTECT_H_

#include <alphaz/type.h>

typedef struct s_descriptor		/* 共 8 个字节 */
{
	u16	limit_low;		    /* Limit */
	u16	base_low;		    /* Base */
	u8	base_mid;		    /* Base */
	u8	attr1;			    /* P(1) DPL(2) DT(1) TYPE(4) */
	u8	limit_high_attr2;	/* G(1) D(1) 0(1) AVL(1) LimitHigh(4) */
	u8	base_high;		    /* Base */
} Descriptor;

/* 门描述符 */
typedef struct s_gate
{
	u16	offset_low;	    /* Offset Low */
	u16	selector;	    /* Selector */
	u8	dcount;         /* 栈切换时要复制的参数数量 */
	u8	attr;		    /* P(1) DPL(2) DT(1) TYPE(4) */
	u16	offset_high;	/* Offset High */
}Gate;


/* 权限 */
#define PRIVILEGE_KRNL 0
#define	PRIVILEGE_TASK 1
#define	PRIVILEGE_USER 3

/* 选择子，与loader中一致 */
#define	SELECTOR_DUMMY		   0
#define	SELECTOR_FLAT_C		0x08
#define	SELECTOR_FLAT_RW	0x10
#define	SELECTOR_VIDEO		(0x18+3)
#define	SELECTOR_TSS		0x20
#define	SELECTOR_LDT_FIRST	0x28

#define	SELECTOR_KERNEL_CS	SELECTOR_FLAT_C
#define	SELECTOR_KERNEL_DS	SELECTOR_FLAT_RW
#define	SELECTOR_KERNEL_GS	SELECTOR_VIDEO


#define	DA_386IGate		0x8E	/* 386 中断门类型值 */

/* 中断向量 */
#define	INT_VECTOR_DIVIDE		    0x0
#define	INT_VECTOR_DEBUG		    0x1
#define	INT_VECTOR_NMI			    0x2
#define	INT_VECTOR_BREAKPOINT	    0x3
#define	INT_VECTOR_OVERFLOW		    0x4
#define	INT_VECTOR_BOUNDS		    0x5
#define	INT_VECTOR_INVAL_OP		    0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT		0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT		    0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
#define	INT_VECTOR_COPROC_ERR		0x10

#endif
