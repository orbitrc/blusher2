#ifndef _BLUSHER_SURFACE_H
#define _BLUSHER_SURFACE_H

#include <wayland-client.h>

#include "color.h"
#include "image.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bl_pointer_event bl_pointer_event;

typedef enum bl_surface_state {
    BL_SURFACE_STATE_NORMAL = 0,
    BL_SURFACE_STATE_HOVER = 1,
    BL_SURFACE_STATE_ACTIVE = 2,
    BL_SURFACE_STATE_ACTIVE_OUT = 3,
    BL_SURFACE_STATE_FOCUS = 4,
} bl_surface_state;

typedef struct bl_surface {
    struct bl_surface *parent;

    struct wl_surface *surface;
    struct wl_subsurface *subsurface;
    struct wl_callback *frame_callback;
    struct wl_buffer *buffer;

    void *shm_data;
    int shm_data_size;

    double x;
    double y;
    double width;
    double height;
    double implicit_width;
    double implicit_height;
    bl_color color;
    bl_surface_state state;

    void (*pointer_click_event)(struct bl_surface*, bl_pointer_event*);
    void (*pointer_move_event)(struct bl_surface*, bl_pointer_event*);
    void (*pointer_press_event)(struct bl_surface*, bl_pointer_event*);
    void (*pointer_release_event)(struct bl_surface*, bl_pointer_event*);
} bl_surface;

bl_surface* bl_surface_new(bl_surface *parent);

void bl_surface_set_parent(bl_surface *surface, bl_surface *parent);

void bl_surface_set_geometry(bl_surface *surface,
        double x, double y, double width, double height);

void bl_surface_set_color(bl_surface *surface, const bl_color color);

void bl_surface_paint(bl_surface *surface);

void bl_surface_render_pixels(bl_surface *surface, const uint32_t *pixels,
        uint64_t width, uint64_t height);

void bl_surface_render_image(bl_surface *surface, const bl_image *image,
        bl_image_scale scale);

void bl_surface_show(bl_surface *surface);

void bl_surface_set_pointer_press_event(bl_surface *surface,
        void (*event)(bl_surface*, bl_pointer_event*));

void bl_surface_free(bl_surface *surface);

#ifdef __cplusplus
}
#endif

#endif /* _BLUSHER_SURFACE_H */
