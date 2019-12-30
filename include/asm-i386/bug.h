#ifndef _ASM_BUG_H_
#define _ASM_BUG_H_

/* 屏幕显示位置 defined in bug.c */
extern int disp_pos;

void bug_init();

void disp_str(char *);

void disp_int(int);

void hlt();

#endif
