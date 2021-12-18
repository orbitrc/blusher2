#ifndef _BLUSHER_IMAGE_H
#define _BLUSHER_IMAGE_H

#include <stdint.h>

typedef enum bl_image_format {
    BL_IMAGE_FORMAT_INVALID = 0,
    BL_IMAGE_FORMAT_ARGB32 = 1,
    BL_IMAGE_FORMAT_RGB32 = 2,
} bl_image_format;

typedef enum bl_image_file_format {
    BL_IMAGE_FILE_FORMAT_INVALID = 0,
    BL_IMAGE_FILE_FORMAT_PNG = 1,
    BL_IMAGE_FILE_FORMAT_BMP = 2,
    BL_IMAGE_FILE_FORMAT_JPEG = 3,
} bl_image_file_format;

typedef enum bl_image_scale {
    /// \brief Keep source image size.
    BL_IMAGE_SCALE_NO_SCALE = 0,
    /// \brief Fit to new size.
    BL_IMAGE_SCALE_FIT = 1,
} bl_image_scale;

typedef struct bl_image {
    bl_image_format format;
    bl_image_file_format file_format;
    uint64_t width;
    uint64_t height;
    unsigned char *data;
} bl_image;

bl_image* bl_image_from_path(const char *path);

uint64_t bl_image_width(const bl_image *image);

uint64_t bl_image_height(const bl_image *image);

void bl_image_free(bl_image *image);

#endif /* _BLUSHER_IMAGE_H */
