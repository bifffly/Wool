#ifndef WOOL_COMPILER_H
#define WOOL_COMPILER_H

#include "chunk.h"
#include "scanner.h"

typedef struct {
    Token current;
    Token previous;
    bool errorFlag;
    bool panicFlag;
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // :=
    PREC_OR,          // ||
    PREC_AND,         // &&
    PREC_EQUALITY,    // = <>
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)();

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

void literal();
void number();
void string();
void expression();
void unary();
void binary();
void grouping();

bool compile(const char* src, Chunk* chunk);

#endif
