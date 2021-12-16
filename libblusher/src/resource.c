#include "resource.h"

#include <stdlib.h>

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

void bl_resource_free(bl_resource *resource)
{
    free(resource->data);
    free(resource);
}
