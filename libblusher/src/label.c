#include "label.h"

// Std libs
#include <stdlib.h>
#include <string.h>

// Cairo / Pango
#include <pango/pangocairo.h>

// Blusher
#include "utils.h"
#include <blusher/core/log.h>

//=================
// Cairo / Pango
//=================
static void get_surface_size_for_text(const char *text, double font_size,
        int *offset_y, int *width, int *height)
{
    PangoLayout *layout;
    PangoFontDescription *desc;
    cairo_surface_t *cairo_surface;
    cairo_t *cr;

    cairo_surface = cairo_image_surface_create(
        CAIRO_FORMAT_ARGB32,
        0,
        0
    );
    cr = cairo_create(cairo_surface);

    layout = pango_cairo_create_layout(cr);

    pango_layout_set_text(layout, text, -1);

    desc = pango_font_description_from_string("serif");
    pango_font_description_set_size(desc,
        pixel_to_pango_size(font_size));

    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);

    cairo_save(cr);

    cairo_move_to(cr, 0, 0);

    pango_cairo_update_layout(cr, layout);

    PangoRectangle ink_rect;
    pango_layout_get_extents(layout, &ink_rect, NULL);
    *height = ink_rect.height;
    *offset_y = ink_rect.y / PANGO_SCALE;
    pango_layout_get_size(layout, width, NULL);
    *height = *height / PANGO_SCALE;
    *width = *width / PANGO_SCALE;

    cairo_surface_destroy(cairo_surface);
    cairo_destroy(cr);
    g_object_unref(layout);
}

static void draw_text(bl_label *label,
        cairo_surface_t *cairo_surface, cairo_t *cr)
{
    PangoLayout *layout;
    PangoFontDescription *desc;

    layout = pango_cairo_create_layout(cr);

    pango_layout_set_text(layout, label->text, -1);

    desc = pango_font_description_from_string("serif");
    pango_font_description_set_size(desc,
        pixel_to_pango_size(label->font_size));

    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);

    cairo_save(cr);

    cairo_move_to(cr, 0, 0);

    cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);

    pango_cairo_update_layout(cr, layout);

    // Get width and height.
    int pango_width = 0;
    int pango_height = 0;
    pango_layout_get_size(layout, &pango_width, &pango_height);
    bl_log(BL_LOG_LEVEL_INFO, "%s() - pango_width: %d, pango_height: %d\n",
        __func__, pango_width, pango_height);
    cairo_move_to(cr, 0, 0);
    pango_cairo_show_layout(cr, layout);

    cairo_restore(cr);

    g_object_unref(layout);

    // Copy cairo data to the shm_data.
    int width = label->surface->width;
    int height = label->surface->height;
    int stride = width * 4;
    int size= stride * height;

    unsigned char *data = cairo_image_surface_get_data(cairo_surface);
    for (int i = 0; i < size; ++i) {
        ((unsigned char*)(label->surface->shm_data))[i] = data[i];
    }
}

//==============
// Label
//==============
bl_label* bl_label_new(bl_surface *parent, const char *text)
{
    bl_label *label = malloc(sizeof(bl_label));

    label->surface = bl_surface_new(parent);

    label->text = malloc(sizeof(char) * strlen(text) + 1);
    strncpy(label->text, text, strlen(text));
    label->text[strlen(text)] = '\0';
    label->font_size = 13;
    label->font_color = bl_color_from_rgb(0, 0, 0);

    return label;
}

const char* bl_label_text(bl_label *label)
{
    return label->text;
}

void bl_label_set_text(bl_label *label, const char *text)
{
    if (label->text != NULL) {
        free(label->text);
    }
    label->text = malloc(sizeof(char) * strlen(text) + 1);
    strncpy(label->text, text, strlen(text));
    label->text[strlen(text)] = '\0';
}

void bl_label_show(bl_label *label)
{
    bl_surface_set_geometry(label->surface, 0, 0, 100, 50);

    cairo_surface_t *cairo_surface = cairo_image_surface_create(
        CAIRO_FORMAT_ARGB32, label->surface->width, label->surface->height);
    cairo_t *cr = cairo_create(cairo_surface);

    draw_text(label, cairo_surface, cr);

//    bl_surface_show(label->surface);
    wl_surface_attach(label->surface->surface, label->surface->buffer, 0, 0);
    wl_surface_commit(label->surface->surface);

    wl_surface_commit(label->surface->parent->surface);
}

void bl_label_free(bl_label *label)
{
    bl_surface_free(label->surface);

    free(label->text);

    free(label);
}
