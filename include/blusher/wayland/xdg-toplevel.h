#ifndef _BL_XDG_TOPLEVEL_H
#define _BL_XDG_TOPLEVEL_H

// C
#include <stdint.h>

struct wl_array;
struct xdg_toplevel;
struct xdg_toplevel_listener;

namespace bl {

class XdgToplevel
{
public:
    class Listener
    {
    public:
        using ConfigureHandler = void (void *data,
            struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height,
            struct wl_array *states);
        using CloseHandler = void (void *data,
            struct xdg_toplevel *xdg_toplevel);

    public:
        Listener();

        Listener(ConfigureHandler configure, CloseHandler close);

        ~Listener();

        const struct xdg_toplevel_listener* xdg_toplevel_listener() const;

    private:
        struct xdg_toplevel_listener *_xdg_toplevel_listener;
    };

public:
    XdgToplevel(struct xdg_toplevel *xdg_toplevel);

    ~XdgToplevel();

    struct xdg_toplevel* xdg_toplevel();

    void add_listener(const XdgToplevel::Listener& listener);

private:
    struct xdg_toplevel *_xdg_toplevel;
    XdgToplevel::Listener _listener;
};

} // namespace bl

#endif // _BL_XDG_TOPLEVEL_H
