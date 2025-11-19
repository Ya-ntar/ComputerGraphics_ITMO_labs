#include "shader.h"

#include <algorithm>

BasicShader::BasicShader(const Model& model,
                         const Camera& camera,
                         const Vec3f& light_direction,
                         const Texture* texture)
    : model_(model),
      camera_(camera),
      light_direction_(light_direction.normalized()),
      texture_(texture)
{
}

Vec3f BasicShader::vertex(int face_index, int vertex_index)
{
    const std::vector<int>& face = model_.face(face_index);
    const Vec3f world = model_.vert(face[vertex_index]);

    world_coords_[vertex_index] = world;
    const Vec3f projected = camera_.project(world);
    screen_coords_[vertex_index] = projected;
    uv_coords_[vertex_index] = model_.texcoord(face_index, vertex_index);
    return projected;
}

bool BasicShader::fragment(const Vec3f& barycentric, TGAColor& color)
{
    const Vec3f v0 = world_coords_[1] - world_coords_[0];
    const Vec3f v1 = world_coords_[2] - world_coords_[0];
    Vec3f normal = v1.cross(v0).normalized();

    const float intensity = std::max(0.0f, normal.dot(light_direction_));
    Vec3f base_color(intensity, intensity, intensity);

    if (texture_ && texture_->is_valid())
    {
        Vec2f uv = uv_coords_[0] * barycentric.x +
                   uv_coords_[1] * barycentric.y +
                   uv_coords_[2] * barycentric.z;
        const Vec3f tex_color = texture_->sample(uv);
        base_color.x *= tex_color.x;
        base_color.y *= tex_color.y;
        base_color.z *= tex_color.z;
    }

    auto clamp01 = [](float value)
    {
        if (value < 0.0f) return 0.0f;
        if (value > 1.0f) return 1.0f;
        return value;
    };

    const unsigned char r = static_cast<unsigned char>(clamp01(base_color.x) * 255.0f);
    const unsigned char g = static_cast<unsigned char>(clamp01(base_color.y) * 255.0f);
    const unsigned char b = static_cast<unsigned char>(clamp01(base_color.z) * 255.0f);
    color = TGAColor(r, g, b, 255);
    return false;
}

