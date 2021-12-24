#include "surface.h"

// Std libs
#include <stdlib.h>
#include <stdio.h>

// Unix
#include <sys/mman.h>
#include <unistd.h>

// Blusher
#include "application.h"
#include "utils.h"
#include <blusher/core/log.h>
#include <blusher/core/collections.h>

//=============
// Drawing
//=============
static struct wl_buffer* create_buffer(bl_surface *surface,
        int width, int height, struct wl_shm *shm)
{
    fprintf(stderr, "create_buffer: %dx%d\n", width, height);
    struct wl_shm_pool *pool;
    int stride = width * 4;
    int size = stride * height;
    int fd;
    struct wl_buffer *buff;

    fd = os_create_anonymous_file(size);
    if (fd < 0) {
        fprintf(stderr, "Creating a buffer file for %d B failed: %m\n",
            size);
        exit(1);
    }

    fprintf(stderr, "Before mmap.");
    fprintf(stderr, " shm_data: %p\n", surface->shm_data);
    surface->shm_data =
        mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    fprintf(stderr, "After mmap. shm_data: %p\n", surface->shm_data);
    if (surface->shm_data == MAP_FAILED) {
        fprintf(stderr, "MMAP_FAILED\n");
        surface->shm_data = NULL;
        close(fd);
        return NULL;
    }

    surface->shm_data_size = size;
    pool = wl_shm_create_pool(shm, fd, size);
    buff = wl_shm_pool_create_buffer(pool, 0, width, height, stride,
        WL_SHM_FORMAT_ARGB8888);

    close(fd);
    wl_shm_pool_destroy(pool);
    return buff;
}

inline static void paint_pixel(bl_surface *surface, uint32_t pixel,
        uint64_t x, uint64_t y)
{
    if (x * y > surface->height * surface->width) {
        fprintf(stderr, "paint_pixel() - position larger than surface.\n");
        return;
    }

    uint32_t *shm_data = surface->shm_data;
    uint64_t target = (surface->width * y) + x;
    shm_data[target] = pixel;
}

static void paint_pixels(bl_surface *surface)
{
    uint32_t *pixel = surface->shm_data;

    const uint32_t color = bl_color_to_argb(surface->color);
    for (int n = 0; n < (surface->width * surface->height); ++n) {
        *pixel++ = color;
    }
}

//============
// Surface
//============
bl_surface* bl_surface_new(bl_surface *parent)
{
    bl_surface *surface = malloc(sizeof(bl_surface));

    surface->parent = parent;

    surface->surface = wl_compositor_create_surface(bl_app->compositor);
    surface->subsurface = NULL;
    surface->frame_callback = NULL;
    surface->buffer = NULL;

    surface->shm_data = NULL;
    surface->shm_data_size = 0;

    surface->x = 0;
    surface->y = 0;
    surface->width = 0;
    surface->height = 0;
    surface->implicit_width = 0;
    surface->implicit_height = 0;
    surface->color = bl_color_from_rgb(255, 255, 255);

    surface->pointer_move_event = NULL;
    surface->pointer_press_event = NULL;
    surface->pointer_release_event = NULL;

    // Create wl_subsurface if has parent.
    if (surface->parent != NULL) {
        surface->subsurface = wl_subcompositor_get_subsurface(
            bl_app->subcompositor,
            surface->surface,
            surface->parent->surface
        );
    }

    // Add surface to BTree map.
    bl_ptr_btree_insert(bl_app->surface_map,
        (uint64_t)surface->surface, (uint64_t)surface);

    return surface;
}

void bl_surface_set_parent(bl_surface *surface, bl_surface *parent)
{
    if (surface->parent != NULL) {
        bl_log(BL_LOG_LEVEL_WARN, "%s() - Parent already exist.\n", __func__);
    }

    surface->parent = parent;

    // Create wl_subsurface if not set.
    if (surface->subsurface == NULL) {
        surface->subsurface = wl_subcompositor_get_subsurface(
            bl_app->subcompositor,
            surface->surface,
            surface->parent->surface
        );
    }
}

void bl_surface_set_geometry(bl_surface *surface,
        double x, double y, double width, double height)
{
    surface->x = x;
    surface->y = y;
    surface->width = width;
    surface->height = height;

    if (surface->shm_data != NULL) {
        munmap(surface->shm_data, surface->shm_data_size);
    }
    if (surface->buffer != NULL) {
        wl_buffer_destroy(surface->buffer);
    }
    surface->buffer = create_buffer(surface, width, height, bl_app->shm);
}

void bl_surface_set_color(bl_surface *surface, const bl_color color)
{
    surface->color = color;
}

void bl_surface_paint(bl_surface *surface)
{
    bl_log(BL_LOG_LEVEL_INFO, "%s() - surface: %p", __func__, surface);

    // Do nothing if size is zero.
    if (surface->width == 0 || surface->height == 0) {
        return;
    }

    paint_pixels(surface);
}

void bl_surface_render_pixels(bl_surface *surface, const uint32_t *pixels,
        uint64_t width, uint64_t height)
{
    for (uint64_t y = 0; y < height; ++y) {
        for (uint64_t x = 0; x < width; ++x) {
            uint64_t target = (width * y) + x;
            paint_pixel(surface, pixels[target], x, y);
        }
    }
}

void bl_surface_render_image(bl_surface*surface, const bl_image *image,
        bl_image_scale scale)
{
    uint64_t base_width = ((uint64_t)(surface->width) >= image->width) ?
        image->width :
        (uint64_t)(surface->width);
    uint64_t base_height = ((uint64_t)(surface->height) >= image->height) ?
        image->height :
        (uint64_t)(surface->height);
    uint64_t offset = ((uint64_t)(surface->width) >= image->width) ?
        (uint64_t)(surface->width) - image->width :
        image->width - (uint64_t)(surface->width);
    const uint32_t *pixels = (const uint32_t*)(image->data);
    for (uint64_t y = 0; y < base_height; ++y) {
        for (uint64_t x = 0; x < base_width; ++x) {
            uint64_t target = (base_width * y) + x + (offset * y);
            paint_pixel(surface, pixels[target], x, y);
        }
    }
}

void bl_surface_show(bl_surface *surface)
{
    wl_surface_attach(surface->surface, surface->buffer,
        0, 0);
    wl_surface_commit(surface->surface);

    if (surface->parent != NULL) {
//        wl_surface_commit(surface->parent->surface);
        wl_subsurface_set_position(surface->subsurface,
            surface->x, surface->y);
        bl_surface_show(surface->parent);
    }
}

void bl_surface_set_pointer_press_event(bl_surface *surface,
        void (*event)(bl_surface*, bl_pointer_event*))
{
    surface->pointer_press_event = event;
}

void bl_surface_free(bl_surface *surface)
{
    if (surface->buffer != NULL) {
        wl_buffer_destroy(surface->buffer);
    }
    wl_surface_destroy(surface->surface);

    bl_ptr_btree_remove(bl_app->surface_map, (uint64_t)(surface->surface));

    free(surface);
}
