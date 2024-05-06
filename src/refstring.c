#include <string.h>
#include "memory.h"
#include "refstring.h"
#include "table.h"
#include "vm.h"

RefString* allocateString(char* chars, int length, uint32_t hash) {
    RefString* string = ALLOCATE_REF(RefString, REF_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    setTable(&vm.strings, string, NULL_VAL);
    return string;
}

uint32_t hashString(const char* key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

RefString* copyString(const char* chars, int length) {
    uint32_t hash = hashString(chars, length);
    RefString* interned = findStringTable(&vm.strings, chars, length, hash);
    if (interned != NULL) {
        return interned;
    }
    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length, hash);
}

RefString* takeString(char* chars, int length) {
    uint32_t hash = hashString(chars, length);
    RefString* interned = findStringTable(&vm.strings, chars, length, hash);
    if (interned != NULL) {
        FREE_ARRAY(char, chars, length + 1);
        return interned;
    }
    return allocateString(chars, length, hash);
}