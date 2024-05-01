#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, char** argv) {
    Chunk chunk;
    initChunk(&chunk);
    writeChunk(&chunk, OP_RET);
    debugChunk(&chunk, "test chunk");
    freeChunk(&chunk);

    return 0;
}