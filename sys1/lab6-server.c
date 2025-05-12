#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

#define SHM_SIZE 4096
#define SHM_KEY 1234

size_t how_many_files(char* path) {
    size_t num_files = 0;
    DIR *dir;
    struct dirent *entry = NULL;

    if (!((dir = opendir(path)))) {
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (entry->d_name[0] == '.') {
            continue;
        }
        num_files++;
    }
    closedir(dir);
    return num_files;
}

void write_needed_path(char* path, size_t pos, char* buffer) {
    DIR *dir;
    struct dirent *entry = NULL;

    dir = opendir(buffer);

    int i = 0;
    while (i != pos) {
        entry = readdir(dir);
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            --i;
        }
        if (entry->d_name[0] == '.') {
            --i;
        }
        ++i;
    }
    entry = readdir(dir);
    strcpy(path, entry->d_name);
}

enum {
    SERVER,
    CLIENT,
    GO_GET,
    WAIT,
    QUIT
};

struct packet {
    int who_talks;
    int client_wait;
    char text[2048];
};

int main() {
    int shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    struct packet* shm_ptr = (struct packet*)shmat(shm_id, NULL, 0);
    if (shm_ptr == (struct packet*)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    while (shm_ptr->who_talks != QUIT) {
        while (shm_ptr->who_talks == CLIENT) {
            printf("Server waits\n");
            sleep(1);
        }

        char path[2048];
        strcpy(path, shm_ptr->text);
        printf("[Server]: got -  <%s>\n", path);

        size_t n = how_many_files(path);
        for (size_t i = 0; i < n; i++) {
            write_needed_path(shm_ptr->text, i, path);
            shm_ptr->client_wait = GO_GET;
            while (shm_ptr->client_wait != WAIT) {
                //sleep(1);
            }
        }
        shm_ptr->who_talks = CLIENT;
    }

    if (shmdt(shm_ptr) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    printf("Server process finished.\n");
    return 0;
}