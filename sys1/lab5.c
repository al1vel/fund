#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_CAPACITY 3
#define NUM_WOMEN 5
#define NUM_MEN 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t can_enter = PTHREAD_COND_INITIALIZER;

int num_women = 0;
int num_men = 0;

void woman_wants_to_enter() {
    pthread_mutex_lock(&mutex);
    while (num_men > 0 || num_women >= MAX_CAPACITY) {
        pthread_cond_wait(&can_enter, &mutex);
    }
    num_women++;
    printf("[ЖЕНЩИНА] Вошла (всего женщин: %d)\n", num_women);
    pthread_mutex_unlock(&mutex);
}

void woman_leaves() {
    pthread_mutex_lock(&mutex);
    num_women--;
    printf("[ЖЕНЩИНА] Вышла (осталось женщин: %d)\n", num_women);
    if (num_women == 0) {
        pthread_cond_broadcast(&can_enter);
    }
    pthread_mutex_unlock(&mutex);
}

void man_wants_to_enter() {
    pthread_mutex_lock(&mutex);
    while (num_women > 0 || num_men >= MAX_CAPACITY) {
        pthread_cond_wait(&can_enter, &mutex);
    }
    num_men++;
    printf("  [МУЖЧИНА] Вошёл (всего мужчин: %d)\n", num_men);
    pthread_mutex_unlock(&mutex);
}

void man_leaves() {
    pthread_mutex_lock(&mutex);
    num_men--;
    printf("  [МУЖЧИНА] Вышел (осталось мужчин: %d)\n", num_men);
    if (num_men == 0) {
        pthread_cond_broadcast(&can_enter);
    }
    pthread_mutex_unlock(&mutex);
}

void* woman_thread() {
    woman_wants_to_enter();
    sleep(rand() % 3 + 1);
    woman_leaves();
    return NULL;
}


void* man_thread() {
    man_wants_to_enter();
    sleep(rand() % 3 + 1);
    man_leaves();
    return NULL;
}

int main() {
    pthread_t women[NUM_WOMEN], men[NUM_MEN];
    srand(time(NULL));

    for (int i = 0; i < NUM_WOMEN; ++i) {
        pthread_create(&women[i], NULL, woman_thread, NULL);
        usleep(100000);
    }

    for (int i = 0; i < NUM_MEN; ++i) {
        pthread_create(&men[i], NULL, man_thread, NULL);
        usleep(100000);
    }


    for (int i = 0; i < NUM_WOMEN; ++i) {
        pthread_join(women[i], NULL);
    }
    for (int i = 0; i < NUM_MEN; ++i) {
        pthread_join(men[i], NULL);
    }

    printf("\nВсе потоки завершили работу.\n");
    return 0;
}