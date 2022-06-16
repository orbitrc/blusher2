#include <blusher/wayland/xdg-toplevel.h>

// Wayland Protocol
#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

//========================
// XdgToplevel::Listener
//========================

XdgToplevel::Listener::Listener()
{
    struct xdg_toplevel_listener *listener = new struct xdg_toplevel_listener();
    listener->configure = nullptr;
    listener->close = nullptr;

    this->_xdg_toplevel_listener = listener;
}

XdgToplevel::Listener::Listener(
        XdgToplevel::Listener::ConfigureHandler configure,
        XdgToplevel::Listener::CloseHandler close)
{
    struct xdg_toplevel_listener *listener = new struct xdg_toplevel_listener();
    listener->configure = configure;
    listener->close = close;

    this->_xdg_toplevel_listener = listener;
}

XdgToplevel::Listener::~Listener()
{
    delete this->_xdg_toplevel_listener;
}

using Listener = XdgToplevel::Listener;
const struct xdg_toplevel_listener* Listener::xdg_toplevel_listener() const
{
    return this->_xdg_toplevel_listener;
}

//===============
// XdgToplevel
//===============

XdgToplevel::XdgToplevel(struct xdg_toplevel *xdg_toplevel)
{
    this->_xdg_toplevel = xdg_toplevel;
}

XdgToplevel::~XdgToplevel()
{
    xdg_toplevel_destroy(this->_xdg_toplevel);
}

struct xdg_toplevel* XdgToplevel::xdg_toplevel()
{
    return this->_xdg_toplevel;
}

void XdgToplevel::add_listener(const XdgToplevel::Listener &listener)
{
    this->_listener = listener;
    xdg_toplevel_add_listener(this->_xdg_toplevel,
        listener.xdg_toplevel_listener(), NULL);
}

} // namespace bl
