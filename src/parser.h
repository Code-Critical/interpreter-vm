#ifndef PARSER_H
#define PARSER_H

#include "bytecode.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Error codes

#define ERROR_NONE                      0x00
#define ERROR_TERMINATOR_SYMBOL         0x01
#define ERROR_UNKNOWN_SYMBOL            0x02
#define ERROR_UNEXPECTED_SYMBOL         0x03

struct symbol_table_entry {
    int address;
    char symbol[64];
};

typedef struct {
    char* cursor;
    int error;

    int symbol_table_top;
    struct symbol_table_entry* symbol_table;
    
    int executable_top;
    bytecode* executable;
} compilation_unit;

compilation_unit compileFromSource(char* source);

#endif