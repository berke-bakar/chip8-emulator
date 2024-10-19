# CHIP-8 Emulator (w/ WASM support)

The CHIP-8 is an interpreted programming language created in the mid-1970s by Joseph Weisbecker for use on the RCA COSMAC VIP, an early personal computer. It was designed to simplify game development for hobbyists, providing an easy way to create interactive graphics programs. CHIP-8 programs run on a virtual machine with a small, simple instruction set, making it popular for early game programming. Over time, building a CHIP-8 emulator has become a rite of passage for emulator developers. This project is a CHIP-8 emulator written in C++. It implements all of the original 35 CHIP-8 opcodes based on the instruction set on [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8#Opcode_table). Instruction sets differ among sources unfortunately, however this set is widely accurate for lots of CHIP-8 ROMs.

The emulator includes two main build options:

* **WASM:** The emulator is compiled to WebAssembly (WASM) to run in a browser. An example of running the emulator inside a web worker thread can be seen [here](https://chip8emulator.vercel.app). The [repo](https://github.com/berke-bakar/chip8-wasm-demo) of the demo is also available.
* **Desktop GUI:** A desktop version is available, built with SDL for cross-platform support.

## **Features**

* Full implementation of all 35 original CHIP-8 instructions.
* WebAssembly support using Emscripten bindings for running in a browser/nodejs environment.
* SDL-based desktop application for cross-platform support.

## Building

### WebAssembly (WASM)

To build the emulator module for WebAssembly, only the Chip8.cpp file is compiled using Emscripten:

1. Setup Emscripten for your environment from [here](https://emscripten.org/docs/getting_started/downloads.html#sdk-download-and-install).
2. Clone the repo:

```
$ git clone https://github.com/berke-bakar/chip8-emulator-wasm.git
```

3. Open terminal and change directory to project root, and run the following:

```
$ emcc -o Chip8.js ./src/Chip8.cpp -O3 -s EXPORTED_FUNCTIONS="['_malloc']" --bind
```

**Note:** Emscripten exports `malloc` function by default, however sometimes `-O3` optimization can prune it away if `malloc` is not explicitly used in code. So we manually add it to exported functions list just in case.

This command generates the necessary .js and .wasm files, which can be used in a web project. An example web project is available in [this repo](https://github.com/berke-bakar/chip8-wasm-demo).

### Desktop (SDL)

The desktop version of the emulator uses SDL for rendering and input handling.

**Note:** The project originally built using CMake and ninja build tool, however given the appropriate SDL path and make program parameters, it should work on other build tools as well, hence the given commands are kept as generic as possible:

1. Setup SDL2 for your environment [here](https://wiki.libsdl.org/SDL2/Installation).
2. Clone the repo:

```
$ git clone https://github.com/berke-bakar/chip8-emulator-wasm.git
```

3. Open terminal and change directory to project root, and run the following to configure the project:

```
$ cmake \
  -DCMAKE_BUILD_TYPE=Release \  # Build type (e.g. Debug, Release)
  -DCMAKE_MAKE_PROGRAM="path/to/make_program_executable" \  # Path to make program (e.g. ninja.exe)
  -DSDL2_PATH="/path/to/sdl2" \  # Path to SDL2 installation (e.g. "C:/mingw_dev/SDL2-2.30.8/x86_64-w64-mingw32")
  -G "make_program_name" \  # Specify the generator (e.g. Ninja)
  -S . \  # Source directory
  -B ./cmake-build-release  # Build directory
```

4. Then to build the project run the following:

```
$ cmake --build ./cmake-build-release --target chip8_emulator -j 30
```

This command builds the executable inside `cmake-build-release` folder. To run the program:

- Drag a CHIP-8 ROM file on the executable, or
- Navigate to `cmake-build-release` folder in terminal, and run the executable from command line:

```
$ chip8_emulator /path/to/chip8_rom.ch8
```

## Roadmap

- **Super CHIP-8:** Adding support for Super CHIP-8 instructions to extend functionality.
