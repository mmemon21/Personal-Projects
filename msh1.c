#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

/*
 * A simple shell
 */

#define MAX_BUF 160
#define MAX_TOKS 100

int main() {
	int ch;
	int i;
	char *pos;
	char *tok;
	char s[MAX_BUF+2];   // 2 extra for the newline and ending '\0'
	static const char prompt[] = "msh> ";
	char *toks[MAX_TOKS];
	time_t rawtime;
	struct tm *timeinfo;

	while (1) {
		// prompt for input if input from terminal
		if (isatty(fileno(stdin))) {
			printf(prompt);
		}

		// read input
		char *status = fgets(s, MAX_BUF+2, stdin);

		// exit if ^d entered
		if (status == NULL) {
			printf("\n");
			break;
		}

		// input is too long if last character is not newline 
		if ((pos = strchr(s, '\n')) == NULL) {
			printf("error: input too long\n");
			// clear the input buffer
			while ((ch = getchar()) != '\n' && ch != EOF) ;
			continue; 
		}

		// remove trailing newline
		*pos = '\0';

		// break input into tokens
		i = 0;
		char *rest = s;
		while ((tok = strtok_r(rest, " ", &rest)) != NULL && i < MAX_TOKS) {
			toks[i] = tok;
			i++;
		}
		if (i == MAX_TOKS) {
			printf("error: too many tokens\n");
			continue;
		}
		toks[i] = NULL;

		// if no tokens do nothing
		if (toks[0] == NULL) {
			continue;
		}

		// exit if command is 'exit'
		if (strcmp(toks[0], "exit") == 0) {
			break;
		}

		// print help info if command is 'help'
		if (strcmp(toks[0], "help") == 0) {
			printf("enter 'help', 'today', or 'exit' to quit\n");
			continue;
		}

		// print date if command is 'date'
		if (strcmp(toks[0], "today") == 0) {
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			printf("%02d/%02d/%4d\n", 1 + timeinfo->tm_mon, timeinfo->tm_mday, 1900 + timeinfo->tm_year);
			continue;
		}

		// otherwise print all tokens
		for (i = 0; toks[i] != NULL; i++) {
			printf("token: '%s'\n", toks[i]);
		}
	}
	exit(EXIT_SUCCESS);
}
