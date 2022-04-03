#include "lexer.h"
#include "parser.h"

// TODO: convert lookup tables to hashtables for more efficient compilation

char keywords[][5] = {
    ".data", ".text", // Compiler section modes
    ".word", ".byte" // Identifier types
};

char operators[][3] = {
    "add", "sub", "mul", "div",                 // Arithmetic
    "and", "orr", "shl", "shr",                 // Logic
    "jeq", "jne", "jgt", "jlt", "jge", "jle",   // Control
    "ldr", "str",                               // Memory
};

char registers[][4] = {
    "$R00", "$R01", "$R02", "$R03", "$R04", "$R05", "$R06", "$R07", 
    "$R08", "$R09", "$R10", "$R11", "$R12", "$R13", "$R14", "$R15"
};

int evalKeyword(token* tok, compilation_unit* compiler) {
    if (tok->length == 5) {
        for (int idx = 0; idx < sizeof(keywords) / sizeof(keywords[0]); idx++) {
            if (!strncmp(tok->symbol, keywords[idx], 5)) {
                return idx;
            }
        }
    }

    compiler->error = ERROR_UNKNOWN_SYMBOL;
    return 0;    
}

int evalOperator(token* tok, compilation_unit* compiler) {
    if (tok->length == 3) {
        for (int idx = 0; idx < sizeof(operators) / sizeof(operators[0]); idx++) {
            if (!strncmp(tok->symbol, operators[idx], 3)) {
                return idx;
            }
        }
    }
    
    compiler->error = ERROR_UNKNOWN_SYMBOL;
    return 0;
}

int evalIdentifier(token* tok, compilation_unit* compiler) {
    // consider using hashtables
    if (tok->symbol[0] == '$') {
        for (int idx = 0; idx < sizeof(registers) / sizeof(registers[0]); idx++) {
            if (!strncmp(tok->symbol, registers[idx], 4)) {
                return idx;
            }
        }
    } else if (tok->symbol[0] == '@') {
        // Search for symbol in symbol_table, if found return value
        for (int i = 0; i < compiler->n_symbols; i++) {
            if (!strncmp(tok->symbol, compiler->symbol_table[i].symbol, tok->length)) {
                return compiler->symbol_table[i].address;
            }
        }
        // If symbol not found, create one and return value
        strncpy(compiler->symbol_table[compiler->n_symbols].symbol, tok->symbol, tok->length);
        return compiler->n_symbols++;
    }

    compiler->error = ERROR_UNKNOWN_SYMBOL;
    return 0;
}

int evalLiteral(token* tok, compilation_unit* compiler) {
    unsigned int value = 0;

    for (int i = 0; i < tok->length; i++) {
        unsigned int temp = value * 10 + (tok->symbol[i] - '0');
        // Overflow test
        if (temp > value) {
            value = temp;
        } else {
            value = 0xFFFFFFFF;
        }
    }

    return value;
}

compilation_unit compileFromSource(char* source) {
    compilation_unit compiler = {
        .cursor = source,
        .error = 0,

        .n_symbols = 0,
        .symbol_table = malloc(sizeof(struct symbol_table_entry) * 256)
    };

    return compiler;
}