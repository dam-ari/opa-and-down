//To compile: gcc -l pthread t6_4_New.c
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
pthread_t ntid;

void* thr_fn(void *arg)
{
    int i,x;
    x=fork();
    if(x>0){ // run this code twice: with > and with  ==
		return((void *)0);
    }	

    for(i=0;i<3;i++)
    {
		printf("my tid is %lu, my pid is: %d, the loop value is: %d\n",pthread_self(),getpid(),i);
		sleep(1);
    }
    
    return((void *)0);
}

int main(void)
{
    int     err;
    void *retVal;
    int i;
    err = pthread_create(&ntid, NULL, thr_fn, NULL);
    if (err != 0)
	perror("can't create thread\n");
    
    pthread_join(ntid, &retVal);
    printf("returned\n");
    pthread_exit((void*)0);
}
