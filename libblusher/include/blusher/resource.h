#ifndef _BLUSHER_RESOURCE_H
#define _BLUSHER_RESOURCE_H

#include <stdint.h>

#define BLUSHER_RESOURCE_CAPACITY_MULTIPLE 8

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bl_resource_data {
    const char *path;
    const unsigned char *data;
    uint64_t size;
} bl_resource_data;

typedef struct bl_resource {
    bl_resource_data const **data;
    uint32_t length;
    uint32_t capacity;
} bl_resource;

bl_resource* bl_resource_new();

void bl_resource_add_data(bl_resource *resource, const bl_resource_data *data);

const bl_resource_data* bl_resource_resource_data(bl_resource *resource,
        const char *path);

void bl_resource_free(bl_resource *resource);

#ifdef __cplusplus
}
#endif

#endif /* _BLUSHER_RESOURCE_H */
