#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>

bool changeUser(char *userName, char *groupName) {
    struct passwd *p = getpwnam(userName);
    struct group *group = getgrnam(groupName);

    if (p != NULL && group != NULL) {
        if (setgid(group->gr_gid) == 0 && setuid(p->pw_uid) == 0) {
            printf("Everything is cool!\n");
            return true;
        } else {
            printf("It seems that you don't have the rights to run this program! Please run it as root\n");
        }
    } else if (p == NULL) {
        printf("Something is wrong with the username\n");
    } else {
        printf("Something is wrong with the groupname\n");
    }

    return false;
}

void printdir(char *dir, int depth) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);

    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
            printf("%*s%s/\n", depth, "", entry->d_name);
            
            printdir(entry->d_name, depth + 4);
        } else {
            printf("%*s%s\n", depth, " ", entry->d_name);
        }
    }
    chdir("..");
    closedir(dp);
}

int main(int argc, char* argv[]) {

    char *userName = "";
    char *groupName = "";
    char *topdir = ".";

    userName = argv[1];
    groupName = argv[2];
    topdir = argv[3];

    changeUser(userName, groupName);

    printf("Directory scan of %s\n", topdir);
    printdir(topdir, 0);
    printf("done.\n");
    exit(0);
   }