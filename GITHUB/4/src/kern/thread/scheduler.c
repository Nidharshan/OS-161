/*
 * Scheduler.
 *
 * The default scheduler is very simple, just a round-robin run queue.
 * You'll want to improve it.
 */

#include <types.h>
#include <lib.h>
#include <scheduler.h>
#include <thread.h>
#include <machine/spl.h>
#include <queue.h>

/*
 *  Scheduler data
 */

// Queue of runnable threads
static struct queue *runqueue;
static struct queue *test;
int size=0;
int y=0;
/*
 * Setup function
 */
void
scheduler_bootstrap(void)
{
	runqueue = q_create(32);
        test=q_create(64);
	if (runqueue == NULL) {
		panic("scheduler: Could not create run queue\n");
	}
}

/*
 * Ensure space for handling at least NTHREADS threads.
 * This is done only to ensure that make_runnable() does not fail -
 * if you change the scheduler to not require space outside the 
 * thread structure, for instance, this function can reasonably
 * do nothing.
 */
int
scheduler_preallocate(int nthreads)
{
	assert(curspl>0);
	size= q_preallocate(runqueue, nthreads);
        y=size+ q_preallocate(test, nthreads);
        return y;
}

/*
 * This is called during panic shutdown to dispose of threads other
 * than the one invoking panic. We drop them on the floor instead of
 * cleaning them up properly; since we're about to go down it doesn't
 * really matter, and freeing everything might cause further panics.
 */
void
scheduler_killall(void)
{
	assert(curspl>0);
	while (!q_empty(runqueue)) {
		struct thread *t = q_remhead(runqueue);
		kprintf("scheduler: Dropping thread %s.\n", t->t_name);
	}
        while (!q_empty(runqueue)) {
                struct thread *t = q_remhead(test);
                kprintf("scheduler: Dropping thread %s.\n", t->t_name);
        }
}

/*
 * Cleanup function.
 *
 * The queue objects to being destroyed if it's got stuff in it.
 * Use scheduler_killall to make sure this is the case. During
 * ordinary shutdown, normally it should be.
 */
void
scheduler_shutdown(void)
{
	scheduler_killall();

	assert(curspl>0);
	q_destroy(runqueue);
        q_destroy(test);
	runqueue = NULL;
}

/*
 * Actual scheduler. Returns the next thread to run.  Calls cpu_idle()
 * if there's nothing ready. (Note: cpu_idle must be called in a loop
 * until something's ready - it doesn't know whether the things that
 * wake it up are going to make a thread runnable or not.) 
 */
struct thread *
scheduler(void)
{ int q=0;
	// meant to be called with interrupts off
	assert(curspl>0);
	
	while (q_empty(runqueue)) {
		cpu_idle();
	}
        if(q<=(q_getsize(runqueue)-1))
        {
	return q_remhead(runqueue);
        q++;
         if(q==(q_getsize(runqueue)-1))
           {
           while(q_empty)
             {
             return q_remhead(runqueue);
             }
           }

        }
        else
        {
        return q_remhead(test);
        }

}

/* 
 * Make a thread runnable.
 * With the base scheduler, just add it to the end of the run queue.
 */
int
make_runnable(struct thread *t)
{ int q=0;
	// meant to be called with interrupts off
	assert(curspl>0);
        if(q<=(q_getsize(runqueue)-1))        
         {
         q++;
       	 return q_addtail(runqueue, t);
         }
        else
         {
         return q_addtail(test,t);
         }
}

/*
 * Debugging function to dump the run queue.
 */
void
print_run_queue(void)
{
	/* Turn interrupts off so the whole list prints atomically. */
	int spl = splhigh();

	int i,k=0;
	i = q_getstart(test);
	
	while (i!=q_getend(test)) {
		struct thread *t = q_getguy(test, i);
		kprintf("  %2d: %s %p\n", k, t->t_name, t->t_sleepaddr);
		i=(i+1)%q_getsize(test);
		k++;
	}
	
	splx(spl);
}
