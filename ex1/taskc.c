#include <time.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>

int main(){
    int ns_max = 2000;
    int histogram_clock[ns_max];   // native unit: ns
    memset(histogram_clock, 0, sizeof(int)*ns_max);
    int n = 10*1000*1000;
    struct timespec clock_start, clock_end;
    for (int i = 0; i < n; i++){
        clock_gettime(CLOCK_MONOTONIC, &clock_start);
        sched_yield();
        clock_gettime(CLOCK_MONOTONIC, &clock_end);
        int d_clock = (int)(clock_end.tv_nsec - clock_start.tv_nsec);
        if (d_clock >= 0 && d_clock < ns_max) {
            histogram_clock[d_clock]++;
        }
    }
    for (int i = 0; i < ns_max; i++){
        printf("%d\n", histogram_clock[i]);
    }
    return 0;
}