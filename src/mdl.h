
#ifndef MDL_H
#define MDL_H

#include <core.h>
#include <ll.h>

struct Tri
{
        Vertex abc[3];
};

ll<Tri, size_t> loadModel(std::string path);

#endif
