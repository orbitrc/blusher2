#include "svg.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "application.h"
#include "resource.h"
#include "image.h"
#include <blusher/svg/svg-core.h>

bl_svg* bl_svg_from_path(const char *path)
{
    bl_svg *svg = malloc(sizeof(bl_svg));

    if (strlen(path) > 4) {
        if (path[0] == 'b' && path[1] == 'r' && path[2] == 'c' &&
                path[3] == ':') {
            // Path is blusher resource.
            const char *rc_path = path + 4;
            const bl_resource_data *rc_data =
                bl_resource_resource_data(bl_app->resource, rc_path);
            // If resource data not found, free memory and return null.
            if (rc_data == NULL) {
                free(svg);
                return NULL;
            }
            svg->core = bl_svg_core_from_data(rc_data->data, rc_data->size);
        } else {
            // Path is file system path.
            fprintf(stderr, "WARNING - %s() - NOT IMPLEMENTED!\n", __func__);
        }
    }


    return svg;
}

bl_image* bl_svg_to_image(bl_svg *svg, uint64_t width, uint64_t height)
{
    unsigned char *data;
    uint32_t size;

    data = bl_svg_core_png_data(svg->core, width, height, &size);

    if (data == NULL) {
        return NULL;
    }

    bl_image *image = bl_image_from_data(data, size);

    return image;
}

void bl_svg_free(bl_svg *svg)
{
    bl_svg_core_free(svg->core);
    free(svg);
}
