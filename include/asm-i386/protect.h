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

#endif