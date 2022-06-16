#include <blusher/wayland/xdg-toplevel.h>

namespace bl {

XdgToplevel::XdgToplevel(struct xdg_toplevel *xdg_toplevel)
{
    this->_xdg_toplevel = xdg_toplevel;
}

} // namespace bl
