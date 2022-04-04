#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "bytecode.h"
#include "parser.h"

// bytecode32b: |OPCD|REGA|REGB|REGC|XXXX|XXXX|XXXX|XXXX|

void printRegisters(int registers[]) {
    for (int i = 0; i < 16; i++) {
        printf("$r%.2d = %d\n", i, registers[i]);
    }
    printf("\n");
}

void printMemory(int memory[]) {
    for (int i = 0; i < 10; i++) {
        printf("stack[%d] = %d\n", i, memory[i]);
    }
    printf("\n");
}

int ioReadFile(const char* path, char* buffer[], int* length) {
    FILE* file = fopen(path, "r");

    if (file == NULL) {
        return -1;
    }

    fseek(file, 0L, SEEK_END);
    long __length = ftell(file) * sizeof(char);
    fseek(file, 0L, SEEK_SET);

    char* __buffer = (char*)malloc(__length);
    fread((void*)__buffer, 1, __length, file);

    *buffer = __buffer;
    *length = __length;

    return 0;
}


int execute(bytecode program[]) {
    int registers[16] = {0};
    int memory[20] = {0};

    int opcode = 0;
    int reg_a = 0;
    int reg_b = 0;
    int reg_c = 0;
    int immediate = 0;

    bytecode instruction = 0;

    int instruction_n = 0;

    while (registers[REG_COUNTER] < 20) {
        instruction = program[registers[REG_COUNTER]++];
        instruction_n++;

        decode(instruction, opcode, reg_a, reg_b, reg_c, immediate);

        //printf("%d, %d, %d, %d, %d\n", opcode, reg_a, reg_b, reg_c, immediate);

        switch (opcode) {
        case OP_ADD:
            registers[reg_a] = registers[reg_b] + (registers[reg_c] + immediate);
            break;    
        case OP_SUB:
            registers[reg_a] = registers[reg_b] - (registers[reg_c] + immediate);
            break; 
        case OP_MUL:
            registers[reg_a] = registers[reg_b] * (registers[reg_c] + immediate);
            break; 
        case OP_DIV:
            registers[reg_a] = registers[reg_b] / (registers[reg_c] + immediate);
            break;
        case OP_AND:
            registers[reg_a] = registers[reg_b] & (registers[reg_c] + immediate);
            break;
        case OP_ORR:
            registers[reg_a] = registers[reg_b] | (registers[reg_c] + immediate);
            break; 
        case OP_SHL:
            registers[reg_a] = registers[reg_b] << (registers[reg_c] + immediate);
            break;
        case OP_SHR:
            registers[reg_a] = registers[reg_b] >> (registers[reg_c] + immediate);
            break;
        case OP_JEQ:
            if (registers[reg_a] == registers[reg_b]) {
                registers[REG_COUNTER] = (registers[reg_c] + immediate);
            }
            break;
        case OP_JNE:
            if (registers[reg_a] != registers[reg_b]) {
                registers[REG_COUNTER] = (registers[reg_c] + immediate);
            }
            break;
        case OP_JGT:
            if (registers[reg_a] > registers[reg_b]) {
                registers[REG_COUNTER] = (registers[reg_c] + immediate);
            }
            break;
        case OP_JLT:
            if (registers[reg_a] < registers[reg_b]) {
                registers[REG_COUNTER] = (registers[reg_c] + immediate);
            }
            break;
        case OP_JGE:
            if (registers[reg_a] >= registers[reg_b]) {
                registers[REG_COUNTER] = (registers[reg_c] + immediate);
            }
            break;
        case OP_JLE:
            if (registers[reg_a] <= registers[reg_b]) {
                registers[REG_COUNTER] = (registers[reg_c] + immediate);
            }
            break;
        case OP_STR:
            memory[registers[reg_c] + immediate] = registers[reg_a];
            break;
        case OP_LDR:
            registers[reg_a] = memory[registers[reg_c] + immediate];  
            break;
        }
    }

    return instruction_n;
}

int main(int argc, char* argv[]) {
    char* source; 
    int length; 
    int err = ioReadFile(argv[1], &source, &length);

    if (err) {
        return -1;
    }

    compilation_unit compiler = compileFromSource(source);

    for (int i = 0; i < compiler.symbol_table_top; i++) {
        printf("%d: %s = %d\n", i, compiler.symbol_table[i].symbol, compiler.symbol_table[i].address);
    }
    printf("\n");

    for (int i = 0; i < compiler.executable_top; i++) {
        printf("%d: 0x%.8X\n", i, compiler.executable[i]);
    }
    printf("\n");

    clock_t tb_start = clock();
    int n = execute(compiler.executable);
    clock_t tb_end = clock();

    double latency = (double)(tb_end - tb_start) / CLOCKS_PER_SEC;

    printf("Run in %lf seconds.\n", latency);
    printf("Executed %lf instructions per second.\n", n / latency);

    return 0;
}