#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"

#include <array>
#include <cstdio>
#include <limits>
#include <vector>

#include "geometry.h"
#include "model.h"
#include "renderer.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "libs/tgaimage.h"

namespace
{
constexpr int kWidth = 1800;
constexpr int kHeight = 1800;
constexpr char kModelPath[] = "ImageToStl.com_gabriel_plush_ultrakill/gabriel_plush_ultrakill.obj";
constexpr char kDiffuseTexturePath[] = "ImageToStl.com_gabriel_plush_ultrakill/3DModel_LowPoly_baseColor.png";

constexpr char kColorBufferTga[] = "output.tga";
constexpr char kColorBufferPng[] = "output.png";
constexpr char kDepthBufferTga[] = "zbuffer.tga";
constexpr char kDepthBufferPng[] = "zbuffer.png";

const Vec3f kLightDirection(0.0f, 0.0f, -1.0f);

struct Framebuffer
{
    Framebuffer()
        : color(kWidth, kHeight, TGAImage::RGB),
          depth(kWidth * kHeight, std::numeric_limits<float>::lowest())
    {
    }

    TGAImage color;
    std::vector<float> depth;
};

void render_model(const Model& model, Framebuffer& framebuffer, IShader& shader)
{
    for (int face_index = 0; face_index < model.nfaces(); ++face_index)
    {
        std::array<Vec3f, 3> screen_vertices{};
        for (int vertex_index = 0; vertex_index < 3; ++vertex_index)
        {
            screen_vertices[vertex_index] = shader.vertex(face_index, vertex_index);
        }

        renderer::barycentric_triangle(
            screen_vertices,
            framebuffer.color,
            framebuffer.depth,
            shader);
    }
}

void dump_depth_buffer(const std::vector<float>& depth, const char* file_path)
{
    TGAImage depth_image(kWidth, kHeight, TGAImage::GRAYSCALE);
    for (int x = 0; x < kWidth; ++x)
    {
        for (int y = 0; y < kHeight; ++y)
        {
            const float depth_value = depth[x + y * kWidth];
            const float clamped = depth_value < 0.0f ? 0.0f : (depth_value > 1.0f ? 1.0f : depth_value);
            const unsigned char value = static_cast<unsigned char>(clamped * 255.0f);
            depth_image.set(x, y, TGAColor(value, 1));
        }
    }

    depth_image.flip_vertically();
    depth_image.write_tga_file(file_path);
}

bool convert_tga_to_png(const char* input_path, const char* output_path)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* data = stbi_load(input_path, &width, &height, &channels, 0);
    if (!data)
    {
        printf("Failed to load %s\n", input_path);
        return false;
    }

    const int stride_in_bytes = width * channels;
    const int result = stbi_write_png(output_path, width, height, channels, data, stride_in_bytes);
    stbi_image_free(data);
    return result != 0;
}
} // namespace

int main()
{
    Framebuffer framebuffer{};
    const Model model(kModelPath);
    const Texture diffuse_texture(kDiffuseTexturePath);
    const Camera camera(
        Vec3f(0.0f, 0.0f, 1.f),
        Vec3f(0.0f, 0.0f, 0.0f),
        Vec3f(0.0f, 1.0f, 0.0f),
        60.0f,
        static_cast<float>(kWidth) / static_cast<float>(kHeight),
        0.1f,
        100.0f,
        kWidth,
        kHeight);

    BasicShader shader(model, camera, kLightDirection, &diffuse_texture);
    render_model(model, framebuffer, shader);

    framebuffer.color.flip_vertically();
    framebuffer.color.write_tga_file(kColorBufferTga);
    dump_depth_buffer(framebuffer.depth, kDepthBufferTga);

    convert_tga_to_png(kColorBufferTga, kColorBufferPng);
    convert_tga_to_png(kDepthBufferTga, kDepthBufferPng);
    return 0;
}
