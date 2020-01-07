#ifndef _ASM_SCHED_H_
#define _ASM_SCHED_H_

#include <alphaz/type.h>
#include <alphaz/compiler.h>
#include <asm/cpu.h>

/**
 * defined in entry.asm
 */
extern void __switch_to_first_task(u32);

void setup_thread(struct thread_struct *, u32, u32, unsigned int);

struct task_struct * __current(void);

#endif
