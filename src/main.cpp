#include "raytrace.h"

int main(int, char **)
{
    Raytracer raytracer(480, 480);

    raytracer.add_form(Wall {
        { 235, 235, 235 },
        0,
        0,
        { 0, 480, 0 },
        { 0, 479, 0 },
    });

    raytracer.add_form(Wall {
        { 235, 235, 235 },
        0,
        0,
        { 0, 0, 0 },
        { 0, 1, 0 },
    });

    raytracer.add_form(Wall {
        { 235, 235, 235 },
        0,
        0,
        { 0, 0, 900 },
        { 0, 0, 901 },
    });

    raytracer.add_form(Wall {
        { 80, 250, 80 },
        0,
        0,
        { 480, 0, 0 },
        { 479, 0, 0 },
    });

    raytracer.add_form(Wall {
        { 250, 80, 80 },
        0,
        0,
        { 0, 0, 0 },
        { 1, 0, 0 },
    });

    raytracer.add_form(Sphere {
        { 235, 235, 235 },
        0.2,
        0,
        { 480 / 2 - 80, 480 / 2 + 80, 400 },
        100,
    });

    //raytracer.set_light({50, 50, -200});
    raytracer.set_light({420, 70, -400});
    raytracer.set_background({213, 210, 210});

    raytracer.render();
    raytracer.save("out.png");

    return 0;
}
