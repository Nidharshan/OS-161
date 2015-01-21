#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */
//pid_t pid;
int sys_reboot(int code);
int sys_read(int filehandle, char *buf, size_t size);
int sys_write(int filehandle, char *buf, size_t size);
//pid_t sys_fork(struct trapframe *tf, int *err);
//int sys_fork(struct trapframe *tf);
int sys_fork(struct trapframe *tf, pid_t *retval);
//static void child_thread(void *vtf, unsigned long junk);
void sys_exit();
//static pid_t getpid1();
//int sys_execv (char *progname, char **args);
int sys_execv(userptr_t progname, userptr_t args);
int sys_getpid(pid_t *retval);
//pid_t sys_getpid(void);

static pid_t create_tpid();
int sbrk(int size);




#endif /* _SYSCALL_H_ */
