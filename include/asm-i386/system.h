#ifndef _ASM_SYSTEM_H_
#define _ASM_SYSTEM_H_

#include <asm/cpu.h>

#define move_to_user_mode(user_stack) do {                              \
    asm volatile(                                                       \
        "pushl %%ebx\n\t"                                               \
        "pushl %%eax\n\t"                                               \
        "pushfl\n\t"                                                    \
        "pushl %%edx\n\t"                                               \
        "pushl $1f\n\t"                                                 \
        "iretl\n\t"                                                     \
        "1:\t"                                                          \
        "mov %%bx, %%ax\n\t"                                            \
        "mov %%ax, %%ds\n\t"                                            \
        "mov %%ax, %%es\n\t"                                            \
        "mov %%ax, %%fs\n\t"                                            \
        :                                                               \
        :"a"(user_stack), "b"(USER_DATA_SELECTOR), "d"(USER_CODE_SELECTOR)); \
} while (0)


#endif
