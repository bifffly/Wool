#ifndef WOOL_REFSTRING_H
#define WOOL_REFSTRING_H

#include "ref.h"

#define IS_STRING(value) isRefType(value, REF_STRING)

#define AS_STRING(value) ((RefString*) AS_REF(value))
#define AS_CSTRING(value) (((RefString*) AS_REF(value))->chars)

struct RefString {
    Ref ref;
    int length;
    char* chars;
    uint32_t hash;
};

RefString* copyString(const char* chars, int length);
RefString* takeString(char* chars, int length);

#endif
