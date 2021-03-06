#include "title-bar.h"

#include <stdlib.h>
#include <stdio.h>

#include "surface.h"
#include "window.h"
#include "application.h"
#include "pointer-event.h"
#include "color.h"
#include "svg.h"
#include <blusher/core/log.h>

//=================
// Events
//=================
static void close_button_pointer_click_handler(bl_surface *surface,
        bl_pointer_event *event)
{
    if (event->button == BTN_LEFT) {
        bl_log(BL_LOG_LEVEL_INFO, "%s() - Closing...", __func__);
        bl_application_remove_window(bl_app, bl_app->toplevel_windows[0]);
    }
}

//=================
// Title Bar
//=================
bl_title_bar* bl_title_bar_new(bl_window *window)
{
    bl_title_bar *title_bar = malloc(sizeof(bl_title_bar));

    title_bar->surface = bl_surface_new(window->surface);

    title_bar->surface->parent = window->surface;
    bl_surface_set_geometry(title_bar->surface,
        BLUSHER_WINDOW_SHADOW_WIDTH, BLUSHER_WINDOW_SHADOW_WIDTH,
        window->width, BLUSHER_TITLE_BAR_HEIGHT);
    bl_color color = bl_color_from_rgb(100, 100, 100);
    bl_surface_set_color(title_bar->surface, color);

    title_bar->window = window;

    // Set close button.
    bl_svg *hydrogen_close_svg = bl_svg_from_path(
        "brc:/io.orbitrc.blusher/hydrogen-close.svg");
    if (hydrogen_close_svg == NULL) {
        fprintf(stderr, "Close SVG is NULL!\n");
        exit(1);
    }
    bl_image *hydrogen_close_image = bl_svg_to_image(hydrogen_close_svg,
        20, 20);
    if (hydrogen_close_image == NULL) {
        fprintf(stderr, "Close Image is NULL!\n");
    }

    title_bar->close_button = bl_surface_new(title_bar->surface);
    bl_surface_set_geometry(title_bar->close_button, 5, 5,
        20, 20);
    color = bl_color_from_rgb(255, 0, 0);
    bl_surface_set_color(title_bar->close_button, color);
    bl_surface_render_image(title_bar->close_button, hydrogen_close_image,
        BL_IMAGE_SCALE_NO_SCALE);
    title_bar->close_button->pointer_click_event =
        close_button_pointer_click_handler;

    // Set maximize/restore button.
    title_bar->maximize_button = bl_surface_new(title_bar->surface);
    bl_surface_set_geometry(title_bar->maximize_button, 30, 5,
        20, 20);
    color = bl_color_from_rgb(0, 255, 0);
    bl_surface_set_color(title_bar->maximize_button, color);

    // Set minimize button.
    title_bar->minimize_button = bl_surface_new(title_bar->surface);
    bl_surface_set_geometry(title_bar->minimize_button, 55, 5,
        20, 20);
    color = bl_color_from_rgb(255, 255, 0);
    bl_surface_set_color(title_bar->minimize_button, color);

    return title_bar;
}

void bl_title_bar_show(bl_title_bar *title_bar)
{
    bl_surface_paint(title_bar->surface);
    bl_surface_show(title_bar->surface);

//    bl_surface_paint(title_bar->close_button);
    bl_surface_show(title_bar->close_button);

    bl_surface_paint(title_bar->maximize_button);
    bl_surface_show(title_bar->maximize_button);

    bl_surface_paint(title_bar->minimize_button);
    bl_surface_show(title_bar->minimize_button);
}

void bl_title_bar_free(bl_title_bar *title_bar)
{
    bl_surface_free(title_bar->close_button);
    bl_surface_free(title_bar->surface);

    free(title_bar);
}
