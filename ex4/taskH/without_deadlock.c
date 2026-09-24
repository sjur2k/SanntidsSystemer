#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>

#define N 5 // Number of forks and philosophers
int colors[N] = {91, 92, 93, 94, 96};
typedef enum {
    THINKING,
    HUNGRY,
    EATING,
} PhilosopherState;

pthread_mutex_t critical_section_mtx;
sem_t both_forks[N];
pthread_t philosophers[N];
PhilosopherState states[N];

void* philosopher(void* arg);

void think(int idx);
void take_forks(int idx);
void eat(int idx);
void put_forks(int idx);
int left(int idx);
int right(int idx);

int main(){   
    pthread_mutex_init(&critical_section_mtx, NULL);
    for(int i = 0; i < N; i++){
        sem_init(&both_forks[i], 0, 0);
    }
    for(int i = 0; i < N; i++){
        pthread_create(&philosophers[i],NULL,philosopher,(void*)(long)i);
    }
    
    for(int i = 0; i < N; i++){
        pthread_join(philosophers[i], NULL);
    }
    for(int i = 0; i < N; i++){
        sem_destroy(&both_forks[i]);
    }
    pthread_mutex_destroy(&critical_section_mtx);
    return 0;
}

void* philosopher(void* arg){
    int self_idx = (int)(long)arg;

    while (1){
        think(self_idx);
        take_forks(self_idx);
        eat(self_idx);
        put_forks(self_idx);
    }
}

void think(int idx){
    printf("Philosopher %d is thinking\n",idx + 1);
    usleep(100 * 1000);
}

// Must only be called within a critical section.
void try_forks(int idx){
    if (states[idx] == HUNGRY &&
        states[left(idx)] != EATING &&
        states[right(idx)] != EATING)
    {
        states[idx] = EATING;
        sem_post(&both_forks[idx]);
    }
}

void take_forks(int idx){
    pthread_mutex_lock(&critical_section_mtx);
    {
        states[idx] = HUNGRY;
        printf("Philosopher %d is hungry\n", idx + 1);
        try_forks(idx); // Check if forks are available.
    }
    pthread_mutex_unlock(&critical_section_mtx);

    sem_wait(&both_forks[idx]);
}

void eat(int idx){
    usleep(200 * 1000);
    printf("\033[1;%dmPhilosopher %d is eating\033[0;0m\n", colors[idx],idx + 1);
}

void put_forks(int idx){
    pthread_mutex_lock(&critical_section_mtx);
    {
        states[idx] = THINKING;
        try_forks(left(idx)); //Tries to release neighbor fork semaphores.
        try_forks(right(idx));
    }
    pthread_mutex_unlock(&critical_section_mtx);
}

int left(int idx){
    return (idx + N - 1) % N;
}

int right(int idx){
    return (idx + 1) % N;
}