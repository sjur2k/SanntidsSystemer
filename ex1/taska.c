#include "common/utils.h"
#include <time.h>
#include <stdio.h>

int main(){
    struct timespec t0 = {1,0};
    struct timespec t1 = {1,1};
    struct timespec t2 = {1,2};
    printf("%d\n",timespec_cmp(t0,t1));
    printf("%d\n",timespec_cmp(t1,t2));
    return 0;
}