#ifndef _COLOR_H
#define _COLOR_H

#include <cstdint>
#include <initializer_list>

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color operator+(uint8_t) const;
    Color operator-(uint8_t) const;
    Color operator*(uint8_t) const;
    Color operator*(double x) const;
    Color operator/(uint8_t) const;
    Color operator+(const Color &) const;
    Color operator-(const Color &) const;
};

Color color_blend(std::initializer_list<Color>);

#endif
