#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
int main() {
    pid_t bll;
    int stat;
    if ((bll = fork()) == 0)
        printf("1\n");
    else {
        wait(&stat);
        printf("2\n");
    }
}
