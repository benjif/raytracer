#include "raytrace.h"

int main(int, char **)
{
    Raytracer raytracer(720, 480);

    raytracer.add_form(Sphere {
        { 230, 90, 100 },
        0.5,
        { 720 / 2 + 100, 480 / 2, 100 },
        100
    });

    raytracer.add_form(Sphere {
        { 90, 230, 100 },
        0.5,
        { 720 / 2 - 100, 480 / 2, 100 },
        100
    });

    raytracer.set_background({53, 50, 50});
    raytracer.render();
    raytracer.save("out.png");

    return 0;
}
