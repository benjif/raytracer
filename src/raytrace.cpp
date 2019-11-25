#include <ctime>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <algorithm>
#include <png++/png.hpp>
#include "raytrace.h"

#define EPSILON 1e-4

static inline void clamp(double &v, double min, double max)
{
    if (v < min)
        v = min;
    else if (v > max)
        v = max;
}

std::pair<XYZ, Form *> Raytracer::intersect(const XYZ &from, const XYZ &to)
{
    auto delta = to - from;
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
            double t = (-b - sqrt(disc)) / (2 * a);
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

    for (auto &tri : m_triangles) {
        XYZ h = cross(delta, tri.edges[1]);
        double a = dot(h, tri.edges[0]);
        if (fabs(a) < EPSILON)
            continue;
        double f = 1 / a;
        XYZ s = from - tri.vertices[0];
        double u = f * dot(s, h);
        if (u < 0 || u > 1)
            continue;
        XYZ q = cross(s, tri.edges[0]);
        double v = f * dot(delta, q);
        if (v < 0 || u + v > 1)
            continue;
        double t = f * dot(tri.edges[1], q);
        if (t < closest_t) {
            closest_t = t;
            nearest_form = &tri;
        }
    }

    if (nearest_form == nullptr)
        return std::make_pair(XYZ{ 0, 0, 0 }, nullptr);
    else
        return std::make_pair(from + delta * closest_t, nearest_form);
}

Color Raytracer::cast_ray(
    const XYZ &from,
    const XYZ &to,
    unsigned depth = 0,
    bool is_reflect = false
){
    auto hit = intersect(from, to);
    auto &nearest_hit = hit.first;
    auto nearest_form = hit.second;

    if (nearest_form == nullptr)
        if (is_reflect)
            return { 0, 0, 0 };
        else
            return m_background_color;

    return nearest_form->render(this, nearest_hit, (to - from).normal(), depth);
}

void Raytracer::render()
{
    for (unsigned y = 0; y < m_height; y++) {
        for (unsigned x = 0; x < m_width; x++) {
            int r0 = 0, g0 = 0, b0 = 0;
            for (unsigned p = 0; p < m_pixel_sample_size; p++) {
                Color tmp = cast_ray(
                    m_camera,
                    {
                        (double)x-1 + ((double)p / m_pixel_sample_size) - 0.5,
                        (double)y-1 + ((double)p / m_pixel_sample_size) - 0.5,
                        0,
                    },
                    m_reflection_depth
                );
                r0 += tmp.r;
                g0 += tmp.g;
                b0 += tmp.b;
            }

            m_image[y][x] = png::rgb_pixel(
                (uint8_t)(r0 / m_pixel_sample_size),
                (uint8_t)(g0 / m_pixel_sample_size),
                (uint8_t)(b0 / m_pixel_sample_size)
            );
        }
    }
}

Raytracer::Raytracer(unsigned w, unsigned h)
    : m_width(w),
      m_height(h),
      m_image(w, h),
      m_camera(w/2, h/2, -700)
{
    srand(time(NULL));
}

void Raytracer::save(const std::string &filename)
{
    m_image.write(filename);
}

Sphere::Sphere(const Color &c, double refl, double refr, double tran, const XYZ &pos, double rad)
    : radius (rad)
{
    color = c;
    reflectance = refl;
    refractive_index = refr;
    transmittance = tran;
    position = pos;
}

Sphere::Sphere()
{
}

Wall::Wall(const Color &c, double refl, double refr, double tran, const XYZ &pos, const XYZ &norm)
    : normal(norm)
{
    color = c;
    reflectance = refl;
    refractive_index = refr;
    transmittance = tran;
    position = pos;
}

Wall::Wall()
{
}

static bool xyz_ccw_cmp(const XYZ &center, const XYZ &a, const XYZ &b)
{
    int dax = (a.x - center.x) > 0 ? 1 : 0;
    int day = (a.y - center.y) > 0 ? 1 : 0;
    int qa = (1 - dax) + (1 - day) + ((dax & (1 - day)) << 1);

    int dbx = (b.x - center.x) > 0 ? 1 : 0;
    int dby = (b.y - center.y) > 0 ? 1 : 0;
    int qb = (1 - dbx) + (1 - dby) + ((dbx & (1 - dby)) << 1);

    if (qa == qb)
        return (b.x - center.x) * (a.y - center.y) < (b.y - center.y) * (a.x - center.x);
    else
        return qa < qb;
}

