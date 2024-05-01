#include <stdio.h>
#include "debug.h"

void debugChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = debugInstruction(chunk, offset);
    }
}

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

int debugInstruction(Chunk* chunk, int offset) {
    printf("%d: ", offset);

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_RET:
            return simpleInstruction("ret", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
