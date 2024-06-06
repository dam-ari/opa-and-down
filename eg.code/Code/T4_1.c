#include <sys/stat.h>          
#include <unistd.h>
#include <stdio.h>

int main (int argc, char* argv[])
{
	char* filename = argv[1];
	char msg[64];
	 
	struct stat stat_p;
	 
	if(stat(filename, &stat_p) == -1) 
	{
		sprintf(msg," Error occurred attempting to stat %s\n", filename); // printf
		perror(msg);
		return 1;
	}
	 
	printf("Stats for %s \n", filename);
	printf("File size is %ld bytes\n", stat_p.st_size);
	 
	if(S_ISREG(stat_p.st_mode))
		printf("This is a regular file\n");
	 
	if(S_ISDIR(stat_p.st_mode))
		printf("This is a directory\n");
	 
	return 0;
}
