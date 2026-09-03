#include <stdio.h>
//include <x86intrin.h>
#include <time.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdint.h>

uint64_t rdtsc(void){
    uint64_t val;
    asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(val) :: "memory"); 
    // You can check the current CPU frequency with $sudo dmesg | grep MHz
    return val;
}

uint64_t rdtsc_fast(void){
    uint64_t val;
    asm volatile("mrs %0, cntvct_el0" : "=r"(val));
    return val;
}

int main(){

    /* int ns_max = 50;
    int histogram_rdtsc[ns_max];   // native unit: cycles
    int histogram_clock[ns_max];   // native unit: ns
    int histogram_times[ns_max]; // native unit: ticks

    memset(histogram_rdtsc, 0, sizeof(int)*ns_max);
    memset(histogram_clock, 0, sizeof(int)*ns_max);
    memset(histogram_times, 0, sizeof(int)*ns_max);

    uint64_t rdtsc_start, rdtsc_end;
    struct timespec clock_start, clock_end;
    struct tms t;
    clock_t times_start, times_end;
    double tpns = sysconf(_SC_CLK_TCK)*1e-9;
    int n = 10*1000*1000;
    uint64_t cntfrq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq));
    rdtsc
    for(int i = 0; i < n; i++){
        rdtsc_start = rdtsc();
        rdtsc_end = rdtsc();

        clock_gettime(CLOCK_MONOTONIC, &clock_start);
        clock_gettime(CLOCK_MONOTONIC, &clock_end);

        times_start = times(&t);
        times_end = times(&t);
        
        
        int d_rdtsc = (int)((double)(rdtsc_end - rdtsc_start)/cntfrq *1e9);
        int d_clock = (int)(clock_end.tv_nsec - clock_start.tv_nsec);
        int d_times = (int)((double)(times_end - times_start)/tpns *1e-6);

        if(d_rdtsc >= 0 && d_rdtsc < ns_max) histogram_rdtsc[d_rdtsc]++;
        if(d_clock >= 0 && d_clock < ns_max) histogram_clock[d_clock]++;
        if(d_times >= 0 && d_times < ns_max) histogram_times[d_times]++;
    }

    for (int i = 0; i < ns_max; i++){
        printf("%d\n", histogram_rdtsc[i]);
        //printf("%d\n", histogram_clock[i]);
        //printf("%d\n", histogram_times[i]);
    }
    
 */ 
    struct timespec clock_start, clock_end;
    clock_gettime(CLOCK_MONOTONIC, &clock_start);
    for(int i = 0; i < 100*1000*1000; i++){
        rdtsc();
    }
    clock_gettime(CLOCK_MONOTONIC, &clock_end);
    double elapsed = (double)(clock_end.tv_sec - clock_start.tv_sec) 
                    + (double)(clock_end.tv_nsec - clock_start.tv_nsec)/1e9;
    printf("rdtsc():      total %.6f s, %.3f ns/iteration\n",
       elapsed, elapsed / (100*1000*1000) * 1e9);

    struct timespec clock_start_, clock_end_;
    clock_gettime(CLOCK_MONOTONIC, &clock_start_);
    for(int i = 0; i < 100*1000*1000; i++){
        rdtsc_fast();
    }
    clock_gettime(CLOCK_MONOTONIC, &clock_end_);

    double elapsed_ = (double)(clock_end_.tv_sec - clock_start_.tv_sec)
                    + (double)(clock_end_.tv_nsec - clock_start_.tv_nsec) / 1e9;
    printf("rdtsc_fast(): total %.6f s, %.3f ns/iteration\n",
        elapsed_, elapsed_ / (100*1000*1000) *1e9);

    
    return 0;
}