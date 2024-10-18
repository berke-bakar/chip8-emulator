// #define __EMSCRIPTEN__
#ifndef CHIP8_H
#define CHIP8_H
#include <cstdint>
#include <random>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#endif //__EMSCRIPTEN__

#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_REGISTER_COUNT 16
#define CHIP8_ADDR_PROGRAM_START 0x200 // Start address of CHIP-8's program
#define CHIP8_STACK_SIZE 16
#define CHIP8_KEY_SIZE 16
#define CHIP8_TIMER_PERIOD_MICROSECONDS ((1000.0 / 60) * 1000.0) // Timers are at a constant 60 Hz, for now
/**
 * @brief A class representing Chip-8 virtual machine.
 *
 * This class simulates the basic architecture of the Chip-8 VM,
 * currently only the original 35 instructions are supported.
 */
class Chip8 {
    public:
    Chip8();
    void initialize();
    void reset_program();
    void load_program(const std::string& filename); // method for loading directly from file
    void load_program(const std::vector<uint8_t> & program); // method for loading from vector
    void dump_memory(std::ostream & os) const;
    void dump_memory() const;
    void run();
    [[nodiscard]] const uint8_t* get_gfx() const;
    void set_input_key(uint8_t key, bool is_pressed);
    void update_timers();
    bool draw_gfx = false;
    private:
    uint8_t gfx[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT] = {}; // Display is 64 x 32
    uint8_t memory[CHIP8_MEMORY_SIZE] = {};
    uint8_t registers_v[CHIP8_REGISTER_COUNT] = {};
    uint16_t stack[CHIP8_STACK_SIZE] = {};
    uint8_t input_keys[CHIP8_KEY_SIZE] = {};
    uint16_t idx_register = 0; // index register
    uint16_t pc = CHIP8_ADDR_PROGRAM_START; // Program counter (PC) starts at 0x200 (Addresses are 12 bit)
    uint16_t sp = 0; // Stack Pointer (SP) resets to 0
    uint16_t opcode = 0; // Current fetched opcode
    uint8_t delay_timer = 0;
    uint8_t sound_timer = 0;
    std::random_device random_device;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;

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
#ifdef __EMSCRIPTEN__
// Binding code
EMSCRIPTEN_BINDINGS(chip8_class) {
    emscripten::register_vector<uint8_t>("Uint8Vector");


    emscripten::class_<Chip8>("Chip8")
        .constructor()
        .function("initialize", &Chip8::initialize)
        .function("resetProgram", &Chip8::reset_program)
        .function("loadProgram", static_cast<void(Chip8::*)(const std::vector<uint8_t> &)>(&Chip8::load_program))
        .function("dumpMemory", static_cast<void(Chip8::*)()const>(&Chip8::dump_memory))
        .function("run", &Chip8::run)
        .function("updateTimers", &Chip8::update_timers)
        .function("getGfx", emscripten::optional_override([](const Chip8& chip) {
            return emscripten::val(emscripten::typed_memory_view(CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT, chip.get_gfx()));
        }))
        .function("setInputKey", &Chip8::set_input_key)
        .property("drawGfx", &Chip8::draw_gfx);
}
#endif //__EMSCRIPTEN__

#endif
