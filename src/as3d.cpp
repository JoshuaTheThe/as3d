
#include <stdio.h>
#include <stdlib.h>
#include <core.h>
#include <ll.h>
#include <backends/sdl2.h>

int main(void)
{
        SDL2Backend backend(640, 480);
        float t = 0.01f;
        as3d([&t](Renderer &renderer)
        {
                const float cosT = cos(t), sinT = sin(t);
                auto rotate = [&](Vertex v) -> Vertex
                {
                        float x = v.xyz[2] * cosT - v.xyz[0] * sinT;
                        float y = v.xyz[2] * sinT + v.xyz[0] * cosT;
                        v.xyz[2] = x;
                        v.xyz[0] = y;
                        return v;
                };

                const static Vertex a={.tint = {1.0, 0.0, 1.0, 1.0}, .xyz = {-0.5, -0.5, 0.0}},
                                    b={.tint = {0.0, 1.0, 1.0, 1.0}, .xyz = {0.0,  0.5, 0.0}},
                                    c={.tint = {0.0, 0.0, 1.0, 1.0}, .xyz = {0.5, -0.5, 0.0}};
                const Vertex a_prime = rotate(a),
                             b_prime = rotate(b),
                             c_prime = rotate(c);
                const Vertex a_translated = {.tint = a_prime.tint, .xyz = {a_prime.xyz[0], a_prime.xyz[1], a_prime.xyz[2] + 1.0f}},
                             b_translated = {.tint = b_prime.tint, .xyz = {b_prime.xyz[0], b_prime.xyz[1], b_prime.xyz[2] + 1.0f}},
                             c_translated = {.tint = c_prime.tint, .xyz = {c_prime.xyz[0], c_prime.xyz[1], c_prime.xyz[2] + 1.0f}};
                renderer.DrawTri(a_translated, b_translated, c_translated);
                t += 0.01f;
        }, backend);
        return  0;
}
