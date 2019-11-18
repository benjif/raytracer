#ifndef _RAYTRACE_H
#define _RAYTRACE_H

#include <string>
#include <vector>
#include <png++/png.hpp>
#include "form.h"
#include "light.h"

class Raytracer {
public:
    Raytracer(unsigned, unsigned);

    void render();
    Color cast_ray(const XYZ &, const XYZ &, unsigned, bool);

    void save(const std::string &);

    void add_form(Sphere &&);
    void add_form(const Sphere &);

    void set_diffuse(double);
    void set_ambient(double);
    void set_specular(double);
    void set_specular_size(int);

    void set_background(const Color &);

private:
    const unsigned m_width;
    const unsigned m_height;

    png::image<png::rgb_pixel> m_image;
    std::vector<Sphere> m_spheres;
    unsigned m_form_count { 0 };
    unsigned m_last_hit;
    XYZ m_camera;

    double m_diffuse { 0.7 };
    double m_ambient { 0.2 };
    double m_specular { 0.5 };
    double m_specular_size { 30 };

    Color m_background_color { 0, 0, 0 };

    XYZ m_light;
    Sphere *previous_reflect_hit;
};

#endif
