#include <stdlib.h>
#include "memory.h"
#include "ref.h"
#include "refstring.h"
#include "vm.h"

void* reallocate(void* ptr, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(ptr);
        return NULL;
    }

    void* result = realloc(ptr, newSize);
    if (result == NULL) {
        exit(1);
    }
    return result;
}

void freeRef(Ref* ref) {
    switch (ref->type) {
        case REF_STRING: {
            RefString* string = (RefString*) ref;
            FREE_ARRAY(char, string->chars, string->length + 1);
            FREE(RefString, ref);
            break;
        }
    }
}

void freeRefs() {
    Ref* ref = vm.refs;
    while (ref != NULL) {
        Ref* next = ref->next;
        freeRef(ref);
        ref = next;
    }
}
