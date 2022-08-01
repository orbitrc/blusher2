#ifndef _BL_WL_SEAT_H
#define _BL_WL_SEAT_H

// C++
#include <memory>

// Wayland
#include <wayland-client.h>

// Blusher
#include <blusher/wayland/wl-keyboard.h>
#include <blusher/wayland/wl-pointer.h>

namespace bl {

class WlSeat
{
public:
    //====================
    // WlSeat::Listener
    //====================
    class Listener
    {
    public:
        using CapabilitiesHandler = void (void *data,
            struct wl_seat *seat, uint32_t caps);
        using NameHandler = void (void *data,
            struct wl_seat *seat, const char *name);

    public:
        Listener();

        Listener(CapabilitiesHandler capabilities, NameHandler name);

        ~Listener();

        const struct wl_seat_listener* wl_seat_listener() const;

    private:
        struct wl_seat_listener *_wl_seat_listener;
    };

    //======================
    // WlSeat::Capability
    //======================
    enum class Capability {
        Pointer,
        Keyboard,
        Touch,
    };

public:
    WlSeat(struct wl_seat *wl_seat);

    ~WlSeat();

    struct wl_seat* c_ptr();

    void add_listener(const WlSeat::Listener& listener);

    void add_listener(const WlSeat::Listener& listener, void *data);

    std::shared_ptr<WlKeyboard> get_keyboard();

    std::shared_ptr<WlPointer> get_pointer();

private:
    struct wl_seat *_wl_seat;
    WlSeat::Listener _listener;
};

} // namespace bl

#endif // _BL_WL_SEAT_H
