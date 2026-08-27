#include <stdlib.h>

void sort(int count, char *array[])
{
    int x,y;
    char* holder;
    for(x=1; x<count; x++)
        for(y=1; y<count-1; y++)
            if(atoi(array[y]) > atoi(array[y+1])) {
                holder = array[y+1];
                array[y+1] = array[y];
                array[y] = holder;
            }
}