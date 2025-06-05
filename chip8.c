//
// Created by Daniel Shelgunov on 10/05/2025.
//

#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// start address of the code segment (CS)
#define CS 0x200
// overall memory of CHIP8
#define MEM_SIZE 0xFFF

// user space registers declarations
BYTE reg[0xF];

// creating a shortcut for VF register for convenience
#define VF reg[0xF - 0x1]

WORD I;

BYTE delayTimer;
BYTE soundTimer;

// super space registers declarations

// instructions start at 0x200
WORD IP = CS;
WORD stack[0xF];
BYTE SP;

// video mem
// this implementation uses a 64x32 screen
// the screen supporting black and white, so each
// pixel will have 2 states: 1 - on, 0 - off
BYTE vmem[CHIP8_SCREEN_HEIGHT][CHIP8_SCREEN_WIDTH];

// memory for our ROMs and sprites
BYTE mem[MEM_SIZE];

/// Initializes the fonts used by the emulator
/// by loading them into the memory
/// @return CORRECT_EXIT
ReturnCode CHIP8_init()
{
    // storing fonts as binary
    const char fonts[] = {
        0b11110000, // 0
        0b10010000,
        0b10010000,
        0b10010000,
        0b11110000,

        0b00100000, // 1
        0b01100000,
        0b00100000,
        0b00100000,
        0b01110000,

        0b11110000, // 2
        0b00010000,
        0b11110000,
        0b10000000,
        0b11110000,

        0b11110000, // 3
        0b00010000,
        0b11110000,
        0b00010000,
        0b11110000,

        0b10010000, // 4
        0b10010000,
        0b11110000,
        0b00010000,
        0b00010000,

        0b11110000, // 5
        0b10000000,
        0b11110000,
        0b00010000,
        0b11110000,

        0b11110000, // 6
        0b10000000,
        0b11110000,
        0b10010000,
        0b11110000,

        0b11110000, // 7
        0b00010000,
        0b00100000,
        0b01000000,
        0b01000000,

        0b11110000, // 8
        0b10010000,
        0b11110000,
        0b10010000,
        0b11110000,

        0b11110000, // 9
        0b10010000,
        0b11110000,
        0b00010000,
        0b11110000,

        0b11110000, // A
        0b10010000,
        0b11110000,
        0b10010000,
        0b10010000,

        0b11100000, // B
        0b10010000,
        0b11100000,
        0b10010000,
        0b11100000,

        0b11110000, // C
        0b10000000,
        0b10000000,
        0b10000000,
        0b11110000,

        0b11100000, // D
        0b10010000,
        0b10010000,
        0b10010000,
        0b11100000,

        0b11110000, // E
        0b10000000,
        0b11110000,
        0b10000000,
        0b11110000,

        0b11110000, // F
        0b10000000,
        0b11110000,
        0b10000000,
        0b10000000
    };

    // we will store the fonts in the memory, starting at 0x0
    memcpy(mem, fonts, sizeof(fonts));
    return CORRECT_EXIT;
}

/// The function returns the most significant 4 bits (MSB) of the opcode
/// @param op opcode
/// @return Most Significant Bit
int CHIP8_extractMSB(WORD op) {
    return (op & 0xF000) >> 0xC;
}

int CHIP8_extractNNN(WORD op) {
    return (op & 0x0FFF);
}

int CHIP8_extractN(WORD op) {
    return (op & 0x000F);
}

int CHIP8_extractX(WORD op) {
    return (op & 0x0F00) >> 0x8;
}

int CHIP8_extractY(WORD op) {
    return (op & 0x00F0) >> 0x4;
}

int CHIP8_extractKK(WORD op) {
    return (op & 0x00FF);
}

/// The function loads ROM at the given path into the memory (mem)
/// @param path path to the ROM
/// @return FAILED_TO_LOAD_ROM if anything gone wrong
///         CORRECT_EXIT if ROM was loaded
ReturnCode CHIP8_loadROM(const char* path) {
    FILE* fp = fopen(path, "rb");

    // check for errors while opening the given path
    if (fp == NULL) {
        return FAILED_TO_LOAD_ROM;
    }

    // read 3584 bytes from the ROM onto the CS (starting at 0x200)
    fread(mem + CS, sizeof(WORD), 0xDFF, fp);

    fclose(fp);

    return CORRECT_EXIT;
}

