#include <stdio.h>
#include "memory.h"
#include "value.h"
#include "ref.h"

void initValueArray(ValueArray* arr) {
    arr->count = 0;
    arr->capacity = 0;
    arr->values = NULL;
}

void writeValueArray(ValueArray* arr, Value value) {
    if (arr->capacity < arr->count + 1) {
        int oldCapacity = arr->capacity;
        arr->capacity = GROW_CAPACITY(oldCapacity);
        arr->values = GROW_ARRAY(Value, arr->values,
                                   oldCapacity, arr->capacity);
    }

    arr->values[arr->count] = value;
    arr->count++;
}

void freeValueArray(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void printValue(Value value) {
    switch (value.type) {
        case VAL_BOOL: printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NUM: printf("%g", AS_NUM(value)); break;
        case VAL_REF: printRef(value); break;
        case VAL_NULL: printf("null"); break;
    }
}

bool valuesEqual(Value a, Value b) {
    if (a.type != b.type) return false;
    switch (a.type) {
        case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NUM: return AS_NUM(a) == AS_NUM(b);
        case VAL_REF: return refsEqual(a, b);
        case VAL_NULL: return true;
        default: return false;
    }
}