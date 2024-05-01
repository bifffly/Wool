#ifndef WOOL_CHUNK_H
#define WOOL_CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONST, OP_RET
} OpCode;

typedef struct {
    int count;
    int capacity;
    uint8_t* code;
    ValueArray consts;
} Chunk;

void initChunk(Chunk* chunk);
void writeByteToChunk(Chunk* chunk, uint8_t byte);
int writeValueToChunk(Chunk* chunk, Value value);
void freeChunk(Chunk* chunk);

#endif
