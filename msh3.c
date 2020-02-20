#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * A simple shell
 */

#define MAX_BUF 160
#define MAX_TOKS 100

int main(int argf, char *argv[]) {
    char *pos;
    char *tok;
    char *path;
    char s[MAX_BUF];
    char *toks[MAX_TOKS];
    time_t rawtime;
    struct tm *timeinfo;
    static const char prompt[] = "msh> ";
    FILE *oFile;
    

 /*
  * process command line options
  */


    //opening file and using it as stdin.

    if (argf > 2) {
        fprintf(stderr, "msh: usage: msh [file]\n");
        exit(EXIT_FAILURE);
    }
    if (argf == 2) {
        
        oFile = fopen(argv[1], "r");
        if (oFile == NULL) {
            fprintf(stderr, "msh: cannot open script '%s'.\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        oFile = stdin;
    }


    while (1) {
        // prompt for input if terminal
        if (isatty(fileno(oFile))) {
            printf(prompt);
        }

        //get input from file
        char *status = fgets(s, MAX_BUF - 1, oFile);

        // exit if ^d or "exit" entered
        if (status == NULL || strcmp(s, "exit\n") == 0) {
            if (status == NULL && oFile == stdin) {
                printf("\n");
            }
            exit(EXIT_SUCCESS);
        }

        // remove any trailing newline
        if ((pos = strchr(s, '\n')) != NULL) {
            *pos = '\0';
        }
        // Locate < and > characters
        char * outPos = strchr(s, '>');
        char * inPos = strchr(s, '<');

        if (outPos != NULL) {
            char* rest;
            *outPos=0; // set the end of the string, then advance to the first non-space char
            outPos++;
            // Now we are to try and extract a token from both of them. We should be able to get 1 token
            if ((outPos=strtok_r(outPos, " ", &rest)) != NULL && strtok_r(NULL, " ", &rest)==NULL && strlen(outPos)>0) {
                
            } else {
                fprintf(stderr, "Error when parsing output redirection");
                continue;
            }

        }

        if (inPos != NULL) {
            char* rest;
            *inPos=0; // set the end of the string, then advance to the first non-space char
            inPos++;
            // Now try extract a token from both, we should be able to extract 1 token
            if ((inPos=strtok_r(inPos, " ", &rest)) != NULL && strtok_r(NULL, " ", &rest)==NULL && strlen(inPos)>0) {
               
            } else {
                fprintf(stderr, "Error when parsing input redirection");
                continue;
            }

        }

        // break input line into tokens
        char *rest = s;
        int num_toks = 0;
        while ((tok = strtok_r(rest, " ", &rest)) != NULL && num_toks < MAX_TOKS) {
            toks[num_toks] = tok;
            num_toks++;
        }
        if (num_toks == MAX_TOKS) {
            fprintf(stderr, "msh: too many tokens");
            exit(EXIT_FAILURE);
        }
        toks[num_toks] = NULL;

        /*
         * process a command
         */

        // do nothing if no tokens found in input
        if (num_toks == 0) {
            continue;
        }

        // cd, help, exit & today commands

        if (strcmp(toks[0], "help") == 0) {
            // help
            printf("enter a Linux command, or 'exit' to quit\n");
            continue;
        }
        if (strcmp(toks[0], "today") == 0) {
            // today
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            printf("Current local time: %s", asctime(timeinfo));
            continue;
        }
        if (strcmp(toks[0], "cd") == 0) {
            // cd
            if (num_toks == 1) {
                path = getenv("HOME");
            } else {
                path = toks[1];
            }
            int cd_status = chdir(path);
            if (cd_status != 0) {
                printf("msh: %s: %s: %s\n", toks[0], toks[1], strerror(errno));
            }
            continue;
        }

        //cloning parent process
        int rc = fork();
        if (rc < 0) {
            fprintf(stderr, "msh: fork failed\n");
            exit(1);
        }
        if (rc == 0) {
            if (outPos!=NULL) {
                int outF = open(outPos, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
                if (outF==-1) {
                    printf("msh: error redirecting output to %s\n", outPos);
                    exit(1);
                }
                dup2(outF, STDOUT_FILENO);
            }

            if (inPos!=NULL) {
                // open a file under the new descriptor
                int inF = open(inPos, O_RDONLY);
                if (inF==-1) {
                    printf("msh: error redirecting input from %s\n", inPos);
                    exit(1);
                }
                dup2(inF, STDIN_FILENO);
            }

            execvp(toks[0], toks);
            // error if anything returned by sterror
            printf("msh: %s: %s\n", toks[0], strerror(errno));
            exit(1);
        } else {
            // parent process: wait for child to terminate
            wait(NULL);
        }
    }
}