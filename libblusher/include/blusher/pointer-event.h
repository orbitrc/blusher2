#ifndef _BLUSHER_POINTER_EVENT_H
#define _BLUSHER_POINTER_EVENT_H

#include <stdint.h>

#include <linux/input.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bl_pointer_event {
    uint32_t serial;

    uint32_t button;
    double x;
    double y;
} bl_pointer_event;

bl_pointer_event* bl_pointer_event_new();

void bl_pointer_event_free(bl_pointer_event *event);

#ifdef __cplusplus
}
#endif

#endif /* _BLUSHER_POINTER_EVENT_H */
