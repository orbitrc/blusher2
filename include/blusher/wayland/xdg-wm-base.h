#ifndef _BL_XDG_WM_BASE_H
#define _BL_XDG_WM_BASE_H

// C++
#include <memory>

// Blusher
#include <blusher/wayland/xdg-surface.h>

struct xdg_wm_base;
struct xdg_wm_base_listener;

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

        ~Listener();

        const struct xdg_wm_base_listener* xdg_wm_base_listener() const;

    private:
        struct xdg_wm_base_listener *_xdg_wm_base_listener;
    };

public:
    XdgWmBase(struct xdg_wm_base *base);

    ~XdgWmBase();

    struct xdg_wm_base* c_ptr();

    void add_listener(const XdgWmBase::Listener& listener);

    std::shared_ptr<XdgSurface> get_xdg_surface(WlSurface& surface);

    void pong(uint32_t serial);

private:
    struct xdg_wm_base *_xdg_wm_base;
    XdgWmBase::Listener _listener;
};

} // namespace bl

#endif // _BL_XDG_WM_BASE_H
