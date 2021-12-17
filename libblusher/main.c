#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-cursor.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <unistd.h>

#include <blusher/blusher.h>

//==================
// Raw pixel data
//==================
static uint32_t pixels[] = {
    0xff0000ff, 0xff0000ff, 0xff0000ff,
};

//==================
// Surface events
//==================
static void rect_pointer_press_handler(bl_surface *surface,
        bl_pointer_event *event)
{
    fprintf(stderr, "Rect pressed!\n");
    bl_surface_set_geometry(
        surface,
        surface->x + 1,
        surface->y + 1,
        surface->width,
        surface->height
    );
    bl_surface_show(surface);
}

//===========
// Main
//===========
int main(int argc, char *argv[])
{
    bl_application *app = bl_application_new();

    bl_window *window = bl_window_new();
    bl_application_add_window(app, window);

    bl_window_show(window);

    bl_surface *rect = bl_surface_new(bl_window_body(window));
    // wl_surface_set_buffer_scale(rect->surface, 2);
    bl_surface_set_geometry(rect, 10, 10, 100, 100);
    bl_color rect_color = bl_color_from_rgb(0, 255, 0);
    bl_surface_set_color(rect, rect_color);
    rect->pointer_press_event = rect_pointer_press_handler;
    bl_surface_paint(rect);
    bl_surface_show(rect);
    // wl_surface_commit(window->body->parent->surface);

    bl_surface *subrect = bl_surface_new(rect);
    bl_surface_set_geometry(subrect, 5, 5, 30, 30);
    rect_color = bl_color_from_rgb(255, 0, 0);
    bl_surface_set_color(subrect, rect_color);
    bl_surface_paint(subrect);
    bl_surface_render_pixels(subrect, pixels, 3, 1);
    bl_surface_show(subrect);
    // wl_surface_commit(window->body->parent->surface);

//    bl_label *label = bl_label_new(window->surface, "안녕!");
//    bl_label_show(label);

    return bl_application_exec(app);
}
