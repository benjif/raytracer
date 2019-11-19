#ifndef _RAYTRACE_H
#define _RAYTRACE_H

#include <string>
#include <vector>
#include <utility>
#include <png++/png.hpp>
#include "form.h"
#include "light.h"

class Raytracer {
public:
    Raytracer(unsigned, unsigned);

    void render();
    std::pair<XYZ, Sphere *> intersect(const XYZ &, const XYZ &);
    Color cast_ray(const XYZ &, const XYZ &, unsigned, bool);

    void save(const std::string &);

    void add_form(Sphere &&);
    void add_form(const Sphere &);

    void set_camera(const XYZ &);

    void set_diffuse(double);
    void set_ambient(double);
    void set_specular(double);
    void set_specular_size(int);

    void set_shadow_unit_size(double);
    void set_shadow_grid_size(unsigned);

    void set_background(const Color &);

private:
    const unsigned m_width;
    const unsigned m_height;

    png::image<png::rgb_pixel> m_image;
    std::vector<Sphere> m_spheres;
    // TODO: implement walls
    std::vector<Wall> m_walls;

    unsigned m_form_count { 0 };
    unsigned m_last_hit;

    double m_diffuse { 0.7 };
    double m_ambient { 0.25 };
    double m_specular { 0.5 };
    double m_specular_size { 30 };

    // TODO: randomize grid positions to reduce
    //       color banding effect
    double m_shadow_unit_size { 30.0 };
    unsigned m_shadow_grid_size { 8 };

    Color m_background_color { 0, 0, 0 };

    XYZ m_light;
    XYZ m_camera;
    Sphere *previous_reflect_hit { nullptr };
};

#endif
