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

void XdgToplevel::add_listener(const XdgToplevel::Listener &listener,
        void *data)
{
    this->_listener = listener;
    xdg_toplevel_add_listener(this->_xdg_toplevel,
        listener.xdg_toplevel_listener(), data);
}

void XdgToplevel::move(const WlSeat& seat, uint32_t serial)
{
    xdg_toplevel_move(this->_xdg_toplevel, seat.wl_seat(), serial);
}

void XdgToplevel::resize(const WlSeat &seat, uint32_t serial, ResizeEdge edge)
{
    uint32_t xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
    switch (edge) {
    case XdgToplevel::ResizeEdge::TopLeft:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
        break;
    case XdgToplevel::ResizeEdge::Top:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
        break;
    case XdgToplevel::ResizeEdge::TopRight:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
        break;
    case XdgToplevel::ResizeEdge::Left:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
        break;
    case XdgToplevel::ResizeEdge::Right:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
        break;
    case XdgToplevel::ResizeEdge::BottomLeft:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
        break;
    case XdgToplevel::ResizeEdge::Bottom:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
        break;
    case XdgToplevel::ResizeEdge::BottomRight:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
        break;
    default:
        break;
    }

    xdg_toplevel_resize(this->_xdg_toplevel,
        seat.wl_seat(),
        serial,
        xdg_edge
    );
}

void XdgToplevel::set_maximized()
{
    xdg_toplevel_set_maximized(this->_xdg_toplevel);
}

void XdgToplevel::unset_maximized()
{
    xdg_toplevel_unset_maximized(this->_xdg_toplevel);
}

//===================
// Static Methods
//===================

pr::Vector<XdgToplevel::State> XdgToplevel::states_to_vector(
        struct wl_array *states)
{
    pr::Vector<XdgToplevel::State> v;

    for (size_t i = 0; i < states->size; ++i) {
        enum xdg_toplevel_state state =
            ((enum xdg_toplevel_state*)(states->data))[i];
        switch (state) {
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
            v.push(XdgToplevel::State::Maximized);
            break;
        case XDG_TOPLEVEL_STATE_FULLSCREEN:
            v.push(XdgToplevel::State::Fullscreen);
            break;
        case XDG_TOPLEVEL_STATE_RESIZING:
            v.push(XdgToplevel::State::Resizing);
            break;
        case XDG_TOPLEVEL_STATE_ACTIVATED:
            v.push(XdgToplevel::State::Activated);
            break;
        case XDG_TOPLEVEL_STATE_TILED_LEFT:
            v.push(XdgToplevel::State::TiledLeft);
            break;
        case XDG_TOPLEVEL_STATE_TILED_RIGHT:
            v.push(XdgToplevel::State::TiledRight);
            break;
        case XDG_TOPLEVEL_STATE_TILED_TOP:
            v.push(XdgToplevel::State::TiledTop);
            break;
        case XDG_TOPLEVEL_STATE_TILED_BOTTOM:
            v.push(XdgToplevel::State::TiledBottom);
            break;
        default:
            break;
        }
    }

    return v;
}

} // namespace bl
