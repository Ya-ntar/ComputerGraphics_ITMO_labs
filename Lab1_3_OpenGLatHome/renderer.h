#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include "geometry.h"
#include "libs/tgaimage.h"

namespace renderer
{

inline void line(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color)
{
    bool steep = false;

    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    const int dx = x1 - x0;
    const int dy = std::abs(y1 - y0);
    int error = dx / 2;
    const int y_step = (y0 < y1) ? 1 : -1;
    int y = y0;

    for (int x = x0; x <= x1; ++x)
    {
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }

        error -= dy;
        if (error < 0)
        {
            y += y_step;
            error += dx;
        }
    }
}

inline void line(const Vec2i& p0, const Vec2i& p1, TGAImage& image, const TGAColor& color)
{
    line(p0.x, p0.y, p1.x, p1.y, image, color);
}

inline void triangle_wireframe(const Vec2i& t0, const Vec2i& t1, const Vec2i& t2,
                               TGAImage& image, const TGAColor& color)
{
    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
}

inline void draw_scanline(TGAImage& image, int x0, int x1, int y, const TGAColor& color)
{
    if (x0 > x1)
    {
        std::swap(x0, x1);
    }

    for (int x = x0; x <= x1; ++x)
    {
        image.set(x, y, color);
    }
}

inline void colored_triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, const TGAColor& color)
{
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);

    const float dx02 = static_cast<float>(t2.x - t0.x) / static_cast<float>(t2.y - t0.y);
    const float dx01 = static_cast<float>(t1.x - t0.x) / static_cast<float>(t1.y - t0.y);
    const float dx12 = static_cast<float>(t2.x - t1.x) / static_cast<float>(t2.y - t1.y);

    float x02 = static_cast<float>(t0.x);
    float x01 = static_cast<float>(t0.x);

    for (int y = t0.y; y < t1.y; ++y)
    {
        draw_scanline(image, static_cast<int>(x02), static_cast<int>(x01), y, color);
        x02 += dx02;
        x01 += dx01;
    }

    float x12 = static_cast<float>(t1.x);
    for (int y = t1.y; y <= t2.y; ++y)
    {
        draw_scanline(image, static_cast<int>(x02), static_cast<int>(x12), y, color);
        x02 += dx02;
        x12 += dx12;
    }
}

inline void barycentric_triangle(const Vec3f t0, const Vec3f t1, const Vec3f t2,
                                 TGAImage& image, const TGAColor& color,
                                 std::vector<float>& zbuffer)
{
    const int width = image.get_width();
    const int height = image.get_height();

    int min_x = static_cast<int>(std::floor(std::min({t0.x, t1.x, t2.x})));
    int max_x = static_cast<int>(std::ceil(std::max({t0.x, t1.x, t2.x})));
    int min_y = static_cast<int>(std::floor(std::min({t0.y, t1.y, t2.y})));
    int max_y = static_cast<int>(std::ceil(std::max({t0.y, t1.y, t2.y})));

    min_x = std::max(0, min_x);
    max_x = std::min(width - 1, max_x);
    min_y = std::max(0, min_y);
    max_y = std::min(height - 1, max_y);

    const Vec2f v0(t1.x - t0.x, t1.y - t0.y);
    const Vec2f v1(t2.x - t0.x, t2.y - t0.y);

    const float dot00 = v0.x * v0.x + v0.y * v0.y;
    const float dot01 = v0.x * v1.x + v0.y * v1.y;
    const float dot11 = v1.x * v1.x + v1.y * v1.y;
    const float denom = dot00 * dot11 - dot01 * dot01;

    if (std::abs(denom) < 1e-6f)
    {
        return;
    }

    const float inv_denom = 1.0f / denom;

    for (int y = min_y; y <= max_y; ++y)
    {
        for (int x = min_x; x <= max_x; ++x)
        {
            const Vec2f v2(static_cast<float>(x) - t0.x, static_cast<float>(y) - t0.y);
            const float dot02 = v0.x * v2.x + v0.y * v2.y;
            const float dot12 = v1.x * v2.x + v1.y * v2.y;

            const float u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
            const float v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

            if (u < 0.0f || v < 0.0f || (u + v) > 1.0f)
            {
                continue;
            }

            const float w = 1.0f - u - v;
            const float z = t0.z * w + t1.z * u + t2.z * v;

            const int index = x + y * width;
            if (z > zbuffer[index])
            {
                zbuffer[index] = z;
                image.set(x, y, color);
            }
        }
    }
}
} // namespace renderer

/*void tutorial_triangle(Vec3i t0, Vec3i t1, Vec3i t2, TGAImage& image, TGAColor color, std::vector<float>& zbuffer)
{
    if (t0.y == t1.y && t0.y == t2.y) return; // i dont care about degenerate triangles
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    int total_height = t2.y - t0.y;
    for (int i = 0; i < total_height; i++)
    {
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        float alpha = (float)i / total_height;
        float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;
        // be careful: with above conditions no division by zero here
        Vec3i A = t0 + Vec3<int>(Vec3f(t2 - t0) * alpha);
        Vec3i B = second_half ? t1 + Vec3<int>(Vec3f(t2 - t1) * beta) : t0 + Vec3<int>(Vec3f(t1 - t0) * beta);
        if (A.x > B.x) std::swap(A, B);
        for (int j = A.x; j <= B.x; j++)
        {
            float phi = B.x == A.x ? 1. : (float)(j - A.x) / (float)(B.x - A.x);
            Vec3i P = Vec3<int>(Vec3<float>(A) + Vec3<float>((B - A) * phi));
            size_t idx = P.x + P.y * image.get_width();
            idx = std::min(zbuffer.size() - 1, idx);
            if (zbuffer[idx] < P.z)
            {
                zbuffer[idx] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}*/