ReturnCode CHIP8_decodeOp() {
    if (IP > sizeof(mem)) {
        return UNRECOGNIZED_OPCODE;
    }

    // fetch current op
    WORD op = 0;

    // copy opcode from mem
    // since mem is of type BYTE, and WORD is double the space
    // use memcpy
    memcpy(&op, mem + IP, sizeof(WORD));

    // since x86 and ARM architectures typically use little-endian systems
    // we have to adjust the copied memory by swaping MSB and LSB
    // otherwise little endian puts the opcode as (LSB MSB)
    // for example, CLS opcode from 00E0 would become 0E00 without adjusting
    op = ((op & 0x00FF) << 8) | ((op & 0xFF00) >> 8);

    ReturnCode rc = CORRECT_EXIT;

    switch (CHIP8_extractMSB(op)) {
        case 0x0:
            rc = CHIP8_decode0x0Subset(op);
            break;
        case 0x1:
            rc = CHIP8_decode0x1Subset(op);
            break;
        case 0x2:
            rc = CHIP8_decode0x2Subset(op);
            break;
        case 0x3:
            rc = CHIP8_decode0x3Subset(op);
            break;
        case 0x4:
            rc = CHIP8_decode0x4Subset(op);
            break;
        case 0x5:
            rc = CHIP8_decode0x5Subset(op);
            break;
        case 0x6:
            rc = CHIP8_decode0x6Subset(op);
            break;
        case 0x7:
            rc = CHIP8_decode0x7Subset(op);
            break;
        case 0x8:
            rc = CHIP8_decode0x8Subset(op);
            break;
        case 0x9:
            rc = CHIP8_decode0x9Subset(op);
            break;
        case 0xA:
            rc = CHIP8_decode0xASubset(op);
            break;
        case 0xB:
            rc = CHIP8_decode0xBSubset(op);
            break;
        case 0xC:
            rc = CHIP8_decode0xCSubset(op);
            break;
        case 0xD:
            rc = CHIP8_decode0xDSubset(op);
            break;
        case 0xE:
            rc = CHIP8_decode0xESubset(op);
            break;
        case 0xF:
            rc = CHIP8_decode0xFSubset(op);
            break;
        default:
            break;
    }

    return rc;
}

/// The function increments SP and stores the data onto the stack
/// @param data data to be stored onto the stack
/// @return STACK_OVERFLOW if SP > 0xF (stack size), else CORRECT_EXIT
ReturnCode CHIP8_push(WORD data) {
    if (SP > 0xF) {
        return STACK_OVERFLOW;
    }

    stack[SP] = data;
    SP++;

    return CORRECT_EXIT;
}

/// The function returns top WORD from the stack and decrements SP,
/// and stores in into *data
/// @param data will store WORD from the stack
/// @return STACK_UNDERFLOW if SP < 0, else CORRECT_EXIT
ReturnCode CHIP8_pop(WORD* data) {
    *data = stack[SP];
    SP--;

    if (SP < 0) {
        return STACK_UNDERFLOW;
    }

    return CORRECT_EXIT;
}

/// Decodes CLS and RET opcodes
/// @param op Opcode
/// @return UNRECOGNIZED_OPCODE if the opcode is not CLS or RET |
///         STACK_UNDERFLOW if RET instruction fails |
///         CORRECT_EXIT if succeed
ReturnCode CHIP8_decode0x0Subset(WORD op) {
    int i = 0, j = 0;
    BYTE opType = 0;

    IP += 2;

    // since 0NNN opcode is obsolete, skipping it
    if (CHIP8_extractX(op) != 0) {
        return CORRECT_EXIT;
    }

    // unknown opcode, return error code
    if (CHIP8_extractY(op) != 0xE) {
        return UNRECOGNIZED_OPCODE;
    }

    opType = CHIP8_extractN(op);

    // 00E0 - CLS (clear screen)
    if (opType == 0x0) {
        for (i = 0; i < CHIP8_SCREEN_HEIGHT; i++) {
            for (j = 0; j < CHIP8_SCREEN_WIDTH; j++) {
                // set every pixel off
                vmem[i][j] = 0x0;
            }
        }

        return CORRECT_EXIT;
    }

    // 00EE - RET (return from subroutine)
    if (opType == 0xE) {
        // pop will return STACK_UNDERFLOW if an error occurred
        // else CORRECT_EXIT
        return CHIP8_pop(&IP);
    }

    return UNRECOGNIZED_OPCODE;
}

