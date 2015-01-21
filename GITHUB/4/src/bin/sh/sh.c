/*
 * Shell for OS-161
 * Usage: Testing system calls for Assignment-2
 */

#ifdef HOST
#include "hostcompat.h"
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 255
#define MAX_ARGS 255

int
get_line(char *line)
{
	int pos;
	int ch;

	pos = 0;
	while (1) {
		ch = getchar();
		if (ch == EOF) break;
		putchar(ch);
		if (ch == '\n') { break; }
		line[pos++] = ch;
		if (pos == MAX_LINE_LENGTH - 1) {
			putchar('\n');
			break;
		}
	}
	line[pos] = '\0';
	return 0;
}


int
main(int argc, char *argv[])
{
#ifdef HOST
	hostcompat_init(argc, argv);
#endif

	(void)argc;
	(void)argv;

	char line[MAX_LINE_LENGTH];
	int result;
	pid_t child_pid;
	int status;
	char *tokens[MAX_ARGS+1];
	int parent_pid;
	while (1) {

		printf("os161-sh$$$$ ");
		result = get_line(line);
		if (strcmp(line, "exit") == 0) break;
		if (strlen(line) == 0) continue;

		tokens[0] = (char *)line;
		int count = 0;
		char *ptr;
		for (ptr = line; *ptr != (char)NULL; ptr++) {
			if (*ptr == ' ') {
				*ptr = (char)NULL;
				tokens[++count] = ptr + 1;
			}
		}
		tokens[count+1] = NULL;
		child_pid = fork();
		printf("The pid is %d\n",child_pid);
		if (child_pid == 0) {
	       		 execv(line, tokens);
			printf("%s: execv failed\n", tokens[0]);
			exit(1); // TODO
		} else {
                        printf("i am in waitpid");
	//		waitpid(child_pid, &status, 0);
		}
        printf("End of sh.c file");
	}
	exit(0);
}

