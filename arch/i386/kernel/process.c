#include <asm/process.h>
#include <asm/global.h>
#include <asm/protect.h>
#include <asm/string.h>
#include <asm/bug.h>

void init_process()
{
    task_table[0] = (Task){TestA, STACK_SIZE_TESTA, "TestA"};
    task_table[1] = (Task){TestB, STACK_SIZE_TESTB, "TestB"};
    Task *p_task = task_table;
    Process *p_proc = proc_table;
    unsigned char *p_task_stack = task_stack + STACK_SIZE_TOTAL;
    u16  selector_ldt = SELECTOR_LDT_FIRST;
    int i;
    for (i = 0; i < NR_TASKS; i++) {
        p_proc->ldt_sel = selector_ldt;
        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(Process));
        p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(Descriptor));
        p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;

        /* cs指向LDT中的第一个描述符，ds等指向第二个，gs指向显存 */
        p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;
        p_proc->regs.eip = (u32)p_task->initial_eip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = 0x1202; // IF=1, IOPL=1, bit 2 is always 1.

        disp_int((u32)p_task->initial_eip);
        disp_str("\n");

        p_task_stack -= p_task->stacksize;
        p_proc++;
        p_task++;
        selector_ldt += 1<<3;
    }

    p_proc_ready = proc_table;
    restart();

    while (1)
    {
        /* code */
    }

}


void switch_task()
{
    // disp_str("#");
    p_proc_ready++;
    if(p_proc_ready >= proc_table + NR_TASKS) p_proc_ready = proc_table;
}
