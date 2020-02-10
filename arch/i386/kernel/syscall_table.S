
#include <alphaz/linkage.h>

.section .data

ENTRY(sys_call_table)
    .long  sys_get_ticks
    .long  sys_debug    /* 1 */
    .long  sys_fork
    .long  sys_read
    .long  sys_write
    .long  sys_exit    /* 5 */
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug    /* 10 */
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug    /* 15 */
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug
    .long  sys_getpid    /* 20 */
    .long  sys_sleep
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug    /* 25 */
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug
    .long  sys_debug
    .long  sys_reboot    /* 30 */
    .long  sys_debug
END(sys_call_table)
