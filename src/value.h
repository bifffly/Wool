#ifndef WOOL_VALUE_H
#define WOOL_VALUE_H

#include "common.h"

typedef enum {
    VAL_BOOL,
    VAL_NUM,
    VAL_NULL
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
    } as;
} Value;

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NUM(value) ((value).type == VAL_NUM)
#define IS_NULL(value) ((value).type == VAL_NULL)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUM(value) ((value).as.number)

#define BOOL_VAL(value) ((Value) {VAL_BOOL, {.boolean = value}})
#define NUM_VAL(value) ((Value) {VAL_NUM, {.number = value}})
#define NULL_VAL ((Value) {VAL_NULL, {.number = 0}})

typedef struct {
    int count;
    int capacity;
    Value* values;
} ValueArray;

void initValueArray(ValueArray* arr);
void writeValueArray(ValueArray* arr, Value value);
void freeValueArray(ValueArray* arr);

void printValue(Value value);
bool valuesEqual(Value a, Value b);

#endif
