#pragma once

#include <cmath>
#include "geometry.h"
#include "libs/tgaimage.h"

const TGAColor WHITE = TGAColor(255, 255, 255, 255);
const TGAColor RED   = TGAColor(255, 0, 0, 255);
const TGAColor GREEN = TGAColor(0, 255, 0, 255);

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

    int dx = x1 - x0;
    int dy = std::abs(y1 - y0);
    int error = dx / 2;
    int yStep = (y0 < y1) ? 1 : -1;
    int y = y0;

    for (int x = x0; x <= x1; ++x)
    {
        if (steep)
            image.set(y, x, color);
        else
            image.set(x, y, color);

        error -= dy;
        if (error < 0)
        {
            y += yStep;
            error += dx;
        }
    }
}

inline void line(const Vec2i& p0, const Vec2i& p1, TGAImage& image, const TGAColor& color)
{
    line(p0.x, p0.y, p1.x, p1.y, image, color);
}

inline void triangle(const Vec2i& t0, const Vec2i& t1, const Vec2i& t2, TGAImage& image, const TGAColor& color)
{
    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
}


