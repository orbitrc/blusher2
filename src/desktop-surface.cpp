#include <blusher/desktop-surface.h>

// C
#include <stdio.h>

// Blusher
#include <blusher/application.h>

#include "application-impl.h"

//=========
// XDG
//=========

// XDG surface
static void xdg_surface_configure_handler(void *data,
        struct xdg_surface *xdg_surface, uint32_t serial)
{
    (void)data;
    bl::XdgSurface::ack_configure(xdg_surface, serial);
}

static const bl::XdgSurface::Listener xdg_surface_listener = (
    xdg_surface_configure_handler
);

// XDG toplevel
static void xdg_toplevel_configure_handler(void *data,
        struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height,
        struct wl_array *states)
{
    bl::DesktopSurface *desktop_surface = static_cast<bl::DesktopSurface*>(data);

    // Check if same.
    // assert(xdg_toplevel == surface_impl->_xdg_toplevel);

    (void)xdg_toplevel;
    /*
    fprintf(stderr, "[LOG] xdg_toplevel_configure_handler - size: %dx%d\n",
        width, height);
    */
    // TODO: implement

    pr::Vector<bl::XdgToplevel::State> states_v =
        bl::XdgToplevel::states_to_vector(states);

    // State is resizing.
    if (states_v.index(bl::XdgToplevel::State::Resizing) != std::nullopt) {
        // Sometimes width and height could be zero.
        if (width == 0 && height == 0) {
            return;
        }
        if (desktop_surface->width() != width ||
                desktop_surface->height() != height) {
            // fprintf(stderr, "Resizing...\n");
            desktop_surface->set_geometry(0, 0, width, height);
            // surface->update();
        }
    }
}

static void xdg_toplevel_close_handler(void *data,
        struct xdg_toplevel *xdg_toplevel)
{
    fprintf(stderr, "Closing...\n");
}

static const bl::XdgToplevel::Listener xdg_toplevel_listener =
    bl::XdgToplevel::Listener(
        xdg_toplevel_configure_handler,
        xdg_toplevel_close_handler
    );

namespace bl {

DesktopSurface::DesktopSurface(DesktopSurface::Role role,
        DesktopSurface *parent)
{
    // Initialize.
    this->_xdg_surface = nullptr;
    this->_xdg_toplevel = nullptr;

    this->_role = role;
    this->_parent = parent;

    //=============
    // XDG shell
    //=============
    this->_xdg_surface = app->xdg_wm_base()->get_xdg_surface(
        const_cast<WlSurface&>(Surface::wl_surface()));
    this->_xdg_surface->add_listener(xdg_surface_listener);

    if (this->_role == DesktopSurface::Role::Toplevel) {
        this->_xdg_toplevel = this->_xdg_surface->get_toplevel();
        this->_xdg_toplevel->add_listener(
            xdg_toplevel_listener,
            static_cast<void*>(this)
        );
    } else {
        // TODO: If role is Popup.
    }

    // Signal that the surface is ready to be configured.
    const_cast<WlSurface&>(Surface::wl_surface()).commit();
    // Wait for the surface to be configured.
    app_impl->display()->roundtrip();

    this->_xdg_surface->set_window_geometry(0, 0, 200, 200);

    if (parent == nullptr) {
        //
    } else {
        //
    }
}

DesktopSurface* DesktopSurface::parent() const
{
    return this->_parent;
}

DesktopSurface::Role DesktopSurface::role() const
{
    return this->_role;
}

void DesktopSurface::toplevel_move()
{
    if (this->_role == DesktopSurface::Role::Toplevel) {
        this->_xdg_toplevel->move(*app_impl->seat(),
            app_impl->pointer_state.serial);
        // Manually release button pressed state because after
        // xdg_toplevel_move() call, xdg_toplevel.button event not called.
        app_impl->pointer_state.button = 0;
    }
}

void DesktopSurface::toplevel_resize(XdgToplevel::ResizeEdge edge)
{
    if (this->_role == DesktopSurface::Role::Toplevel) {
        this->_xdg_toplevel->resize(*app_impl->seat(),
            app_impl->pointer_state.serial,
            edge);
        // Manually release button pressed state because after
        // xdg_toplevel_move() call, xdg_toplevel.button event not called.
        app_impl->pointer_state.button = 0;
    }
}

} // namespace bl
