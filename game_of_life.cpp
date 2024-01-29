#include <cstdint>
#include <iostream>
#include "business_logic.hpp"

#include "SDL2/SDL.h"


const int WIDTH = 30;
const int HEIGHT = 30;
const int BOARD_SIZE = WIDTH * HEIGHT;
const int SCALE = 20;
const uint32_t OFF = 0x00000000;
const uint32_t ON  = 0xFFFF0000;

uint32_t board[BOARD_SIZE];

void log(const std::string& message) {
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
                if (event.key.keysym.sym == SDLK_q)
                    quit = true;
                else if (event.key.keysym.sym == SDLK_SPACE) {
                    log("computing next generation");
                    tick();
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
