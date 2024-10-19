#include <chrono>

#include "../include/Chip8.h"
#include <iostream>
#include <SDL.h>

#define CPU_CYCLE_HZ 1000
#define TIMER_HZ 60

// SDL graphics and input initialization
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *texture = nullptr;
constexpr int WINDOW_WIDTH = 64;
constexpr int WINDOW_HEIGHT = 32;

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


/**
* The original keypad:
* 123C
* 456D
* 789G
* A0BF
*
* Keyboard mapping:
* 1234
* QWER
* ASDF
* ZXCV
 */
void handleSDLEvents(Chip8 &chip8) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) exit(0);
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool is_pressed = event.type == SDL_KEYDOWN;
            switch (event.key.keysym.sym) {
                // 1st row
                case SDLK_1: chip8.set_input_key(0x1, is_pressed); break;
                case SDLK_2: chip8.set_input_key(0x2, is_pressed); break;
                case SDLK_3: chip8.set_input_key(0x3, is_pressed); break;
                case SDLK_4: chip8.set_input_key(0xC, is_pressed); break;
                // 2nd row
                case SDLK_q: chip8.set_input_key(0x4, is_pressed); break;
                case SDLK_w: chip8.set_input_key(0x5, is_pressed); break;
                case SDLK_e: chip8.set_input_key(0x6, is_pressed); break;
                case SDLK_r: chip8.set_input_key(0xD, is_pressed); break;
                // 3rd row
                case SDLK_a: chip8.set_input_key(0x7, is_pressed); break;
                case SDLK_s: chip8.set_input_key(0x8, is_pressed); break;
                case SDLK_d: chip8.set_input_key(0x9, is_pressed); break;
                case SDLK_f: chip8.set_input_key(0xE, is_pressed); break;
                // 4th row
                case SDLK_z: chip8.set_input_key(0xA, is_pressed); break;
                case SDLK_x: chip8.set_input_key(0x0, is_pressed); break;
                case SDLK_c: chip8.set_input_key(0xB, is_pressed); break;
                case SDLK_v: chip8.set_input_key(0xF, is_pressed); break;
            }
        }
    }
}

Chip8 chip8;


int main(int argc, char* argv[]) {

    chip8.initialize();
    if(argc < 2)
    {
        std::cout << "Usage: chip8_emulator chip8RomFile.(ch8|c8)" << std::endl << std::endl;
        return 1;
    }
    chip8.load_program(argv[1]);
    initializeSDL();

    auto cpuCycleBeginTime = std::chrono::high_resolution_clock::now();
    auto timerBeginTime = std::chrono::high_resolution_clock::now();
    constexpr float cpu_cycle_period_micro = (1000.0f / CPU_CYCLE_HZ * 1000);
    constexpr float timer_period_micro = (1000.0f / TIMER_HZ * 1000);
    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed_cpu_time = std::chrono::duration_cast<std::chrono::microseconds>(now - cpuCycleBeginTime).count();
        auto elapsed_timer_time = std::chrono::duration_cast<std::chrono::microseconds>(now-timerBeginTime).count();

        if (elapsed_cpu_time >= cpu_cycle_period_micro) {
            chip8.run();
            cpuCycleBeginTime = now;
        }

        if (elapsed_timer_time >= timer_period_micro) {
            handleSDLEvents(chip8);
            chip8.update_timers();
            timerBeginTime = now;
        }

        if (chip8.draw_gfx)
            renderSDL(chip8.get_gfx());
        chip8.draw_gfx = false;
    }

    return 0;
}
