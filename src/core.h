
#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>
#include <thread>
#include <functional>
#include <string>

#define NEAR 0.1

enum    Backend
{
        BACKEND_NONE,
        BACKEND_SDL2,
};

struct Color { float rgba[4]; };

struct  Vertex
{
        Color   tint;
        float   xyz[3];
};

struct Tri { Vertex v[3]; };

class   Renderer
{
protected:
        uint16_t w, h;
        size_t   buf;
        Color   *fbufs[2];
        float   *zbufs[2];

        bool     keymap[256];
        float    mouseX, mouseY; // X,Y * W,H
        bool     isRunning = false;

        Vertex Screen(const Vertex &p)
        {
                Vertex new_v = {.tint = p.tint};
                new_v.xyz[0] = (p.xyz[0] + 1) / 2 * this->w;
                new_v.xyz[1] = (1 - (p.xyz[1] + 1) / 2) * this->h;
                new_v.xyz[2] = p.xyz[2];
                return new_v;
        }

        Vertex Project(const Vertex &p)
        {
                Vertex new_v = {.tint = p.tint};
                new_v.xyz[0] = p.xyz[0] / p.xyz[2];
                new_v.xyz[1] = p.xyz[1] / p.xyz[2];
                new_v.xyz[2] = p.xyz[2];
                return new_v;
        }

        void Place(const Vertex &p)
        {
                if ((int)p.xyz[0] < 0.0 ||
                    (int)p.xyz[1] < 0.0 ||
                    (int)p.xyz[0] >= this->w ||
                    (int)p.xyz[1] >= this->h)
                        return;
                const size_t row = (int)p.xyz[1] * this->w;
                if (zbufs[buf][row + (int)p.xyz[0]] > p.xyz[2])
                {
                        fbufs[buf][row + (int)p.xyz[0]] = p.tint;
                        zbufs[buf][row + (int)p.xyz[0]] = p.xyz[2];
                }
        }
public:
        Renderer(uint16_t w, uint16_t h)
        {
                this->w = w;
                this->h = h;
                buf = 0;
                fbufs[0] = new Color[w*h];
                fbufs[1] = new Color[w*h];
                zbufs[0] = new float[w*h];
                zbufs[1] = new float[w*h];
        }

        ~Renderer()
        {
                delete[] fbufs[0];
                delete[] fbufs[1];
                delete[] zbufs[0];
                delete[] zbufs[1];
        }

        // General
        virtual void Swap(void) { buf^=1; } // swap buffers
        virtual void Commit(void) {} // commit to screen
        virtual void Update(void) {} // update loop function
        virtual bool IsRunning(void) { return isRunning; }

        virtual void Clear(Color color)
        {
                for (size_t i = 0; i < w*h; ++i)
                        fbufs[buf][i] = color;
        }

        virtual void ZClear(void) { for (size_t i = 0; i < w*h; ++i) zbufs[buf][i] = INFINITY; }

        // 3D Features (allow override for optimisations)
        virtual void DrawPoint(const Vertex &p0) { Place(Screen(Project(p0))); }
        virtual void DrawLine(const Vertex &p0, const Vertex &p1){}
        virtual void DrawTri(const Tri &tri) {}

        // 2D Features (allow override for optimisations)
        virtual void DrawPixel(const Vertex &p0) { Place(Screen(p0)); }

        virtual void DrawFlatLine(const Vertex p0, const Vertex p1)
        {
                Vertex a = Screen(p0), b = Screen(p1);
                float Δx = b.xyz[0] - a.xyz[0];
                float Δy = b.xyz[1] - a.xyz[1];
                bool steep = abs(Δy) > abs(Δx);
                if (steep)
                {
                        std::swap(a.xyz[0], a.xyz[1]);
                        std::swap(b.xyz[0], b.xyz[1]);
                }

                if (a.xyz[0] > b.xyz[0])
                {
                        std::swap(a, b);
                }

                Δx = b.xyz[0] - a.xyz[0];
                Δy = b.xyz[1] - a.xyz[1];
                float Δ  = 2 * abs(Δy) - Δx;
                float y  = a.xyz[1];

                for (size_t x = a.xyz[0]; x < b.xyz[0]; ++x)
                {
                        // TODO - lerp Tint
                        Vertex vert = {.tint = a.tint};
                        vert.xyz[0] = x;
                        vert.xyz[1] = y;
                        // TODO - lerp Z
                        vert.xyz[2] = a.xyz[2];
                        if (steep)
                                std::swap(vert.xyz[0], vert.xyz[1]);
                        Place(vert);
                        if (Δ > 0)
                        {
                                y += (Δy < 0) ? -1 : 1;
                                Δ += 2 * (abs(Δy) - Δx);
                                continue;
                        }
                        Δ += 2 * abs(Δy);
                }
        }

        virtual void DrawFlatTri(const Tri &tri) {}
};

void as3d(std::function<void(Renderer&)> main, Renderer &Renderer);

#endif