Triangle::Triangle(
    const Color &c,
    double refl,
    double refr,
    double tran,
    const XYZ &v1,
    const XYZ &v2,
    const XYZ &v3
)
    : vertices({v1, v2, v3})
{
    color = c;
    reflectance = refl;
    refractive_index = refr;
    transmittance = tran;
    position = (v1 + v2 + v3) / 3;
    auto cmp_func = [&](const XYZ &a, const XYZ &b) {
        return xyz_ccw_cmp(position, a, b);
    };
    std::sort(vertices, vertices + 3, cmp_func);
    edges[0] = vertices[1] - vertices[0];
    edges[1] = vertices[2] - vertices[0];
}

Triangle::Triangle()
{
}

double Raytracer::shadow_amount(const XYZ &hit)
{
    double shadow_hits = 0;
    for (unsigned sx = 0; sx < m_shadow_grid_size; sx++) {
        for (unsigned sy = 0; sy < m_shadow_grid_size; sy++) {
            auto antiband = rand() % (int)m_shadow_unit_size - m_shadow_unit_size/2;
            XYZ shadow_grid_spot = {
                m_light.x+((double)sx-m_shadow_grid_size/2)*m_shadow_unit_size + antiband,
                m_light.y+((double)sy-m_shadow_grid_size/2)*m_shadow_unit_size + antiband,
                m_light.z,
            };
            auto shadow_hit = intersect(hit, shadow_grid_spot);
            if (shadow_hit.second != nullptr &&
                    distance(shadow_hit.first, hit) < distance(hit, m_light))
                shadow_hits += 1 - shadow_hit.second->transmittance;
        }
    }
    return shadow_hits / (m_shadow_grid_size * m_shadow_grid_size);
}

static double fresnel_amount(const XYZ &delta, const XYZ &norm, double ior)
{
    double cos_i = dot(delta, norm);
    clamp(cos_i, -1, 1);

    double eta_i = 1, eta_t = ior;
    if (cos_i > 0) std::swap(eta_i, eta_t);

    double sin_t = eta_i / eta_t * sqrt(MAX(0, 1 - cos_i * cos_i));

    if (sin_t >= 1) {
        return 1;
    } else {
        double cos_t = sqrt(MAX(0, 1 - sin_t * sin_t));
        cos_i = fabs(cos_i);
        double r_s = ((eta_t * cos_i) - (eta_i * cos_t)) /
                     ((eta_t * cos_i) + (eta_i * cos_t));
        double r_p = ((eta_i * cos_i) - (eta_t * cos_t)) /
                     ((eta_i * cos_i) + (eta_t * cos_t));
        return (r_s * r_s + r_p * r_p) / 2;
    }
}

Color Sphere::render(
    Raytracer *rt,
    const XYZ &hit,
    const XYZ &delta,
    unsigned depth
){
    XYZ unit_norm = (hit - position) / radius;
    double dot_norm = dot(delta, unit_norm);
    double light_mag = distance(rt->m_light, hit);
    XYZ unit_light = (rt->m_light - hit) / light_mag;
    double compute_factor = dot(unit_norm, unit_light);
    Color diffuse_color = color * 0.5 * rt->m_diffuse * compute_factor +
        color * rt->m_ambient;
    double sight_mag = distance(hit, rt->m_camera);
    XYZ unit_sight = (hit - rt->m_camera) / sight_mag;
    double combined_mag = distance({ 0, 0, 0 }, unit_light + unit_sight);
    XYZ unit_bisect = (unit_light + unit_sight) / combined_mag;
    double specular_amount = std::pow(std::max(0.0, dot(unit_norm, unit_bisect)),
            rt->m_specular_size);
    Color specular_color = color * (specular_amount * rt->m_specular);
    Color out_color = diffuse_color + specular_color;
    double fresnel = fresnel_amount(delta, unit_norm, refractive_index);
    if (depth > 0 && reflectance > 0) {
        XYZ eps_norm = unit_norm * EPSILON;
        XYZ delta_reflect = delta - unit_norm * 2 * dot_norm;
        Color reflect_color = rt->cast_ray(hit + eps_norm, hit + delta_reflect, depth - 1, true);
        out_color = out_color +
            reflect_color * reflectance * fresnel;
    }
    if (depth > 0 && transmittance > 0 && fresnel < 1) {
        bool outside = true;
        if (dot_norm > 0) {
            outside = false;
            unit_norm = -unit_norm;
            dot_norm = -dot_norm;
        }
        XYZ bias = unit_norm * 1e-4;
        double cos_i = dot(delta, unit_norm);
        clamp(cos_i, -1, 1);
        double eta_i = 1, eta_t = refractive_index;
        if (cos_i > 0) std::swap(eta_i, eta_t);
        double eta = eta_i / eta_t;
        double r_amount = MAX(0, 1 - eta * eta * (1 - cos_i * cos_i));
        if (r_amount > 0) {
            XYZ dir = delta * eta + unit_norm * (eta * cos_i - sqrt(r_amount));
            Color refract_color = rt->cast_ray(outside ? hit - bias : hit + bias, hit + dir, depth - 1);
            out_color = out_color * (1 - transmittance) +
                refract_color * (1 - fresnel) * transmittance;
        }
    }
    double shadow_amount = rt->shadow_amount(hit);
    return out_color * (1 - shadow_amount) + color * rt->m_ambient * shadow_amount;
}

