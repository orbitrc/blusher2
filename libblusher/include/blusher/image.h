#ifndef _BLUSHER_IMAGE_H
#define _BLUSHER_IMAGE_H

#include <stdint.h>

typedef enum bl_image_format {
    BL_IMAGE_FORMAT_INVALID = 0,
} bl_image_format;

typedef enum bl_image_file_format {
    BL_IMAGE_FILE_FORMAT_INVALID = 0,
    BL_IMAGE_FILE_FORMAT_PNG = 1,
    BL_IMAGE_FILE_FORMAT_BMP = 2,
    BL_IMAGE_FILE_FORMAT_JPEG = 3,
} bl_image_file_format;

typedef struct bl_image {
    bl_image_format format;
    bl_image_file_format file_format;
    uint64_t width;
    uint64_t height;
    uint32_t *data;
} bl_image;

bl_image* bl_image_from_path(const char *path);

void bl_image_free(bl_image *image);

#endif /* _BLUSHER_IMAGE_H */
