#include <string.h>
#include "memory.h"
#include "refstring.h"

RefString* allocateString(char* chars, int length) {
    RefString* string = ALLOCATE_REF(RefString, REF_STRING);
    string->length = length;
    string->chars = chars;
    return string;
}

RefString* copyString(const char* chars, int length) {
    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length);
}

RefString* takeString(char* chars, int length) {
    return allocateString(chars, length);
}