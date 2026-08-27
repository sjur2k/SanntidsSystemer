#include "common/utils.h"
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdio.h>

void busy_wait(struct timespec t){
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);
    
    while(timespec_cmp(now, then) < 0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

void busy_wait_times(struct timespec t){
    struct tms now;
    long tickrate = sysconf(_SC_CLK_TCK);
    struct timespec now_spec = ticks_to_timespec(times(&now));
    printf("%f\n", (double)(now_spec.tv_sec);
    struct timespec onesec = {1,0};
    struct timespec then_spec = timespec_add(now_spec, onesec);
    while(timespec_cmp(ticks_to_timespec(times(&now)),then_spec)<0){
        for(int i = 0; i < 10000; i++){}
    }
}

int main(){
    const struct timespec t0 = {1,0};
    const struct timespec t1 = {10,1};
    busy_wait_times(t0);
    return 0;
}