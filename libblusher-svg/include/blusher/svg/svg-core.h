#ifndef _BLUSHER_SVG_SVG_CORE_H
#define _BLUSHER_SVG_SVG_CORE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bl_svg_core {
    double width;
    double height;
    unsigned char *data;
    uint64_t data_size;
} bl_svg_core;

bl_svg_core* bl_svg_core_from_data(const unsigned char *data, uint64_t size);

unsigned char* bl_svg_core_png_data(bl_svg_core *svg_core,
        uint64_t width, uint64_t height, uint32_t *size);

void bl_svg_core_free(bl_svg_core *svg_core);

#ifdef __cplusplus
}
#endif

#endif /* _BLUSHER_SVG_SVG_CORE_H */
