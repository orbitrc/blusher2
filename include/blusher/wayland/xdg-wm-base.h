#ifndef _BL_XDG_WM_BASE_H
#define _BL_XDG_WM_BASE_H

#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

class XdgWmBase
{
public:
    class Listener
    {
    public:
        using PingHandler = void (void *data,
            struct xdg_wm_base *xdg_wm_base, uint32_t serial);

    public:
        Listener();

        Listener(PingHandler ping);

        const struct xdg_wm_base_listener& xdg_wm_base_listener() const;

    private:
        struct xdg_wm_base_listener _xdg_wm_base_listener;
    };

public:
    XdgWmBase(struct xdg_wm_base *base);

    ~XdgWmBase();

    struct xdg_wm_base* xdg_wm_base();

    void add_listener(const XdgWmBase::Listener& listener);

private:
    struct xdg_wm_base *_xdg_wm_base;
    XdgWmBase::Listener _listener;
};

} // namespace bl

#endif // _BL_XDG_WM_BASE_H
