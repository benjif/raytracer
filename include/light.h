#ifndef _LIGHT_H
#define _LIGHT_H

#include "color.h"
#include "linear.h"

typedef struct PointLight {
    XYZ source;
    Color color;
    double intensity;
} PointLight;

#endif
