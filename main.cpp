#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

void printdir(char *dir, int depth) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    entry = readdir(dp);
    while (entry != NULL) {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            /* Находит каталог, но игнорирует . и .. */
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
            printf("%*s%s/\n", depth, "", entry->d_name);
            /* Рекурсивный вызов с новый отступом */
            printdir(entry->d_name, depth + 4);
        } else {
            printf("%*s%s\n", depth, " ", entry->d_name);
        }

        entry = readdir(dp);
    }
    chdir("..");
    closedir(dp);
}

int main(int argc, char* argv[]) {
    char *user = "";
    char *group = "";
    char *topdir = ".";

    if (argc >= 2) {
        user = argv[1];
        group = argv[2];
        topdir = argv[3];
        printf("%s %s %s", user, group, topdir);
    }
    printf("Directory scan of %s\n", topdir);
    printdir(topdir, 0);
    printf("done.\n");
    exit(0);
   }