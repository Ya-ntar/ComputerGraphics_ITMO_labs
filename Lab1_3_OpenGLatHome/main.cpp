#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <vector>

#include "geometry.h"
#include "model.h"
#include "libs/stb_image_write.h"

#include "tgaimage.h"


const TGAColor WHITE = TGAColor(255, 255, 255, 255);
const TGAColor RED = TGAColor(255, 0, 0, 255);
const auto TGA_FILE_NAME = "output.tga";
const auto OUTPUT_FILE_NAME = "output.png";
constexpr int width = 800;
constexpr int height = 800;

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

//Bresenham-like algorithm.
void line(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color)
{
    bool steep = false;

    // if steep, simplify
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    // left to right
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    const int dx = x1 - x0;
    const int dy = std::abs(y1 - y0);
    int error = dx / 2;

    const int yStep = (y0 < y1) ? 1 : -1;
    int y = y0;

    // rasterize
    for (int x = x0; x <= x1; ++x) {
        if (steep)
            image.set(y, x, color);
        else
            image.set(x, y, color);

        error -= dy;
        if (error < 0) {
            y += yStep;
            error += dx;
        }
    }
}


int main(int argc, char** argv)

{
    TGAImage image(width, height, TGAImage::RGB);
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
            int y1 = (v1.y + 1.) * height / 2.;
            line(x0, y0, x1, y1, image, WHITE);
        }
    }

    image.set(52, 41, RED);
    image.flip_vertically();

    image.write_tga_file(TGA_FILE_NAME);

    convertTGAtoPNG(TGA_FILE_NAME, OUTPUT_FILE_NAME);
    return 0;
}
