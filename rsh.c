#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12

extern char **environ;

char *allowed[N] = {"cp","touch","mkdir","ls","pwd","cat","grep","chmod","diff","cd","exit","help"};

int isAllowed(const char*cmd) {
	for (int i = 0; i < N; i++) {// TODO
		if (strcmp(cmd, allowed[i]) == 0) {
			return 1; // return 1 if cmd is one of the allowed commands
		}

	}
	return 0;	// return 0 otherwise
}

int main() {

    char *argv[21];    // TODO
    // Add variables as needed

    char line[256];

    while (1) {

	fprintf(stderr,"rsh>");

	if (fgets(line,256,stdin)==NULL) continue;

	if (strcmp(line,"\n")==0) continue;

	line[strlen(line)-1]='\0';

	        // Tokenize the input
        int argc = 0;
        char *token = strtok(line, " ");
        while (token != NULL && argc < 20) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        // Skip if no command
        if (argv[0] == NULL)
            continue;

        // Check if allowed
        if (!isAllowed(argv[0])) {
            printf("NOT ALLOWED!\n");
            continue;
        }

        // Built-in: exit
        if (strcmp(argv[0], "exit") == 0) {
            return 0;
        }

        // Built-in: help
        else if (strcmp(argv[0], "help") == 0) {
            printf("The allowed commands are:\n");
            for (int i = 0; i < N; i++) {
                printf("%d: %s\n", i + 1, allowed[i]);
            }
            continue;
        }

        // Built-in: cd
        else if (strcmp(argv[0], "cd") == 0) {
            if (argc > 2) {
                printf("-rsh: cd: too many arguments\n");
            } else if (argc == 2 && chdir(argv[1]) != 0) {
                perror("cd failed");
            }
            continue;
        }

        // Spawn external command
        if (posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ) != 0) {
            printf("NOT ALLOWED!\n");
            continue;
        }

        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
        }
    }
// Add code to spawn processes for the first 9 commands
	// And add code to execute cd, exit, help commands
	// Use the example provided in myspawn.c

    return 0;
}
