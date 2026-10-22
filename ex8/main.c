#define _GNU_SOURCE
#include <wiringPi.h>
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
#define PINISR 
#define N_DIST_THREADS 10
#define N_TASK_THREADS 3
#define N_PIN_PAIRS 3

int input_pins[N_TASK_THREADS] = {PININ1, PININ2, PININ3};
int output_pins[N_TASK_THREADS] = {PINOUT1, PINOUT2, PINOUT3};

typedef struct{
    int cpu_id;
    struct {
        uint32_t test;
        uint32_t response;
    }pin;
}responseTaskArgs;

void io_init( void ) {
    wiringPiSetup();
    for (int i = 0; i < N_PIN_PAIRS; i++){
        pinMode(input_pins[i], INPUT);
        pinMode(output_pins[i], OUTPUT);
        digitalWrite(output_pins[i], HIGH);
    }
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

void* responseTask(void* args){
    responseTaskArgs rta = *(responseTaskArgs*)args;
    set_cpu(rta.cpu_id);

    while(true){
        sleep_realtime(2000000);
        if (digitalRead(rta.pin.test) == LOW){
            digitalWrite(rta.pin.response, LOW);
        }
        if (digitalRead(rta.pin.test) == HIGH){
            digitalWrite(rta.pin.response, HIGH);
        }
    }
};

void* busyWait(void* args){
    set_cpu(1);

    while(true){
        asm volatile("" ::: "memory");
    }
}

int main(){
    io_init();

    // DISTURBANCE THREADS
    pthread_t dist_threads[N_DIST_THREADS];
    pthread_attr_t dist_threadattrs[N_DIST_THREADS];
    struct sched_param dist_schedparams[N_DIST_THREADS];
    
    for(int i=0; i < N_DIST_THREADS; i++){
        pthread_attr_init(&dist_threadattrs[i]);
        pthread_attr_setschedpolicy(&dist_threadattrs[i], SCHED_RR);
        dist_schedparams[i].sched_priority = 40;
        pthread_attr_setschedparam(&dist_threadattrs[i], &dist_schedparams[i]);
        pthread_attr_setinheritsched(&dist_threadattrs[i], PTHREAD_EXPLICIT_SCHED);
        if (pthread_create(&dist_threads[i], &dist_threadattrs[i], busyWait, NULL) != 0){
            perror("Failed to create disturbance thread\n");
        }
    }

    pthread_t task_threads[N_TASK_THREADS];
    pthread_attr_t task_threadattrs[N_TASK_THREADS];
    struct sched_param task_schedparams[N_TASK_THREADS];
    responseTaskArgs rtas[N_TASK_THREADS];
    int cpu_id = 1;
    

    for(int i = 0; i < N_TASK_THREADS; i++){
        rtas[i] = (responseTaskArgs){cpu_id, {input_pins[i], output_pins[i]}};
        pthread_attr_init(&task_threadattrs[i]);
        pthread_attr_setschedpolicy(&task_threadattrs[i], SCHED_RR);
        task_schedparams[i].sched_priority = 80;
        pthread_attr_setschedparam(&task_threadattrs[i], &task_schedparams[i]);
        pthread_attr_setinheritsched(&task_threadattrs[i], PTHREAD_EXPLICIT_SCHED);
        if (pthread_create(
                &task_threads[i], 
                &task_threadattrs[i], 
                responseTask, 
                (void*)&rtas[i]
            ) != 0
        ){
            perror("Failed to create task thread");
        }
    }
    
    for(int i = 0; i < N_TASK_THREADS; i++){
        pthread_join(task_threads[i], NULL);
    }

    for (int i=0; i<N_DIST_THREADS; i++){
        pthread_join(dist_threads[i], NULL);
    }
    
    return 0;
}