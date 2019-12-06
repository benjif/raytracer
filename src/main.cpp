#include "raytrace.h"

int main(int, char **)
{
    Raytracer raytracer(480, 480);

    raytracer.add_form(Wall {
        { 255, 255, 255 },
        0.8,
        1.8,
        0,
        { 0, 480, 0 },
        { 0, 479, 0 },
    });

    raytracer.add_form(Wall {
        { 255, 255, 255 },
        0.8,
        1.8,
        0,
        { 0, 0, 0 },
        { 0, 1, 0 },
    });

    raytracer.add_form(Wall {
        { 240, 240, 240 },
        0.6,
        3.4,
        0,
        { 0, 0, 550 },
        { 0, 0, 549 },
    });

    raytracer.add_form(Wall {
        { 70, 250, 80 },
        0.8,
        1.8,
        0,
        { 480, 0, 0 },
        { 479, 0, 0 },
    });

    raytracer.add_form(Wall {
        { 250, 70, 80 },
        0.6,
        1.8,
        0,
        { 0, 0, 0 },
        { 1, 0, 0 },
    });

    raytracer.add_form(Wall {
        { 255, 255, 255 },
        0.6,
        1.8,
        0,
        { 0, 0, -720 },
        { 0, 0, -721 },
    });

    raytracer.add_form(Sphere {
        { 250, 70, 80 },
        0.9,
        2.4,
        0,
        { 480 / 2 - 60, 380, 340 },
        100,
    });

    raytracer.add_form(Sphere {
        { 245, 245, 245 },
        0.9,
        2.4,
        0,
        { 480 / 2 + 10, 480 - 50, 170 },
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

    raytracer.set_pixel_sample_size(2);
    raytracer.set_reflection_depth(3);
    raytracer.set_shadow_unit_size(24);
    raytracer.set_shadow_grid_size(8);

    raytracer.set_light({480 - 50, 50, -400});
    raytracer.set_background({213, 210, 210});

    raytracer.render();
    raytracer.save("out.png");

    return 0;
}
