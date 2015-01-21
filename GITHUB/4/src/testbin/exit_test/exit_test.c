/*
 * exit_test.c
 *
 * 	Accept console input (or rather, stdin) and throw it away.
 *
 * This should work once the basic system calls are complete.
 */

#include <unistd.h>
#include <err.h>

int
main(void)
{
	 char ch=0;
        int r;

        while (ch!='q') {

	printf("I can't handle this!!I think I'll just exit\n");
		exit(0);

	return 0;
	}

}
