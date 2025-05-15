//
// Created by Daniel Shelgunov on 10/05/2025.
//

#ifndef CHIP8_H
#define CHIP8_H

// 8 bits
typedef unsigned char BYTE;
// 16 bits
typedef unsigned short WORD;

typedef enum returnCode {
    CORRECT_EXIT,
    UNRECOGNIZED_OPCODE,
    STACK_OVERFLOW,
    STACK_UNDERFLOW,
    FAILED_TO_LOAD_ROM,
} ReturnCode;

int CHIP8_extractMSB(WORD);
int CHIP8_extractNNN(WORD);
int CHIP8_extractN(WORD);
int CHIP8_extractX(WORD);
int CHIP8_extractY(WORD);
int CHIP8_extractKK(WORD);

ReturnCode CHIP8_loadROM(const char* path);
ReturnCode CHIP8_fetchOp();

extern BYTE vmem[24];

#endif //CHIP8_H
