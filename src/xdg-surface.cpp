#include <blusher/wayland/xdg-surface.h>

// Wayland Protocol
#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

//=======================
// XdgSurface::Listener
//=======================
XdgSurface::Listener::Listener()
{
    struct xdg_surface_listener *listener = new struct xdg_surface_listener();
    listener->configure = nullptr;

    this->_xdg_surface_listener = listener;
}

XdgSurface::Listener::Listener(XdgSurface::Listener::ConfigureHandler configure)
{
    struct xdg_surface_listener *listener = new struct xdg_surface_listener();
    listener->configure = configure;

    this->_xdg_surface_listener = listener;
}

XdgSurface::Listener::~Listener()
{
    delete this->_xdg_surface_listener;
}

using Listener = XdgSurface::Listener;
const struct xdg_surface_listener* Listener::xdg_surface_listener() const
{
    return this->_xdg_surface_listener;
}

//==============
// XdgSurface
//==============

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

struct xdg_surface* XdgSurface::xdg_surface()
{
    return this->_xdg_surface;
}

void XdgSurface::add_listener(const XdgSurface::Listener& listener)
{
    this->_listener = listener;
    xdg_surface_add_listener(this->_xdg_surface,
        listener.xdg_surface_listener(), NULL);
}

XdgToplevel XdgSurface::get_toplevel()
{
    struct xdg_toplevel *xdg_toplevel =
        xdg_surface_get_toplevel(this->_xdg_surface);

    XdgToplevel toplevel(xdg_toplevel);

    return toplevel;
}

void XdgSurface::ack_configure(struct xdg_surface *xdg_surface, uint32_t serial)
{
    xdg_surface_ack_configure(xdg_surface, serial);
}

} // namespace bl
