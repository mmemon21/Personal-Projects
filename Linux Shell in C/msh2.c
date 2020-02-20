#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
/*
 * This is a simple custom shell made to support 'help', 'exit' & 'today'. It also supports linux cmds.
 */

#define MAX_BUF 160
#define MAX_TOKS 100

int main() {
	int ch;
	char* pos;
	char* newtok = "";
	int helperPvd = 1;
	int datePvd = 1;
	int exitPvd = 1;
	int pid = 1;
	char s[MAX_BUF + 2];   // 2 extra for the newline and ending '\0'
	static const char prompt[] = "msh> ";
	char* toks[MAX_TOKS];

	while (1) {
		// prompt for input if input from terminal
		if (isatty(fileno(stdin))) {
			printf(prompt);
		}

		// read input
		char* status = fgets(s, MAX_BUF + 2, stdin);

		// exit if ^d entered
		if (status == NULL) {
			printf("\n");
			break;
		}

		// input is too long if last character is not newline 
		if ((pos = strchr(s, '\n')) == NULL) {
			printf("error: input too long\n");
			// clear the input buffer
			while ((ch = getchar()) != '\n' && ch != EOF);
			continue;
		}

		// remove trailing newline
		*pos = '\0';

		newtok = strtok(s, ">");
		helperPvd = strcmp(newtok, "help");
		datePvd = strcmp(newtok, "today");
		exitPvd = strcmp(newtok, "exit");
		pid = strcmp(newtok, "ls");
		
		//helper command
		if (helperPvd == 0) {
			printf("enter Linux commands, or 'exit' to exit\n");
			continue;
		}

		//display today's date
		if (datePvd == 0) {
			time_t now;
			time(&now);
			struct tm* local = localtime(&now);
			int month, day, year;
			month = local->tm_mon + 1;
			day = local->tm_mday;
			year = local->tm_year + 1900;
			printf("%02d/%02d/%02d\n", month, day, year);
			continue;
		} 
		//exit command
		if (exitPvd == 0) {
			break;
		}
		//For linux cmds
		else {
            char **args = (char**) calloc(5, sizeof(char*));
            newtok  = strtok(s, " \n\t"); 
            
            int i = 0;
            while(newtok != NULL)
            {
                if(newtok != "\n"){
                    args[i] = strdup(newtok);
                }
                if(i == 5)
                    break;
                i++;
                newtok  = strtok(NULL, " \n\t");
            }
            args[i] = NULL;
            
            int rc = fork();
            if (rc < 0) { // exit if fork fails
                fprintf(stderr, "fork failed\n");
                exit(1);
            } else if (rc == 0) { // checks if there are any errors. 
                int check = execvp(args[0], args);
                if(check < 0)
                    printf("msh: %s: No such file or directory\n", args[0]);
                	break;
            } else {
                int rc_wait = wait(NULL);
            }
		
		} //else condition for linux cmds ends here 


	} //While loop ends here
	exit(EXIT_SUCCESS);
}
