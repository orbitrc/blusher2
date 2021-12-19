#include "svg-core.h"

#include <stdlib.h>
#include <string.h>

#include <librsvg/rsvg.h>

typedef struct bl_cairo_png_write_closure {
    unsigned char *data;
    unsigned int written;
} bl_cairo_png_write_closure;

static cairo_status_t cairo_png_write_func(void* closure,
        const unsigned char *data, unsigned int length)
{
    bl_cairo_png_write_closure *cl = (bl_cairo_png_write_closure*)closure;

    if (cl->data == NULL) {
        cl->data = malloc(sizeof(unsigned char) * length);
    } else {
        unsigned char *tmp = cl->data;
        cl->data = malloc(
            sizeof(unsigned char) * cl->written
                + sizeof(unsigned char) * length
        );
        memcpy(cl->data, tmp, cl->written);
        free(tmp);
    }

    for (unsigned int i = 0; i < length; ++i) {
        cl->data[cl->written + i] = data[i];
    }
    cl->written += length;

    return CAIRO_STATUS_SUCCESS;
}

bl_svg_core* bl_svg_core_from_data(const unsigned char *data, uint64_t size)
{
    bl_svg_core *svg_core = malloc(sizeof(bl_svg_core));

    GError *error = NULL;
    RsvgHandle *handle = rsvg_handle_new_from_data(data, size, &error);

    if (error != NULL) {
        fprintf(stderr, "%s\n", error->message);
        free(svg_core);
        return NULL;
    }

    rsvg_handle_get_intrinsic_size_in_pixels(handle,
        &svg_core->width, &svg_core->height);

    svg_core->data_size = size;
    svg_core->data = data;

    g_object_unref(handle);

    return svg_core;
}

unsigned char* bl_svg_core_png_data(bl_svg_core *svg_core,
        uint64_t width, uint64_t height, uint32_t *size)
{
    GError *error = NULL;
    RsvgHandle *handle = rsvg_handle_new_from_data(svg_core->data,
        svg_core->data_size, &error);

    if (error != NULL) {
        fprintf(stderr, "%s\n", error->message);
        return NULL;
    }

    RsvgRectangle viewport = {
        .x = 0.0,
        .y = 0.0,
        .width = width,
        .height = height,
    };

    bl_cairo_png_write_closure closure = {
        .data = NULL,
        .written = 0,
    };

    cairo_surface_t *cairo_surface = cairo_image_surface_create(
        CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(cairo_surface);

    rsvg_handle_render_document(handle, cr, &viewport, &error);
    if (error != NULL) {
        fprintf(stderr, "%s\n", error->message);
        return NULL;
    }

    cairo_surface_write_to_png_stream(cairo_surface, cairo_png_write_func,
        &closure);
    *size = closure.written;

    // Free Rsvg and Cairo resources.
    cairo_surface_destroy(cairo_surface);
    cairo_destroy(cr);
    g_object_unref(handle);

    return closure.data;
}

void bl_svg_core_free(bl_svg_core *svg_core)
{
    free(svg_core);
}
