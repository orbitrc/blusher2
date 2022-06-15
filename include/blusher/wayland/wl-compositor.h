#ifndef _BL_WL_COMPOSITOR_H
#define _BL_WL_COMPOSITOR_H

#include <wayland-client.h>

#include <blusher/wayland/wl-surface.h>

namespace bl {

extern class WlCompositor *wl_compositor_singleton;

/// WlCompositor is a singleton object.
class WlCompositor
{
public:
    WlCompositor(struct wl_compositor *compositor);

    struct wl_compositor* wl_compositor();

    WlSurface create_surface();

    /// Returns singleton instance.
    static WlCompositor* instance();

private:
    struct wl_compositor *_wl_compositor;
};

} // namespace bl

#endif // _BL_WL_COMPOSITOR_H
