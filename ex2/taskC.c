#include <stdio.h>
#include "common/array.h"

int main(void) {

    Array arr = array_new(1);
    for (int i = 0; i < 100; i++){
        array_insertBack(&arr, (long)i);
        printf("%ld\n",arr.data[i]);
    }
    array_destroy(arr);
    return 0;
}

