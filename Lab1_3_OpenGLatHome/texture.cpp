#include "texture.h"

#include "libs/stb_image.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace
{
float wrap_coord(float value)
{
    float wrapped = std::fmod(value, 1.0f);
    if (wrapped < 0.0f)
    {
        wrapped += 1.0f;
    }
    return wrapped;
}
} // namespace

Texture::Texture(const std::string& path)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, 3);
    if (!pixels)
    {
        throw std::runtime_error("Failed to load texture: " + path);
    }

    width_ = width;
    height_ = height;
    channels_ = 3;
    data_.assign(pixels, pixels + width_ * height_ * channels_);
    stbi_image_free(pixels);
}

Vec3f Texture::sample(const Vec2f& uv) const
{
    if (data_.empty() || width_ == 0 || height_ == 0)
    {
        return Vec3f(1.0f, 1.0f, 1.0f);
    }

    const float u = wrap_coord(uv.x);
    const float v = wrap_coord(uv.y);

    const int x = std::min(width_ - 1, static_cast<int>(u * width_));
    const int y = std::min(height_ - 1, static_cast<int>((1.0f - v) * height_));

    const int index = (x + y * width_) * channels_;
    const float r = data_[index] / 255.0f;
    const float g = data_[index + 1] / 255.0f;
    const float b = data_[index + 2] / 255.0f;
    return Vec3f(r, g, b);
}

