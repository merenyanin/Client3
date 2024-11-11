#pragma once
#ifndef GraphicsLib_h
#define GraphicsLib_h

#include <stdint.h>

#ifndef RGB
#define RGB(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3)))
#endif


uint16_t toRGB565(uint8_t red, uint8_t green, uint8_t blue) {
    return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}

class GraphicsLib {
public:
    GraphicsLib(uint_least16_t w, uint_least16_t h) : width(w), height(h) {}
    GraphicsLib() : width(0), height(0) {}

    int_least16_t getWidth(void) { return this->width; }
    int_least16_t getHeight(void) { return this->height; }

    virtual void fillScreen(uint_least16_t color) = 0;
    virtual void drawPixel(int_least16_t x0, int_least16_t y0, uint_least16_t color) = 0;
    virtual void drawLine(int_least16_t x0, int_least16_t y0, int_least16_t x1, int_least16_t y1, uint_least16_t color) = 0;
    virtual void drawRect(int_least16_t x0, int_least16_t y0, int_least16_t w, int_least16_t h, uint_least16_t color) = 0;
    virtual void fillRect(int_least16_t x0, int_least16_t y0, int_least16_t w, int_least16_t h, uint_least16_t color) = 0;
    virtual void drawEllipse(int_least16_t x0, int_least16_t y0, int_least16_t r_x, int_least16_t r_y, uint_least16_t color) = 0;
    virtual void fillEllipse(int_least16_t x0, int_least16_t y0, int_least16_t r_x, int_least16_t r_y, uint_least16_t color) = 0;

private:
    int_least16_t width, height; 
};

#endif // GraphicsLib_h
