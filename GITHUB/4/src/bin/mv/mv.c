#include <unistd.h>
#include <err.h>
//#include <kern/lib.h>
/*
 * mv - move (rename) files.
 * Usage: mv oldfile newfile
 *
 * Just calls rename() on them. If it fails, we don't attempt to
 * figure out which filename was wrong or what happened.
 *
 * In certain circumstances, Unix mv will fall back to copying and
 * deleting the old copy. We don't do that.
 *
 * We also don't allow the Unix form of
 *     mv file1 file2 file3 destination-dir
 */

static
void
dorename(const char *oldfile, const char *newfile)
{
	if (rename(oldfile, newfile)) {
		err(1, "%s or %s", oldfile, newfile);
	}
}

int
main(int argc, char *argv[])
{
	if (argc!=3) {
		errx(1, "Usage: mv oldfile newfile");
	}
	dorename(argv[1], argv[2]);
	return 0;
}
/*
int main(int argc, char *argv[])

{

     int i = 0;

     volatile int j = 0;

     int k = 0;

     pid_t pid, ori_pid;

     ori_pid = getpid();

     pid = getpid();

     while(1){

         k++;

         pid = getpid();

	//fork several processes
	
	if(i < 20 && pid == ori_pid){

           i++;

           pid = fork();

           if(pid == 0)

               k = 0;

          pid = getpid();

         }

	//half processes will be CPU-bounded
	
	if(pid % 2 != 0){

           for(j = 0; j < 50000; j++);    

         }
	else{

         //half processes will be I/O-bounded
	
//	clocksleep(1);

           printf("PID %d wakeup\n", pid);

         }

     }

     return 0;

}*/
