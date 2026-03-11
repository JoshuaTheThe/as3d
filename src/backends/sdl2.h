
#ifndef BACKEND_SDL2_H
#define BACKEND_SDL2_H

#include <core.h>
#include <ll.h>

class   SDL2Backend : public Renderer
{
private:
        SDL_Window *SDLWindow;
        SDL_Renderer *SDLRenderer;
        SDL_Texture *SDLTexture;
public:
        SDL2Backend(uint16_t w, uint16_t h) : Renderer(w, h)
        {
                if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
                {
                        printf(" [error] : %s\n", SDL_GetError());
                        exit(1);
                }

                SDLWindow = SDL_CreateWindow("Title",
                                             SDL_WINDOWPOS_CENTERED,
                                             SDL_WINDOWPOS_CENTERED,
                                             w,
                                             h,
                                             SDL_WINDOW_SHOWN);
                if (!SDLWindow)
                {
                        printf(" [error] : %s\n", SDL_GetError());
                        exit(1);
                }

                SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
                if (!SDLRenderer)
                {
                        printf(" [error] : %s\n", SDL_GetError());
                        exit(1);
                }
                isRunning = true;

                SDLTexture = SDL_CreateTexture(SDLRenderer,
                                               SDL_PIXELFORMAT_RGBA32,
                                               SDL_TEXTUREACCESS_STREAMING,
                                               w, h);
                if (!SDLTexture)
                {
                        printf(" [error] : %s\n", SDL_GetError());
                        exit(1);
                }
        }

        ~SDL2Backend(void)
        {
                isRunning = false;
                SDL_DestroyRenderer(SDLRenderer);
                SDL_DestroyWindow(SDLWindow);
                SDL_Quit();
        }

        void Update(void)
        {
                SDL_Event e;
                while (SDL_PollEvent(&e))
                {
                        switch (e.type)
                        {
                                case SDL_QUIT:
                                        isRunning = false;
                                        break;
                        }
                }
        }

        void Clear(Color color)
        {
                for (size_t i = 0; i < w*h; ++i)
                        fbufs[buf][i] = color;
        }

        // copy fbuf[buf] to screen
        void Commit(void)
        {
                uint32_t *pixels = new uint32_t[w*h];
                for (size_t i = 0; i < w*h; ++i)
                {
                        Color &c = fbufs[buf][i];
                        uint8_t r = c.rgba[0] * 255;
                        uint8_t g = c.rgba[1] * 255;
                        uint8_t b = c.rgba[2] * 255;
                        uint8_t a = c.rgba[3] * 255;
                        pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
                }
                SDL_UpdateTexture(SDLTexture, nullptr, pixels, w * sizeof(uint32_t));
                delete[] pixels;
                SDL_RenderCopy(SDLRenderer, SDLTexture, nullptr, nullptr);
                SDL_RenderPresent(SDLRenderer);
        }
};

#endif
