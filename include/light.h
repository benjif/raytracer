#ifndef _LIGHT_H
#define _LIGHT_H

#include "color.h"
#include "linear.h"

struct PointLight {
    XYZ source;
    Color color;
    double intensity;
};

#endif
