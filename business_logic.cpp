#include "business_logic.hpp"

#include <string.h>
#include <memory>


std::unique_ptr<bool[]> current_generation;
std::unique_ptr<bool[]> next_generation;

size_t width, height;

void init(size_t _width, size_t _height){
    width = _width;
    height = _height;
    current_generation = std::make_unique<bool[]>(width*height);
    next_generation= std::make_unique<bool[]>(width*height);
}

int mod(int a, int b) {
    int ret = a % b;
    if (ret < 0) {
        ret += b;
    }
    return ret;
}

void set_cell(int x, int y, bool alive) {
    current_generation[y * width + x] = alive;
}

bool get_cell(int x, int y){
    return current_generation[y * width + x];
}

size_t neighbors(int x, int y) {
    size_t count = 0;

    // Check cell on the right
    if (current_generation[mod((x+1), width) + mod((y+0), height) * width])
        count++;

    // Check cell on bottom right
    if (current_generation[mod((x+1), width) + mod((y+1), height) * width])
        count++;

    // Check cell on the bottom
    if (current_generation[mod((x+0), width) + mod((y+1), height) * width])
        count++;

    // Check cell on bottom left
    if (current_generation[mod((x-1), width) + mod((y+1), height) * width])
        count++;

    // Check cell on the left
    if (current_generation[mod((x-1), width) + mod((y+0), height) * width])
        count++;

    // Check cell on the top left
    if (current_generation[mod((x-1), width) + mod((y-1), height) * width])
        count++;

    // Check cell on the top
    if (current_generation[mod((x+0), width) + mod((y-1), height) * width])
        count++;

    // Check cell on the top right
    if (current_generation[mod((x+1), width) + mod((y-1), height) * width])
        count++;

    return count;
}


void tick() {    
    memcpy(next_generation.get(), current_generation.get(), width*height*sizeof(bool));
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            uint32_t current_cell = current_generation[y * width + x];
            int live_neighbors = neighbors(x, y);
            if (current_cell && !(live_neighbors == 2 || live_neighbors == 3)) {
                next_generation[y * width + x] = false;
            }
            if (!current_cell && live_neighbors == 3) {
                next_generation[y * width + x] = true;
            }
        }
    }
    std::swap(current_generation, next_generation);
}
