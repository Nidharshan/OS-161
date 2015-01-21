#include <types.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <lib.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <vm.h>
#include <vfs.h>
#include <test.h>
#include <kern/limits.h>
#include <machine/pcb.h>
#include <machine/spl.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <syscall.h>
#include <addrspace.h>
#define HEAP_MAX    0x40000000
struct fork_info {
	struct semaphore * sem;	// used to make the parent wait until the child is ready
	struct thread * parent; // used by child, so it can copy over the addrspace, etc.
	struct trapframe *tf;	// child copies this trapframe
	pid_t child_pid;	// used to pass the child's pid to the parent
				// -1 when child process ran out of memory, 0 when no more processes available
 };
int
sys_getpid (pid_t *retval)
{
        kprintf("The pid syscall is %d",(int)curthread->t_pid);
        *retval = (pid_t)curthread -> t_pid;
        return *retval;
}

int
sys_sbrk(intptr_t change,int *err){
 
vaddr_t vadd=curthread->t_vmspace->heap_top;
    kprintf("Printing new heap top :%d\n",curthread->t_vmspace->heap_top);
if(change==0)
{
*err= curthread->t_vmspace->heap_top;
//return 0;
}
if((curthread->t_vmspace->heap_top+( change *PAGE_SIZE) <= HEAP_MAX)&&(change!=0))
{   kprintf("printing indside the second if loop top:%d\n",curthread->t_vmspace->heap_top);
curthread->t_vmspace->heap_top = curthread->t_vmspace->heap_top+ (change *PAGE_SIZE);
kprintf("inside second change\n");
*err= curthread->t_vmspace->heap_top;
kprintf("printing indside the sddddddddddddddddecond if loop top:%d\n",curthread->t_vmspace->heap_top);
}
return 0;
}                                                                                                                    

static 
pid_t
create_tpid(){ /* This function is used to create the pid and save the pid's using the int proc_table*/
  int i;
  for (i=proc_table; i<PID_MAX; i++)
  {
	    return (pid_t)i;
  }
  return -1; // No pids available
}

void 
sys_exit(){/*This funtion is used to exit the threads created*/

thread_exit();
}

