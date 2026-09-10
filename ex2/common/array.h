#pragma once

typedef struct Array Array;
struct Array {
    long* data;
    long front;
    long back;   
    long capacity; 
};

// Construction / Destruction
Array array_new(long capacity);
void array_destroy(Array a);

// Primitives
long array_empty(Array a);
long array_front(Array a);
long array_back(Array a);
void array_popFront(Array* a);
void array_popBack(Array* a);
Array array_save(Array a);

// Iteration
void array_foreach(Array a, void fn(long));
void array_foreachReverse(Array a, void fn(long));
void array_print(Array a);

// Capacity
long array_length(Array a);
void array_reserve(Array* a, long capacity);

// Modifiers
void array_insertBack(Array* a, long stuff);
