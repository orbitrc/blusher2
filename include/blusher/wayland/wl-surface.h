#ifndef _BL_WL_SURFACE_H
#define _BL_WL_SURFACE_H

#include <wayland-client.h>

namespace bl {

class WlSurface
{
public:
    WlSurface(struct wl_surface *surface);

    ~WlSurface();

    struct wl_surface* c_ptr();

    void commit();

private:
    struct wl_surface *_wl_surface;
};

} // namespace bl

#endif // _BL_WL_SURFACE_H
