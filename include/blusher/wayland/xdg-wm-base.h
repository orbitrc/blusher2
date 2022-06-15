#ifndef _BL_XDG_WM_BASE_H
#define _BL_XDG_WM_BASE_H

#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

class XdgWmBase
{
public:
    XdgWmBase(struct xdg_wm_base *base);

    ~XdgWmBase();

    struct xdg_wm_base* xdg_wm_base();

private:
    struct xdg_wm_base *_xdg_wm_base;
};

} // namespace bl

#endif // _BL_XDG_WM_BASE_H
