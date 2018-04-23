#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define MAXLINE 80 /*The maximum length command*/

int main(void) {
	char *args[MAXLINE / 2 + 1]; //command line with max 40 arguments
	int should_run = 1; //flag to determine when to exit program
	printf("Shell Interface\n");
	while (should_run == 1) {
		for (int j = 0; j < MAXLINE / 2; j++) //sets it back to beginning normal
				{
			args[j] = NULL;
		}
		printf("Minh An-707>"); //prompt
		fflush(stdout);
		char temp[80];
		fgets(temp, 80, stdin);
		if (strcmp(temp, "exit") == 10) //why is it equal when the return value is 10?? 1 = true? 0 = return value for strings to be equal?
				{
			should_run = 0;
			break;
		}
		char *p = strchr(temp, '\n'); //this is to get rid of newline character
		if (p)
			*p = 0;

		char *holder = strtok(temp, " "); //IN C A STRING IS A CHAR*
		int i = 0;
		while (holder != NULL) {
			args[i++] = holder;
			holder = strtok(NULL, " ");
		}

		int amperSand = 0; //boolean to check for ampersand in command line

		if (strcmp(args[i - 1], "&") == 0) {
			amperSand = 1;
			args[i - 1] = NULL;
		}

		pid_t pid, wpid;
		int childStatus = 0;
		pid = fork();
		if (pid == 0) //child process
				{
			if (execvp(args[0], args) == -1) {
				perror("Error: ");
			}
			exit(EXIT_FAILURE);
		}

		else if (pid < 0) {
			perror("Error: ");
		}

		else //parent process
		{
			if (amperSand != 1) //wait for child if no & at the end of command line
					{
				do { //wait for child to finish
					wpid = wait(&childStatus);
				} while (wpid != pid);
			}

		}
	}

	return 0;
}
