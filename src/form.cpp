#include "form.h"

Sphere::Sphere(Color c, double ref, XYZ pos, double rad)
    : radius(rad)
{
    color = c;
    reflectance = ref;
    position = pos;
}

Sphere::Sphere()
{
}
