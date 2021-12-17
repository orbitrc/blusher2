#include "image.h"

// Std libraries
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Cairo
#include <cairo.h>

#include "application.h"
#include "resource.h"

static cairo_status_t cairo_read_png_func(void *closure,
        unsigned char *data, unsigned int length)
{
    FILE *f = (FILE*)closure;

    unsigned long read = fread(data, 1, length, f);
    if (read == 0) {
        return CAIRO_STATUS_READ_ERROR;
    }

    return CAIRO_STATUS_SUCCESS;
}

bl_image_file_format get_image_file_format(bl_image *image, int min_length)
{
    // PNG
    if (min_length > 8) {
        if (image->data[0] == 0x89 && image->data[1] == 0x50 &&
                image->data[2] == 0x4e && image->data[3] == 0x47 &&
                image->data[4] == 0x0d && image->data[5] == 0x0a &&
                image->data[6] == 0x1a && image->data[7] == 0x0a) {
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
            image->data = (uint32_t*)(rc_data->data);
            image->file_format = get_image_file_format(image, rc_data->size);
            if (image->file_format == BL_IMAGE_FILE_FORMAT_PNG) {
                cairo_surface_t *surface =
                    cairo_image_surface_create_from_png_stream(
                        cairo_read_png_func,
                        image->data
                    );
            }
        } else {
            // Path is file system path.
        }
    }

    return image;
}

void bl_image_free(bl_image *image)
{
    free(image->data);
    free(image);
}
