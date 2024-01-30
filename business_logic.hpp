#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern "C" {
    
void init(size_t width, size_t height);
void set_cell(int x, int y, bool alive);
bool get_cell(int x, int y);
void tick();

}