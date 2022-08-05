#include <stdint.h>
#include <stdio.h>

// Unix
#include <sys/time.h>

// Blusher
#include <blusher/image.h>
#include <blusher/color.h>

uint64_t get_now_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int main(int argc, char *argv[])
{
    bl::Image image_base(128, 128);
    image_base.fill(bl::Color::from_rgba(0, 0, 0, 0));

    bl::Image image(50, 50);
    image.fill(bl::Color::from_rgb(0, 255, 0));

    auto begin = get_now_ms();

    uint32_t pos = 0;
    for (uint32_t i = 0; i < 10000; ++i) {
        image_base.fill(bl::Color::from_rgba(0, 0, 0, 0));
        image_base.add(image, pos, pos);
        ++pos;
        if (pos > 128) {
            pos = 0;
        }
    }

    auto end = get_now_ms();

    fprintf(stderr, "Total: %ldms\n", end - begin);

    return 0;
}
