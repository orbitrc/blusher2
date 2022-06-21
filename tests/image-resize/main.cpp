#include <stdint.h>
#include <stdio.h>

#include <blusher/image.h>
#include <blusher/color.h>

void print_image(const uint8_t *data, uint64_t width, uint64_t height)
{
    uint32_t *pixel = (uint32_t*)data;

    for (uint64_t y = 0; y < height; ++y) {
        for (uint64_t x = 0; x < width; ++x) {
            printf("0x%08x ", *(pixel + (y * x)));
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    bl::Image image(5, 5);
    image.fill(bl::Color::from_rgba(255, 0, 0, 255));

    print_image(image.data(), image.width(), image.height());

    image.resize(8, 3);

    printf("\n\n");

    print_image(image.data(), image.width(), image.height());

    return 0;
}
