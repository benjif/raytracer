#include <ctime>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <algorithm>
#include <png++/png.hpp>
#include "raytrace.h"

#define EPSILON 1e-5
//#define EPSILON 1e-6

std::pair<XYZ, Form *> Raytracer::intersect(const XYZ &from, const XYZ &to)
{
    auto delta = to - from;

    std::vector<XYZ> hits;
    //XYZ *nearest_hit = nullptr;
    Form *nearest_form = nullptr;

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
            if (t > EPSILON && t < closest_t) {
                closest_t = t;
                nearest_form = &sphere;
            }
        }
    }

    for (auto &wall : m_walls) {
        float denom = dot(wall.normal, delta);
        if (fabs(denom) > EPSILON) {
            float t = dot(wall.position - from, wall.normal) / denom;
            if (t > EPSILON && t < closest_t) {
                closest_t = t;
                nearest_form = &wall;
            }
        }
    }

    if (nearest_form == nullptr)
        return std::make_pair(XYZ{0,0,0}, nullptr);
    else
        return std::make_pair(from + delta * closest_t, nearest_form);
}

Color Raytracer::cast_ray(
    const XYZ &from,
    const XYZ &to,
    unsigned depth = 0,
    bool is_reflect = false
){
    srand(time(NULL));

    auto hit = intersect(from, to);
    auto &nearest_hit = hit.first;
    auto nearest_form = hit.second;

    if (nearest_form == nullptr && !is_reflect)
        return m_background_color;
    else if (nearest_form == nullptr && is_reflect)
        return { 0, 0, 0 };
    else if (nearest_form->id == m_last_hit && is_reflect)
        return { 0, 0, 0 };

    Color out_color =
        nearest_form->render(this, nearest_hit, to - from, depth, is_reflect);

    unsigned shadow_hits = 0;
    for (unsigned sx = 0; sx < m_shadow_grid_size; sx++) {
        for (unsigned sy = 0; sy < m_shadow_grid_size; sy++) {
            auto antiband = rand() % (int)m_shadow_unit_size - m_shadow_unit_size/2;
            XYZ shadow_grid_spot = {
                m_light.x+((double)sx-m_shadow_grid_size/2)*m_shadow_unit_size + antiband,
                m_light.y+((double)sy-m_shadow_grid_size/2)*m_shadow_unit_size + antiband,
                m_light.z,
            };
            auto shadow_hit = intersect(nearest_hit, shadow_grid_spot);
            if (shadow_hit.second != nullptr &&
                    distance(shadow_hit.first, nearest_hit) < distance(nearest_hit, m_light))
                shadow_hits++;
        }
    }

    double shadow_amount = (double)shadow_hits / (m_shadow_grid_size * m_shadow_grid_size);

    m_last_hit = nearest_form->id;

    return out_color *
        (1 - shadow_amount) + nearest_form->color * m_ambient * shadow_amount;
}

void Raytracer::render()
{
    for (unsigned y = 0; y < m_height; y++) {
        for (unsigned x = 0; x < m_width; x++) {
            Color pixel = cast_ray(m_camera, {(double)x, (double)y, 0}, 3);
            m_image[y][x] = png::rgb_pixel(pixel.r, pixel.g, pixel.b);
        }
    }
}

Raytracer::Raytracer(unsigned w, unsigned h)
    : m_width(w),
      m_height(h),
      m_image(w, h),
      m_camera(w/2, h/2, -1500)
{
}

void Raytracer::save(const std::string &filename)
{
    m_image.write(filename);
}

Sphere::Sphere(const Color &c, double ref, double tran, const XYZ &pos, double rad)
    : radius (rad)
{
    color = c;
    reflectance = ref;
    transparency = tran;
    position = pos;
}

Sphere::Sphere()
{
}

Wall::Wall(const Color &c, double ref, double tran, const XYZ &pos, const XYZ &norm)
    : normal(norm)
{
    color = c;
    reflectance = ref;
    transparency = tran;
    position = pos;
}

Wall::Wall()
{
}

/*
static double fresnel_effect(const XYZ &hit, const XYZ &normal, double i)
{
    double dot_norm = dot(hit, normal);
    double e_i = 1.0;
    double e_t = i;
    if (dot_norm > 0.0) {
        e_i = e_t;
        e_t = 1.0;
    }

    double sin_t = e_i / e_t * sqrt(std::max(0.0, 1.0 - dot_norm * dot_norm));
    if (sin_t > 1.0) {
        return 1.0;
    } else {
        double cos_t = sqrt(std::max(0.0, 1.0 - sin_t * sin_t));
        double cos_i = fabs(cos_t);
        double r_s = ((e_t * cos_i) - (e_i * cos_t)) / ((e_t * cos_i) + (e_i * cos_t));
        double r_p = ((e_i * cos_i) - (e_t * cos_t)) / ((e_i * cos_i) + (e_t * cos_t));
        return (r_s * r_s + r_p * r_p) / 2.0;
    }
}
*/

