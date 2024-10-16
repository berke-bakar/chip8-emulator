cmake_minimum_required(VERSION 3.29)
project(chip8_emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "C:/mingw_dev/SDL2-2.30.8/x86_64-w64-mingw32")
set(EMSDK_INCLUDE_DIR "C:/mingw_dev/emsdk/upstream/emscripten/cache/sysroot/include")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

if (DEFINED __EMSCRIPTEN__)
        include_directories(${EMSDK_INCLUDE_DIR})
endif ()

add_executable(chip8_emulator src/main.cpp
        src/Chip8.cpp
        )

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

