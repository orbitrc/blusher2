#ifndef _BL_WL_POINTER_H
#define _BL_WL_POINTER_H

#include <wayland-client.h>

namespace bl {

class WlPointer
{
public:
    //====================
    // WlPointer::Listener
    //====================
    class Listener
    {
    public:
        using EnterHandler = void (void *data,
            struct wl_pointer *pointer,
            uint32_t serial,
            struct wl_surface *wl_surface,
            wl_fixed_t surface_x,
            wl_fixed_t surface_y);
        using LeaveHandler = void (void *data,
            struct wl_pointer *pointer,
            uint32_t serial,
            struct wl_surface *wl_surface);
        using MotionHandler = void (void *data,
            struct wl_pointer *pointer,
            uint32_t time,
            wl_fixed_t sx,
            wl_fixed_t sy);
        using ButtonHandler = void (void *data,
            struct wl_pointer *pointer,
            uint32_t serial,
            uint32_t time,
            uint32_t button,
            uint32_t state);
        using AxisHandler = void (void *data,
            struct wl_pointer *wl_pointer,
            uint32_t time,
            uint32_t axis,
            wl_fixed_t value);
        using FrameHandler = void (void *data,
            struct wl_pointer *wl_pointer);
        using AxisSourceHandler = void (void *data,
            struct wl_pointer *wl_pointer,
            uint32_t axis_source);
        using AxisStopHandler = void (void *data,
            struct wl_pointer *wl_pointer,
            uint32_t time,
            uint32_t axis);
        using AxisDiscreteHandler = void (void *data,
            struct wl_pointer *wl_pointer,
            uint32_t axis,
            int32_t discrete);

    public:
        Listener();

        Listener(EnterHandler enter,
                LeaveHandler leave,
                MotionHandler motion,
                ButtonHandler button,
                AxisHandler axis,
                FrameHandler frame,
                AxisSourceHandler axis_source,
                AxisStopHandler axis_stop,
                AxisDiscreteHandler axis_discrete);

        ~Listener();

        const struct wl_pointer_listener* wl_pointer_listener() const;

    private:
        struct wl_pointer_listener *_wl_pointer_listener;
    };

public:
    WlPointer(struct wl_pointer *wl_pointer);

    ~WlPointer();

    struct wl_pointer* c_ptr();

    void add_listener(const WlPointer::Listener& listener);

private:
    struct wl_pointer *_wl_pointer;
    WlPointer::Listener _listener;
};

} // namespace bl

#endif // _BL_WL_POINTER_H
