#ifndef _BL_WL_DISPLAY_H
#define _BL_WL_DISPLAY_H

// Wayland
#include <wayland-client.h>

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/wl-registry.h>

namespace bl {

extern class WlDisplay *wl_display_singleton;

/// The core global object. This is a special singleton object. It is used
/// for internal Wayland protocol features.
class WlDisplay
{
public:
    ~WlDisplay();

    static WlDisplay connect();

    static WlDisplay connect(const pr::String& name);

    struct wl_display* wl_display();

    WlRegistry get_registry();

    void dispatch();

    void roundtrip();

    /// Get singleton instance.
    static WlDisplay* instance();

private:
    struct wl_display *_wl_display;
};

} // namespace bl

#endif // _BL_WL_DISPLAY_H
