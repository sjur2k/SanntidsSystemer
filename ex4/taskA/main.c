#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

long int global_var = 0;
pthread_barrier_t barr;

void* fn(void* args){
    int res = pthread_barrier_wait(&barr);
    if(res == PTHREAD_BARRIER_SERIAL_THREAD){
        long int local_var = 0;
        for (int i = 0; i < 1e6; i++){
            local_var++;
            global_var++;
        }
        printf("Local var: %ld\n", local_var);
    } else {
        printf("Thread not chosen\n");
    }
    return NULL;
}


int main(){
    pthread_barrier_init(&barr, NULL, 2);
    
    pthread_t thread1,thread2;
    pthread_create(&thread1, NULL, fn, NULL);    
    pthread_create(&thread2, NULL, fn, NULL);    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Global var: %ld\n",global_var);
    return 0;
}
