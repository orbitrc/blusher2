#ifndef _BL_WL_OUTPUT_H
#define _BL_WL_OUTPUT_H

#include <wayland-client.h>

namespace bl {

class WlOutput
{
public:
    class Listener
    {
    public:
        using GeometryHandler = void (void *data,
            struct wl_output *wl_output,
            int32_t x,
            int32_t y,
            int32_t physical_width,
            int32_t physical_height,
            int32_t subpixel,
            const char *make,
            const char *model,
            int32_t transform);
        using ModeHandler = void (void *data,
            struct wl_output *wl_output,
            uint32_t flags,
            int32_t width,
            int32_t height,
            int32_t refresh);
        using DoneHandler = void (void *data,
            struct wl_output *wl_output);
        using ScaleHandler = void (void *data,
            struct wl_output *wl_output,
            int32_t scale);

    public:
        Listener();

        Listener(GeometryHandler geometry,
                ModeHandler mode,
                DoneHandler done,
                ScaleHandler scale);

        ~Listener();

        const struct wl_output_listener* wl_output_listener() const;

    private:
        struct wl_output_listener *_wl_output_listener;
    };

public:
    WlOutput(struct wl_output *output);

    ~WlOutput();

    struct wl_output* c_ptr();

    void add_listener(const WlOutput::Listener& listener);

private:
    struct wl_output *_wl_output;
    WlOutput::Listener _listener;
};

} // namespace bl

#endif // _BL_WL_OUTPUT_H
