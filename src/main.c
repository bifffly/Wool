#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, char** argv) {
    Chunk chunk;
    initChunk(&chunk);
    int constant = writeValueToChunk(&chunk, 1.2);
    writeByteToChunk(&chunk, OP_CONST, 1);
    writeByteToChunk(&chunk, constant, 1);
    writeByteToChunk(&chunk, OP_RET, 1);
    debugChunk(&chunk, "test chunk");
    freeChunk(&chunk);

    return 0;
}