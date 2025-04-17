#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12

extern char **environ;

char *allowed[N] = {
    "cp", "touch", "mkdir", "ls", "pwd", "cat",
    "grep", "chmod", "diff", "cd", "exit", "help"
};

int isAllowed(const char *cmd) {
    for (int i = 0; i < N; i++) {
        if (strcmp(cmd, allowed[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int main() {
    char line[256];
    char *argv[21]; // max 20 args + NULL
    pid_t pid;
    int status;

    while (1) {
        fprintf(stderr, "rsh>");

        // Get a line of input
        if (fgets(line, sizeof(line), stdin) == NULL)
            continue;

        // Skip empty input
        if (strcmp(line, "\n") == 0)
            continue;

        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        // Tokenize the input
        int argc = 0;
        char *token = strtok(line, " ");
        while (token != NULL && argc < 20) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        if (argv[0] == NULL)
            continue;

        // Check if allowed
        if (!isAllowed(argv[0])) {
            printf("NOT ALLOWED!\n");
            continue;
        }

        // Built-in commands
        if (strcmp(argv[0], "exit") == 0) {
            return 0;
        } else if (strcmp(argv[0], "help") == 0) {
            printf("The allowed commands are:\n");
            for (int i = 0; i < N; i++) {
                printf("%d: %s\n", i + 1, allowed[i]);
            }
            continue;
        } else if (strcmp(argv[0], "cd") == 0) {
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

    return 0;
}

