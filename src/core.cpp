
#include <core.h>
#include <ll.h>
#include <backends/sdl2.h>

void as3d(std::function<void(Renderer&)> main, Renderer &backend)
{
        while (backend.IsRunning())
        {
                backend.Update();
                backend.ZClear();
                backend.Clear((Color){0.0,0.0,0.0,1.0});
                main(backend);
                backend.Commit();
                backend.Swap();
        }
        return;
}
