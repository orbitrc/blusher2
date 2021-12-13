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

    bl_surface *rect = bl_surface_new(window->surface);
    // wl_surface_set_buffer_scale(rect->surface, 2);
    bl_surface_set_geometry(rect, 10, 10, 100, 100);
    rect->pointer_press_event = rect_pointer_press_handler;
    bl_surface_show(rect);

//    bl_label *label = bl_label_new(window->surface, "안녕!");
//    bl_label_show(label);

    return bl_application_exec(app);
}
