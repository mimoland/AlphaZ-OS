#ifndef _ASM_SCHED_H_
#define _ASM_SCHED_H_

/**
 * defined in entry.asm
 */
extern void __switch_to_first_task(u32);

void setup_thread(struct thread_struct *, u32, u32, unsigned int);

#endif
