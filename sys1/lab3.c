#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define OK

#define NUM 5
pthread_mutex_t forks[NUM];

#ifdef OK
sem_t room;
#endif

void* philosopher(void* num) {
    int id = *(int*)num;
    int left = id;
    int right = (id + 1) % NUM;

    sleep(1);

    printf("Philosopher %d thinks...\n", id);

#ifdef OK
    sem_wait(&room);
#endif

    printf("Philosopher %d tries to take left fork (%d)...\n", id, left);
    pthread_mutex_lock(&forks[left]);
    printf("Philosopher %d took left fork (%d).\n", id, left);

    usleep(100000);

    printf("Philosopher %d tries to take right fork (%d)...\n", id, right);
    pthread_mutex_lock(&forks[right]);
    printf("Philosopher %d took right fork (%d).\n", id, right);

    printf("Philosopher %d eats.\n", id);
    sleep(2);

    pthread_mutex_unlock(&forks[right]);
    pthread_mutex_unlock(&forks[left]);
    printf("Philosopher %d put down forks and thinks again.\n", id);

#ifdef OK
    sem_post(&room);
#endif

    return NULL;
}

int main() {
    pthread_t threads[NUM];
    int ids[NUM];

    for (int i = 0; i < NUM; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

#ifdef OK
    sem_init(&room, 0, NUM - 1);
#endif

    for (int i = 0; i < NUM; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < NUM; i++) {
        pthread_join(threads[i], NULL);
    }

#ifdef OK
    sem_destroy(&room);
#endif

    return 0;
}