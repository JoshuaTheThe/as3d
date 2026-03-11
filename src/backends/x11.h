#ifndef BACKEND_X11_H
#define BACKEND_X11_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

class X11Backend : public Renderer
{
private:
        Display *display;
        Window window;
        GC gc;
        XImage *image;
        uint32_t *pixels;
public:
        X11Backend(uint16_t w, uint16_t h) : Renderer(w, h)
        {
                display = XOpenDisplay(nullptr);
                if (!display) { printf("[error] cannot open display\n"); exit(1); }
                int screen = DefaultScreen(display);
                window = XCreateSimpleWindow(display,
                                             RootWindow(display, screen),
                                             0, 0, w, h, 0,
                                             BlackPixel(display, screen),
                                             BlackPixel(display, screen));
                XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask);
                XMapWindow(display, window);
                gc = XDefaultGC(display, screen);
                pixels = new uint32_t[w * h];
                image = XCreateImage(display,
                                     DefaultVisual(display, screen),
                                     DefaultDepth(display, screen),
                                     ZPixmap, 0,
                                     (char *)pixels,
                                     w, h, 32, 0);
                isRunning = true;
        }
        ~X11Backend(void)
        {
                isRunning = false;
                image->data = nullptr; // prevent XDestroyImage from freeing pixels
                XDestroyImage(image);
                delete[] pixels;
                XDestroyWindow(display, window);
                XCloseDisplay(display);
        }
        void Commit(void)
        {
                for (size_t i = 0; i < w * h; ++i)
                {
                        Color &c = fbufs[buf][i];
                        uint8_t r = c.rgba[0] * 255;
                        uint8_t g = c.rgba[1] * 255;
                        uint8_t b = c.rgba[2] * 255;
                        pixels[i] = (r << 16) | (g << 8) | b;
                }
                XPutImage(display, window, gc, image, 0, 0, 0, 0, w, h);
                XFlush(display);
        }
        void Update(void)
        {
                XEvent e;
                while (XPending(display))
                {
                        XNextEvent(display, &e);
                        switch (e.type)
                        {
                                case KeyPress:
                                        if (e.xkey.keycode < 256)
                                                keymap[e.xkey.keycode] = true;
                                        break;
                                case KeyRelease:
                                        if (e.xkey.keycode < 256)
                                                keymap[e.xkey.keycode] = false;
                                        break;
                        }
                }
        }
};

#endif
