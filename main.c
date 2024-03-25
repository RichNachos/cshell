#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>

#define BUF_SIZE 128
#define CWD_SIZE 256

void exit_msg(void);
int process(char* cmd);

int main(int argc, char **argv) {
    if (argc > 1) {
        printf("Sorry, this version does not support running cshell scripts\n");
        exit(-1);
    }

    // Buffer for shell input
    char buf[BUF_SIZE] = {'\0'};
    char cwd[CWD_SIZE] = {'\0'};

    getcwd(cwd, CWD_SIZE);
    printf("%s:$ ", cwd);
    // Read until fgets returns null (Encoutered EOF symbol)
    while (fgets(buf, BUF_SIZE, stdin)) {
        
        // Check for cd
        if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ') {
            int i;
            for (i = 3; buf[i] != '\n' && buf[i] != '\0' && buf[i] != ' '; i++) {
                continue;
            }
            buf[i] = '\0';

            chdir(&buf[3]);
            goto start;
        }


        // Not cd, fork and exec command
        pid_t p = fork();
        if (p == -1) {
            printf("Fork failed for unknown reason! Maybe check process count?\n");
            exit(1);
        }

        if (p == 0) {
            // Process is child
            if (process(buf) == 1) {
                printf("%s program not found\n", buf);
                exit(1);
            }
        }
        int wstatus;
        wait(&wstatus);
        // printf("EXIT CODE: %d\n", wstatus);

    start:
        getcwd(cwd, CWD_SIZE);
        printf("%s:$ ", cwd);
    }
    
    exit_msg();
    exit(0);
}

int process(char *buf) {
    char *file = buf;
    char *argv[10] = {0};
    int argv_i = 1;
    argv[0] = buf;

    for (int i = 0; i < BUF_SIZE; i++) {
        if (buf[i] == ' ') {
            buf[i] = '\0';
            argv[argv_i++] = &buf[i+1];
            continue;
        }
        if (buf[i] == '\n') {
            buf[i] = '\0';
            argv[argv_i++] = NULL;
            break;
        }
    }
    
    execvp(file, argv);
    
    return 1;
}


void exit_msg(void) {
    printf("\n-------TERMINATING CSH--------\n\n");
    printf("            Goodbye!            \n\n");
}
