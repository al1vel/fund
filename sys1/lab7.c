#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

pthread_mutex_t mt;

void print_permissions(mode_t mode) {
    char perms[11] = "----------";

    if (S_ISDIR(mode)) perms[0] = 'd';
    if (S_ISLNK(mode)) perms[0] = 'l';
    if (S_ISCHR(mode)) perms[0] = 'c';
    if (S_ISBLK(mode)) perms[0] = 'b';
    if (S_ISFIFO(mode)) perms[0] = 'p';
    if (S_ISSOCK(mode)) perms[0] = 's';

    if (mode & S_IRUSR) perms[1] = 'r';
    if (mode & S_IWUSR) perms[2] = 'w';
    if (mode & S_IXUSR) perms[3] = 'x';
    if (mode & S_IRGRP) perms[4] = 'r';
    if (mode & S_IWGRP) perms[5] = 'w';
    if (mode & S_IXGRP) perms[6] = 'x';
    if (mode & S_IROTH) perms[7] = 'r';
    if (mode & S_IWOTH) perms[8] = 'w';
    if (mode & S_IXOTH) perms[9] = 'x';

    printf("%s ", perms);
}

void* cout_content_detailed(void *arg) {
    pthread_mutex_lock(&mt);

    char* dirname = (char*)arg;
    DIR *dir = opendir(dirname);
    if (!dir) {
        pthread_mutex_unlock(&mt);
        return NULL;
    }

    printf("\nDirectory listing for %s:\n", dirname);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (entry->d_name[0] == '.') {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        struct stat st;
        if (lstat(path, &st) == -1) {
            continue;
        }

        print_permissions(st.st_mode);
        printf("%2lu ", st.st_nlink);

        struct passwd *pw = getpwuid(st.st_uid);
        struct group  *gr = getgrgid(st.st_gid);
        printf("%s %s ", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");

        printf("%8ld ", st.st_size);

        char timebuf[64];
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
        printf("%s ", timebuf);

        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    pthread_mutex_unlock(&mt);
    return (void*)pthread_self();
}

void* cout_content_detailed_hidden(void *arg) {
    pthread_mutex_lock(&mt);

    char* dirname = (char*)arg;
    DIR *dir = opendir(dirname);
    if (!dir) {
        pthread_mutex_unlock(&mt);
        return NULL;
    }

    printf("\nDirectory listing for %s:\n", dirname);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        struct stat st;
        if (lstat(path, &st) == -1) {
            continue;
        }

        print_permissions(st.st_mode);
        printf("%2lu ", st.st_nlink);

        struct passwd *pw = getpwuid(st.st_uid);
        struct group  *gr = getgrgid(st.st_gid);
        printf("%s %s ", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");

        printf("%8ld ", st.st_size);

        char timebuf[64];
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
        printf("%s ", timebuf);

        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    pthread_mutex_unlock(&mt);
    return (void*)pthread_self();
}

void* cout_content_normal(void *arg) {
    pthread_mutex_lock(&mt);
    char* name = (char*)arg;
    DIR *dir;
    struct dirent *entry = NULL;

    if (!((dir = opendir(name)))) {
        pthread_mutex_unlock(&mt);
        return NULL;
    }

    printf("\nDirectory %s contains:\n", name);

    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (entry->d_name[0] == '.') {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
        printf("- %s\n", entry->d_name);
    }

    closedir(dir);
    pthread_mutex_unlock(&mt);
    return (void*)pthread_self();
}

void* cout_content_hidden(void *arg) {
    pthread_mutex_lock(&mt);
    char* name = (char*)arg;
    DIR *dir;
    struct dirent *entry = NULL;

    if (!((dir = opendir(name)))) {
        pthread_mutex_unlock(&mt);
        return NULL;
    }

    printf("\nDirectory %s contains:\n", name);

    while ((entry = readdir(dir)) != NULL) {
        printf("- %s\n", entry->d_name);
    }

    closedir(dir);
    pthread_mutex_unlock(&mt);
    return (void*)pthread_self();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("No path presented.\n");
        return -1;
    }

    size_t paths_cnt = argc - 1;
    int flag_type = 0;

    if (strcmp(argv[argc - 1], "-l") == 0) {
        --paths_cnt;
        flag_type = 1;

    } else if (strcmp(argv[argc - 1], "-a") == 0) {
        flag_type = 2;
        --paths_cnt;

    } else if ((strcmp(argv[argc - 1], "-la") == 0) || (strcmp(argv[argc - 1], "-al") == 0)) {
        flag_type = 3;
        --paths_cnt;
    }

    pthread_mutex_init(&mt, NULL);

    pthread_t *thr_ids = (pthread_t *)malloc(paths_cnt * sizeof(pthread_t));
    for (int i = 0; i < paths_cnt; ++i) {
        pthread_t thr_id = 0;

        if (flag_type == 0) {
            if (pthread_create(&thr_id, NULL, &cout_content_normal, (void*)argv[i + 1]) != 0) {
                printf("Thread creation error\n");
                continue;
            }
            thr_ids[i] = thr_id;

        } else if (flag_type == 1) {
            if (pthread_create(&thr_id, NULL, &cout_content_detailed, (void*)argv[i + 1]) != 0) {
                printf("Thread creation error\n");
                continue;
            }
            thr_ids[i] = thr_id;

        } else if (flag_type == 2) {
            if (pthread_create(&thr_id, NULL, &cout_content_hidden, (void*)argv[i + 1]) != 0) {
                printf("Thread creation error\n");
                continue;
            }
            thr_ids[i] = thr_id;

        } else if (flag_type == 3) {
            if (pthread_create(&thr_id, NULL, &cout_content_detailed_hidden, (void*)argv[i + 1]) != 0) {
                printf("Thread creation error\n");
                continue;
            }
            thr_ids[i] = thr_id;
        }

    }

    for (int i = 0; i < paths_cnt; ++i) {
        pthread_t joined_thr_id = 0;
        if (pthread_join(thr_ids[i], (void*)&joined_thr_id) != 0) {
            printf("Error on joining %lu thread", thr_ids[i]);
        }
    }
    free(thr_ids);
    return 0;
}