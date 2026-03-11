
#include <stdio.h>
#include <stdlib.h>
#include <core.h>
#include <ll.h>

int main(void)
{
        ll<size_t, size_t, 0> verts;
        for (size_t i = 0; i < 1024; ++i)
                verts[i] = 1;
        // std::thread Renderer(CoreRendererMain);
        // Renderer.join();
        return  0;
}
