
#include <stdio.h>
#include <stdlib.h>
#include <core.h>
#include <ll.h>
#include <backends/x11.h>
#include <mdl.h>

int main(void)
{
        X11Backend backend(640, 480);
        float t = 0.01f;
        ll<Tri, size_t> mdl = loadModel("assets/skull.obj");
        backend.setFragShader([](const Vertex &v) {
                return v.tint;
        });

        as3d([&t, &mdl](Renderer &renderer)
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

                auto translate = [&](Vertex v, float x, float y, float z) -> Vertex
                {
                        return {.tint = v.tint, .xyz = {v.xyz[0] + x, v.xyz[1] + y, v.xyz[2] + z}};
                };

                auto tri = mdl.begin();
                while (tri)
                {
                        renderer.DrawTri(translate(rotate(tri->value.abc[0]), 0.0, 0.0, 5.0),
                                         translate(rotate(tri->value.abc[1]), 0.0, 0.0, 5.0),
                                         translate(rotate(tri->value.abc[2]), 0.0, 0.0, 5.0));
                        tri = tri->next;
                }

                t += 0.01;
                renderer.AO();
        }, backend);
        return  0;
}
