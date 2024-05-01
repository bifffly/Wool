#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, char** argv) {
    Chunk chunk;
    initChunk(&chunk);
    int constant = writeValueToChunk(&chunk, 1.2);
    writeByteToChunk(&chunk, OP_CONST);
    writeByteToChunk(&chunk, constant);
    writeByteToChunk(&chunk, OP_RET);
    debugChunk(&chunk, "test chunk");
    freeChunk(&chunk);

    return 0;
}