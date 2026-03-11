
#include <stdio.h>
#include <stdlib.h>
#include <core.h>
#include <ll.h>
#include <backends/sdl2.h>

int main(void)
{
        SDL2Backend backend(640, 480);
        as3d([](Renderer &renderer) {;}, backend);
        return  0;
}
