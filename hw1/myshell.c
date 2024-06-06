#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PROMPT "💲 "
#define TERMINATE_STR '\0'
#define EXIT "exit"
#define HISTORY "history"
#define CD "cd"

typedef enum { false, true } bool;
char* logCmd[100];
int excRun = 0;

char** splitStringToArray(char* str) {
    char** res = NULL;
    char* p = strtok(str, " ");
    int n_spaces = 0;

    /* split string and append tokens to 'res' */

    while (p) {
        res = realloc(res, sizeof(char*) * ++n_spaces);

        if (res == NULL) exit(-1); /* memory allocation failed */

        res[n_spaces - 1] = p;

        p = strtok(NULL, " ");
    }

    /* realloc one extra element for the last NULL */

    res = realloc(res, sizeof(char*) * (n_spaces + 1));
    res[n_spaces] = 0;
    return res;
}

/**
 * Remove trailing white space characters from string
 */
void trimTrailing(char* str) {
    int index, i;

    /* Set default index */
    index = -1;

    /* Find last index of non-white space character */
    i = 0;
    while (str[i] != '\0') {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n') {
            index = i;
        }

        i++;
    }

    /* Mark next character to last non-white space character as NULL */
    str[index + 1] = '\0';
}

void printHIstory() {
    int i;
    for (i = 0; i <= excRun; ++i) {
        printf(" %s\n", logCmd[i]);
    }
}

void logHistory(int pid, char* a) {
    char entry[100] = "";
    sprintf(entry, "%d ", pid);
    strcat(entry, a);
    logCmd[excRun] = malloc(100);
    strcpy(logCmd[excRun], entry);
}

int main(int argc, char const* argv[]) {
    pid_t pid = getpid();
    char envs[100] = "";
    int i;
    for (i = 1; i < argc; ++i) {
        strcat(strcat(envs, ":"), argv[i]);
    }
    char* allenv = getenv("PATH");
    strcat(allenv, envs);
    setenv("PATH", allenv, 0);
    char a[100];
    char* token;

    while (1) {
        char* str = malloc(100);
        printf(PROMPT);
        fflush(stdout);
        fgets(a, sizeof(a), stdin);
        trimTrailing(a);
        strcpy(str, a);

        if (!strcmp(a, EXIT)) {
            int i;
            for (i = 0; i < excRun; i++) {
                char* currentPtr = logCmd[i];
                free(currentPtr);
            }
            exit(1);
        }

        token = strtok(a, " ");
        if (token == NULL) {
            logHistory(pid, a);
        } else if (!strcmp(token, HISTORY)) {
            logHistory(pid, a);
            printHIstory();
        } else if (!strcmp(token, CD)) {
            logHistory(pid, str);

            token = a + strlen(token) + 1;
            int res = chdir(token);
            if (res != 0) {
                perror(strcat(token, " failed"));
            }

        } else {
            pid_t child = fork();
            int stat;
            logHistory(child, str);
            if (child == 0) {
                // split params
                char** argv = splitStringToArray(str);
                int res = execvp(token, argv);
                if (res != 0) {
                    perror(strcat(token, " failed"));
                }
                break;
            } else {
                // wait for child to finish execute
                wait(&stat);
            }
        }
        if (str != NULL) {
            free(str);
        }

        excRun++;
    }
    return 0;
}
