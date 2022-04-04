#ifndef LEXER_H
#define LEXER_H

typedef enum {
    KEYWORD,
    IDENTIFIER,
    SEPARATOR,
    OPERATOR,
    LITERAL,
    TERMINATOR,
    ERROR
} token_type;

typedef struct {
    char* symbol;
    int length;
    token_type type;
} token;

token getNextToken(char** cursor_ptr);

#endif