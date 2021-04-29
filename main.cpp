#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <unistd.h>

char *userName;
char *groupName;

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

bool isOwner(struct dirent *entry, char *userName) {
    struct passwd *userPwd = getpwnam(userName);

    struct stat statbuf;
    lstat(entry->d_name, &statbuf);

    struct passwd *entryPwd = getpwuid(statbuf.st_uid);

    if (userPwd->pw_uid == entryPwd->pw_uid) {
        return true;
    } else {
        return false;
    }
}

bool isRoot(char *userName) {
    struct passwd *userPwd = getpwnam(userName);
    struct passwd *rootPwd = getpwnam("root");


}

bool isInGroup(struct dirent *entry, char *groupName) {
    struct group *group = getgrnam(groupName);
    struct stat statbuf;
    lstat(entry->d_name, &statbuf);

    if (group->gr_gid == statbuf.st_gid) {
        return true;
    } else {
        return false;
    }
}

bool hasWritePermission(struct dirent *entry, char *userName, char *groupName) {
    struct stat statbuf;
    lstat(entry->d_name, &statbuf);

    if (isOwner(entry, userName)) {
        if (S_IWUSR & statbuf.st_mode) {
            return true;
        }
    } else if (isInGroup(entry, groupName)) {
        if (S_IWGRP & statbuf.st_mode) {
            return true;
        }
    } else if (geteuid() == 0) {
        return true;
    } else {
        if (S_IWOTH	& statbuf.st_mode) {
            return true;
        }
    }

    return false;
}

void checkDir(char *dir, int depth) {
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
            
            checkDir(entry->d_name, depth + 4);
        } else {
            if (hasWritePermission(entry, userName, groupName)) {
                printf("%*s f %s\n", depth, " ", entry->d_name);
            }
        }
    }
    chdir("..");
    closedir(dp);
}

int main(int argc, char* argv[]) {

    char *topdir = ".";

    userName = argv[1];
    groupName = argv[2];
    topdir = argv[3];

    if (changeUser(userName, groupName)) {
        printf("Directory scan of %s\n", topdir);
        checkDir(topdir, 0);
        printf("done.\n");
    }

    exit(0);
}