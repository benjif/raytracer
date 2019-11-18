#include <cmath>
#include <limits>
#include <algorithm>
#include <png++/png.hpp>
#include "raytrace.h"

#define EPSILON 1e-4

Color Raytracer::cast_ray(const XYZ &from, const XYZ &to, unsigned depth, bool is_reflect)
{
    auto delta = to - from;

    std::vector<XYZ> hits;
    XYZ *nearest_hit = nullptr;
    Sphere *nearest_sphere = nullptr;

    double a = dot(delta, delta);
    double closest_t = std::numeric_limits<double>::max();
    for (auto &sphere : m_spheres) {
        double b = dot(delta * 2, from - sphere.position);
        double c = dot(sphere.position, sphere.position) +
            dot(from, from) + -2 * dot(sphere.position, from) -
            std::pow(sphere.radius, 2);
        double disc = std::pow(b, 2) - 4 * a * c;

        if (disc > 0) {
            double t = (-b - std::sqrt(disc)) / (2 * a);
            hits.push_back(from + delta * t);
            if (t > EPSILON && t < closest_t) {
                closest_t = t;
                nearest_hit = &hits.back();
                nearest_sphere = &sphere;
            }
        }
    }

    if (nearest_hit == nullptr && !is_reflect)
        return m_background_color;
    else if (nearest_hit == nullptr && is_reflect)
        return { 0, 0, 0 };
    else if (nearest_sphere->id == m_last_hit && is_reflect)
        return { 0, 0, 0 };

    XYZ unit_norm = (*nearest_hit - nearest_sphere->position) /
        nearest_sphere->radius;
    double light_mag = distance(m_light, *nearest_hit);
    XYZ unit_light = (m_light - *nearest_hit) / light_mag;
    double compute_factor = dot(unit_norm, unit_light);
    Color diffuse_color = nearest_sphere->color * 0.5 * m_diffuse * compute_factor +
        nearest_sphere->color * m_ambient;
    double sight_mag = distance(*nearest_hit, m_camera);
    XYZ unit_sight = (*nearest_hit - m_camera) / sight_mag;
    double combined_mag = distance({0, 0, 0}, unit_light + unit_sight);
    XYZ unit_bisect = (unit_light + unit_sight) / combined_mag;
    double specular_amount = std::pow(std::max(0.0, dot(unit_norm, unit_bisect)),
            m_specular_size);
    Color specular_color = nearest_sphere->color * (specular_amount * m_specular);

    XYZ delta_reflect = *nearest_hit - from;
    XYZ reflect = unit_norm * 2.0 * dot(delta_reflect, unit_norm) -
        delta_reflect;
    Color reflect_color = depth > 0 ?
        cast_ray(*nearest_hit, reflect, depth - 1, true) : Color{ 0, 0, 0 };

    m_last_hit = nearest_sphere->id;

    if (is_reflect)
        return (diffuse_color + specular_color + reflect_color) *
            nearest_sphere->reflectance;
    else
        return diffuse_color + specular_color + reflect_color;
}

void Raytracer::render()
{
    for (unsigned y = 0; y < m_height; y++) {
        for (unsigned x = 0; x < m_width; x++) {
            Color pixel = cast_ray(m_camera, {(double)x, (double)y, 0}, 3, false);
            m_image[y][x] = png::rgb_pixel(pixel.r, pixel.g, pixel.b);
        }
    }
}

Raytracer::Raytracer(unsigned w, unsigned h)
    : m_width(w),
      m_height(h),
      m_image(w, h),
      m_camera(w/2, h/2, -1000.0)
{
    m_light = { 0,
                0,
                -500.0 };
}

void Raytracer::save(const std::string &filename)
{
    m_image.write(filename);
}

void Raytracer::add_form(Sphere &&sphere)
{
    m_spheres.push_back(sphere);
    m_spheres.back().id = m_form_count;
    m_form_count++;
}

void Raytracer::add_form(const Sphere &sphere)
{
    m_spheres.push_back(sphere);
    m_spheres.back().id = m_form_count;
    m_form_count++;
}

void Raytracer::set_diffuse(double coeff)
{
    m_diffuse = coeff;
}

void Raytracer::set_ambient(double coeff)
{
    m_ambient = coeff;
}

void Raytracer::set_specular(double coeff)
{
    m_specular = coeff;
}

void Raytracer::set_specular_size(int power)
{
    m_specular_size = power;
}

void Raytracer::set_background(const Color &c)
{
    m_background_color = c;
}
