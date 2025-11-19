#pragma once

#include <string>
#include <vector>

#include "geometry.h"

class Texture
{
public:
    explicit Texture(const std::string& path);
    ~Texture() = default;

    [[nodiscard]] bool is_valid() const { return !data_.empty(); }
    [[nodiscard]] Vec3f sample(const Vec2f& uv) const;

private:
    int width_{0};
    int height_{0};
    int channels_{0};
    std::vector<unsigned char> data_;
};

