#ifndef _BL_XDG_SURFACE_H
#define _BL_XDG_SURFACE_H

// Wayland
#include <wayland-client.h>

// Blusher
#include <blusher/wayland/wl-surface.h>
#include <blusher/wayland/xdg-toplevel.h>

struct xdg_surface;
struct xdg_surface_listener;

namespace bl {

class XdgSurface
{
public:
    class Listener
    {
    public:
        using ConfigureHandler = void (void *data,
            struct xdg_surface *xdg_surface, uint32_t serial);

    public:
        Listener();

        Listener(ConfigureHandler configure);

        ~Listener();

        const struct xdg_surface_listener* xdg_surface_listener() const;

    private:
        struct xdg_surface_listener *_xdg_surface_listener;
    };

public:
    XdgSurface(struct xdg_surface *xdg_surface, WlSurface *bl_wl_surface);

    ~XdgSurface();

    struct xdg_surface* xdg_surface();

    void add_listener(const XdgSurface::Listener& listener);

    XdgToplevel get_toplevel();

    /// Call xdg_surface_ack_configure.
    /// This needed in listener handler, so it is a static method.
    static void ack_configure(struct xdg_surface *xdg_surface, uint32_t serial);

private:
    struct xdg_surface *_xdg_surface;
    WlSurface *_surface;
    XdgSurface::Listener _listener;
};

} // namespace bl

#endif // _BL_XDG_SURFACE_H
