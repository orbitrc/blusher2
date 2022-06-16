#include <blusher/wayland/xdg-wm-base.h>

// C
#include <assert.h>

// Wayland Protocol
#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

//=======================
// XdgWmBase::Listener
//=======================
XdgWmBase::Listener::Listener()
{
    struct xdg_wm_base_listener *listener = new struct xdg_wm_base_listener();
    listener->ping = nullptr;

    this->_xdg_wm_base_listener = listener;
}

XdgWmBase::Listener::Listener(XdgWmBase::Listener::PingHandler ping)
{
    struct xdg_wm_base_listener *listener = new struct xdg_wm_base_listener();
    listener->ping = ping;

    this->_xdg_wm_base_listener = listener;
}

XdgWmBase::Listener::~Listener()
{
    delete this->_xdg_wm_base_listener;
}

using Listener = XdgWmBase::Listener;
const struct xdg_wm_base_listener* Listener::xdg_wm_base_listener() const
{
    return this->_xdg_wm_base_listener;
}

//==============
// XdgWmBase
//==============

XdgWmBase::XdgWmBase(struct xdg_wm_base *base)
{
    this->_xdg_wm_base = base;
}

XdgWmBase::~XdgWmBase()
{
    if (this->_xdg_wm_base != nullptr) {
        // FIXME: Temporary disable next line.
        // xdg_wm_base_destroy(this->_xdg_wm_base);
    }
}

struct xdg_wm_base* XdgWmBase::xdg_wm_base()
{
    return this->_xdg_wm_base;
}

void XdgWmBase::add_listener(const XdgWmBase::Listener& listener)
{
    this->_listener = listener;
    xdg_wm_base_add_listener(this->_xdg_wm_base,
        listener.xdg_wm_base_listener(), NULL);
}

XdgSurface XdgWmBase::get_xdg_surface(WlSurface& surface)
{
    struct xdg_surface *xdg_surface =
        xdg_wm_base_get_xdg_surface(this->_xdg_wm_base, surface.wl_surface());
    assert(xdg_surface != NULL);

    XdgSurface bl_xdg_surface = XdgSurface(xdg_surface, &surface);

    return bl_xdg_surface;
}

void XdgWmBase::pong(uint32_t serial)
{
    xdg_wm_base_pong(this->_xdg_wm_base, serial);
}

} // namespace bl
