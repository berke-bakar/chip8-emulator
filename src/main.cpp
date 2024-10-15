#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "../include/Chip8.h"

#include <iostream>
#include <SDL.h>

#ifdef __EMSCRIPTEN__

extern "C" {
    void render_chip8_display(const uint8_t* gfx);
}
#endif


// SDL graphics and input initialization
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *texture = nullptr;
const int WINDOW_WIDTH = 64;
const int WINDOW_HEIGHT = 32;

void initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL Initialization failed: " << SDL_GetError() << std::endl;
        exit(1);
    }
    window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH * 10, WINDOW_HEIGHT* 10, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void renderSDL(const uint8_t* gfx) {
    uint32_t pixel_buffer[WINDOW_HEIGHT][WINDOW_WIDTH];

    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            uint8_t pixel_value = gfx[y * WINDOW_WIDTH + x];
            pixel_buffer[y][x] = (pixel_value == 1) ? 0xFFFFFFFF : 0x000000FF;
        }
    }

    SDL_UpdateTexture(texture, nullptr, pixel_buffer, WINDOW_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void handleSDLEvents(Chip8 &chip8) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) exit(0);
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool is_pressed = event.type == SDL_KEYDOWN;
            switch (event.key.keysym.sym) {
                // TODO: Change later
                case SDLK_1: chip8.set_input_key(0x1, is_pressed); break;
                case SDLK_2: chip8.set_input_key(0x2, is_pressed); break;
            }
        }
    }
}

Chip8 chip8;
#ifdef __EMSCRIPTEN__
void emscripten_main_loop() {
    chip8.run();
    render_chip8_display(chip8.get_gfx()); // Call JavaScript to render gfx
}
#endif

int main(int argc, char* argv[]) {

    chip8.initialize();
    chip8.load_program("./roms/invaders.c8");
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(emscripten_main_loop, 0, 1);
    #endif
    // #elifdef _SDL_H
    initializeSDL();

    while (true) {

        handleSDLEvents(chip8);
        chip8.run();

        if (chip8.draw_gfx)
            renderSDL(chip8.get_gfx());
        chip8.draw_gfx = false;
    }
    // #endif

    return 0;
}
