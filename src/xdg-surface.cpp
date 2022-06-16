#include <blusher/wayland/xdg-surface.h>

// Wayland Protocol
#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

XdgSurface::XdgSurface(struct xdg_surface *xdg_surface,
        WlSurface *bl_wl_surface)
{
    this->_xdg_surface = xdg_surface;
    this->_surface = bl_wl_surface;
}

XdgSurface::~XdgSurface()
{
    xdg_surface_destroy(this->_xdg_surface);
}

XdgToplevel XdgSurface::get_toplevel()
{
    struct xdg_toplevel *xdg_toplevel =
        xdg_surface_get_toplevel(this->_xdg_surface);

    XdgToplevel toplevel(xdg_toplevel);

    return toplevel;
}

} // namespace bl
