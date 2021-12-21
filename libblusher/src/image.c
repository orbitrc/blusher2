#include "image.h"

// Std libraries
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Cairo
#include <cairo.h>

#include "application.h"
#include "resource.h"
#include <blusher/core/log.h>

typedef struct bl_cairo_png_closure {
    unsigned char const* data;
    unsigned int read;
} bl_cairo_png_closure;

static cairo_status_t cairo_read_png_func(void *closure,
        unsigned char *data, unsigned int length)
{
    bl_cairo_png_closure *c = closure;

    for (unsigned int i = 0; i < length; ++i) {
        data[i] = c->data[i + c->read];
    }
    c->read += length;

    return CAIRO_STATUS_SUCCESS;
}

bl_image_file_format get_image_file_format(const unsigned char *data,
        int min_length)
{
    // PNG
    if (min_length > 8) {
        if (data[0] == 0x89 && data[1] == 0x50 &&
                data[2] == 0x4e && data[3] == 0x47 &&
                data[4] == 0x0d && data[5] == 0x0a &&
                data[6] == 0x1a && data[7] == 0x0a) {
            return BL_IMAGE_FILE_FORMAT_PNG;
        }
    }

    return BL_IMAGE_FILE_FORMAT_INVALID;
}

bl_image* bl_image_from_path(const char *path)
{
    bl_image *image = malloc(sizeof(bl_image));

    if (strlen(path) > 4) {
        if (path[0] == 'b' && path[1] == 'r' && path[2] == 'c' &&
                path[3] == ':') {
            // Path is blusher resource.
            const char *rc_path = path + 4;
            const bl_resource_data *rc_data =
                bl_resource_resource_data(bl_app->resource, rc_path);
            // If resource data not found, free memory and return null.
            if (rc_data == NULL) {
                free(image);
                return NULL;
            }
            image->file_format = get_image_file_format(rc_data->data,
                rc_data->size);
            if (image->file_format == BL_IMAGE_FILE_FORMAT_PNG) {
                // FIXME: remove below code. memory leak.
                image->data = malloc(sizeof(unsigned char) * rc_data->size);
                bl_cairo_png_closure closure = {
                    .data = rc_data->data,
                    .read = 0,
                };
                cairo_surface_t *cairo_surface =
                    cairo_image_surface_create_from_png_stream(
                        cairo_read_png_func,
                        &closure
                    );
                cairo_t *cr = cairo_create(cairo_surface);
                if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
                    free(image);
                    return NULL;
                }
                image->width = cairo_image_surface_get_width(cairo_surface);
                image->height = cairo_image_surface_get_height(cairo_surface);
                cairo_format_t cairo_format =
                    cairo_image_surface_get_format(cairo_surface);

                // Set image format.
                switch (cairo_format) {
                case CAIRO_FORMAT_ARGB32:
                    image->format = BL_IMAGE_FORMAT_ARGB32;
                    break;
                case CAIRO_FORMAT_RGB24:
                    image->format = BL_IMAGE_FORMAT_RGB32;
                    break;
                default:
                    image->format = BL_IMAGE_FORMAT_INVALID;
                }

                uint64_t data_size =
                    sizeof(uint32_t) * (image->width * image->height);
                image->data = malloc(data_size);
                memcpy(
                    image->data,
                    cairo_image_surface_get_data(cairo_surface),
                    data_size
                );

                // Free the Cairo resources.
                cairo_surface_destroy(cairo_surface);
                cairo_destroy(cr);
            } else if (image->file_format == BL_IMAGE_FILE_FORMAT_INVALID) {
                free(image);
                return NULL;
            }
        } else {
            // Path is file system path.
            bl_log(BL_LOG_LEVEL_ERROR,
                "%s() - File system path not implemented.", __func__);
        }
    }

    return image;
}

bl_image* bl_image_from_data(const unsigned char *data, uint64_t size)
{
    bl_image *image = malloc(sizeof(bl_image));

    bl_image_file_format file_format = get_image_file_format(data, size);

    if (file_format == BL_IMAGE_FILE_FORMAT_PNG) {
        bl_cairo_png_closure closure = {
            .data = data,
            .read = 0,
        };
        // Create Cairo surface.
        cairo_surface_t *cairo_surface =
            cairo_image_surface_create_from_png_stream(
                cairo_read_png_func,
                &closure
            );
        // Create Cairo.
        cairo_t *cr = cairo_create(cairo_surface);
        if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
            free(image);
            return NULL;
        }
        // Get image width and height.
        image->width = cairo_image_surface_get_width(cairo_surface);
        image->height = cairo_image_surface_get_height(cairo_surface);

        cairo_format_t cairo_format =
            cairo_image_surface_get_format(cairo_surface);
        // Set image format.
        switch (cairo_format) {
        case CAIRO_FORMAT_ARGB32:
            image->format = BL_IMAGE_FORMAT_ARGB32;
            break;
        case CAIRO_FORMAT_RGB24:
            image->format = BL_IMAGE_FORMAT_RGB32;
            break;
        default:
            image->format = BL_IMAGE_FORMAT_INVALID;
        }

        uint64_t data_size =
            sizeof(uint32_t) * (image->width * image->height);
        image->data = malloc(data_size);
        memcpy(
            image->data,
            cairo_image_surface_get_data(cairo_surface),
            data_size
        );

        // Free the Cairo resources.
        cairo_surface_destroy(cairo_surface);
        cairo_destroy(cr);
    } else if (file_format == BL_IMAGE_FILE_FORMAT_INVALID) {
        free(image);
        return NULL;
    }

    return image;
}

uint64_t bl_image_width(const bl_image *image)
{
    return image->width;
}

uint64_t bl_image_height(const bl_image *image)
{
    return image->height;
}

void bl_image_free(bl_image *image)
{
    free(image->data);
    free(image);
}
