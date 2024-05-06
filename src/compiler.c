#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "compiler.h"
#include "refstring.h"
#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

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

bool checkParser(TokenType type) {
    return parser.current.type == type;
}

bool matchParser(TokenType type) {
    if (!checkParser(type)) {
        return false;
    }
    advanceParser();
    return true;
}

void consume(TokenType type, const char* msg) {
    if (parser.current.type == type) {
        advanceParser();
        return;
    }

    errorAtCurrent(msg);
}

void synchronize() {
    parser.panicFlag = false;

    while (parser.current.type != TOK_EOF) {
        if (parser.previous.type == TOK_SEMICOLON) return;
        switch (parser.current.type) {
            case TOK_STRUCT:
            case TOK_FUNC:
            case TOK_LET:
            case TOK_FOR:
            case TOK_IF:
            case TOK_WHILE:
            case TOK_RETURN: return;
            default:;
        }

        advanceParser();
    }
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
    [TOK_IDENT] = {variable, NULL, PREC_NONE},
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

    bool canAssign = prec <= PREC_ASSIGNMENT;
    prefixRule(canAssign);

    while (prec <= getRule(parser.current.type)->precedence) {
        advanceParser();
        TokenType prevType = parser.previous.type;
        ParseFn infixRule = getRule(prevType)->infix;
        infixRule(canAssign);
    }

    if (canAssign && matchParser(TOK_WALRUS)) {
        error("Invalid assignment target.");
    }
}

uint8_t identConst(Token* name) {
    return makeConst(REF_VAL(copyString(name->start, name->length)));
}

uint8_t parseVariable(const char* errorMessage) {
    consume(TOK_IDENT, errorMessage);
    return identConst(&parser.previous);
}

void defineVariable(uint8_t global) {
    emitBytes(OP_DEFG, global);
}

// Expressions

void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

void literal(bool canAssign) {
    switch (parser.previous.type) {
        case TOK_FALSE: emitByte(OP_FALSE); break;
        case TOK_NULL: emitByte(OP_NULL); break;
        case TOK_TRUE: emitByte(OP_TRUE); break;
        default: return;
    }
}

void namedVariable(Token name, bool canAssign) {
    uint8_t arg = identConst(&name);
    if (canAssign && matchParser(TOK_WALRUS)) {
        expression();
        emitBytes(OP_SETG, arg);
    } else {
        emitBytes(OP_GETG, arg);
    }
}

void variable(bool canAssign) {
    namedVariable(parser.previous, canAssign);
}

void number(bool canAssign) {
    double val = strtod(parser.previous.start, NULL);
    emitConst(NUM_VAL(val));
}

void string(bool canAssign) {
    emitConst(REF_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2)));
}

void unary(bool canAssign) {
    TokenType operatorType = parser.previous.type;

    parsePrecedence(PREC_UNARY);

    switch (operatorType) {
        case TOK_MINUS: emitByte(OP_NEG); break;
        case TOK_BANG: emitByte(OP_NOT); break;
        default: return;
    }
}

void binary(bool canAssign) {
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

void grouping(bool canAssign) {
    expression();
    consume(TOK_RIGHT_PAREN, "Expect ')' after expression.");
}

// Statements

void expressionStatement() {
    expression();
    consume(TOK_SEMICOLON, "Expect ';' after expression.");
    emitByte(OP_POP);
}

void statement() {
    expressionStatement();

    if (parser.panicFlag) {
        synchronize();
    }
}

void letDeclaration() {
    uint8_t global = parseVariable("Expect variable name");

    if (matchParser(TOK_WALRUS)) {
        expression();
    } else {
        emitByte(OP_NULL);
    }
    consume(TOK_SEMICOLON, "Expect ';' after variable declaration");

    defineVariable(global);
}

void declaration() {
    if (matchParser(TOK_LET)) {
        letDeclaration();
    } else {
        statement();
    }
}

// Compilation

void endCompiler() {
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser.errorFlag) {
        debugChunk(currentChunk(), "code");
    }
#endif
}

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    compilingChunk = chunk;
    parser.errorFlag = false;

    advanceParser();
    while (!matchParser(TOK_EOF)) {
        declaration();
    }
    endCompiler();

    return !parser.errorFlag;
}