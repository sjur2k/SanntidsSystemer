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
    struct timespec now_spec = ticks_to_timespec(times(&now));
    struct timespec then_spec = timespec_add(now_spec, (struct timespec){1,0});
    while(timespec_cmp(ticks_to_timespec(times(&now)),then_spec)<0){
        for(int i = 0; i < 10000; i++){}
    }
}

int main(int argc, char* argv[]){
    const struct timespec t0 = {1,0};
    argc == 1 ? busy_wait(t0) : busy_wait_times(t0);
    return 0;
}