/// Decodes JP opcode.
/// Sets IP to NNN of the opcode
/// @param op Opcode
/// @return EXIT_FAILURE if tried to jump to an illegal address |
///         CORRECT_EXIT if succeed
ReturnCode CHIP8_decode0x1Subset(WORD op) {
    const WORD addr = CHIP8_extractNNN(op);

    // check if the address is inside the code segment
    // and didn't jump to and illegal space at mem
    if (addr < CS || addr >= MEM_SIZE)
    {
        return EXIT_FAILURE;
    }

    IP = addr;

    return CORRECT_EXIT;
}

/// Decodes CALL opcode
/// @param op Opcode
/// @return EXIT_FAILURE if tried to jump to an illegal address |
///         STACK_OVERFLOW if push failed |
///         CORRECT_EXIT if succeed
ReturnCode CHIP8_decode0x2Subset(WORD op) {
    const int addr = CHIP8_extractNNN(op);
    ReturnCode rc = CORRECT_EXIT;
    
    if (addr < CS || addr >= MEM_SIZE)
    {
        return EXIT_FAILURE;
    }

    // if push fails, rc will be set to STACK_OVERFLOW
    rc = CHIP8_push(IP);
    
    IP = addr;

    return rc;
}

/// Decodes SE (Vx, KK) opcode
/// @param op Opcode
/// @return CORRECT_EXIT
ReturnCode CHIP8_decode0x3Subset(WORD op) {
    const int val = CHIP8_extractKK(op);
    const int x = CHIP8_extractX(op);

    // if Vx == val, skip next instruction
    if (reg[x] == val)
        IP += 4;
    else
        IP += 2;

    return CORRECT_EXIT;
}

/// Decodes SNE opcode
/// @param op Opcode
/// @return CORRECT_EXIT
ReturnCode CHIP8_decode0x4Subset(WORD op) {
    const int val = CHIP8_extractKK(op);
    const int x = CHIP8_extractX(op);

    // if Vx != val, skip next instruction
    if (reg[x] != val)
        IP += 4;
    else
        IP += 2;

    return CORRECT_EXIT;
}

/// Decodes SE (Vx, Vy) opcode
/// @param op Opcode
/// @return CORRECT_EXIT
ReturnCode CHIP8_decode0x5Subset(WORD op) {
    const int x = CHIP8_extractX(op);
    const int y = CHIP8_extractY(op);

    // if Vx == Vy, skip next instruction
    if (reg[x] == reg[y])
        IP += 4;
    else
        IP += 2;

    return CORRECT_EXIT;
}

/// Decodes LD (Vx, KK) opcode
/// @param op Opcode
/// @return CORRECT_EXIT
ReturnCode CHIP8_decode0x6Subset(WORD op) {
    const int x = CHIP8_extractX(op);
    const int val = CHIP8_extractKK(op);

    IP += 2;

    reg[x] = val;

    return CORRECT_EXIT;
}

/// Decodes ADD (Vx, KK) opcode
/// @param op Opcode
/// @return CORRECT_EXIT
ReturnCode CHIP8_decode0x7Subset(WORD op) {
    const int x = CHIP8_extractX(op);
    const int val = CHIP8_extractKK(op);

    IP += 2;
    reg[x] += val;

    return CORRECT_EXIT;
}

/// Decodes LD (Vx, Vy) opcode
/// @param op Opcode
/// @return CORRECT_EXIT
ReturnCode CHIP8_decode0x8Subset(WORD op) {
    const int x = CHIP8_extractX(op);
    const int y = CHIP8_extractY(op);

    IP += 2;
    reg[x] = reg[y];

    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0x9Subset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0xASubset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0xBSubset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0xCSubset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0xDSubset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0xESubset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0xFSubset(WORD) {
    return CORRECT_EXIT;
}