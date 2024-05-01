#ifndef WOOL_VALUE_H
#define WOOL_VALUE_H

#include "common.h"

typedef double Value;

typedef struct {
    int count;
    int capacity;
    Value* values;
} ValueArray;

void initValueArray(ValueArray* arr);
void writeValueArray(ValueArray* arr, Value value);
void freeValueArray(ValueArray* arr);

void printValue(Value value);

#endif
