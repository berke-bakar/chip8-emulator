cmake_minimum_required(VERSION 3.29)
project(chip8_emulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(chip8_emulator src/main.cpp
        src/Chip8.cpp
        include/Chip8.h)

set(CHIP8_ROM ${CMAKE_SOURCE_DIR}/roms/Maze.ch8)
file(COPY ${CHIP8_ROM} DESTINATION ${CMAKE_BINARY_DIR}/roms)

