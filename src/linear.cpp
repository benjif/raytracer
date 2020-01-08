#include <cmath>
#include "linear.h"

XYZ::XYZ(double x, double y, double z)
    : x(x), y(y), z(z)
{
}

XYZ XYZ::operator+(double a) const
{
    return {
        x + a,
        y + a,
        z + a,
    };
}

XYZ XYZ::operator-(double a) const
{
    return {
        x - a,
        y - a,
        z - a,
    };
}

XYZ XYZ::operator*(double a) const
{
    return {
        x * a,
        y * a,
        z * a,
    };
}

XYZ XYZ::operator/(double a) const
{
    return {
        x / a,
        y / a,
        z / a,
    };
}

XYZ XYZ::operator+(const XYZ &other) const
{
    return {
        x + other.x,
        y + other.y,
        z + other.z,
    };
}

XYZ XYZ::operator-(const XYZ &other) const
{
    return {
        x - other.x,
        y - other.y,
        z - other.z,
    };
}

XYZ XYZ::operator-() const
{
    return {
        -x,
        -y,
        -z,
    };
}

double XYZ::magnitude() const
{
    return sqrt(pow(x, 2) +
                pow(y, 2) +
                pow(z, 2));
}

XYZ XYZ::normal() const
{
    double mag = magnitude();
    return {
        x / mag,
        y / mag,
        z / mag,
    };
}

double dot(const XYZ &a, const XYZ &b)
{
    return a.x * b.x +
           a.y * b.y +
           a.z * b.z;
}

XYZ cross(const XYZ &a, const XYZ &b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.x * b.z - a.z * b.x,
        a.x * b.y - a.y * b.x,
    };
}

double distance(const XYZ &a, const XYZ &b)
{
    return sqrt(pow(b.z-a.z, 2) +
                pow(b.y-a.y, 2) +
                pow(b.x-a.x, 2));
}
