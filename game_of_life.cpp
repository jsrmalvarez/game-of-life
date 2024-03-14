#include <cstdint>
#include <iostream>
#include <string>
#include <fmt/core.h>
#include "business_logic.hpp"

#include "SDL2/SDL.h"


const int WIDTH = 50;
const int HEIGHT = 50;
const int BOARD_SIZE = WIDTH * HEIGHT;
const int SCALE = 20;
const uint32_t OFF = 0x00000000;
const uint32_t ON  = 0xFFFF0000;

uint32_t board[BOARD_SIZE];

template<typename ...Args>
void log(const std::string& format, Args... args) {
    std::string message = fmt::format(format, args...);
    std::cout << "[game_of_life] " << message << std::endl;
}


void draw(int x, int y) {
    int scaled_x = x/SCALE;
    int scaled_y = y/SCALE;
    set_cell(scaled_x, scaled_y, !get_cell(scaled_x, scaled_y));
}

int main(int argc, char* argv[]) {
    log("initializing video");
    SDL_Init(SDL_INIT_VIDEO);

    log("creating window");
    SDL_Window *window = SDL_CreateWindow(
        "Game of Life",
        100,
        100,
        WIDTH * SCALE,
        HEIGHT * SCALE,
        SDL_WINDOW_SHOWN
    );

    log("creating renderer");
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    log("clearing renderer");
    SDL_RenderClear(renderer);

    log("creating base texture");
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );

    log("creating board");
    size_t current_generation = 1;
    log("Current generation {}", current_generation);
    init(WIDTH, HEIGHT);


    bool quit = false;
    bool drawing = false;


    SDL_UpdateTexture(texture, NULL, board, WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Event event;


    while (!quit) {
        SDL_WaitEvent(&event);

        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                draw(event.button.x, event.button.y);
                drawing = true;
                break;
            case SDL_MOUSEMOTION:
                if (event.motion.state & SDL_BUTTON(1)) {
                    draw(event.motion.x, event.motion.y);
                    drawing = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                drawing = false;
                break;
            case SDL_FINGERMOTION:
                break;
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_q) {
                    quit = true;
                }
                else if (event.key.keysym.sym == SDLK_SPACE) {
                    current_generation++;
                    log("Current generation {}", current_generation);
                    tick();
                }
                else if (event.key.keysym.sym == SDLK_r || event.key.keysym.sym == SDLK_0) {
                    current_generation = 1;
                    log("- Set pattern 0");
                    log("Current generation {}", current_generation);
                    reset(0);
                }
                else if (event.key.keysym.sym == SDLK_1) {
                    current_generation = 1;
                    log("- Set pattern 1");
                    log("Current generation {}", current_generation);
                    reset(1);
                }
                else if (event.key.keysym.sym == SDLK_2) {
                    current_generation = 1;
                    log("- Set pattern 2");
                    log("Current generation {}", current_generation);
                    reset(2);
                }
                else if (event.key.keysym.sym == SDLK_3) {
                    current_generation = 1;
                    log("- Set pattern 3");
                    log("Current generation {}", current_generation);
                    reset(3);
                }
                else if (event.key.keysym.sym == SDLK_4) {
                    current_generation = 1;
                    log("- Set pattern 4");
                    log("Current generation {}", current_generation);
                    reset(4);
                }
                drawing = true;
                break;
            case SDL_KEYUP:
                drawing = false;
                break;
            default:
                break;
        }

        if (drawing) {

            for(int x = 0 ; x < WIDTH; x++){
                for(int y = 0 ; y < HEIGHT; y++){
                    board[y * WIDTH + x] = get_cell(x, y) ? ON : OFF;
                }
            }
            SDL_UpdateTexture(texture, NULL, board, WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            drawing = false;
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
