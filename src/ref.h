#ifndef WOOL_REF_H
#define WOOL_REF_H

#include "common.h"
#include "value.h"

#define ALLOCATE_REF(type, refType) (type*) allocateRef(sizeof(type), refType)

#define REF_TYPE(value) (AS_REF(value)->type) \

typedef enum {
    REF_STRING
} RefType;

struct Ref {
    RefType type;
    struct Ref* next;
};

bool isRefType(Value value, RefType type);
Ref* allocateRef(size_t size, RefType type);
void printRef(Value value);
bool refsEqual(Value a, Value b);

#endif
