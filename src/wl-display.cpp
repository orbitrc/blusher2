#include <blusher/wayland/wl-display.h>

// C
#include <assert.h>
#include <stdio.h>

namespace bl {

WlDisplay *wl_display_singleton = nullptr;

WlDisplay::~WlDisplay()
{
    fprintf(stderr, "WlDisplay::~WlDisplay()\n");
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

struct wl_display* WlDisplay::c_ptr()
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
    fprintf(stderr, "[LOG] WlDisplay::dispatch()\n");
    wl_display_dispatch(this->_wl_display);
    fprintf(stderr, "[LOG] WlDisplay::dispatch() done.\n");
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
