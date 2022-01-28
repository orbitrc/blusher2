#ifndef _BLUSHER_CORE_COLLECTIONS_H
#define _BLUSHER_CORE_COLLECTIONS_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bl_ptr_btree {
    void *btree;
} bl_ptr_btree;

bl_ptr_btree* bl_ptr_btree_new();

bool bl_ptr_btree_insert(bl_ptr_btree *btree,
        uint64_t wl_surface, uint64_t bl_surface);

bool bl_ptr_btree_contains(bl_ptr_btree *btree,
        uint64_t wl_surface);

uint64_t bl_ptr_btree_get(bl_ptr_btree *btree,
        uint64_t wl_surface);

bool bl_ptr_btree_remove(bl_ptr_btree *btree,
        uint64_t wl_surface);

void bl_ptr_btree_free(bl_ptr_btree *btree);

#ifdef __cplusplus
}
#endif

#endif /* _BLUSHER_CORE_COLLECTIONS_H */
