#pragma once
#include <time.h>
#include <sys/times.h>
#include <unistd.h>

/*struct timespec {
    time_t tv_sec;  // Seconds
    long   tv_nsec; // Nanoseconds [0, 999999999]
};*/

struct timespec timespec_normalized(time_t sec, long nsec);
struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);
struct timespec timespec_add(struct timespec lhs, struct timespec rhs);
int timespec_cmp(struct timespec lhs, struct timespec rhs);
struct timespec ticks_to_timespec(clock_t ticks);