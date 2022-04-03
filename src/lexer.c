#include "lexer.h"

char* getText(char* source) {
    char* cursor = source;

    for (; *cursor != '\0'; cursor++) {
        if (
            (*cursor >= 'a') && (*cursor <= 'z') ||
            (*cursor >= 'A') && (*cursor <= 'Z') ||
            (*cursor >= '0') && (*cursor <= '9') ||
            (*cursor == '_')        
        ) {
            continue;
        }
        break;
    }

    return cursor;
}

char* getNumber(char* source) {
    char* cursor = source;

    for (; *cursor != '\0'; cursor++) {
        if (
            (*cursor >= '0') && (*cursor <= '9') ||
            (*cursor == '.')
        ) {
            continue;
        }
        break;
    }

    return cursor;
}

char* getOperator(char* source) {
    char* cursor = source;

    for (; *cursor != '\0'; cursor++) {
        if (
            (*cursor == '=') ||
            (*cursor == '>') ||
            (*cursor == '<') ||
            (*cursor == '+') ||
            (*cursor == '-') ||
            (*cursor == '*') ||
            (*cursor == '/') ||
            (*cursor == '&') ||
            (*cursor == '^') ||
            (*cursor == '!')
        ) {
            continue;
        }
        break;
    }

    return cursor;
}

char* getSeparator(char* source) {
    char* cursor = source;

    for (; *cursor != '\0'; cursor++) {
        if (
            (*cursor == '\'') ||
            (*cursor == '"') ||
            (*cursor == ',') ||
            (*cursor == ';') ||
            (*cursor == '(') ||
            (*cursor == ')') ||
            (*cursor == '[') ||
            (*cursor == ']') ||
            (*cursor == '{') ||
            (*cursor == '}')           
        ) {
            continue;
        }
        break;
    }

    return cursor;    
}

token getNextToken(char** cursor_ptr) {
    char* cursor = *cursor_ptr;
    char* update = cursor;
    token next;

    while (
        (*cursor == ' ') ||
        (*cursor == '\t') ||
        (*cursor == '\n')
    ) {
        cursor++;
    }

    if (
        (*cursor == '@') || 
        (*cursor == '$')
    ) {
        update = getText(cursor + 1);
        next.type = IDENTIFIER;
    } else if (
        (*cursor == '.')
    ) {
        update = getText(cursor + 1);
        next.type = KEYWORD;    
    } else if (
        (*cursor >= 'a') && (*cursor <= 'z') ||
        (*cursor >= 'A') && (*cursor <= 'Z')
    ) {
        update = getText(cursor + 1);
        next.type = OPERATOR;
    } else if (
        (*cursor >= '0' ) && (*cursor <= '9')
    ) {
        update = getNumber(cursor + 1);
        next.type = LITERAL;
    } else if (
        (*cursor == '\'') ||
        (*cursor == '"') ||
        (*cursor == ',') ||
        (*cursor == ';') ||
        (*cursor == '(') ||
        (*cursor == ')') ||
        (*cursor == '[') ||
        (*cursor == ']') ||
        (*cursor == '{') ||
        (*cursor == '}')           
    ) {
        update = getSeparator(cursor + 1);
        next.type = SEPARATOR;
    } else if (
        (*cursor == '\0')
    ) {
        next.type = TERMINATOR;
    } else {
        next.type = ERROR;
    }
    
    *cursor_ptr = update;

    next.symbol = cursor;
    next.length = update - cursor;

    return next;
}