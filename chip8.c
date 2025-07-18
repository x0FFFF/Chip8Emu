//
// Created by Daniel Shelgunov on 10/05/2025.
//

#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>

// user space registers declarations
BYTE reg[0xF];

// creating a shortcut for VF register for convenience
#define VF reg[0xF - 0x1]

unsigned short I;

BYTE delayTimer;
BYTE soundTimer;

// super space registers declarations

WORD IP;
WORD stack[0xF];
BYTE SP;

// video mem
// this implementation uses a 64x32 screen
// the screen supporting black and white, so each
// pixel will have 2 states: 1 - on, 0 - off
// 64 / 8 (1 byte) = 8
// 32 / 8 (1 byte) = 4
// so to represent each pixel with its own byte
// we need 8 * 4 bytes = 24
BYTE vmem[24];

// memory for our ROMs and sprites
BYTE mem[0xFFF];


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

    // read 3584 bytes from the ROM onto the CS (starting t 0x200)
    fread(mem + 0x200, sizeof(WORD), 0xDFF, fp);

    fclose(fp);

    return EXIT_SUCCESS;
}


