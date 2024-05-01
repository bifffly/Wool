#ifndef WOOL_DEBUG_H
#define WOOL_DEBUG_H

#include "chunk.h"

void debugChunk(Chunk* chunk, const char* name);
int debugInstruction(Chunk* chunk, int offset);

#endif //WOOL_DEBUG_H
