//
// Created by Daniel Shelgunov on 10/05/2025.
//

#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// user space registers declarations
BYTE reg[0xF];

// creating a shortcut for VF register for convenience
#define VF reg[0xF - 0x1]

WORD I;

BYTE delayTimer;
BYTE soundTimer;

// super space registers declarations

// instructions start at 0x200
WORD IP = 0x200;
WORD stack[0xF];
BYTE SP;

// video mem
// this implementation uses a 64x32 screen
// the screen supporting black and white, so each
// pixel will have 2 states: 1 - on, 0 - off
BYTE vmem[CHIP8_SCREEN_HEIGHT][CHIP8_SCREEN_WIDTH];

// memory for our ROMs and sprites
BYTE mem[0xFFF];

/// Initializes the fonts used by the emulator
/// by loading them into the memory
/// @return EXIT_SUCCESS
ReturnCode CHIP8_init()
{
    // storing fonts as binary
    char fonts[] = {
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
    return EXIT_SUCCESS;
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
///         EXIT_SUCCESS if ROM was loaded
ReturnCode CHIP8_loadROM(const char* path) {
    FILE* fp = fopen(path, "rb");

    // check for errors while opening the given path
    if (fp == NULL) {
        return FAILED_TO_LOAD_ROM;
    }

    // read 3584 bytes from the ROM onto the CS (starting at 0x200)
    fread(mem + 0x200, sizeof(WORD), 0xDFF, fp);

    fclose(fp);

    return EXIT_SUCCESS;
}

ReturnCode CHIP8_decodeOp() {
    if (IP > sizeof(mem)) {
        return UNRECOGNIZED_OPCODE;
    }

    // fetch current op
    WORD op = mem[IP];

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

    IP++;

    return rc;
}

/// The function increments SP and stores the data onto the stack
/// @param data data to be stored onto the stack
/// @return STACK_OVERFLOW if SP > 0xF (stack size), else CORRECT_EXIT
ReturnCode CHIP8_push(WORD data) {
    SP++;

    if (SP > 0xF) {
        return STACK_OVERFLOW;
    }

    stack[SP] = data;

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

ReturnCode CHIP8_decode0x0Subset(WORD op) {
    int i = 0, j = 0;
    BYTE opType = 0;
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
        // pop will return STACK_UNDERFLOW if an error occured
        // else CORRECT_EXIT
        return CHIP8_pop(&IP);
    }

    return UNRECOGNIZED_OPCODE;
}
ReturnCode CHIP8_decode0x1Subset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0x2Subset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0x3Subset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0x4Subset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0x5Subset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0x6Subset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0x7Subset(WORD) {
    return CORRECT_EXIT;
}
ReturnCode CHIP8_decode0x8Subset(WORD) {
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