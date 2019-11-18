#ifndef _FORM_H
#define _FORM_H

#include "color.h"
#include "linear.h"

typedef struct Form {
    unsigned id;
    Color color;
    XYZ position;
    double reflectance;
} Form;

typedef struct Sphere : Form {
    Sphere();
    Sphere(Color, double, XYZ, double);
    double radius;
} Sphere;

typedef struct Wall : Form {
    XYZ direction;
} Wall;

#endif
