#ifndef _BL_XDG_SURFACE_H
#define _BL_XDG_SURFACE_H

// Wayland
#include <wayland-client.h>

// Blusher
#include <blusher/wayland/wl-surface.h>
#include <blusher/wayland/xdg-toplevel.h>

// Wayland Protocol
#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

class XdgSurface
{
public:
    XdgSurface(struct xdg_surface *xdg_surface, WlSurface *bl_wl_surface);

    ~XdgSurface();

    XdgToplevel get_toplevel();

private:
    struct xdg_surface *_xdg_surface;
    WlSurface *_surface;
};

} // namespace bl

#endif // _BL_XDG_SURFACE_H
