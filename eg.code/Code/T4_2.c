#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
   struct stat sb;

   if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

   if (stat(argv[1], &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

   printf("File type: ");

    if(S_ISBLK(sb.st_mode))
    	printf("block device\n");     
    else if(S_ISCHR(sb.st_mode)) 
    	printf("character device\n");  
    else if(S_ISDIR(sb.st_mode))
    	printf("directory\n");
    else if(S_ISFIFO(sb.st_mode))
    	printf("FIFO/pipe\n");              
    else if(S_ISLNK(sb.st_mode))
    	printf("symlink\n");   	
    else if(S_ISREG(sb.st_mode))
    	printf("regular file\n");          	
    else if(S_ISSOCK(sb.st_mode))
    	printf("socket\n");
    else 
    	printf ("unknown\n");
    

   printf("I-node number:            %ld\n", (long) sb.st_ino);

   printf("Mode:                     %lo (octal)\n", (unsigned long) sb.st_mode);

   printf("Link count:               %ld\n", (long) sb.st_nlink);
   printf("Ownership:                UID=%ld   GID=%ld\n",(long) sb.st_uid, (long) sb.st_gid);

   printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
   printf("File size:                %lld bytes\n", (long long) sb.st_size);
   printf("Blocks allocated:         %lld\n",(long long) sb.st_blocks);

   printf("Last status change:       %s", ctime(&sb.st_ctime));
   printf("Last file access:         %s", ctime(&sb.st_atime));
   printf("Last file modification:   %s", ctime(&sb.st_mtime));

   exit(EXIT_SUCCESS);
}