Color Wall::render(
    Raytracer *rt,
    const XYZ &hit,
    const XYZ &delta,
    unsigned depth
){
    XYZ unit_norm = normal - position;
    XYZ eps_norm = unit_norm * EPSILON;
    double dot_norm = dot(unit_norm, delta);
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
    Color out_color = diffuse_color + specular_color;
    double fresnel = fresnel_amount(delta, unit_norm, refractive_index);
    if (depth > 0 && reflectance > 0) {
        XYZ delta_reflect = delta - unit_norm * 2 * dot_norm;
        Color reflect_color = rt->cast_ray(hit + eps_norm, hit + delta_reflect, depth - 1, true);
        out_color = out_color
            + reflect_color * reflectance * fresnel;
    }
    double shadow_amount = rt->shadow_amount(hit);
    return out_color * (1 - shadow_amount) + color * rt->m_ambient * shadow_amount;
}

Color Triangle::render(
    Raytracer *rt,
    const XYZ &hit,
    const XYZ &delta,
    unsigned depth
){
    XYZ unit_norm = cross(edges[0], edges[1]).normal();
    XYZ eps_norm = unit_norm * EPSILON;
    double dot_norm = dot(unit_norm, delta);
    double light_mag = distance(rt->m_light, hit);
    XYZ unit_light = (rt->m_light - hit) / light_mag;
    double compute_factor = dot(unit_norm, unit_light);
    Color diffuse_color = color * 0.5 * rt->m_diffuse * compute_factor +
        color * rt->m_ambient;
    double sight_mag = distance(hit, rt->m_camera);
    XYZ unit_sight = (hit - rt->m_camera) / sight_mag;
    double combined_mag = distance({ 0, 0, 0 }, unit_light + unit_sight);
    XYZ unit_bisect = (unit_light + unit_sight) / combined_mag;
    double specular_amount = std::pow(std::max(0.0, dot(unit_norm, unit_bisect)),
            rt->m_specular_size);
    Color specular_color = color * (specular_amount * rt->m_specular);
    Color out_color = diffuse_color + specular_color;
    double fresnel = fresnel_amount(delta, unit_norm, refractive_index);
    if (depth > 0 && reflectance > 0) {
        XYZ delta_reflect = delta - unit_norm * 2 * dot_norm;
        Color reflect_color = rt->cast_ray(hit + eps_norm, hit + delta_reflect, depth - 1, true);
        out_color = out_color
            + reflect_color * reflectance * fresnel;
    }
    double shadow_amount = rt->shadow_amount(hit);
    return out_color * (1 - shadow_amount) + color * rt->m_ambient * shadow_amount;
}

void Raytracer::add_form(Sphere &&sphere)
{
    m_spheres.push_back(sphere);
}

void Raytracer::add_form(const Sphere &sphere)
{
    m_spheres.push_back(sphere);
}

void Raytracer::add_form(Wall &&wall)
{
    m_walls.push_back(wall);
}

void Raytracer::add_form(const Wall &wall)
{
    m_walls.push_back(wall);
}

void Raytracer::add_form(Triangle &&tri)
{
    m_triangles.push_back(tri);
}

void Raytracer::add_form(const Triangle &tri)
{
    m_triangles.push_back(tri);
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

void Raytracer::set_reflection_depth(unsigned depth)
{
    m_reflection_depth = depth;
}

void Raytracer::set_shadow_unit_size(double size)
{
    m_shadow_unit_size = size;
}

void Raytracer::set_shadow_grid_size(unsigned size)
{
    m_shadow_grid_size = size;
}

void Raytracer::set_pixel_sample_size(unsigned size)
{
    m_pixel_sample_size = size;
}

void Raytracer::set_light(const XYZ &pos)
{
    m_light = pos;
}

void Raytracer::set_camera(const XYZ &pos)
{
    m_camera = pos;
}
