#ifndef WOOL_VM_H
#define WOOL_VM_H

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stackTop;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERR,
    INTERPRET_RUNTIME_ERR
} InterpretResult;

void initVM();
void freeVM();

InterpretResult interpretChunk(Chunk* chunk);
void push(Value value);
Value pop();
InterpretResult run();

#endif
