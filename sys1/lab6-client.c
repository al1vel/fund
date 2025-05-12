#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

int nl_pos(const char *str) {
    int pos = 0;
    for (int i = 0; i < 256; ++i) {
        if (str[i] == '\n') {
            return pos;
        }
        ++pos;
    }
    return -1;
}

// int validate_path(char *str) {
//     int pos = 0;
//     if (str[pos] == '\0') {
//         return -1;
//     }
//     ++pos;
//     if (str[pos] == '/') {
//         return -1;
//     }
//     ++pos;
//
//
// }

#define SHM_SIZE 4096
#define SHM_KEY 1234

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
    int shm_id = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    struct packet* shm_ptr = (struct packet*)shmat(shm_id, NULL, 0);
    if (shm_ptr == (struct packet*)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    char command[256];
    printf("Type full path to get its content.\nType quit to close.\n\n");
    while (1) {
        if (fgets(command, sizeof(command), stdin) != NULL) {
            command[nl_pos(command)] = '\0';

            if (strcmp(command, "quit") == 0) {
                printf("Exiting program...\n");
                shm_ptr->who_talks = QUIT;
                break;
            }
            printf("Path: %s\n", command);

            snprintf(shm_ptr->text, sizeof(shm_ptr->text), "%s", command);
            shm_ptr->who_talks = SERVER;
            shm_ptr->client_wait = WAIT;
            printf("<Client>: sent!\n");

            while (shm_ptr->who_talks == SERVER) {
                while (1) {
                    sleep(1);
                    if (shm_ptr->client_wait == GO_GET) {
                        break;
                    }
                }
                printf("- %s\n", shm_ptr->text);
            }

        } else {
            printf("Error reading input!\n");
            break;
        }
    }

    if (shmdt(shm_ptr) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    printf("Client process finished.\n");
    return 0;
}