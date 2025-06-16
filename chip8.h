//
// Created by Daniel Shelgunov on 10/05/2025.
//

#ifndef CHIP8_H
#define CHIP8_H

// screen dimensions measured in pixels
#define CHIP8_SCREEN_WIDTH 64
#define CHIP8_SCREEN_HEIGHT 32

// 8 bits
typedef unsigned char BYTE;
// 16 bits
typedef unsigned short WORD;

typedef enum returnCode {
    CORRECT_EXIT,
    FAILED,
    UNRECOGNIZED_OPCODE,
    STACK_OVERFLOW,
    STACK_UNDERFLOW,
    FAILED_TO_LOAD_ROM,
} ReturnCode;

extern BYTE vmem[CHIP8_SCREEN_HEIGHT][CHIP8_SCREEN_WIDTH];
extern BYTE keys[0x10];

int CHIP8_extractMSB(WORD);
int CHIP8_extractNNN(WORD);
int CHIP8_extractN(WORD);
int CHIP8_extractX(WORD);
int CHIP8_extractY(WORD);
int CHIP8_extractKK(WORD);

ReturnCode CHIP8_init();

ReturnCode CHIP8_push(WORD);
ReturnCode CHIP8_pop(WORD*);

ReturnCode CHIP8_loadROM(const char* path);
ReturnCode CHIP8_decodeOp();

ReturnCode CHIP8_decode0x0Subset(WORD);
ReturnCode CHIP8_decode0x1Subset(WORD);
ReturnCode CHIP8_decode0x2Subset(WORD);
ReturnCode CHIP8_decode0x3Subset(WORD);
ReturnCode CHIP8_decode0x4Subset(WORD);
ReturnCode CHIP8_decode0x5Subset(WORD);
ReturnCode CHIP8_decode0x6Subset(WORD);
ReturnCode CHIP8_decode0x7Subset(WORD);
ReturnCode CHIP8_decode0x8Subset(WORD);
ReturnCode CHIP8_decode0x9Subset(WORD);
ReturnCode CHIP8_decode0xASubset(WORD);
ReturnCode CHIP8_decode0xBSubset(WORD);
ReturnCode CHIP8_decode0xCSubset(WORD);
ReturnCode CHIP8_decode0xDSubset(WORD);
ReturnCode CHIP8_decode0xESubset(WORD);
ReturnCode CHIP8_decode0xFSubset(WORD);

#endif //CHIP8_H
