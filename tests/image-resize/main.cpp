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
    bl::Image image(5, 5);
    image.fill(bl::Color::from_rgba(255, 0, 0, 255));

    auto begin = get_now_ms();

    image.resize(7, 7);

    for (uint32_t i = 1024; i < 1920; ++i) {
        image.resize(i, i);
    }

    auto end = get_now_ms();

    fprintf(stderr, "Total: %ldms\n", end - begin);

    return 0;
}
