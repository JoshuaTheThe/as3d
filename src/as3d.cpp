
#include <stdio.h>
#include <stdlib.h>
#include <core.h>
#include <ll.h>
#include <backends/sdl2.h>

int main(void)
{
        SDL2Backend backend(640, 480);
        as3d([](Renderer &renderer) {                
                const static Vertex a={.tint = {0.0, 1.0, 1.0, 1.0}, .xyz = {-0.5, -0.5, 0.1}},
                                    b={.tint = {1.0, 0.0, 1.0, 1.0}, .xyz = {0.0, 0.5, 0.1}},
                                    c={.tint = {0.0, 0.0, 1.0, 1.0}, .xyz = {0.5, -0.5, 0.1}};
                renderer.DrawFlatTri(a, b, c);
        }, backend);
        return  0;
}