Color Sphere::render(
    Raytracer *rt,
    const XYZ &hit,
    const XYZ &delta,
    unsigned depth,
    bool is_reflect
){
    XYZ unit_norm = (hit - position) / radius;
    XYZ eps_norm = unit_norm * EPSILON;
    double dot_norm = dot(unit_norm, delta);
    bool inside = false;
    if (dot_norm > 0) {
        inside = true;
        unit_norm = -unit_norm;
        dot_norm = -dot_norm;
    }
    double light_mag = distance(rt->m_light, hit);
    XYZ unit_light = (rt->m_light - hit) / light_mag;
    double compute_factor = dot(unit_norm, unit_light);
    Color diffuse_color = color * 0.5 * rt->m_diffuse * compute_factor +
        color * rt->m_ambient;
    double sight_mag = distance(hit, rt->m_camera);
    XYZ unit_sight = (hit - rt->m_camera) / sight_mag;
    double combined_mag = distance({0, 0, 0}, unit_light + unit_sight);
    XYZ unit_bisect = (unit_light + unit_sight) / combined_mag;
    double specular_amount = std::pow(std::max(0.0, dot(unit_norm, unit_bisect)),
            rt->m_specular_size);
    Color specular_color = color * (specular_amount * rt->m_specular);
    double fresnel = reflectance + (1.0 - reflectance) * std::pow(1.0 - std::max(0.0, -dot_norm), 5.0);
    Color out_color = diffuse_color + specular_color;
    if (depth > 0 && reflectance > 0 && !inside) {
        XYZ delta_reflect = delta - unit_norm * 2.0 * dot_norm;
        Color reflect_color = rt->cast_ray(hit + eps_norm, delta_reflect, depth - 1, true);
        out_color = out_color + reflect_color;
    }
    if (transparency > 0) {
        double ior = 1.5;
        double ce = dot(delta, unit_norm) * -1;
        ior = inside ? 1 / ior : ior;
        double eta = 1 / ior;
        XYZ gf = (delta + unit_norm * ce) * eta;
        double sin_t1_2 = 1 - ce * ce;
        double sin_t2_2 = sin_t1_2 * (eta * eta);
        if (sin_t2_2 < 1) {
            XYZ gc = unit_norm * sqrt(1 - sin_t2_2);
            XYZ delta_refract = gf - gc;
            Color refract_color = rt->cast_ray(hit - unit_norm * 1e-4, delta_refract, depth - 1, true);
            out_color = out_color + refract_color * transparency;
        }
    }
    if (is_reflect)
        out_color = out_color * reflectance;
    return out_color;
}

Color Wall::render(
    Raytracer *rt,
    const XYZ &hit,
    const XYZ &delta,
    unsigned depth,
    bool is_reflect
){
    XYZ unit_norm = normal - position;
    XYZ eps_norm = unit_norm * EPSILON;
    double dot_norm = dot(unit_norm, delta);
    bool inside = false;
    if (dot_norm > 0) {
        inside = true;
        unit_norm = -unit_norm;
        dot_norm = -dot_norm;
    }
    double light_mag = distance(rt->m_light, hit);
    XYZ unit_light = (rt->m_light - hit) / light_mag;
    double compute_factor = dot(unit_norm, unit_light);
    Color diffuse_color = color * 0.5 * rt->m_diffuse * compute_factor +
        color * rt->m_ambient;
    double sight_mag = distance(hit, rt->m_camera);
    XYZ unit_sight = (hit - rt->m_camera) / sight_mag;
    double combined_mag = distance({0, 0, 0}, unit_light + unit_sight);
    XYZ unit_bisect = (unit_light + unit_sight) / combined_mag;
    double specular_amount = std::pow(std::max(0.0, dot(unit_norm, unit_bisect)),
            rt->m_specular_size);
    Color specular_color = color * (specular_amount * rt->m_specular);
    //double fresnel = reflectance + (1 - reflectance) *
    //    std::pow(1 - std::max(0.0, -dot_norm), 5);
    Color out_color = diffuse_color + specular_color;
    if (depth > 0 && reflectance > 0 && !inside) {
        XYZ delta_reflect = delta - unit_norm * 2 * dot_norm;
        Color reflect_color = rt->cast_ray(hit + eps_norm, delta_reflect, depth - 1, true);
        out_color = out_color + reflect_color;
    }
    if (is_reflect)
        out_color = out_color * reflectance;
    return out_color;
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

void Raytracer::add_form(Wall &&wall)
{
    m_walls.push_back(wall);
    m_walls.back().id = m_form_count;
    m_form_count++;
}

void Raytracer::add_form(const Wall &wall)
{
    m_walls.push_back(wall);
    m_walls.back().id = m_form_count;
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

void Raytracer::set_shadow_unit_size(double size)
{
    m_shadow_unit_size = size;
}

void Raytracer::set_shadow_grid_size(unsigned size)
{
    m_shadow_grid_size = size;
}

void Raytracer::set_light(const XYZ &pos)
{
    m_light = pos;
}

void Raytracer::set_camera(const XYZ &pos)
{
    m_camera = pos;
}
