#include "color.h"

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

Color::Color()
{
}

Color::Color(uint8_t r, uint8_t g, uint8_t b)
    : r(r), g(g), b(b)
{
}

Color Color::operator+(uint8_t x) const
{
    return {
        (uint8_t)(MIN(256, r + x)),
        (uint8_t)(MIN(256, g + x)),
        (uint8_t)(MIN(256, b + x)),
    };
}

Color Color::operator-(uint8_t x) const
{
    return {
        (uint8_t)(MAX(0, r - x)),
        (uint8_t)(MAX(0, g - x)),
        (uint8_t)(MAX(0, b - x)),
    };
}

Color Color::operator*(double x) const
{
    return {
        (uint8_t)(MIN(256, MAX(0, (double)r * x))),
        (uint8_t)(MIN(256, MAX(0, (double)g * x))),
        (uint8_t)(MIN(256, MAX(0, (double)b * x))),
    };
}

Color Color::operator*(uint8_t x) const
{
    return {
        (uint8_t)(MIN(256, r * x)),
        (uint8_t)(MIN(256, g * x)),
        (uint8_t)(MIN(256, b * x)),
    };
}

Color Color::operator/(uint8_t x) const
{
    return {
        (uint8_t)(r / x),
        (uint8_t)(g / x),
        (uint8_t)(b / x),
    };
}

Color Color::operator+(const Color &other) const
{
    return {
        (uint8_t)(MIN(256, r + other.r)),
        (uint8_t)(MIN(256, g + other.g)),
        (uint8_t)(MIN(256, b + other.b)),
    };
}

Color Color::operator-(const Color &other) const
{
    return {
        (uint8_t)(MAX(0, r - other.r)),
        (uint8_t)(MAX(0, g - other.g)),
        (uint8_t)(MAX(0, b - other.b)),
    };
}
