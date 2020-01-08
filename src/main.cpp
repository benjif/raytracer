#include "raytrace.h"

int main(int, char **)
{
    Raytracer raytracer(480, 480);

    raytracer.add_form(Wall {
        { 240, 240, 240 },
        0.6,
        1,
        0,
        { 0, 480, 0 },
        { 0, 479, 0 },
    });

    raytracer.add_form(Wall {
        { 240, 240, 240 },
        0.8,
        1,
        0,
        { 0, 0, 850 },
        { 0, 0, 849 },
    });

    raytracer.add_form(Wall {
        { 240, 240, 240 },
        0.6,
        1.5,
        0,
        { 0, -100, 0 },
        { 0, -99, 0 },
    });

    raytracer.add_form(Wall {
        { 80, 250, 70 },
        0.6,
        1.6,
        0,
        { 580, 0, 0 },
        { 579, 0, 0 },
    });

    raytracer.add_form(Wall {
        { 250, 70, 80 },
        0.5,
        1.6,
        0,
        { -100, 0, 0 },
        { -99, 0, 0 },
    });

    raytracer.add_form(Sphere {
        { 250, 70, 80 },
        0.9,
        2,
        0,
        { 480 / 2 - 60, 380, 320 },
        100,
    });

    raytracer.add_form(Sphere {
        { 245, 245, 245 },
        1,
        1,
        1,
        { 480 / 2 + 10, 480 - 50, 160 },
        50,
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

    raytracer.set_pixel_sample_size(20);
    raytracer.set_reflection_depth(5);
    raytracer.set_shadow_unit_size(24);
    raytracer.set_shadow_grid_size(2);

    raytracer.set_light({480 - 70, 70, -400});
    raytracer.set_background({213, 210, 210});

    raytracer.render();
    raytracer.save("out.png");

    return 0;
}
