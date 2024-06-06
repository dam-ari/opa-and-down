// 315394874 Bareket Damari
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define CONFIG_FILES_MISSING "file not exist"

#define COMPILE_C "/usr/bin/gcc"
#define COMPILED_PNAME "output"
#define RUNNABLE "./output"
#define GCC_ERROR_FILE "errors.txt"
#define OUTPUT_FILE "user_output.txt"
#define TIMEOUT 5
#define COMPARE "./comp.out"
#define LAST_RES "/usr/bin/echo"
// #define ERRORS "errors.txt"
#define RES(CHR) "RES" CHR ".txt"

char* getFullPath(char* path, char* fileName) {
    char* fullPath = malloc(strlen(path) + strlen(fileName) + 2);
    strcpy(fullPath, path);
    strcat(fullPath, "/");
    strcat(fullPath, fileName);
    return fullPath;
}

int startsWith(const char* a, const char* b) {
    if (strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}
void getFileName(char* path) {
    if (!startsWith(path, "/")) {
        char fullpath[1024] = "";
        if (getcwd(fullpath, sizeof(fullpath)) != NULL) {
            strcat(strcat(fullpath, "/"), path);
            strcpy(path, fullpath);
        }
    }
}

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

void freeConfigOnExit(char* config[3]) {
    int i;
    for (i = 0; i < 3; i++) {
        if (config[i] != NULL) {
            free(config[i]);
        }
    }
}

void extractConfig(char* configFile, char* config[3]) {
    // char* config[3] = {NULL, NULL, NULL};
    char* line_buf = NULL;
    size_t line_buf_size = 0;
    int line_count = 0;
    size_t line_size;
    FILE* fp = fopen(configFile, "r");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    /* Get the first line of the file. */
    line_size = getline(&line_buf, &line_buf_size, fp);

    while (line_size > 0 && line_count < 3) {
        /* Increment our line count */
        line_count++;
        if (line_buf != NULL) getFileName(line_buf);

        config[line_count - 1] = malloc(sizeof(char) * (strlen(line_buf) + 1));
        strcpy(config[line_count - 1], line_buf);
        trimTrailing(config[line_count - 1]);
        // VALIDATE PATH EXISTS
        int fd = access(config[line_count - 1], F_OK);
        if (fd == -1) {
            if (line_count == 2)
                printf("Input %s", CONFIG_FILES_MISSING);
            else
                printf("Output %s", CONFIG_FILES_MISSING);

            exit(EXIT_FAILURE);
        }
        /* Get the next line */
        line_size = getline(&line_buf, &line_buf_size, fp);
    }
    /* Free the allocated line buffer */
    if (line_buf != NULL) free(line_buf);
    line_buf = NULL;

    /* Close the file now that we are done with it */
    fclose(fp);
    // return config;
}

char* gradeMap(char* name, int gradeIndex) {
    char* grade = NULL;

    switch (gradeIndex) {
        case 1:
            grade = ",0,NO_C_FILE\n";
            break;
        case 2:
            grade = ",10,COMPILATION_ERROR\n";
            break;
        case 3:
            grade = ",20,TIMEOUT\n";
            break;
        case 4:
            grade = ",50,WRONG\n";
            break;
        case 5:
            grade = ",75,SIMILAR\n";
            break;
        case 6:
            grade = ",100,EXCELLENT\n";
            break;
        default:
            grade = "?\n";
    }
    char* record = malloc(sizeof(char) * strlen(name) + strlen(grade) + 2);
    strcpy(record, name);
    strcat(record, grade);
    return record;
}

int compileFile(char* fileName) {
    // Compile the file
    int isCompiled = 0;
    pid_t pid;
    int st;
    if ((pid = fork()) < 0)
        perror("fork");
    else {
        if (pid == 0) {
            char* args[] = {COMPILE_C, "-o", COMPILED_PNAME, fileName, NULL};
            int res = execvp(COMPILE_C, args);
            if (res != 0) {
                perror("execvp");
            }
            exit(0);
        }
        wait(&st);
        if (WIFEXITED(st)) {
            st = WEXITSTATUS(st);
        }

        if (st == 0)
            isCompiled = 1;
        else
            isCompiled = 0;

        return isCompiled;
    }
    return -1;
}

int runCompiledProgWithFile(char* inputFile) {
    pid_t pid = fork();
    time_t start = time(NULL);
    int runTime;
    int st;
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else {
        if (pid == 0) {
            // replace standard input with input file
            int in_fd = open(inputFile, O_RDONLY);
            dup2(in_fd, 0);
            // replace standard output with output file
            int out_fd = open(OUTPUT_FILE, O_WRONLY | O_TRUNC | O_CREAT,
                              S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
            dup2(out_fd, 1);
            close(in_fd);
            close(out_fd);
            char* args[] = {RUNNABLE, NULL};
            int res = execvp(RUNNABLE, args);
            if (res != 0) {
                perror("execvp");
            }
            exit(0);
        }
        waitpid(pid, &st, 0);
        runTime = time(NULL) - start;

        if (runTime >= TIMEOUT)
            st = 3;
        else
            st = 4;
    }

    return st;
}

int comare(char* outputFile) {
    pid_t pid = fork();
    int st, res;
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else {
        if (pid == 0) {
            char* args[] = {COMPARE, outputFile, OUTPUT_FILE, NULL};
            execvp(COMPARE, args);
            exit(0);

        } else {
            wait(&st);
            if (WIFEXITED(st)) {
                st = WEXITSTATUS(st);
            }
            if (st == 1)
                res = 6;
            else if (st == 2)
                res = 4;
            else if (st == 3)
                res = 5;
        }

        // if output file exists remove it
        int fd = access(OUTPUT_FILE, F_OK);
        if (fd != -1) remove(OUTPUT_FILE);
    }

    return res;
}

int calcUserGrade(char* userDir, char* inputFile, char* outputFile) {
    DIR* dip;
    struct dirent* entry;

    // open users directory
    dip = opendir(userDir);
    if (dip == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    int grade = 0;
    int fileToCompile = 0;
    while ((entry = readdir(dip)) != NULL && !fileToCompile) {
        const char* dot = strrchr(entry->d_name, '.');
        if (dot && !strcmp(dot + 1, "c")) {
            // ⚒️ build the c program inside the directory
            fileToCompile = 1;

            char* filePath = getFullPath(userDir, entry->d_name);

            int isCompiled = compileFile(filePath);
            if (isCompiled) {
                // ⚒️ run the c program with the input file
                grade = runCompiledProgWithFile(inputFile);
                if (grade != 3) {
                    // ⚒️ compare the output file with the output file
                    // in the directory
                    grade = comare(outputFile);
                }

            } else
                grade = 2;

            if (filePath != NULL) free(filePath);
        }
    }

    if (!fileToCompile) grade = 1;

    closedir(dip);
    remove(COMPILED_PNAME);
    // a function to delete the output file

    // ⚒️ calculate the grade: 1-6
    // ⚒️ clear exe & output files
    // ⚒️ return the grade
    return grade;
}

void logGrade(int fd, char* record) {
    int res = write(fd, record, strlen(record));
    if (res < 0) {
        perror("write");
        close(fd);
        exit(EXIT_FAILURE);
    }
}

void handleError(char* config[3], int resFd) {
    freeConfigOnExit(config);
    close(resFd);
    exit(EXIT_FAILURE);
}

void grade(char* config[3], int resFd) {
    DIR* dip;
    struct dirent* dit;
    // open users directory
    dip = opendir(config[0]);
    if (dip == NULL) {
        perror("opendir");
        handleError(config, resFd);
    }

    // int pathlen = strlen(config[0]);
    // loop through users directory
    while ((dit = readdir(dip)) != NULL) {
        struct stat stat_p;
        char* userFolder = getFullPath(config[0], dit->d_name);
        stat(userFolder, &stat_p);
        if (S_ISDIR(stat_p.st_mode) && strcmp(dit->d_name, ".") != 0 &&
            strcmp(dit->d_name, "..") != 0) {
            int grade = calcUserGrade(userFolder, config[1], config[2]);

            char* record = gradeMap(dit->d_name, grade);
            logGrade(resFd, record);

            free(record);
            free(userFolder);
        }
    }
    // close users directory
    if (closedir(dip) == -1) {
        perror("closedir");
        handleError(config, resFd);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Error: not enough arguments\n");
        return 1;
    }
    int gccerr_fd = open(GCC_ERROR_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    dup2(gccerr_fd, 2);
    close(gccerr_fd);

    char* configFile = argv[1];
    char* config[3] = {NULL, NULL, NULL};

    extractConfig(configFile, config);

    int newFd = open("results.csv", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (newFd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    // open user folder (config[0])
    // for each subfolder: fork and compile the only c file in there
    // if compilation is successful, run the executable on child process with
    // the input file (config[1]) and compare the output with the expected
    // output (config[2]) on another fork

    grade(config, newFd);

    close(newFd);
    freeConfigOnExit(config);
    exit(EXIT_SUCCESS);
}
