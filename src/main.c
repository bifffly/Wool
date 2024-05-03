#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char** argv) {
    initVM();
    Chunk chunk;
    initChunk(&chunk);

    int constant = writeValueToChunk(&chunk, 1.2);
    writeByteToChunk(&chunk, OP_CONST, 123);
    writeByteToChunk(&chunk, constant, 123);

    constant = writeValueToChunk(&chunk, 3.4);
    writeByteToChunk(&chunk, OP_CONST, 123);
    writeByteToChunk(&chunk, constant, 123);

    writeByteToChunk(&chunk, OP_ADD, 123);

    constant = writeValueToChunk(&chunk, 5.6);
    writeByteToChunk(&chunk, OP_CONST, 123);
    writeByteToChunk(&chunk, constant, 123);

    writeByteToChunk(&chunk, OP_DIV, 123);
    writeByteToChunk(&chunk, OP_NEG, 123);
    writeByteToChunk(&chunk, OP_RET, 123);

    debugChunk(&chunk, "test chunk");
    interpretChunk(&chunk);

    freeVM();
    freeChunk(&chunk);

    return 0;
}