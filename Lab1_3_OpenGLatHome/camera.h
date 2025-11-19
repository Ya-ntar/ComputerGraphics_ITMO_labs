#pragma once

#include "geometry.h"

class Camera
{
public:
    Camera(const Vec3f& position,
           const Vec3f& target,
           const Vec3f& up_dir,
           float fov_deg,
           float aspect_ratio,
           float near_plane,
           float far_plane,
           int screen_width,
           int screen_height);

    Vec3f project(const Vec3f& vertex) const;

private:
    Vec3f position_;
    Vec3f target_;
    Vec3f up_;

    float fov_deg_;
    float aspect_;
    float near_plane_;
    float far_plane_;

    int screen_width_;
    int screen_height_;
};
