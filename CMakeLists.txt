cmake_minimum_required(VERSION 3.29)
project(chip8_emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

option(SDL2_PATH "Path to SDL2 installation" "")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(chip8_emulator src/main.cpp
        src/Chip8.cpp
        )

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

if (WIN32)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SDL2_PATH}/bin/SDL2.dll"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
endif()