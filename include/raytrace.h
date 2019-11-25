#ifndef _RAYTRACE_H
#define _RAYTRACE_H

#include <string>
#include <vector>
#include <utility>
#include <png++/png.hpp>
#include "light.h"

struct Form;
struct Sphere;
struct Wall;
struct Triangle;

class Raytracer {
public:
    Raytracer(unsigned, unsigned);

    void render();
    std::pair<XYZ, Form *> intersect(const XYZ &, const XYZ &);
    Color cast_ray(const XYZ &, const XYZ &, unsigned, bool);

    void save(const std::string &);

    void add_form(Sphere &&);
    void add_form(const Sphere &);

    void add_form(Wall &&);
    void add_form(const Wall &);

    void add_form(Triangle &&);
    void add_form(const Triangle &);

    void set_light(const XYZ &);
    void set_camera(const XYZ &);

    void set_diffuse(double);
    void set_ambient(double);
    void set_specular(double);
    void set_specular_size(int);

    void set_reflection_depth(unsigned);

    void set_shadow_unit_size(double);
    void set_shadow_grid_size(unsigned);

    void set_pixel_sample_size(unsigned);

    void set_background(const Color &);

private:
    const unsigned m_width;
    const unsigned m_height;

    png::image<png::rgb_pixel> m_image;
    std::vector<Sphere> m_spheres;
    std::vector<Wall> m_walls;
    std::vector<Triangle> m_triangles;

    double shadow_amount(const XYZ &);

    double m_diffuse { 0.7 };
    double m_ambient { 0.26 };
    double m_specular { 0.5 };
    double m_specular_size { 10 };

    unsigned m_reflection_depth { 3 };

    double m_shadow_unit_size { 12.0 };
    unsigned m_shadow_grid_size { 12 };

    unsigned m_pixel_sample_size { 8 };

    Color m_background_color { 0, 0, 0 };

    XYZ m_light;
    XYZ m_camera;

    friend class Sphere;
    friend class Wall;
    friend class Triangle;
};

struct Form {
    virtual Color render(Raytracer *, const XYZ &, const XYZ &, unsigned) = 0;
    unsigned id;
    Color color;
    double reflectance;
    double refractive_index;
    double transmittance;
    XYZ position;
};

struct Sphere : Form {
    Sphere();
    Sphere(const Color &, double, double, double, const XYZ &, double);
    Color render(Raytracer *, const XYZ &, const XYZ &, unsigned) override;
    double radius;
};

struct Wall : Form {
    Wall();
    Wall(const Color &, double, double, double, const XYZ &, const XYZ &);
    Color render(Raytracer *, const XYZ &, const XYZ &, unsigned) override;
    XYZ normal;
};

struct Triangle : Form {
    Triangle();
    Triangle(const Color &, double, double, double, const XYZ &, const XYZ &, const XYZ &);
    Color render(Raytracer *, const XYZ &, const XYZ &, unsigned) override;
    XYZ vertices[3];
    XYZ edges[2];
};

#endif
