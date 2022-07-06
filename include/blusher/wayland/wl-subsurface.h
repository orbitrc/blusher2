#ifndef _BL_WL_SUBSURFACE_H
#define _BL_WL_SUBSURFACE_H

// C
#include <stdint.h>

// Wayland
#include <wayland-client.h>

// Blusher
#include <blusher/wayland/wl-surface.h>

namespace bl {

class WlSubsurface
{
public:
    WlSubsurface(struct wl_subsurface *subsurface);

    ~WlSubsurface();

    /// Get the underlying struct wl_subsurface pointer.
    struct wl_subsurface* c_ptr();

    void set_position(uint32_t x, uint32_t y);

    void place_above(const WlSurface& sibling);

    void place_below(const WlSurface& sibling);

    void set_sync();

    void set_desync();

private:
    struct wl_subsurface *_wl_subsurface;
};

} // namespace bl

#endif // _BL_WL_SUBSURFACE_H
