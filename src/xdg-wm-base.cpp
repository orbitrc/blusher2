#include <blusher/wayland/xdg-wm-base.h>

namespace bl {

//=======================
// XdgWmBase::Listener
//=======================
XdgWmBase::Listener::Listener()
{
    this->_xdg_wm_base_listener = {
        nullptr,
    };
}

XdgWmBase::Listener::Listener(XdgWmBase::Listener::PingHandler ping)
{
    this->_xdg_wm_base_listener = {
        ping,
    };
}

using Listener = XdgWmBase::Listener;
const struct xdg_wm_base_listener& Listener::xdg_wm_base_listener() const
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
    xdg_wm_base_destroy(this->_xdg_wm_base);
}

struct xdg_wm_base* XdgWmBase::xdg_wm_base()
{
    return this->_xdg_wm_base;
}

void XdgWmBase::add_listener(const XdgWmBase::Listener& listener)
{
    this->_listener = listener;
    xdg_wm_base_add_listener(this->_xdg_wm_base,
        &listener.xdg_wm_base_listener(), NULL);
}

} // namespace bl
