#pragma once
#include <time.h>

/*struct timespec {
    time_t tv_sec;  // Seconds
    long   tv_nsec; // Nanoseconds [0, 999999999]
};*/

struct timespec timespec_normalized(time_t sec, long nsec);
struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);
struct timespec timespec_add(struct timespec lhs, struct timespec rhs);
int timespec_cmp(struct timespec lhs, struct timespec rhs);