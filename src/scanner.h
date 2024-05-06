#ifndef WOOL_SCANNER_H
#define WOOL_SCANNER_H

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

typedef enum {
    TOK_LEFT_PAREN, TOK_RIGHT_PAREN,
    TOK_LEFT_BRACE, TOK_RIGHT_BRACE,
    TOK_LEFT_BRACKET, TOK_RIGHT_BRACKET,
    TOK_COMMA, TOK_POINT,
    TOK_COLON, TOK_SEMICOLON,
    TOK_PLUS, TOK_MINUS, TOK_SLASH, TOK_STAR,

    TOK_BANG, TOK_AND, TOK_OR,
    TOK_BITAND, TOK_BITOR,
    TOK_EQ, TOK_UNEQ,
    TOK_GT, TOK_GE,
    TOK_LT, TOK_LE,
    TOK_WALRUS,

    TOK_IDENT, TOK_STR, TOK_NUM,

    TOK_FUNC, TOK_STRUCT,
    TOK_IF, TOK_ELIF, TOK_ELSE,
    TOK_LET, TOK_FOR, TOK_WHILE,
    TOK_TRUE, TOK_FALSE,
    TOK_NULL, TOK_RETURN,

    TOK_ERR, TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

void initScanner(const char* src);
Token scanToken();

#endif
