#include <iostream>

#include "../include/Chip8.h"

int main() {
    Chip8 chip8;

    // TODO: Implement initializeGfx();
    // TODO: Implement initializeInputHandler();

    chip8.initialize();
    chip8.load_program("../roms/Maze.ch8");
    chip8.dump_memory(std::cout);
    for (;;) {
        chip8.run();
    }

    return 0;
}
