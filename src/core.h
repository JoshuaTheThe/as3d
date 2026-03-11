
#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>
#include <thread>

typedef float   Color[4];

struct  Vertex
{
        Color   color;
        float   xyz[3];
};

struct  Tri
{
        Vertex   v[3];
};

void as3d(void);

#endif
