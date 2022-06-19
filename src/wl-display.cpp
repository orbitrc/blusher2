#include <blusher/wayland/wl-display.h>

// C
#include <assert.h>

namespace bl {

WlDisplay *wl_display_singleton = nullptr;

WlDisplay::~WlDisplay()
{
    // TODO: Close connection.

    wl_display_singleton = nullptr;
}

WlDisplay WlDisplay::connect()
{
    assert(wl_display_singleton == nullptr);

    struct wl_display *display = wl_display_connect(NULL);
    WlDisplay ret;

    ret._wl_display = display;

    wl_display_singleton = &ret;

    return ret;
}

WlDisplay WlDisplay::connect(const pr::String &name)
{
    assert(wl_display_singleton == nullptr);

    struct wl_display *display = wl_display_connect(name.c_str());
    WlDisplay ret;

    ret._wl_display = display;

    wl_display_singleton = &ret;

    return ret;
}

struct wl_display* WlDisplay::wl_display()
{
    return this->_wl_display;
}

WlRegistry WlDisplay::get_registry()
{
    struct wl_registry *reg = wl_display_get_registry(this->_wl_display);
    WlRegistry registry(reg);

    return registry;
}

void WlDisplay::dispatch()
{
    wl_display_dispatch(this->_wl_display);
}

void WlDisplay::roundtrip()
{
    wl_display_roundtrip(this->_wl_display);
}

WlDisplay* WlDisplay::instance()
{
    return wl_display_singleton;
}

} // namespace bl
