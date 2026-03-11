#ifndef BACKEND_WINDOWS_H
#define BACKEND_WINDOWS_H

#include <windows.h>

class Win32Backend : public Renderer
{
private:
        HWND hwnd;
        HDC hdc;
        BITMAPINFO bmi;
        uint32_t *pixels;
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
        {
                Win32Backend *self = (Win32Backend *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                switch (msg)
                {
                        case WM_DESTROY:
                                if (self) self->isRunning = false;
                                PostQuitMessage(0);
                                return 0;
                        case WM_KEYDOWN:
                                if (self && wp < 256) self->keymap[wp] = true;
                                return 0;
                        case WM_KEYUP:
                                if (self && wp < 256) self->keymap[wp] = false;
                                return 0;
                }
                return DefWindowProc(hwnd, msg, wp, lp);
        }
public:
        Win32Backend(uint16_t w, uint16_t h) : Renderer(w, h)
        {
                pixels = new uint32_t[w * h];
                WNDCLASS wc = {};
                wc.lpfnWndProc   = WndProc;
                wc.hInstance     = GetModuleHandle(nullptr);
                wc.lpszClassName = "as3d";
                RegisterClass(&wc);
                hwnd = CreateWindowEx(0, "as3d", "Title",
                                      WS_OVERLAPPEDWINDOW,
                                      CW_USEDEFAULT, CW_USEDEFAULT,
                                      w, h, nullptr, nullptr,
                                      GetModuleHandle(nullptr), nullptr);
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
                hdc = GetDC(hwnd);
                memset(&bmi, 0, sizeof(bmi));
                bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth       = w;
                bmi.bmiHeader.biHeight      = -h; // top-down
                bmi.bmiHeader.biPlanes      = 1;
                bmi.bmiHeader.biBitCount    = 32;
                bmi.bmiHeader.biCompression = BI_RGB;
                ShowWindow(hwnd, SW_SHOW);
                isRunning = true;
        }
        ~Win32Backend(void)
        {
                isRunning = false;
                ReleaseDC(hwnd, hdc);
                DestroyWindow(hwnd);
                delete[] pixels;
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
                StretchDIBits(hdc, 0, 0, w, h, 0, 0, w, h,
                              pixels, &bmi, DIB_RGB_COLORS, SRCCOPY);
        }
        void Update(void)
        {
                MSG msg;
                while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
                {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                }
        }
};

#endif
