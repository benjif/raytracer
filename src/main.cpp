#include "raytrace.h"

int main(int, char **)
{
    Raytracer raytracer(480, 480);

    raytracer.add_form(Wall {
        { 255, 255, 255 },
        0.6,
        1.5,
        0,
        { 0, 480, 0 },
        { 0, 479, 0 },
    });

    raytracer.add_form(Wall {
        { 255, 255, 255 },
        0,
        1.5,
        0,
        { 0, 0, 0 },
        { 0, 1, 0 },
    });

    raytracer.add_form(Wall {
        { 255, 255, 255 },
        0.6,
        1.5,
        0,
        { 0, 0, 950 },
        { 0, 0, 951 },
    });

    raytracer.add_form(Wall {
        { 70, 250, 80 },
        0.6,
        1.5,
        0,
        { 480, 0, 0 },
        { 479, 0, 0 },
    });

    raytracer.add_form(Wall {
        { 250, 70, 80 },
        0.6,
        1.5,
        0,
        { 0, 0, 0 },
        { 1, 0, 0 },
    });

    raytracer.add_form(Wall {
        { 255, 255, 255 },
        0.6,
        1.5,
        0,
        { 0, 0, -700 },
        { 0, 0, -701 },
    });

    raytracer.add_form(Sphere {
        { 255, 255, 255 },
        0.9,
        1.8,
        0,
        { 480 / 2 - 60, 380, 340 },
        100,
    });

    /*
    raytracer.add_form(Triangle {
        { 255, 80, 80 },
        0,
        1.5,
        0,
        { 340, 340, 400 },
        { 340+100, 340, 400 },
        { 340, 340+100, 400 }
    });
    */

    raytracer.set_pixel_sample_size(2);

    raytracer.set_light({380, 100, -400});
    raytracer.set_background({213, 210, 210});

    raytracer.render();
    raytracer.save("out.png");

    return 0;
}
