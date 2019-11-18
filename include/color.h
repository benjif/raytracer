#ifndef _COLOR_H
#define _COLOR_H

#include <cstdint>

typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color();
    Color(uint8_t, uint8_t, uint8_t);

    Color operator+(uint8_t) const;
    Color operator-(uint8_t) const;
    Color operator*(uint8_t) const;
    Color operator*(double x) const;
    Color operator/(uint8_t) const;
    Color operator+(const Color &) const;
    Color operator-(const Color &) const;
} Color;

#endif
