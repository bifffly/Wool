#include <string.h>
#include "common.h"
#include "scanner.h"

Scanner scanner;

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || (c == '_');
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isAtEnd() {
    return *scanner.start == '\0';
}

char advance() {
    scanner.current++;
    return scanner.current[-1];
}

char peek() {
    return *scanner.current;
}

char peekNext() {
    if (isAtEnd()) {
        return '\0';
    }
    return scanner.current[1];
}

bool match(char expected) {
    if (isAtEnd() || *scanner.current != expected) {
        return false;
    }
    scanner.current++;
    return true;
}

TokenType check(int start, int length, const char* rest, TokenType type) {
    if (scanner.current - scanner.start == start + length
        && memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }

    return TOK_IDENT;
}

Token makeToken(TokenType type) {
    Token tok;
    tok.type = type;
    tok.start = scanner.start;
    tok.length = (int) (scanner.current - scanner.start);
    tok.line = scanner.line;

    return tok;
}

Token errorToken(const char* msg) {
    Token tok;
    tok.type = TOK_ERR;
    tok.start = msg;
    tok.length = (int) strlen(msg);
    tok.line = scanner.line;

    return tok;
}

void skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\t':
            case '\r': {
                advance();
                break;
            }
            case '\n': {
                scanner.line++;
                advance();
                break;
            }
            case '#': {
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
                break;
            }
            default: return;
        }
    }
}

TokenType identifierType() {
    switch (scanner.start[0]) {
        case 'e': {
            if (scanner.current - scanner.start > 2 && scanner.start[1] == 'l') {
                switch (scanner.start[2]) {
                    case 'i': return check(3, 1, "f", TOK_ELIF);
                    case 's': return check(3, 1, "e", TOK_ELSE);
                }
            }
            break;
        }
        case 'f': {
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a': return check(2, 3, "lse", TOK_FALSE);
                    case 'o': return check(2, 1, "r", TOK_FOR);
                    case 'u': return check(2, 2, "nc", TOK_FUNC);
                }
            }
            break;
        }
        case 'i': return check(1, 1, "f", TOK_IF);
        case 'l': return check(1, 2, "et", TOK_LET);
        case 'n': return check(1, 3, "ull", TOK_NULL);
        case 'r': return check(1, 5, "eturn", TOK_RETURN);
        case 's': return check(1, 5, "truct", TOK_STRUCT);
        case 't': return check(1, 3, "rue", TOK_TRUE);
        case 'w': return check(1, 4, "hile", TOK_WHILE);
    }
    return TOK_IDENT;
}

Token scanString(char delim) {
    while (peek() != delim && !isAtEnd()) {
        if (peek() == '\n') {
            scanner.line++;
        }
        advance();
    }

    if (isAtEnd()) {
        return errorToken("Unexpected newline within scanString.");
    }

    advance();
    return makeToken(TOK_STR);
}

Token scanNumber() {
    while (isDigit(peek())) {
        advance();
    }

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) {
            advance();
        }
    }

    return makeToken(TOK_NUM);
}

Token scanIdentifier() {
    while (isAlpha(peek()) || isDigit(peek())) {
        advance();
    }

    return makeToken(identifierType());
}

Token scanToken() {
    skipWhitespace();
    scanner.start = scanner.current;

    if (isAtEnd()) {
        return makeToken(TOK_EOF);
    }

    char c = advance();
    if (isDigit(c)) {
        return scanNumber();
    }
    if (isAlpha(c)) {
        return scanIdentifier();
    }
    switch (c) {
        case '(': return makeToken(TOK_LEFT_PAREN);
        case ')': return makeToken(TOK_RIGHT_PAREN);
        case '{': return makeToken(TOK_LEFT_BRACE);
        case '}': return makeToken(TOK_RIGHT_BRACE);
        case '[': return makeToken(TOK_LEFT_BRACKET);
        case ']': return makeToken(TOK_RIGHT_BRACKET);
        case ',': return makeToken(TOK_COMMA);
        case '.': return makeToken(TOK_POINT);
        case ';': return makeToken(TOK_SEMICOLON);
        case '+': return makeToken(TOK_PLUS);
        case '-': return makeToken(TOK_MINUS);
        case '/': return makeToken(TOK_SLASH);
        case '*': return makeToken(TOK_STAR);
        case '!': return makeToken(TOK_BANG);
        case '=': return makeToken(TOK_EQ);
        case ':': return makeToken(match('=') ? TOK_WALRUS : TOK_COLON);
        case '&': return makeToken(match('&') ? TOK_AND : TOK_BITAND);
        case '|': return makeToken(match('|') ? TOK_OR : TOK_BITOR);
        case '>': return makeToken(match('=') ? TOK_GE : TOK_GT);
        case '<': return makeToken(match('=') ? TOK_LE : match('>') ? TOK_UNEQ : TOK_LT);
        case '\'':
        case '\"': return scanString(c);
    }

    return errorToken("Unexpected character.");
}