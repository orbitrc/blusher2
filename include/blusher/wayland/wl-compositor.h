#ifndef _BL_WL_COMPOSITOR_H
#define _BL_WL_COMPOSITOR_H

#include <memory>

#include <wayland-client.h>

#include <blusher/wayland/wl-surface.h>

namespace bl {

extern class WlCompositor *wl_compositor_singleton;

/// WlCompositor is a singleton object.
class WlCompositor
{
public:
    WlCompositor();

    WlCompositor(struct wl_compositor *compositor);

    ~WlCompositor();

    struct wl_compositor* c_ptr();

    std::shared_ptr<WlSurface> create_surface();

    /// Returns singleton instance.
    static WlCompositor* instance();

    //==============
    // Operators
    //==============
    WlCompositor& operator=(const WlCompositor& other);

private:
    struct wl_compositor *_wl_compositor;
};

} // namespace bl

#endif // _BL_WL_COMPOSITOR_H
