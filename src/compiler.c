#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "compiler.h"
#include "refstring.h"

// Helper functions

Parser parser;
Chunk* compilingChunk;

Chunk* currentChunk() {
    return compilingChunk;
}

void errorAt(Token* token, const char* message) {
    if (parser.panicFlag) {
        return;
    }
    parser.panicFlag = true;

    fprintf(stderr, "[line %d] ERROR", token->line);
    if (token->type == TOK_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type != TOK_ERR) {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.errorFlag = true;
}

static void error(const char* msg) {
    errorAt(&parser.previous, msg);
}

void errorAtCurrent(const char* msg) {
    errorAt(&parser.current, msg);
}

void advanceParser() {
    parser.previous = parser.current;

    while (true) {
        parser.current = scanToken();
        if (parser.current.type != TOK_ERR) {
            break;
        }
        errorAtCurrent(parser.current.start);
    }
}

void consume(TokenType type, const char* msg) {
    if (parser.current.type == type) {
        advanceParser();
        return;
    }

    errorAtCurrent(msg);
}

// Emission

void emitByte(uint8_t byte) {
    writeByteToChunk(currentChunk(), byte, parser.previous.line);
}

void emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

void emitReturn() {
    emitByte(OP_RET);
}

uint8_t makeConst(Value value) {
    int constant = writeValueToChunk(currentChunk(), value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t) constant;
}

void emitConst(Value value) {
    emitBytes(OP_CONST, makeConst(value));
}

// Parsing

ParseRule rules[] = {
    [TOK_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOK_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOK_PLUS] = {unary, binary, PREC_TERM},
    [TOK_MINUS] = {unary, binary, PREC_TERM},
    [TOK_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOK_STAR] = {NULL, binary, PREC_FACTOR},
    [TOK_BANG] = {unary, NULL, PREC_NONE},
    [TOK_NUM] = {number, NULL, PREC_NONE},
    [TOK_STR] = {string, NULL, PREC_NONE},
    [TOK_TRUE] = {literal, NULL, PREC_NONE},
    [TOK_FALSE] = {literal, NULL, PREC_NONE},
    [TOK_NULL] = {literal, NULL, PREC_NONE},
    [TOK_UNEQ] = {NULL, binary, PREC_EQUALITY},
    [TOK_EQ] = {NULL, binary, PREC_EQUALITY},
    [TOK_GT] = {NULL, binary, PREC_COMPARISON},
    [TOK_GE] = {NULL, binary, PREC_COMPARISON},
    [TOK_LT] = {NULL, binary, PREC_COMPARISON},
    [TOK_LE] = {NULL, binary, PREC_COMPARISON},
    [TOK_EOF] = {NULL, NULL, PREC_NONE}
};

ParseRule* getRule(TokenType type) {
    return &rules[type];
}

void parsePrecedence(Precedence prec) {
    advanceParser();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL) {
        error("Expect expression.");
        return;
    }

    prefixRule();

    while (prec <= getRule(parser.current.type)->precedence) {
        advanceParser();
        TokenType prevType = parser.previous.type;
        ParseFn infixRule = getRule(prevType)->infix;
        infixRule();
    }
}

void literal() {
    switch (parser.previous.type) {
        case TOK_FALSE: emitByte(OP_FALSE); break;
        case TOK_NULL: emitByte(OP_NULL); break;
        case TOK_TRUE: emitByte(OP_TRUE); break;
        default: return;
    }
}

void number() {
    double val = strtod(parser.previous.start, NULL);
    emitConst(NUM_VAL(val));
}

void string() {
    emitConst(REF_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2)));
}

void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

void unary() {
    TokenType operatorType = parser.previous.type;

    parsePrecedence(PREC_UNARY);

    switch (operatorType) {
        case TOK_MINUS: emitByte(OP_NEG); break;
        case TOK_BANG: emitByte(OP_NOT); break;
        default: return;
    }
}

void binary() {
    TokenType operatorType = parser.previous.type;
    ParseRule* rule = getRule(operatorType);
    parsePrecedence((Precedence) (rule->precedence + 1));

    switch (operatorType) {
        case TOK_PLUS: emitByte(OP_ADD); break;
        case TOK_MINUS: emitByte(OP_SUB); break;
        case TOK_STAR: emitByte(OP_MULT); break;
        case TOK_SLASH: emitByte(OP_DIV); break;
        case TOK_UNEQ: emitBytes(OP_EQ, OP_NOT); break;
        case TOK_EQ: emitByte(OP_EQ); break;
        case TOK_GT: emitByte(OP_GCMP); break;
        case TOK_GE: emitBytes(OP_LCMP, OP_NOT); break;
        case TOK_LT: emitByte(OP_LCMP); break;
        case TOK_LE: emitBytes(OP_GCMP, OP_NOT); break;
        default: return;
    }
}

void grouping() {
    expression();
    consume(TOK_RIGHT_PAREN, "Expect ')' after expression.");
}

// Compilation

void endCompiler() {
    emitReturn();
}

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    compilingChunk = chunk;
    parser.errorFlag = false;

    advanceParser();
    expression();
    consume(TOK_EOF, "Expect end of expression.");
    endCompiler();

    return !parser.errorFlag;
}