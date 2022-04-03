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

char registers[][3] = {
    "$00", "$01", "$02", "$03", "$04", "$05", "$06", "$07", 
    "$08", "$09", "$10", "$11", "$12", "$13", "$14", "$15"
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
            if (!strncmp(tok->symbol, registers[idx], sizeof(registers[0]))) {
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

static inline void rewindCursor(token* tok, compilation_unit* compiler) {
    compiler->cursor = tok->symbol;
}

compilation_unit compileFromSource(char* source) {
    // No preprocessing, all symbols are directly converted to bytecode without intermediate steps
    compilation_unit compiler = {
        .cursor = source,
        .error = 0,

        .data_section_top = 0,

        .symbol_table_top = 0,
        .symbol_table = malloc(sizeof(struct symbol_table_entry) * 256),

        .executable_top = 0,
        .executable = malloc(sizeof(bytecode) * 256)
    };

    do {
        token next = getNextToken(&compiler.cursor);
        
        printf("%.*s\n", next.length, next.symbol);

        if (next.type == IDENTIFIER) {
            // Identifier is being used to mark instruction address
            token token1 = getNextToken(&compiler.cursor);
            if ((token1.type == OPERATOR)) {
                int identifier = evalIdentifier(&next, &compiler);

                compiler.symbol_table[identifier].address = compiler.executable_top;
                
                rewindCursor(&token1, &compiler);
                continue;
            }

            // Identifier is being used as a variable declaration
            token token2 = getNextToken(&compiler.cursor);
            if ((token1.type == KEYWORD) && (token2.type == LITERAL)) {
                int identifier = evalIdentifier(&next, &compiler);
                int keyword = evalKeyword(&token1, &compiler);
                int literal = evalLiteral(&token2, &compiler);

                // Generate instructions to push literal onto stack
                compiler.executable[compiler.executable_top++] = encode(
                    OP_ORR, 
                    REG_RESERVED, 
                    REG_ZERO, 
                    REG_ZERO, 
                    literal
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

                compiler.symbol_table[identifier].address = compiler.data_section_top++;

                continue;
            }
            
        } else if (next.type == OPERATOR) {
            token token1 = getNextToken(&compiler.cursor);
            token token2 = getNextToken(&compiler.cursor);
            token token3 = getNextToken(&compiler.cursor);
            token token4 = getNextToken(&compiler.cursor);
            
            if (
                (token1.type == IDENTIFIER) && 
                (token2.type == IDENTIFIER) &&
                (token3.type == IDENTIFIER)
            ) {
                int operator = evalOperator(&next, &compiler);
                int register_a = evalIdentifier(&token1, &compiler);
                int register_b = evalIdentifier(&token2, &compiler);
                int register_c = evalIdentifier(&token3, &compiler);
                int literal = 0;

                if (token4.type == LITERAL) {
                    literal = evalLiteral(&token4, &compiler);
                } else if (token4.type == IDENTIFIER) {
                    literal = compiler.symbol_table[evalIdentifier(&token4, &compiler)].address;
                }
                          
                compiler.executable[compiler.executable_top++] = encode(
                    operator, 
                    register_a, 
                    register_b, 
                    register_c, 
                    literal
                );
                
                continue;
            }
        }

        if (next.type == TERMINATOR || next.type == ERROR) {

            break;
        }

    } while(!compiler.error);

    printf("%d\n", compiler.error);

    return compiler;
}