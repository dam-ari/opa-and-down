#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

enum resemblanceStatus {
    identical = 1,  // identical files
    different = 2,  // different files
    similar = 3,    // similar files
};

int checkSimilarity(int fd1, int fd2, char chr1, char chr2, int* keepRead);

// This function closes the file descriptor and set iteration to false
void handleExit(int fd1, int fd2, int* keepRead, int isErr) {
    if (isErr) {
        perror("Error occurred");
    }
    if (fd1 != -1) {
        close(fd1);
    }
    if (fd2 != -1) {
        close(fd2);
    }
    *keepRead = 0;
}

int handleSpaces(int fd1, int fd2, char chr1, char chr2, int* keepRead) {
    int isSpace, byteRes, res = 0;
    if (isspace(chr1)) {
        byteRes = read(fd1, &chr1, 1);
        if (byteRes == -1) {
            // close files and exit with error
            handleExit(fd1, fd2, keepRead, 1);
            *keepRead = 0;
            res = different;
        }
        isSpace = 1;
    }

    if (isspace(chr2)) {
        byteRes = read(fd2, &chr2, 1);
        if (byteRes == -1) {
            // close files and exit with error
            handleExit(fd1, fd2, keepRead, 1);
            exit(1);
            // *keepRead = 0;
            // res = different;
        }
        isSpace = 1;
    }
    if (isSpace) {
        if (chr1 != chr2) {
            res = handleSpaces(fd1, fd2, chr1, chr2, keepRead);
        } else {
            res = similar;
        }
    }
    return res;  // return resemblance status
}

int checkSimilarity(int fd1, int fd2, char chr1, char chr2, int* keepRead) {
    int res;
    if (tolower(chr1) != tolower(chr2) && !isspace(chr1) && !isspace(chr2)) {
        handleExit(fd1, fd2, keepRead, 0);
        res = different;
    } else {
        res = handleSpaces(fd1, fd2, chr1, chr2, keepRead);
    }
    return res;
}

int checkLeft(int left, char lastChr, int* keepRead) {
    int byteRes, dirty = 0;
    int res;
    char chr;
    while (keepRead) {
        byteRes = read(left, &chr, 1);
        if (byteRes == -1) {
            // close files and exit with error
            handleExit(left, -1, keepRead, 1);
            *keepRead = 0;
            return different;
        }

        if (byteRes == 0) {
            *keepRead = 0;
            break;
        }
        if (!isspace(chr)) {
            if (chr != lastChr || dirty) {
                return different;
            } else {
                dirty = 1;
                res = similar;
            }
        }
    }
    if ((!isspace(chr) && tolower(lastChr) == tolower(chr)) ||
        (isspace(chr) && isspace(lastChr)) || (isspace(lastChr) && !dirty)) {
        res = similar;
    } else
        res = different;

    return res;
}

int startsWith(const char* a, const char* b) {
    if (strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}

void getFileName(char* path) {
    if (!startsWith(path, "/")) {
        char fullpath[1024] = "";
        if (getcwd(fullpath, sizeof(fullpath)) != NULL) {
            // printf("%s/%s\n", fullpath, path);
            strcat(fullpath, path);
            strcpy(fullpath, path);
        }
    }
}

// compare two files and return resemblance status
// if the files content is exactly the same, return identical (1)
// else, if the files content is different in spaces, capitals, or line break,
// return similar (3)
// otherwise, return different (2)
int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Error: not enough arguments\n");
        return 1;
    }
    // retrive file names
    getFileName(argv[1]);
    char* file1 = argv[1];
    if (file1 == NULL) return 1;
    getFileName(argv[2]);
    char* file2 = argv[2];
    if (file2 == NULL) return 1;

    int fd1, fd2;
    fd1 = open(file1, O_RDONLY);
    if (fd1 == -1) {
        printf("Error opening file\n");
        return 1;
    }
    fd2 = open(file2, O_RDONLY);
    if (fd2 == -1) {
        printf("Error opening file\n");
        close(fd1);
        return 1;
    }

    enum resemblanceStatus res = 0;
    char chr1, chr2;
    int byteRes1, byteRes2;
    int keepRead = 1;

    // reading the files char by char
    while (keepRead) {
        byteRes1 = read(fd1, &chr1, 1);
        // if byteRes == -1 then error
        if (byteRes1 == -1) {
            handleExit(fd1, fd2, &keepRead, byteRes1 == -1);
            exit(1);
            break;
        }
        byteRes2 = read(fd2, &chr2, 1);
        // if byteRes == -1 then error
        if (byteRes2 == -1) {
            handleExit(fd1, fd2, &keepRead, byteRes2 == -1);
            exit(1);
            break;
        }
        // if byteRes == 0 then end of file
        if (byteRes1 == 0 && byteRes2 == 0) {
            res = !res ? identical : res;
            handleExit(fd1, fd2, &keepRead, 0);

            break;
        }
        if ((byteRes1 == 0 && isspace(chr2)) ||
            (byteRes2 == 0 && isspace(chr1))) {
            res = different;
            int left = byteRes1 == 0 ? fd2 : fd1;
            char lastChr = byteRes1 == 0 ? chr2 : chr1;
            res = checkLeft(left, lastChr, &keepRead);
            handleExit(fd1, fd2, &keepRead, 0);
            break;
        }
        if (byteRes1 == 0 || byteRes2 == 0) {
            res = different;
            handleExit(fd1, fd2, &keepRead, 0);
            break;
        }
        if (chr1 != chr2) {
            if (tolower(chr1) == tolower(chr2)) {
                res = similar;
                // break;
            } else
                res = checkSimilarity(fd1, fd2, chr1, chr2, &keepRead);
        }
    }
    if (fd1 != -1) {
        close(fd1);
    }
    if (fd2 != -1) {
        close(fd2);
    }
    return res;
}
