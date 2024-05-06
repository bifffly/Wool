#ifndef WOOL_CHUNK_H
#define WOOL_CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
    OP_ADD, OP_SUB,
    OP_MULT, OP_DIV,
    OP_NULL, OP_NOT,
    OP_TRUE, OP_FALSE,
    OP_EQ, OP_GCMP, OP_LCMP,
    OP_NEG, OP_CONST,
    OP_DEFG, OP_SETG, OP_GETG,
    OP_POP, OP_RET
} OpCode;

typedef struct {
    int count;
    int capacity;
    uint8_t* code;
    int* lines;
    ValueArray consts;
} Chunk;

void initChunk(Chunk* chunk);
void writeByteToChunk(Chunk* chunk, uint8_t byte, int line);
int writeValueToChunk(Chunk* chunk, Value value);
void freeChunk(Chunk* chunk);

#endif
