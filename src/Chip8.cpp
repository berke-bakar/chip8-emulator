#include "../include/Chip8.h"

#include <iostream>
#include <process.h>
#include <cstring>
#include <cwchar>

void Chip8::initialize() {

    I = 0; // Reset index register
    pc = 0x200; // Reset PC to 0x200, where programs start
    sp = 0; // Reset stack pointer
    opcode = 0; // Reset current fetched opcode
    sound_timer = 0; // Reset sound timer
    delay_timer = 0; // Reset delay timer

    // Clear display
    memset(gfx, 0, sizeof(gfx));
    // Clear stack
    memset(stack, 0, sizeof(stack));
    // Clear memory
    memset(memory, 0, sizeof(memory));
    // Clear registers
    memset(registers_v, 0, sizeof(registers_v));

    // Load font set
    for (int i = 0; i < 80; ++i) {
        memory[CHIP8_ADDR_FONT_START + i] = font_set[i];
    }

}

void Chip8::load_program(const char *filename) {
    // Open file
    FILE *f = fopen(filename, "rb");
    if (f == nullptr) {
        std::cout << "Failed to open file " << filename << std::endl;
        exit(1);
    }
    // Seek to end of the file
    fseek(f, 0, SEEK_END);
    // Count byte size from start to current position (end), effectively file size
    const int size = ftell(f);
    // Seek to beginning of the file
    fseek(f, 0, SEEK_SET);
    // Read ROM contents into memory
    fread(memory + CHIP8_ADDR_PROGRAM_START, 1, size, f);
    // Close file
    fclose(f);
}

void Chip8::dump_memory(std::ostream & os) const {
    for (int i = 0; i < CHIP8_MEMORY_SIZE; i++) {
        os << "[" << i << "]: " << memory[i] << std::endl;
    }
}

void Chip8::run() {
    
}