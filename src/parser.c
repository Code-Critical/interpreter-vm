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
        for (int idx = 0; idx < compiler->symbol_table_top; idx++) {
            if (!strncmp(tok->symbol, compiler->symbol_table[idx].symbol, tok->length)) {
                return idx;
            }
        }
        // If symbol not found, create one and return value
        strncpy(compiler->symbol_table[compiler->symbol_table_top].symbol, tok->symbol, tok->length);
        return compiler->symbol_table_top++;
    }

    compiler->error = ERROR_UNKNOWN_SYMBOL;
    return 0;
}

int evalLiteral(token* tok, compilation_unit* compiler) {
    unsigned int value = 0;

    for (int idx = 0; idx < tok->length; idx++) {
        unsigned int temp = value * 10 + (tok->symbol[idx] - '0');
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
    // No preprocessing, all symbols are directly converted to bytecode without intermediate steps
    compilation_unit compiler = {
        .cursor = source,
        .error = 0,

        .symbol_table_top = 0,
        .symbol_table = malloc(sizeof(struct symbol_table_entry) * 256),

        .executable_top = 0,
        .executable = malloc(sizeof(bytecode) * 256)
    };

    do {
        token next = getNextToken(&compiler.cursor);
        
        printf("%.*s\n", next.length, next.symbol);

        if (next.type == IDENTIFIER) {
            token keyword = getNextToken(&compiler.cursor);
            token literal = getNextToken(&compiler.cursor);

            if ((keyword.type == KEYWORD) && (literal.type == LITERAL)) {
                int identifier_idx = evalIdentifier(&next, &compiler);
                int keyword_idx = evalKeyword(&keyword, &compiler);
                int literal_idx = evalLiteral(&literal, &compiler);

                printf("%d, %d, %d\n", identifier_idx, keyword_idx, literal_idx);

                // Generate instructions to push literal onto stack
                compiler.executable[compiler.executable_top++] = encode(
                    OP_ORR, 
                    REG_RESERVED, 
                    REG_ZERO, 
                    REG_ZERO, 
                    literal_idx
                );
                compiler.executable[compiler.executable_top++] = encode(
                    OP_STR, 
                    REG_RESERVED, 
                    REG_ZERO, 
                    REG_STACK, 
                    0
                );
                compiler.executable[compiler.executable_top++] = encode(
                    OP_ADD, 
                    REG_STACK, 
                    REG_STACK, 
                    REG_ZERO, 
                    1
                );
            }
        }

        if (next.type == TERMINATOR || next.type == ERROR) {
            break;
        }

    } while(!compiler.error);

    for (int i = 0; i < compiler.symbol_table_top; i++) {
        printf("%d: %s = %d\n", i, compiler.symbol_table[i].symbol, compiler.symbol_table[i].address);
    }
    
    printf("\n");

    for (int i = 0; i < compiler.executable_top; i++) {
        printf("%d: 0x%.8X\n", i, compiler.executable[i]);
    }

    return compiler;
}