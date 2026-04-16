
#include <fstream>
#include <string>
#include <vector>
#include <mdl.h>

ll<Tri, size_t> loadModel(std::string path)
{
        printf("debug: attempting load file\n");
        ll<Tri, size_t> tris;
        ll<Vertex, size_t> verts;
        std::ifstream file(path);
        std::string line;
        size_t count = 0;

        while (std::getline(file, line))
        {
                if (line.substr(0, 2) == "v ")
                {
                        Vertex v = {.tint = {1.0, 1.0, 1.0, 1.0}};
                        sscanf(line.c_str(), "v %f %f %f",
                               &v.xyz[0], &v.xyz[1], &v.xyz[2]);
                        verts(v, count);
                        count++;
                }
                else if (line.substr(0, 2) == "f ")
                {
                        size_t i0, i1, i2;
                        sscanf(line.c_str(), "f %zu %zu %zu", &i0, &i1, &i2);
                        Tri tri = {verts[i0 - 1], verts[i1 - 1], verts[i2 - 1]};
                        size_t k = tris.length();
                        tris(tri, k);
                }
        }

        printf("debug: loaded file\n");
        return tris;
}
