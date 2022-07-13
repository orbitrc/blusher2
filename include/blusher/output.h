#ifndef _BL_OUTPUT_H
#define _BL_OUTPUT_H

// C
#include <stdint.h>

// Wayland
#include <wayland-client.h>

// Primer
#include <primer/string.h>

namespace bl {

class Output
{
    friend class Application;
private:
    Output(struct wl_output *wl_output);

    void set_name(const pr::String& name);

    void set_scale(uint32_t scale);

public:
    const pr::String name() const;

    uint32_t scale() const;

    uint32_t width() const;

    uint32_t height() const;

private:
    struct wl_output *_wl_output;
    pr::String _name;
    uint32_t _scale;
    uint32_t _width;
    uint32_t _height;
};

} // namespace bl

#endif // _BL_OUTPUT_H
