#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <array>
#include <vector>

#include "geometry.h"
#include "renderer.h"
#include "model.h"
#include "libs/stb_image_write.h"
#include "libs/tgaimage.h"


const auto TGA_FILE_NAME = "output.tga";
const auto OUTPUT_FILE_NAME = "output.png";
constexpr int width = 1800;
constexpr int height = 1800;

void convertTGAtoPNG(const char* inFile, const char* outFile)
{
    int w;
    int h;
    int ch;
    unsigned char* data = stbi_load(inFile, &w, &h, &ch, 0);
    if (!data)
    {
        printf("Failed to load %s\n", inFile);
        return;
    }

    stbi_write_png(outFile, w, h, ch, data, w * ch);
    stbi_image_free(data);
}


void draw_vertices(TGAImage& image)
{
    auto model = Model("carrot_soup/carrot_soup.obj");

    for (int i = 0; i < model.nfaces(); i++)
    {
        std::vector<int> face = model.face(i);
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model.vert(face[j]);
            Vec3f v1 = model.vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = static_cast<int>((v1.y + 1.) * height / 2.);
            line(x0, y0, x1, y1, image, WHITE);
        }
    }
}

void zbuf_debug(const std::vector<float>& zbuffer)
{
    // dump z-buffer (debugging purposes only)
    TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            zbimage.set(i, j, TGAColor(zbuffer[i + j * width], 1));
        }
    }
    zbimage.flip_vertically();
    zbimage.write_tga_file("zbuffer.tga");
}

void draw_vertices_colored(TGAImage& image)
{
    auto model = Model("carrot_soup/carrot_soup.obj");
    std::vector zbuffer(width * height, std::numeric_limits<float>::min());


    Vec3f light_dir(0, 0, -1);

    for (int face_index = 0; face_index < model.nfaces(); face_index++)
    {
        const std::vector<int>& current_face = model.face(face_index);

        std::array<Vec3i, 3> screen_points;
        std::array<Vec3f, 3> world_points;

        for (int vertex_index = 0; vertex_index < 3; vertex_index++)
        {
            Vec3f vertex = model.vert(current_face[vertex_index]);

            float screen_x = (vertex.x + 1.0f) * width / 2.0f;
            float screen_y = (vertex.y + 1.0f) * height / 2.0f;
            float screen_z = (vertex.z + 1.0f) * 512 / 2.0f;

            screen_points[vertex_index] = Vec3i(static_cast<int>(screen_x), static_cast<int>(screen_y),
                                                static_cast<int>(screen_z));
            world_points[vertex_index] = vertex;
        }

        Vec3f edge1 = world_points[2] - world_points[0];
        Vec3f edge2 = world_points[1] - world_points[0];
        Vec3f face_normal = edge1.cross(edge2);

        face_normal = face_normal.normalized();

        float light_intensity = face_normal.dot(light_dir);

        if (light_intensity > 0)
        {
            unsigned char color_value = light_intensity * 255;
            TGAColor triangle_color = TGAColor(color_value, color_value, color_value, 255);

            barycentric_triangle(
                screen_points[0],
                screen_points[1],
                screen_points[2],
                image,
                triangle_color, zbuffer
            );
        }
    }
    {
        zbuf_debug(zbuffer);
    }
}

int main(int argc, char** argv)

{
    TGAImage image(width, height, TGAImage::RGB);
    draw_vertices_colored(image);
    image.flip_vertically();
    image.write_tga_file(TGA_FILE_NAME);


    convertTGAtoPNG(TGA_FILE_NAME, OUTPUT_FILE_NAME);

    convertTGAtoPNG("zbuffer.tga", "zbuffer.png");
    return 0;
}
