#ifndef WOOL_VM_H
#define WOOL_VM_H

#include "chunk.h"
#include "table.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stackTop;
    Table globals;
    Table strings;
    Ref* refs;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERR,
    INTERPRET_RUNTIME_ERR
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();

InterpretResult interpretChunk(Chunk* chunk);
InterpretResult interpret(const char* src);
void push(Value value);
Value pop();
InterpretResult run();

#endif
