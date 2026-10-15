#include <wiringPi.h>

#define _GNU_SOURCE
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <time.h>

#define PININ1  8
#define PINOUT1 9
#define PININ2  7
#define PINOUT2 15
#define PININ3  16
#define PINOUT3 1
#define N_THREADS 10

typedef struct{
    int cpu_id;
    struct {
        uint32_t test;
        uint32_t response;
    }pin;
}responseTaskArgs;

void io_init( void ) {
    wiringPiSetup();
    
    pinMode (PININ1, INPUT); pinMode (PINOUT1, OUTPUT); 
    pinMode (PININ2, INPUT); pinMode (PINOUT2, OUTPUT); 
    pinMode (PININ3, INPUT); pinMode (PINOUT3, OUTPUT); 
    
    digitalWrite(PINOUT1, HIGH);
    digitalWrite(PINOUT2, HIGH);
    digitalWrite(PINOUT3, HIGH);
}
void sleep_realtime(long nanoseconds)
{
    struct timespec ts;
    ts.tv_sec = nanoseconds / 1e9;
    ts.tv_nsec = nanoseconds % (long)1e9;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
}

int set_cpu(int cpu_id)
{
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpu_id < 0 || cpu_id >= num_cores)
        return EINVAL;

    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_id, &cpu);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void responseTask(void* rta_void){
    responseTaskArgs rta = *(responseTaskArgs*)rta_void;
    set_cpu(1);

    while(true){
        sleep_realtime(2000000);
        if (digitalRead(rta.pin.test) == LOW){
            digitalWrite(rta.pin.response, LOW);
        }
        if (digitalRead(rta.pin.test) == HIGH){
            digitalWrite(rta.pin.response, HIGH);
        }
    }
    return;
};

void busyWait(void){
    set_cpu(1);

    while(true){
        asm volatile("" ::: "memory");
    }
}



int main(){
    io_init();
    pthread_t a[N_THREADS];
    for(int i=0; i<N_THREADS; i++){
        pthread_create(&a[i], NULL, (void*)busyWait, NULL);
    }

    pthread_t t1, t2, t3;
    responseTaskArgs r1 = {1,{PININ1, PINOUT1}};
    responseTaskArgs r2 = {2,{PININ2, PINOUT2}};
    responseTaskArgs r3 = {3,{PININ3, PINOUT3}};
    
    pthread_create(&t1, NULL, (void*)responseTask, (void*)&r1);
    pthread_create(&t2, NULL, (void*)responseTask, (void*)&r2);
    pthread_create(&t3, NULL, (void*)responseTask, (void*)&r3);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    for (int i=0; i<N_THREADS; i++){
        pthread_join(a[i], NULL);
    }
    
    return 0;
}