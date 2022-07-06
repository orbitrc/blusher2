#include <blusher/wayland/wl-subsurface.h>

namespace bl {

WlSubsurface::WlSubsurface(struct wl_subsurface *subsurface)
{
    this->_wl_subsurface = subsurface;
}

WlSubsurface::~WlSubsurface()
{
    wl_subsurface_destroy(this->_wl_subsurface);
}

struct wl_subsurface* WlSubsurface::wl_subsurface()
{
    return this->_wl_subsurface;
}

void WlSubsurface::place_above(const WlSurface& sibling)
{
    wl_subsurface_place_above(this->_wl_subsurface,
        const_cast<WlSurface&>(sibling).wl_surface());
}

void WlSubsurface::place_below(const WlSurface& sibling)
{
    wl_subsurface_place_below(this->_wl_subsurface,
        const_cast<WlSurface&>(sibling).wl_surface());
}

void WlSubsurface::set_sync()
{
    wl_subsurface_set_sync(this->_wl_subsurface);
}

void WlSubsurface::set_desync()
{
    wl_subsurface_set_desync(this->_wl_subsurface);
}

} // namespace bl
