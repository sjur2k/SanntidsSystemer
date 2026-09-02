# Exercise 1

## task A:
Sleep() is very effective.
u and nano differ in input format and error handling.

busywait is ineffective (high user time)
busywait with times() also ineffective

## task B:
### Access latency $(n=10^8)$:

 

$\\ T_{rdtsc} = 1.345s \implies \Delta T_{rdtsc} = 13.45ns$

$\\ T_{clock\_gettime} = 3.122s \implies \Delta T_{clock\_gettime} = 31.22ns$

$\\ T_{times} = 106.984s \implies \Delta T_{times} \approx 1.07\mu s \approx80\times\Delta T_{rdtsc}$

This might all be affected by me using wsl to benchmark.