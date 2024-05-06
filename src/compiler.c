#include <stdio.h>
#include "common.h"
#include "compiler.h"
#include "scanner.h"

void compile(const char* source) {
    initScanner(source);
    int line = -1;
    while (true) {
        Token token = scanToken();
        line = token.line;
        printf("(%d) %d '%.*s'\n", line, token.type, token.length, token.start);

        if (token.type == TOK_EOF) {
            break;
        }
    }
}