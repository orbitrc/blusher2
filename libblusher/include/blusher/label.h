#ifndef _BLUSHER_LABEL_H
#define _BLUSHER_LABEL_H

#include "color.h"
#include "surface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bl_label {
    bl_surface *surface;

    char *text;
    double font_size;
    bl_color font_color;
} bl_label;

bl_label* bl_label_new(bl_surface *parent, const char *text);

const char* bl_label_text(bl_label *label);

void bl_label_set_text(bl_label *label, const char *text);

void bl_label_show(bl_label *label);

void bl_label_free(bl_label* label);

#ifdef __cplusplus
}
#endif

#endif /* _BLUSHER_LABEL_H */
