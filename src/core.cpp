
#include <core.h>
#include <ll.h>
#include <backends/sdl2.h>

void as3d(std::function<void(Renderer&)> main, Renderer &backend)
{
        Vertex TestVertex = {.tint = (Color){1.0,1.0,1.0,1.0}};
        TestVertex.xyz[0] = 0;
        TestVertex.xyz[1] = 0;
        TestVertex.xyz[2] = 0;
        while (backend.IsRunning())
        {
                backend.Update();
                backend.ZClear();
                backend.Clear((Color){1.0,0.0,0.0,1.0});
                backend.DrawPixel(TestVertex);
                main(backend);
                backend.Commit();
                backend.Swap();
        }
        return;
}
