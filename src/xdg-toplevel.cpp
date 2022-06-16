#include <blusher/wayland/xdg-toplevel.h>

// Wayland Protocol
#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

XdgToplevel::XdgToplevel(struct xdg_toplevel *xdg_toplevel)
{
    this->_xdg_toplevel = xdg_toplevel;
}

} // namespace bl
