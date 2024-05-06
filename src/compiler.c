#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "compiler.h"

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
    [TOK_NUM] = {number, NULL, PREC_NONE}
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
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}

void number() {
    double val = strtod(parser.previous.start, NULL);
    emitConst(val);
}

void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

void unary() {
    TokenType operatorType = parser.previous.type;

    parsePrecedence(PREC_UNARY);

    switch (operatorType) {
        case TOK_MINUS: emitByte(OP_NEG); break;
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