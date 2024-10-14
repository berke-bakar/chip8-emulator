#ifndef CHIP8_H
#define CHIP8_H
#include <cstdint>
#include <ostream>
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_REGISTER_COUNT 16
#define CHIP8_ADDR_PROGRAM_START 0x200 // Start address of CHIP-8's program
#define CHIP8_ADDR_FONT_START 0x050 // Start address of CHIP-8's built-in 4x5 pixel font set from 0 to F
#define CHIP8_STACK_SIZE 16
#define CHIP8_KEY_SIZE 16

class Chip8 {
    public:
    void initialize();
    void load_program(const char *filename);
    void dump_memory(std::ostream & os) const;
    void run();

    private:
    uint8_t gfx[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT] = {}; // Display is 64 x 32
    uint8_t memory[CHIP8_MEMORY_SIZE] = {};
    uint8_t registers_v[CHIP8_REGISTER_COUNT] = {};
    uint16_t stack[CHIP8_STACK_SIZE] = {};
    uint8_t key[CHIP8_KEY_SIZE] = {};
    uint8_t I = 0; // index register
    uint16_t pc = CHIP8_ADDR_PROGRAM_START; // Program counter (PC) starts at 0x200 (Addresses are 12 bit)
    uint16_t sp = 0; // Stack Pointer (SP) resets to 0
    uint16_t opcode = 0; // Current fetched opcode
    uint8_t delay_timer = 0;
    uint8_t sound_timer = 0;
    const uint8_t font_set[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
      };
};



#endif //CHIP8_H
