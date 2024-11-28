#pragma once
#ifndef GraphicsLib_h
#define GraphicsLib_h

#include <stdint.h>
#include <string>
#include <vector>

#ifndef RGB
#define RGB(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3)))
#endif

uint16_t toRGB565(uint8_t red, uint8_t green, uint8_t blue) {
    return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}

class GraphicsLib {
public:
    GraphicsLib(uint_least16_t w, uint_least16_t h) : width(w), height(h), orientation(0) {}
    GraphicsLib() : width(0), height(0), orientation(0) {}

    int_least16_t getWidth() const { return this->width; }
    int_least16_t getHeight() const { return this->height; }




    virtual void fillScreen(uint_least16_t color) = 0;
    virtual void drawPixel(int_least16_t x0, int_least16_t y0, uint_least16_t color) = 0;
    virtual void drawLine(int_least16_t x0, int_least16_t y0, int_least16_t x1, int_least16_t y1, uint_least16_t color) = 0;
    virtual void drawRect(int_least16_t x0, int_least16_t y0, int_least16_t w, int_least16_t h, uint_least16_t color) = 0;
    virtual void fillRect(int_least16_t x0, int_least16_t y0, int_least16_t w, int_least16_t h, uint_least16_t color) = 0;
    virtual void drawEllipse(int_least16_t x0, int_least16_t y0, int_least16_t r_x, int_least16_t r_y, uint_least16_t color) = 0;
    virtual void fillEllipse(int_least16_t x0, int_least16_t y0, int_least16_t r_x, int_least16_t r_y, uint_least16_t color) = 0;
    virtual void drawText(int16_t x, int16_t y, uint16_t color, const std::string& text, int_least16_t spacing = 19) = 0;
    virtual void setOrientation(int_least16_t orientation) = 0;
    virtual void loadSprite(uint16_t index, uint16_t width, uint16_t height, const std::vector<uint8_t>& data) = 0;
    virtual void showSprite(uint16_t index, int16_t x, int16_t y) = 0;

protected:
    int_least16_t width, height;
    int_least16_t orientation; 
};

#endif // GraphicsLib_h