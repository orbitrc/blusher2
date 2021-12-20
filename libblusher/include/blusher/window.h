#ifndef _BLUSHER_WINDOW_H
#define _BLUSHER_WINDOW_H

#include <stdlib.h>

#include <wayland-client.h>

//#include <unstable/xdg-shell.h>
#include "wayland-protocols/stable/xdg-shell.h"

#define BLUSHER_WINDOW_SHADOW_WIDTH 40
#define BLUSHER_WINDOW_RESIZE_WIDTH 5
#define BLUSHER_WINDOW_BORDER_WIDTH 1

typedef struct bl_surface bl_surface;
typedef struct bl_title_bar bl_title_bar;

typedef struct bl_window {
    bl_surface *surface;

    struct xdg_wm_base *xdg_wm_base;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;

    struct xdg_wm_base_listener xdg_wm_base_listener;
    struct xdg_surface_listener xdg_surface_listener;
    struct xdg_toplevel_listener xdg_toplevel_listener;

    struct wl_region *input_region;

    int width;
    int height;
    const char *title;
    bl_title_bar *title_bar;
    bl_surface *decoration;
    bl_surface *resize;
    bl_surface *border;
    bl_surface *body;

    uint32_t resize_edge;
} bl_window;

bl_window* bl_window_new();

void bl_window_set_size(bl_window *window, int width, int height);

void bl_window_show(bl_window *window);

void bl_window_set_body(bl_window *window, bl_surface *surface);

bl_surface* bl_window_body(bl_window *window);

/// \brief Free the window. Should not call manually.
void bl_window_free(bl_window *window);

#endif /* _BLUSHER_WINDOW_H */
