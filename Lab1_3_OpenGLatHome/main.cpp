#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "tgaimage.h"


const TGAColor WHITE = TGAColor(255, 255, 255, 255);
const TGAColor RED   = TGAColor(255, 0,   0,   255);
const auto TGA_FILE_NAME = "output.tga";
const auto OUTPUT_FILE_NAME = "output.png";

void convertTGAtoPNG(const char* inFile, const char* outFile) {
    int w, h, ch;
    unsigned char* data = stbi_load(inFile, &w, &h, &ch, 0);
    if (!data) {
        printf("Failed to load %s\n", inFile);
        return;
    }

    stbi_write_png(outFile, w, h, ch, data, w * ch);
    stbi_image_free(data);
}


int main(int argc, char** argv) {
    TGAImage image(100, 100, TGAImage::RGB);
    image.set(52, 41, RED);
    image.flip_vertically();

    image.write_tga_file(TGA_FILE_NAME);

    convertTGAtoPNG(TGA_FILE_NAME, OUTPUT_FILE_NAME);
    return 0;
}

