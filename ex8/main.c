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
#define CPU_ID 1

int input_pins[N_PIN_PAIRS] = {PININ1, PININ2, PININ3};
int output_pins[N_PIN_PAIRS] = {PINOUT1, PINOUT2, PINOUT3};

typedef struct{
    int cpu_id;
    struct {
        uint32_t test;
        uint32_t response;
    }pin;
}responseTaskArgs;

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

void isrA(void) {digitalWrite(PINOUT1, digitalRead(PININ1));}
void isrB(void) {digitalWrite(PINOUT2, digitalRead(PININ2));}
void isrC(void) {digitalWrite(PINOUT3, digitalRead(PININ3));}

void* responseTask(void* args){
    responseTaskArgs rta = *(responseTaskArgs*)args;
    set_cpu(rta.cpu_id);

    while(true){
        sleep_realtime(2000000);
        digitalWrite(rta.pin.response, digitalRead(rta.pin.test));
    }
};

void* busyWait(void* args){
    set_cpu(1);

    while(true){
        asm volatile("" ::: "memory");
    }
}

int create_rt_thread(pthread_t* t, int policy, int priority, void* task, void* args){
    pthread_attr_t attr;
    struct sched_param param;
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, policy);
    param.sched_priority = priority;
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    return pthread_create(t, &attr, task, args);
}

int main(){
    io_init();

    pthread_t task_threads[N_TASK_THREADS];
    responseTaskArgs rtas[N_TASK_THREADS];
    
    // TASK THREADS
    for(int i = 0; i < N_TASK_THREADS; i++){
        rtas[i] = (responseTaskArgs){CPU_ID, {input_pins[i], output_pins[i]}};
        int ok = create_rt_thread(&task_threads[i], SCHED_RR, 80, responseTask, (void*)&rtas[i]);
        if (ok != 0){
            perror("Failed to create task thread");
        }
    }

    // DISTURBANCE THREADS
    pthread_t dist_threads[N_DIST_THREADS];
    
    for(int i=0; i < N_DIST_THREADS; i++){
        int ok = create_rt_thread(&dist_threads[i], SCHED_RR, 40, busyWait, NULL);
        if (ok != 0){
            perror("Failed to create disturbance thread");
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