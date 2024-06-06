#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int pid = fork();

    if (pid == -1) {
        perror("fork() failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execlp("./child", "child", "1", "5", (char *)NULL);
        perror("execlp() failed");
        exit(EXIT_FAILURE);
    } else {
        printf("Coordinator: forked and waiting for process %d\n", pid);

        int status;
        if (waitpid(pid, &status, 0) != -1) {
            if (WIFEXITED(status)) {
                int returned = WEXITSTATUS(status);
                printf("Exited normally with status %d\n", returned);
            } else if (WIFSIGNALED(status)) {
                int signum = WTERMSIG(status);
                printf("Exited due to receiving signal %d\n", signum);
            } else if (WIFSTOPPED(status)) {
                int signum = WSTOPSIG(status);
                printf("Stopped due to receiving signal %d\n", signum);
            } else {
                printf("Something strange just happened.\n");
            }
        } else {
            perror("waitpid() failed");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}