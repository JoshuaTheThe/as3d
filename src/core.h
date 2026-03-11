
#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>
#include <thread>
#include <functional>
#include <string>

#define NEAR 0.1

enum Backend
{
        BACKEND_NONE,
        BACKEND_SDL2,
};

struct Color
{
        float rgba[4];
};

struct Vertex
{
        Color tint;
        float xyz[3];
};

typedef std::function<Vertex(const Vertex &)> VertexShader;
typedef std::function<Color(const Vertex &)> FragmentShader;

class Renderer
{
protected:
        uint16_t w, h;
        size_t buf;
        Color *fbufs[2];
        float *zbufs[2];

        bool keymap[256];
        float mouseX, mouseY; // X,Y * W,H
        bool isRunning = false;

        VertexShader vertShader = [](const Vertex &v)
        { return v; };
        FragmentShader fragShader = [](const Vertex &v)
        { return v.tint; };

        float Lerp(float a, float b, float t)
        {
                return a + t * (b - a);
        }

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
                if (p.xyz[2] < NEAR)
                {
                        return (Vertex){.tint = p.tint, .xyz = {INFINITY, INFINITY, INFINITY}};
                }

                Vertex new_v = {.tint = p.tint};
                new_v.xyz[0] = p.xyz[0] / p.xyz[2];
                new_v.xyz[1] = p.xyz[1] / p.xyz[2];
                new_v.xyz[2] = p.xyz[2];
                return new_v;
        }

        void Place(const Vertex &p0)
        {
                Vertex p = vertShader(p0);
                if ((int)p.xyz[0] < 0.0 ||
                    (int)p.xyz[1] < 0.0 ||
                    (int)p.xyz[0] >= this->w ||
                    (int)p.xyz[1] >= this->h)
                        return;
                const size_t row = (int)p.xyz[1] * this->w;
                const size_t index = row + (int)p.xyz[0];
                if (zbufs[buf][index] > p.xyz[2])
                {
                        const Color tint = (Color){
                            Lerp(fbufs[buf][index].rgba[0], p.tint.rgba[0], p.tint.rgba[3]),
                            Lerp(fbufs[buf][index].rgba[1], p.tint.rgba[1], p.tint.rgba[3]),
                            Lerp(fbufs[buf][index].rgba[2], p.tint.rgba[2], p.tint.rgba[3]),
                            Lerp(fbufs[buf][index].rgba[3], p.tint.rgba[3], p.tint.rgba[3]),
                        };
                        p.tint = tint;
                        fbufs[buf][index] = fragShader(p);
                        zbufs[buf][index] = p.xyz[2];
                }
        }

