#include <blusher/wayland/wl-surface.h>

#include <stdio.h>

namespace bl {

WlSurface::WlSurface(struct wl_surface *surface)
{
    this->_wl_surface = surface;
}

WlSurface::~WlSurface()
{
    wl_surface_destroy(this->_wl_surface);
}

struct wl_surface* WlSurface::c_ptr()
{
    return this->_wl_surface;
}

void WlSurface::commit()
{
    fprintf(stderr, "WlSurface::commit() - %p\n", this->_wl_surface);
    wl_surface_commit(this->_wl_surface);
}

} // namespace bl

