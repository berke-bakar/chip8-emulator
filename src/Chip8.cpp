#include "../include/Chip8.h"

#include <iostream>
#include <process.h>
#include <cstring>
#include <cwchar>
#include <ctime>


void Chip8::initialize() {
    srand(time(nullptr));
    idx_register = 0; // Reset index register
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
    // Clear keys
    memset(input_keys, 0, sizeof(registers_v));

    // Load font set
    for (int i = 0; i < 80; ++i) {
        memory[CHIP8_ADDR_FONT_START + i] = font_set[i];
    }

    draw_gfx = true;
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
    // Fetch opcode, each opcode is 16 bits
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Some sources disagree on the instruction set, so I implemented from the following set: https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set
    // It appears to be correct for wide range of ROMs

    // Select based on first hex digit
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x0F00) {
                case 0x0000:
                    switch (opcode & 0x000F) {
                        case 0x0000:
                            // Clear screen
                            memset(gfx, 0, sizeof(gfx));
                            draw_gfx = true;
                            pc += 2;
                            break;
                        case 0x000E:
                            // Return from subroutine
                            sp--;
                            pc = stack[sp];
                            pc += 2;
                            break;
                    }
                    break;
            }
            break;
        case 0x1000:
            // (1NNN) Jump to address NNN
            pc = opcode & 0x0FFF;
            break;
        case 0x2000:
            // (2NNN) Execute subroutine at address NNN
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000:
            // (3XNN) Skip the following instruction if the value of register VX equals NN
            if (registers_v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) pc += 4;
            else pc += 2;
            break;
        case 0x4000:
            // (4XNN) Skip the following instruction if the value of register VX is not equal to NN
            if (registers_v[(opcode & 0x0F00) >> 8] != registers_v[(opcode & 0x00F0) >> 4]) pc += 4;
            else pc += 2;
            break;
        case 0x5000:
            // (5XY0) Skip the following instruction if the value of register VX is equal to the value of register VY
            if (registers_v[(opcode & 0x0F00) >> 8] == registers_v[(opcode & 0x00F0) >> 4]) pc += 4;
            else pc += 2;
            break;
        case 0x6000:
            // (6XNN) Store number NN in register VX
            registers_v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
            break;
        case 0x7000:
            // (7XNN) Add the value NN to register VX
            registers_v[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            pc += 2;
            break;
        case 0x8000:
            // Pattern: (8XY[0-7E])
            switch (opcode & 0x000F) {
                case 0x0000:
                    // Set VX to VY
                    registers_v[(opcode & 0x0F00) >> 8] = registers_v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0001:
                    // Set VX to VX OR VY
                    registers_v[(opcode & 0x0F00) >> 8] = registers_v[(opcode & 0x0F00) >> 8] | registers_v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0002:
                    // Set VX to VX AND VY
                    registers_v[(opcode & 0x0F00) >> 8] = registers_v[(opcode & 0x0F00) >> 8] & registers_v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0003:
                    // Set VX to VX XOR VY
                    registers_v[(opcode & 0x0F00) >> 8] = registers_v[(opcode & 0x0F00) >> 8] ^ registers_v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0004:
                    // Add the value of register VY to register VX
                    // Set VF to 01 if a carry occurs
                    // Set VF to 00 if a carry does not occur
                    if (registers_v[(opcode & 0x00F0) >> 4] > (0xFF - registers_v[(opcode & 0x0F00) >> 8]))
                        registers_v[0xF] = 1;
                    else
                        registers_v[0xF] = 0;
                    registers_v[(opcode & 0x0F00) >> 8] += registers_v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0005:
                    // Subtract the value of register VY from register VX
                    // Set VF to 00 if a borrow occurs
                    // Set VF to 01 if a borrow does not occur
                    if (registers_v[(opcode & 0x00F0) >> 4] > registers_v[(opcode & 0x0F00) >> 8])
                        registers_v[0xF] = 0;
                    else
                        registers_v[0xF] = 1;
                    registers_v[(opcode & 0x0F00) >> 8] -= registers_v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0006:
                    // Store the value of register VY shifted right one bit in register VX
                    // Set register VF to the least significant bit prior to the shift
                    // VY is unchanged
                    registers_v[(opcode & 0x0F00) >> 8] = registers_v[(opcode & 0x00F0) >> 4] >> 1;
                    registers_v[0xF] = registers_v[(opcode & 0x00F0) >> 4] & 0x01;
                    break;
                case 0x0007:
                    // Set register VX to the value of VY minus VX
                    // Set VF to 00 if a borrow occurs
                    // Set VF to 01 if a borrow does not occur
                    if (registers_v[(opcode & 0x0F00) >> 8] > registers_v[(opcode & 0x00F0) >> 4])
                        registers_v[0xF] = 0;
                    else
                        registers_v[0xF] = 1;
                    registers_v[(opcode & 0x0F00) >> 8] = registers_v[(opcode & 0x00F0) >> 4] - registers_v[(opcode & 0x0F00) >> 8];
                    break;
                case 0x000E:
                    // Store the value of register VY shifted left one bit in register VX
                    // Set register VF to the most significant bit prior to the shift VY is unchanged
                    registers_v[(opcode & 0x0F00) >> 8] = registers_v[(opcode & 0x00F0) >> 4] << 1;
                    registers_v[0xF] = (registers_v[(opcode & 0x00F0) >> 4] & 0x80) >> 7;
                    break;
            }
            pc += 2;
            break;
        case 0x9000:
            // (9XY0) Skip the following instruction if the value of register VX is not equal to the value of register VY
            if (registers_v[(opcode & 0x0F00) >> 8] != registers_v[(opcode & 0x00F0) >> 4]) pc += 4;
            else pc += 2;
            break;
        case 0xA000:
            // (ANNN) Store memory address NNN in register I
            idx_register = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000:
            // (BNNN) Jump to address NNN + V0
            pc = opcode & 0x0FFF + registers_v[0];
            break;
        case 0xC000:
            // Set VX to a random number with a mask of NN
            registers_v[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
            pc += 2;
            break;
        case 0xD000:
            // TODO: Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I
            // TODO: Set VF to  01 if any set pixels are changed to unset, and 00 otherwise
            break;
        case 0xE000:

            switch (opcode & 0x00FF) {
                case 0x009E:
                    // Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
                    if (input_keys[registers_v[(opcode & 0x0F00) >> 8]] == 1) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;
                case 0x00A1:
                    // Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
                    if (input_keys[registers_v[(opcode & 0x0F00) >> 8]] == 0) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x07:
                    // Store the current value of the delay timer in register VX
                    registers_v[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;
                case 0x0A:
                    // TODO: Wait for a keypress and store the result in register VX
                    // registers_v[(opcode & 0x0F00) >> 8] = ;
                    break;
                case 0x15:
                    // Set the delay timer to the value of register VX
                    delay_timer = registers_v[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x18:
                    // Set the sound timer to the value of register VX
                    sound_timer = registers_v[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x1E:
                    // Add the value stored in register VX to register I
                    idx_register += registers_v[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x29:
                    // TODO: Set I to the memory address of the sprite data corresponding to the hexadecimal digit stored in register VX
                    break;
                case 0x33:
                    // Store the binary-coded decimal equivalent of the value stored in register VX at addresses I, I + 1, and I + 2
                    memory[idx_register] = registers_v[(opcode & 0x0F00) >> 8] / 100;
                    memory[idx_register + 1] = (registers_v[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[idx_register + 2] = registers_v[(opcode & 0x0F00) >> 8] % 10;
                    pc += 2;
                    break;
                case 0x55:
                    // Store the values of registers V0 to VX inclusive in memory starting at address I
                    // I is set to I + X + 1 after operation
                    const uint8_t X = (opcode & 0x0F00) >> 8;
                    for (int i = 0; i <= X; i++)
                        memory[idx_register + i] = registers_v[i];

                    idx_register = idx_register + X + 1;
                    pc += 2;
                    break;
                case 0x65:
                    // Fill registers V0 to VX inclusive with the values stored in memory starting at address I
                    // I is set to I + X + 1 after operation
                    const uint8_t maxX = (opcode & 0x0F00) >> 8;
                    for (int i = 0; i <= maxX; i++) registers_v[i] = memory[idx_register + i];

                    idx_register = idx_register + maxX + 1;
                    pc += 2;
                    break;
            }
            break;
        default:
            std::cout << "Unknown opcode " << opcode << std::endl;
            exit(1);
    }

    if (delay_timer > 0) {
        delay_timer--;
    }

    if (sound_timer > 0) {
        sound_timer--;
    }
}