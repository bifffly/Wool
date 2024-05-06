#include <stdio.h>
#include <string.h>
#include "common.h"
#include "memory.h"
#include "ref.h"
#include "refstring.h"
#include "vm.h"

bool isRefType(Value value, RefType type) {
    return IS_REF(value) && AS_REF(value)->type == type;
}

Ref* allocateRef(size_t size, RefType type) {
    Ref* ref = (Ref*) reallocate(NULL, 0, size);
    ref->type = type;
    ref->next = vm.refs;
    vm.refs = ref;
    return ref;
}

void printRef(Value value) {
    switch (REF_TYPE(value)) {
        case REF_STRING: {
            printf("%s", AS_CSTRING(value));
            break;
        }
    }
}

bool refsEqual(Value a, Value b) {
    if (REF_TYPE(a) != REF_TYPE(b)) {
        return false;
    }
    switch (REF_TYPE(a)) {
        case REF_STRING: {
            RefString* astr = AS_STRING(a);
            RefString* bstr = AS_STRING(b);
            return astr->length == bstr->length
                && memcmp(astr->chars, bstr->chars, astr->length) == 0;
        }
    }
}
