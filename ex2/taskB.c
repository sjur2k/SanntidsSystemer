#include <stdio.h>
#include "common/array.h"

int main(void) {

    Array arr = array_new(10);
    for (int i = 0; i < 10; i++){
        array_insertBack(&arr, (long)i);
        printf("%ld\n",arr.data[i]);
    }
    return 0;
}
