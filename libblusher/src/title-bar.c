#include "title-bar.h"

#include <stdlib.h>
#include <stdio.h>

#include "surface.h"
#include "window.h"
#include "application.h"
#include "pointer-event.h"
#include "color.h"

//=================
// Events
//=================
static void title_bar_pointer_press_handler(bl_surface *surface,
        bl_pointer_event *event)
{
    if (event->button == BTN_LEFT) {
//        bl_application_remove_window()
        fprintf(stderr, "Close!\n");
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
    title_bar->close_button = bl_surface_new(title_bar->surface);
    bl_surface_set_geometry(title_bar->close_button, 5, 5,
        20, 20);
    color = bl_color_from_rgb(255, 0, 0);
    bl_surface_set_color(title_bar->close_button, color);
    title_bar->close_button->pointer_press_event =
        title_bar_pointer_press_handler;

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

    bl_surface_paint(title_bar->close_button);
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
