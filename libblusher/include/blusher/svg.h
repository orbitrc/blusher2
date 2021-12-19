#ifndef _BLUSHER_SVG_H
#define _BLUSHER_SVG_H

#include <stdint.h>

typedef struct bl_svg_core bl_svg_core;
typedef struct bl_image bl_image;

typedef struct bl_svg {
    bl_svg_core *core;
} bl_svg;

bl_svg* bl_svg_from_path(const char *path);

bl_image* bl_svg_to_image(bl_svg *svg, uint64_t width, uint64_t height);

void bl_svg_free(bl_svg *svg);

#endif /* _BLUSHER_SVG_H */
