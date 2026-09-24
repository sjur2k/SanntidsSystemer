#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define N 5 // Number of forks and philosophers
pthread_t philosophers[N];
pthread_mutex_t forks[N];

int philosopher(void* arg){
    int self_idx = (int)(long)arg;
    int left_fork_idx = (self_idx + N - 1) % N;
    int right_fork_idx = (self_idx + 1) % N;
    while (1){
        pthread_mutex_lock(&forks[left_fork_idx]);
        usleep(10000); // Helps induce deadlock.
        pthread_mutex_lock(&forks[right_fork_idx]);
        printf("Philosopher %d eating\n", self_idx);

        pthread_mutex_unlock(&forks[left_fork_idx]);
        pthread_mutex_unlock(&forks[right_fork_idx]);
        printf("Philosopher %d thinking\n", self_idx);
    }
}

int main(){
    
    for(int i = 0; i < N; i++){
        pthread_create(&philosophers[i],NULL,(void*)philosopher,(void*)(long)i);
        pthread_mutex_init(&forks[i],NULL);
    }

    for(int i = 0; i < N; i++){
        pthread_join(philosophers[i], NULL);
        pthread_mutex_destroy(&forks[i]);
    }
    return 0;
}