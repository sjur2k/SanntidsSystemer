# Exercise 1

## task A:
Sleep() is very effective.
u and nano differ in input format and error handling.

busywait is ineffective (high user time)
busywait with times() also ineffective

## task B:
### Access latency $(n=10^8)$:

 
### (WSL)
$\\ T_{rdtsc} = 1.345s \implies \Delta T_{rdtsc} = 13.45ns$

$\\ T_{clock\_gettime} = 3.122s \implies \Delta T_{clock\_gettime} = 31.22ns$

$\\ T_{times} = 106.984s \implies \Delta T_{times} \approx 1.07\mu s \approx80\times\Delta T_{rdtsc}$

### (RPI)
$\\ T_{rdtsc} = 5.554s \implies \Delta T_{rdtsc} = 55.54ns$

$\\ T_{clock\_gettime} = 4.681s \implies \Delta T_{clock\_gettime} = 46.81ns$

$\\ T_{times} = 94.974s \implies \Delta T_{times} \approx 949.74ns \approx17\times\Delta T_{rdtsc}$

```bash
rdtsc():      total 5.570190 s, 55.702 ns/iteration
rdtsc_fast(): total 0.500681 s, 5.007 ns/iteration
```
With
```c
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
```
