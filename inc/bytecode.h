#ifndef BYTECODE_H
#define BYTECODE_H
/*

32-bit bytecode format(MSB -> LSB):

|OPCD|REGA|REGB|REGC|XXXX|XXXX|XXXX|XXXX|

.text
    ldr $r01 $r00 $r00 0xFFFFABCD

(!) Since 0xFFFFABCD > 0xFFFF, perform staged immediate encoding

.text
    orr $r01 $r00 $r00 0xFFFF
    shl $r01 $r00 $r00 0x4
    ldr $r01 $r00 $r01 0xABCD

*/

typedef unsigned int bytecode;

#define encode(                             \
    OPCODE,                                 \
    REGISTER_A,                             \
    REGISTER_B,                             \
    REGISTER_C,                             \
    IMMEDIATE                               \
) (                                         \
    (OPCODE << 28) |                        \
    (REGISTER_A << 24) |                    \
    (REGISTER_B << 20) |                    \
    (REGISTER_C << 16) |                    \
    (IMMEDIATE & 0xFFFF)                    \
)

#define decode(                             \
    BYTECODE,                               \
    OPCODE,                                 \
    REGISTER_A,                             \
    REGISTER_B,                             \
    REGISTER_C,                             \
    IMMEDIATE                               \
) {                                         \
    OPCODE = (BYTECODE >> 28) & 0xF;        \
    REGISTER_A = (BYTECODE >> 24) & 0xF;    \
    REGISTER_B = (BYTECODE >> 20) & 0xF;    \
    REGISTER_C = (BYTECODE >> 16) & 0xF;    \
    IMMEDIATE = (BYTECODE & 0xFFFF);        \
}

typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,                 // Arithmetic
    OP_AND, OP_ORR, OP_SHL, OP_SHR,                 // Logic
    OP_JEQ, OP_JNE, OP_JGT, OP_JLT, OP_JGE, OP_JLE, // Control
    OP_LDR, OP_STR                                  // Memory
} __opcode;

typedef enum {
    REG_ZERO        = 0x0,
    REG_ONE         = 0x1,
    REG_TWO         = 0x2,
    REG_THREE       = 0x3,
    REG_FOUR        = 0x4,
    REG_FIVE        = 0x5,
    REG_SIX         = 0x6,
    REG_SEVEN       = 0x7,
    REG_EIGHT       = 0x8,
    REG_NINE        = 0x9,
    REG_TEN         = 0xA,
    REG_ELEVEN      = 0xB,
    REG_STACK       = 0xC,
    REG_FRAME       = 0xD,
    REG_RETURN      = 0xE,
    REG_COUNTER     = 0xF
} __register;

#endif