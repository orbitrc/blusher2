#include "resource.h"

#include <stdlib.h>
#include <string.h>

bl_resource* bl_resource_new()
{
    bl_resource *resource = malloc(sizeof(bl_resource));

    resource->data = malloc(
        sizeof(const bl_resource_data*) * BLUSHER_RESOURCE_CAPACITY_MULTIPLE);
    resource->capacity = BLUSHER_RESOURCE_CAPACITY_MULTIPLE;
    resource->length = 0;

    return resource;
}

void bl_resource_add_data(bl_resource *resource, const bl_resource_data *data)
{
    resource->data[resource->length] = data;
    resource->length += 1;
}

const bl_resource_data* bl_resource_resource_data(bl_resource *resource,
        const char *path)
{
    bl_resource_data const* resource_data = NULL;

    for (uint32_t i = 0; i < resource->length; ++i) {
        if (strcmp(resource->data[i]->path, path) == 0) {
            resource_data = resource->data[i];
            break;
        }
    }

    return resource_data;
}

void bl_resource_free(bl_resource *resource)
{
    free(resource->data);
    free(resource);
}
