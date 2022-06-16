#include <stdio.h>
#include <string.h>
#include <stdio.h>

#include <blusher/wayland/wl-display.h>
#include <blusher/wayland/wl-compositor.h>
#include <blusher/wayland/wl-surface.h>
#include <blusher/wayland/wl-output.h>
#include <blusher/wayland/xdg-wm-base.h>
#include <blusher/wayland/xdg-toplevel.h>

bl::WlCompositor compositor;
bl::XdgWmBase xdg_wm_base(nullptr);

//======================
// XDG Shell Handlers
//======================

static void xdg_wm_base_ping_handler(void *data,
        struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    fprintf(stderr, "Ping!\n");
}

static const bl::XdgWmBase::Listener xdg_wm_base_listener =
    bl::XdgWmBase::Listener(
        xdg_wm_base_ping_handler
    );

//========================
// XDG Surface Handlers
//========================

static void xdg_surface_configure_handler(void *data,
        struct xdg_surface *xdg_surface, uint32_t serial)
{
    bl::XdgSurface::ack_configure(xdg_surface, serial);
}

static const bl::XdgSurface::Listener xdg_surface_listener =
    bl::XdgSurface::Listener(
        xdg_surface_configure_handler
    );

//=========================
// XDG Toplevel Handlers
//=========================

static void xdg_toplevel_configure_handler(void *data,
        struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height,
        struct wl_array *states)
{
}

static void xdg_toplevel_close_handler(void *data,
        struct xdg_toplevel *xdg_toplevel)
{
}

static const bl::XdgToplevel::Listener xdg_toplevel_listener =
    bl::XdgToplevel::Listener(
        xdg_toplevel_configure_handler,
        xdg_toplevel_close_handler
    );

//============================
// Global Registry Handlers
//============================
static void global_registry_handler(void *data, struct wl_registry *registry,
        uint32_t id, const char *interface, uint32_t version)
{
    if (strcmp(interface, "wl_compositor") == 0) {
        auto registry = bl::WlRegistry::instance();
        auto interface = bl::WlInterface<bl::WlInterfaceType::Compositor>();
        compositor = registry->bind(id,
            interface,
            version
        );
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        auto registry = bl::WlRegistry::instance();
        auto interface = bl::WlInterface<bl::WlInterfaceType::XdgWmBase>();
        xdg_wm_base = registry->bind(id,
            interface,
            1
        );
    }
}

static void global_registry_remove_handler(void *data,
        struct wl_registry *registry, uint32_t id)
{
}

static const bl::WlRegistry::Listener registry_listener =
    bl::WlRegistry::Listener(
        global_registry_handler,
        global_registry_remove_handler
    );

//=============
// Main
//=============

int main(int argc, char *argv[])
{
    bl::WlDisplay display = bl::WlDisplay::connect();

    bl::WlRegistry registry = display.get_registry();
    registry.add_listener(registry_listener);

    display.dispatch();
    display.roundtrip();
    //

    // Create a surface.
    if (bl::WlCompositor::instance() == nullptr) {
        fprintf(stderr, "WlCompositor singleton not ready.\n");
    }
    fprintf(stderr, "Before create surface. wl_compositor: %p\n",
        bl::WlCompositor::instance()->wl_compositor());
    bl::WlSurface surface = bl::WlCompositor::instance()->create_surface();
    fprintf(stderr, "Surface created!\n");

    fprintf(stderr, "xdg_wm_base: %p\n", xdg_wm_base.xdg_wm_base());
    xdg_wm_base.add_listener(xdg_wm_base_listener);
    fprintf(stderr, "xdg_wm_base listener added.\n");

    // Get XDG surface.
    bl::XdgSurface xdg_surface = xdg_wm_base.get_xdg_surface(surface);
    fprintf(stderr, "Got XDG surface.\n");
    xdg_surface.add_listener(xdg_surface_listener);

    // Get XDG toplevel.
    bl::XdgToplevel xdg_toplevel = xdg_surface.get_toplevel();
    xdg_toplevel.add_listener(xdg_toplevel_listener);

    // Commit before roundtrip.
    fprintf(stderr, "Before commit.\n");
    surface.commit();

    display.roundtrip();

    // Commit the surface.
    surface.commit();

    while (true) {
        display.dispatch();
    }

    return 0;
}