int 
sys_execv(userptr_t progname, userptr_t args){ /*This function implements the execv functionality*/
    size_t get, offset;
    struct vnode *v;
    vaddr_t entrypoint, stackptr;
    userptr_t userdest;
    int i=0, append_zero, argc=0, result, part=0;
    char *buf_mem;
    int n=0;
    struct addrspace *old_addr = curthread->t_vmspace;
    char **usr_args = (char**)args;
    buf_mem = (void *) kmalloc(sizeof(void *));
    result = copyin((const_userptr_t)args,buf_mem,4);
    if (result){
        kfree(buf_mem);
        return result;
    }

   lock_acquire(execv_lock);


    kfree(buf_mem);


    while(usr_args[argc] != NULL){
        argc++;
    }
    size_t len_string[argc];
    userptr_t user_argv[argc];

    char *args_buf = kmalloc(PAGE_SIZE*sizeof(char));

    // Check user pointer
    buf_mem = (char *)kmalloc(1024*sizeof(char));
    if (buf_mem == NULL){
        result = ENOMEM;
        kfree(args_buf);
    }
    

    result = copyinstr((const_userptr_t)progname,buf_mem,1024,&get);
    if (result){
        kfree(buf_mem);
    }
	//Implementation from runprogram from here
    /* Open the file. */
    result = vfs_open((char *)progname, O_RDONLY, &v);
    if (result) {
        kfree(buf_mem);
    }
    // Keep old addrspace in case of failure
    struct addrspace *new_addr = as_create();
    if (new_addr == NULL){
        result = ENOMEM;
        vfs_close(v);
    }
    while (usr_args[i] != NULL){
        result = copyinstr((const_userptr_t)usr_args[i], &args_buf[part], PAGE_SIZE, &len_string[i]);
        if (result){
            as_destroy(new_addr);
        }
        part += len_string[i];
        i++;
    }
    /* Swap addrspace and Activate it*/
    curthread->t_vmspace = new_addr;
    as_activate(curthread->t_vmspace);

    /* Load the executable. */
    result = load_elf(v, &entrypoint);
    if (result) {
        curthread->t_vmspace = old_addr;
        as_activate(curthread->t_vmspace);
    }
    /* Define the user stack in the address space */
    result = as_define_stack(curthread->t_vmspace, &stackptr);
    if (result) {
        curthread->t_vmspace = old_addr;
        as_activate(curthread->t_vmspace);
    }

    // Copy args to new addrspace
    offset = 0;
    for (i=argc-1; i>-1; i--){
        part -= len_string[i]; // readjust inherited part index
        append_zero = (4 - (len_string[i]%4) ) % 4; // Word align
        offset += append_zero;
        offset += len_string[i];

        user_argv[i] = (userptr_t)(stackptr - offset);

        result = copyoutstr((const char*)&args_buf[part], user_argv[i], len_string[i], &get);
        if (result){
        curthread->t_vmspace = old_addr;
        as_activate(curthread->t_vmspace);
        }
    }

    // Copy pointers to argv
    userdest = user_argv[0] - 4 * (argc+1);
    stackptr = (vaddr_t)userdest; // Set stack pointer
    for (i=0; i<argc; i++){
        result = copyout((const void *)&user_argv[i], userdest, 4);
        if (result)
        curthread->t_vmspace = old_addr;
        as_activate(curthread->t_vmspace);
        userdest += 4;
    }

    // Wrap up
    kfree(args_buf);
    vfs_close(v);



    /* Warp to user mode. */
    md_usermode(argc, (userptr_t)stackptr, stackptr, entrypoint);


	lock_release(execv_lock);
    	return EINVAL;

}

void 
md_forkentry(void * data1, unsigned long unused) {
	//(void)unused;
	struct fork_info * info = data1;
	struct addrspace * new_as;
	struct trapframe new_tf;
	/* copy address space */
	if (as_copy(info->parent->t_vmspace, &new_as)) {
		info->child_pid = ENOMEM; // Means no memory for process
		V(info->sem);	// child is done, parent should resume.
		thread_exit();
	}
	curthread->t_vmspace = new_as;
	as_activate(new_as);

	/* copy trap frame  */
	memcpy(&new_tf, info->tf, sizeof(struct trapframe));
	/* change tf's registers to return values for syscall */
	new_tf.tf_v0 = 0;
	new_tf.tf_a3 = 0;
	new_tf.tf_epc += 4;
	/* create process and get the pid */
	struct thread *new_process = kmalloc(sizeof(struct thread));
	 new_process->parent_pid = curthread->t_pid;
	proc_table++; 
	new_process->t_pid = create_tpid();
	proc_table++;
	

	V(info->sem);
	mips_usermode(&new_tf);
	/* mips_usermode does not return */
}



int 
sys_fork(struct trapframe *tf, int * retval) {/*This function is implementation to fork the thread*/
	struct fork_info info;

	/* Create fork_info */
	info.sem = sem_create("fork synch", 0);
	if (info.sem == NULL) {
		*retval = -1;
		return ENOMEM;
	}
	info.parent = curthread;
	info.tf = tf;	
	/* fork the thread; child does all the work using fork_info */
	int result;
	result = thread_fork("forked", &info, 0, md_forkentry, NULL);
	
	if (result) {
		sem_destroy(info.sem);
		*retval = -1;
		return ENOMEM;
	}

	P(info.sem); // wait until the child is ready (has a pid)

	*retval = info.child_pid;
	sem_destroy(info.sem);

	// Checks for errors denoted by the child_pid generated by chork
	if (info.child_pid == 0) {
		*retval = -1;
		return EAGAIN;
	} else if (info.child_pid < 0) {
		*retval = -1;
		return ENOMEM;
	} else {
		return 0;
	}
}
