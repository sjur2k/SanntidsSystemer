#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

long int global_var = 0;
sem_t sem;

void* fn(void* args){
    long int local_var = 0;
    for (int i = 0; i < 1e6; i++){
        local_var++;
        sem_wait(&sem);
        global_var++;
        sem_post(&sem);
    }
    printf("Local var: %ld\n", local_var);
    return NULL;
}


int main(){

    pthread_t thread1,thread2;
    sem_init(&sem, 0, 1);
    pthread_create(&thread1, NULL, fn, NULL);    
    pthread_create(&thread2, NULL, fn, NULL);    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    sem_destroy(&sem);
    printf("Global var: %ld\n",global_var);
    return 0;
}
