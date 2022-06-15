#include <blusher/wayland/wl-surface.h>

namespace bl {

WlSurface::WlSurface(struct wl_surface *surface)
{
    this->_wl_surface = surface;
}

WlSurface::~WlSurface()
{
    wl_surface_destroy(this->_wl_surface);
}

} // namespace bl