public:
        Renderer(uint16_t w, uint16_t h)
        {
                this->w = w;
                this->h = h;
                buf = 0;
                fbufs[0] = new Color[w * h];
                fbufs[1] = new Color[w * h];
                zbufs[0] = new float[w * h];
                zbufs[1] = new float[w * h];
        }

        ~Renderer()
        {
                delete[] fbufs[0];
                delete[] fbufs[1];
                delete[] zbufs[0];
                delete[] zbufs[1];
        }

        // General
        virtual void Swap(void) { buf ^= 1; } // swap buffers
        virtual void Commit(void) {}          // commit to screen
        virtual void Update(void) {}          // update loop function
        virtual bool IsRunning(void) { return isRunning; }
        virtual void setFragShader(FragmentShader shader) { fragShader = shader; }
        virtual void setVertShader(VertexShader shader) { vertShader = shader; }

        virtual void Clear(Color color)
        {
                for (size_t i = 0; i < w * h; ++i)
                        fbufs[buf][i] = color;
        }

        virtual void ZClear(void)
        {
                for (size_t i = 0; i < w * h; ++i)
                        zbufs[buf][i] = INFINITY;
        }

        virtual void AO(void)
        {
                for (int y = 0; y < h; y++)
                        for (int x = 0; x < w; x++)
                        {
                                float z = zbufs[buf][y * w + x];
                                if (z == INFINITY)
                                        continue;
                                float occlusion = 0;
                                for (int dy = -4; dy <= 4; dy++)
                                        for (int dx = -4; dx <= 4; dx++)
                                        {
                                                int nx = x + dx, ny = y + dy;
                                                if (nx < 0 || ny < 0 || nx >= w || ny >= h)
                                                        continue;
                                                float nz = zbufs[buf][ny * w + nx];
                                                if (nz < z - 0.01f)
                                                        occlusion += 1.0f;
                                        }
                                occlusion /= 81.0f;
                                fbufs[buf][y * w + x].rgba[0] *= (1.0f - occlusion);
                                fbufs[buf][y * w + x].rgba[1] *= (1.0f - occlusion);
                                fbufs[buf][y * w + x].rgba[2] *= (1.0f - occlusion);
                        }
        }

        // 3D Features (allow override for optimisations)
        virtual void DrawPoint(const Vertex &p0) { Place(Screen(Project(p0))); }
        virtual void DrawLine(const Vertex &p0, const Vertex &p1) { DrawFlatLine(Project(p0), Project(p1)); }
        virtual void DrawTri(const Vertex &p0, const Vertex &p1, const Vertex &p2) { DrawFlatTri(Project(p0), Project(p1), Project(p2)); }

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
                float Δ = 2 * abs(Δy) - Δx;
                float y = a.xyz[1];

                for (size_t x = a.xyz[0]; x < b.xyz[0]; ++x)
                {
                        const float t = (float)(x - a.xyz[0]) / Δx;
                        Vertex vert;
                        vert.tint.rgba[0] = this->Lerp(a.tint.rgba[0], b.tint.rgba[0], t);
                        vert.tint.rgba[1] = this->Lerp(a.tint.rgba[1], b.tint.rgba[1], t);
                        vert.tint.rgba[2] = this->Lerp(a.tint.rgba[2], b.tint.rgba[2], t);
                        vert.tint.rgba[3] = this->Lerp(a.tint.rgba[3], b.tint.rgba[3], t);
                        vert.xyz[0] = x;
                        vert.xyz[1] = y;
                        vert.xyz[2] = this->Lerp(a.xyz[2], b.xyz[2], t);
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

        virtual void DrawFlatTri(const Vertex p0, const Vertex p1, const Vertex p2)
        {
                Vertex a = Screen(p0);
                Vertex b = Screen(p1);
                Vertex c = Screen(p2);
                if (b.xyz[1] > a.xyz[1])
                        std::swap(a, b);
                if (c.xyz[1] > b.xyz[1])
                        std::swap(b, c);
                if (b.xyz[1] > a.xyz[1])
                        std::swap(a, b);
                const float t = (b.xyz[1] - a.xyz[1]) / (c.xyz[1] - a.xyz[1]);
                Vertex d;
                d.xyz[0] = Lerp(a.xyz[0], c.xyz[0], t);
                d.xyz[1] = b.xyz[1];
                d.xyz[2] = Lerp(a.xyz[2], c.xyz[2], t);
                d.tint.rgba[0] = Lerp(a.tint.rgba[0], c.tint.rgba[0], t);
                d.tint.rgba[1] = Lerp(a.tint.rgba[1], c.tint.rgba[1], t);
                d.tint.rgba[2] = Lerp(a.tint.rgba[2], c.tint.rgba[2], t);
                d.tint.rgba[3] = Lerp(a.tint.rgba[3], c.tint.rgba[3], t);
                for (float y = a.xyz[1]; y > b.xyz[1]; --y)
                {
                        float t2 = (y - a.xyz[1]) / (b.xyz[1] - a.xyz[1]);
                        Vertex vl, vr;
                        vl.xyz[0] = Lerp(a.xyz[0], b.xyz[0], t2);
                        vl.xyz[1] = y;
                        vl.xyz[2] = Lerp(a.xyz[2], b.xyz[2], t2);
                        for (size_t i = 0; i < 4; ++i)
                                vl.tint.rgba[i] = Lerp(a.tint.rgba[i], b.tint.rgba[i], t2);
                        vr.xyz[0] = Lerp(a.xyz[0], d.xyz[0], t2);
                        vr.xyz[1] = y;
                        vr.xyz[2] = Lerp(a.xyz[2], d.xyz[2], t2);
                        for (size_t i = 0; i < 4; ++i)
                                vr.tint.rgba[i] = Lerp(a.tint.rgba[i], d.tint.rgba[i], t2);
                        if (vl.xyz[0] > vr.xyz[0])
                                std::swap(vl, vr);
                        for (float x = vl.xyz[0]; x < vr.xyz[0]; ++x)
                        {
                                float tx = (x - vl.xyz[0]) / (vr.xyz[0] - vl.xyz[0]);
                                Vertex vert;
                                vert.xyz[0] = x;
                                vert.xyz[1] = y;
                                vert.xyz[2] = Lerp(vl.xyz[2], vr.xyz[2], tx);
                                for (size_t i = 0; i < 4; ++i)
                                        vert.tint.rgba[i] = Lerp(vl.tint.rgba[i], vr.tint.rgba[i], tx);
                                Place(vert);
                        }
                }
                for (float y = b.xyz[1]; y > c.xyz[1]; --y)
                {
                        float t2 = (y - b.xyz[1]) / (c.xyz[1] - b.xyz[1]);
                        Vertex vl, vr;
                        vl.xyz[0] = Lerp(b.xyz[0], c.xyz[0], t2);
                        vl.xyz[1] = y;
                        vl.xyz[2] = Lerp(b.xyz[2], c.xyz[2], t2);
                        for (size_t i = 0; i < 4; ++i)
                                vl.tint.rgba[i] = Lerp(b.tint.rgba[i], c.tint.rgba[i], t2);
                        vr.xyz[0] = Lerp(d.xyz[0], c.xyz[0], t2);
                        vr.xyz[1] = y;
                        vr.xyz[2] = Lerp(d.xyz[2], c.xyz[2], t2);
                        for (size_t i = 0; i < 4; ++i)
                                vr.tint.rgba[i] = Lerp(d.tint.rgba[i], c.tint.rgba[i], t2);
                        if (vl.xyz[0] > vr.xyz[0])
                                std::swap(vl, vr);
                        for (float x = vl.xyz[0]; x < vr.xyz[0]; ++x)
                        {
                                float tx = (x - vl.xyz[0]) / (vr.xyz[0] - vl.xyz[0]);
                                Vertex vert;
                                vert.xyz[0] = x;
                                vert.xyz[1] = y;
                                vert.xyz[2] = Lerp(vl.xyz[2], vr.xyz[2], tx);
                                for (size_t i = 0; i < 4; ++i)
                                        vert.tint.rgba[i] = Lerp(vl.tint.rgba[i], vr.tint.rgba[i], tx);
                                Place(vert);
                        }
                }
        }
};

void as3d(std::function<void(Renderer &)> main, Renderer &Renderer);

#endif
