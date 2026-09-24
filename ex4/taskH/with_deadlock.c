#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define N 5 // Number of forks and philosophers
pthread_t philosophers[N];
pthread_mutex_t forks[N];

void* philosopher(void* arg);

int main(){
    
    for(int i = 0; i < N; i++){
        pthread_mutex_init(&forks[i],NULL);
    }
    for(int i = 0; i < N; i++){
        pthread_create(&philosophers[i],NULL,philosopher,(void*)(long)i);
    }    

    for(int i = 0; i < N; i++){
        pthread_join(philosophers[i], NULL);
        pthread_mutex_destroy(&forks[i]);
    }
    return 0;
}

void* philosopher(void* arg){
    int self_idx = (int)(long)arg;
    int left_fork_idx = (self_idx + N - 1) % N;
    int right_fork_idx = (self_idx + 1) % N;
    while (1){
        pthread_mutex_lock(&forks[left_fork_idx]);
        usleep(1000);
        printf("Philosopher %d has one fork\n", self_idx);
        pthread_mutex_lock(&forks[right_fork_idx]);
        printf("Philosopher %d has both forks\n", self_idx);

        pthread_mutex_unlock(&forks[left_fork_idx]);
        pthread_mutex_unlock(&forks[right_fork_idx]);
    }
}