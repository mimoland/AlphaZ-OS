#ifndef _ALPHAZ_SYSCALLS_H_
#define _ALPHAZ_SYSCALLS_H_

#include <alphaz/linkage.h>
#include <alphaz/type.h>

asmlinkage unsigned long sys_get_ticks(void);

asmlinkage long sys_fork(void);
asmlinkage ssize_t sys_read(int fd, void *buf, size_t nbytes);
asmlinkage ssize_t sys_write(int fd, const void *buf, size_t nbytes);

asmlinkage unsigned long sys_getpid(void);
asmlinkage long sys_sleep(void);

asmlinkage long sys_reboot(void);
asmlinkage long sys_debug(void);

#endif
