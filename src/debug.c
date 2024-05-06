#include <stdio.h>
#include "debug.h"
#include "value.h"

void debugChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = debugInstruction(chunk, offset);
    }
}

static int constInstruction(const char* name, Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%s '", name);
    printValue(chunk->consts.values[constant]);
    printf("'\n");
    return offset + 2;
}

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

int debugInstruction(Chunk* chunk, int offset) {
    printf("(%d) %d: ", chunk->lines[offset], offset);

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_ADD: return simpleInstruction("add", offset);
        case OP_SUB: return simpleInstruction("sub", offset);
        case OP_MULT: return simpleInstruction("mult", offset);
        case OP_DIV: return simpleInstruction("div", offset);
        case OP_NULL: return simpleInstruction("null", offset);
        case OP_NOT: return simpleInstruction("not", offset);
        case OP_TRUE: return simpleInstruction("true", offset);
        case OP_FALSE: return simpleInstruction("false", offset);
        case OP_EQ: return simpleInstruction("eqcmp", offset);
        case OP_GCMP: return simpleInstruction("gcmp", offset);
        case OP_LCMP: return simpleInstruction("lcmp", offset);
        case OP_NEG: return simpleInstruction("neg", offset);
        case OP_CONST: return constInstruction("const", chunk, offset);
        case OP_DEFG: return constInstruction("defg", chunk, offset);
        case OP_SETG: return constInstruction("setg", chunk, offset);
        case OP_GETG: return constInstruction("getg", chunk, offset);
        case OP_POP: return simpleInstruction("pop", offset);
        case OP_RET: return simpleInstruction("ret", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
