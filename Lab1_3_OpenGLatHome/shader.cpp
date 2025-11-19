#include "shader.h"

#include <algorithm>

BasicShader::BasicShader(const Model& model,
                         const Camera& camera,
                         const Light& light,
                         const Texture* texture)
    : model_(model),
      camera_(camera),
      light_(light),
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

    const float dot_product = std::max(0.0f, normal.dot(light_.get_direction()));
    const float intensity = dot_product * light_.get_intensity();
    const Vec3f light_color = light_.get_color();
    const Vec3f base_color(intensity * light_color.x, intensity * light_color.y, intensity * light_color.z);

    Vec3f final_color = base_color;
    if (texture_ && texture_->is_valid())
    {
        const Vec2f uv = uv_coords_[0] * barycentric.x +
            uv_coords_[1] * barycentric.y +
            uv_coords_[2] * barycentric.z;
        const Vec3f tex_color = texture_->sample(uv);
        final_color.x *= tex_color.x;
        final_color.y *= tex_color.y;
        final_color.z *= tex_color.z;
    }

    const unsigned char r = static_cast<unsigned char>(std::clamp(final_color.x, 0.0f, 1.0f) * 255.0f);
    const unsigned char g = static_cast<unsigned char>(std::clamp(final_color.y, 0.0f, 1.0f) * 255.0f);
    const unsigned char b = static_cast<unsigned char>(std::clamp(final_color.z, 0.0f, 1.0f) * 255.0f);
    color = TGAColor(r, g, b, 255);
    return false;
}
