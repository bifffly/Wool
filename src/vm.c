#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "compiler.h"
#include "memory.h"
#include "refstring.h"
#include "vm.h"

VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
}

void runtimeError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}

void initVM() {
    resetStack();
    vm.refs = NULL;
}

void freeVM() {
    freeRefs();
}

InterpretResult interpretChunk(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

InterpretResult interpret(const char* src) {
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(src, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();
    freeChunk(&chunk);

    return result;
}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

Value peekStack(int dist) {
    return vm.stackTop[-1 - dist];
}

bool isFalsy(Value value) {
    return IS_NULL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

void concat() {
    RefString* b = AS_STRING(pop());
    RefString* a = AS_STRING(pop());

    int length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    RefString* result = takeString(chars, length);
    push(REF_VAL(result));
}

InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONST() (vm.chunk->consts.values[READ_BYTE()])
#define BINARY_OP(valType, op) \
    do { \
        if (!IS_NUM(peekStack(0)) || !IS_NUM(peekStack(1))) { \
            runtimeError("Operands must be numbers."); \
            return INTERPRET_RUNTIME_ERR; \
        } \
        double b = AS_NUM(pop()); \
        double a = AS_NUM(pop()); \
        push(valType(a op b)); \
    } while (false)

    while (true) {
        switch (READ_BYTE()) {
            case OP_ADD: {
                if (IS_STRING(peekStack(0)) && IS_STRING(peekStack(1))) {
                    concat();
                } else if (IS_NUM(peekStack(0)) && IS_NUM(peekStack(1))) {
                    double b = AS_NUM(pop());
                    double a = AS_NUM(pop());
                    push(NUM_VAL(a + b));
                } else {
                    runtimeError("Operands must be both numbers or both strings.");
                    return INTERPRET_RUNTIME_ERR;
                }
                break;
            }
            case OP_SUB: BINARY_OP(NUM_VAL, -); break;
            case OP_MULT: BINARY_OP(NUM_VAL, *); break;
            case OP_DIV: BINARY_OP(NUM_VAL, /); break;
            case OP_NULL: push(NULL_VAL); break;
            case OP_NOT: push(BOOL_VAL(isFalsy(pop()))); break;
            case OP_TRUE: push(BOOL_VAL(true)); break;
            case OP_FALSE: push(BOOL_VAL(false)); break;
            case OP_EQ: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(valuesEqual(a, b)));
                break;
            }
            case OP_GCMP: BINARY_OP(BOOL_VAL, >); break;
            case OP_LCMP: BINARY_OP(BOOL_VAL, <); break;
            case OP_NEG: {
                if (!IS_NUM(peekStack(0))) {
                    runtimeError("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERR;
                }
                push(NUM_VAL(-AS_NUM(pop())));
                break;
            }
            case OP_CONST: {
                Value constant = READ_CONST();
                push(constant);
                break;
            }
            case OP_RET: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }

#undef READ_BYTE
#undef READ_CONST
}