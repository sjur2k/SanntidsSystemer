#include <stdio.h>
#include <x86intrin.h>
#include <time.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

int main(){
    // ---- Step 1: Calibrate TSC frequency using clock_gettime as reference ----
    struct timespec cal_start, cal_end;
    unsigned long long tsc_cal_start, tsc_cal_end;

    clock_gettime(CLOCK_MONOTONIC, &cal_start);
    tsc_cal_start = __rdtsc();

    // Busy-wait ~200ms so we get a stable frequency estimate
    struct timespec sleep_time = {0, 200000000};
    nanosleep(&sleep_time, NULL);

    tsc_cal_end = __rdtsc();
    clock_gettime(CLOCK_MONOTONIC, &cal_end);

    double cal_elapsed_ns = (cal_end.tv_sec - cal_start.tv_sec) * 1e9
                           + (cal_end.tv_nsec - cal_start.tv_nsec);
    unsigned long long cal_cycles = tsc_cal_end - tsc_cal_start;
    double ns_per_cycle = cal_elapsed_ns / (double)cal_cycles;

    printf("Calibration: %.4f ns/cycle (implied freq: %.3f GHz)\n\n",
           ns_per_cycle, 1.0 / ns_per_cycle);

    // ---- Step 2: Histogram back-to-back call deltas, in native units ----
    int ns_max = 50;
    int histogram_rdtsc[ns_max];   // native unit: cycles
    int histogram_clock[ns_max];   // native unit: ns
    int ticks_max = 5;
    int histogram_times[ticks_max]; // native unit: ticks

    memset(histogram_rdtsc, 0, sizeof(histogram_rdtsc));
    memset(histogram_clock, 0, sizeof(histogram_clock));
    memset(histogram_times, 0, sizeof(histogram_times));

    unsigned long long rdtsc_start, rdtsc_end;
    struct timespec clock_start, clock_end;
    struct tms t;
    clock_t times_start, times_end;

    int n = 10*1000*1000;
    for(int i = 0; i < n; i++){
        rdtsc_start = __rdtsc();
        rdtsc_end = __rdtsc();

        clock_gettime(CLOCK_MONOTONIC, &clock_start);
        clock_gettime(CLOCK_MONOTONIC, &clock_end);

        times_start = times(&t);
        times_end = times(&t);

        int d_rdtsc = (int)(rdtsc_end - rdtsc_start);
        int d_clock = (int)(clock_end.tv_nsec - clock_start.tv_nsec);
        int d_times = (int)(times_end - times_start);

        if(d_rdtsc >= 0 && d_rdtsc < ns_max) histogram_rdtsc[d_rdtsc]++;
        if(d_clock >= 0 && d_clock < ns_max) histogram_clock[d_clock]++;
        if(d_times >= 0 && d_times < ticks_max) histogram_times[d_times]++;
    }

    // ---- Step 3: Find minimum nonzero delta for each (= resolution, native unit) ----
    int min_rdtsc_cycles = -1;
    for(int i = 1; i < ns_max; i++){          // start at 1, skip the "0 elapsed" bucket
        if(histogram_rdtsc[i] > 0){ min_rdtsc_cycles = i; break; }
    }

    int min_clock_ns = -1;
    for(int i = 1; i < ns_max; i++){
        if(histogram_clock[i] > 0){ min_clock_ns = i; break; }
    }

    int min_times_ticks = -1;
    for(int i = 1; i < ticks_max; i++){
        if(histogram_times[i] > 0){ min_times_ticks = i; break; }
    }

    // ---- Step 4: Convert to nanoseconds ----
    long tps = sysconf(_SC_CLK_TCK);

    double res_rdtsc_ns = (min_rdtsc_cycles > 0 ? min_rdtsc_cycles : 1) * ns_per_cycle;
    double res_clock_ns = (double)(min_clock_ns > 0 ? min_clock_ns : 1);
    double res_times_ns = 1.0 / tps * 1e9;
    // times(): since a boundary-crossing is essentially never observed between two
    // back-to-back calls (tick period >> call latency), we can't empirically catch
    // it this way — the known resolution is simply 1 tick, so report that directly.

    printf("Resolution results (nanoseconds):\n");
    printf("  rdtsc():          min nonzero delta = %d cycles -> %.3f ns\n",
        min_rdtsc_cycles, res_rdtsc_ns);
    printf("  clock_gettime():  min nonzero delta = %d ns      -> %.3f ns\n",
        min_clock_ns, res_clock_ns);
    printf("  times():          resolution = 1 tick             -> %.3f ns  (CLK_TCK=%ld)\n",
        res_times_ns, tps);

    return 0;
}