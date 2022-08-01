#include <blusher/wayland/wl-seat.h>

namespace bl {

//====================
// WlSeat::Listener
//====================
WlSeat::Listener::Listener()
{
    struct wl_seat_listener *listener = new struct wl_seat_listener();
    listener->capabilities = nullptr;
    listener->name = nullptr;

    this->_wl_seat_listener = listener;
}

WlSeat::Listener::Listener(WlSeat::Listener::CapabilitiesHandler capabilities,
        WlSeat::Listener::NameHandler name)
{
    struct wl_seat_listener *listener = new struct wl_seat_listener();
    listener->capabilities = capabilities;
    listener->name = name;

    this->_wl_seat_listener = listener;
}

WlSeat::Listener::~Listener()
{
    delete this->_wl_seat_listener;
}

const wl_seat_listener* WlSeat::Listener::wl_seat_listener() const
{
    return this->_wl_seat_listener;
}

//================
// WlSeat
//================

WlSeat::WlSeat(struct wl_seat *wl_seat)
{
    this->_wl_seat = wl_seat;
}

WlSeat::~WlSeat()
{
    wl_seat_destroy(this->_wl_seat);
//    wl_seat_release(this->_wl_seat);
}

struct wl_seat* WlSeat::c_ptr()
{
    return this->_wl_seat;
}

void WlSeat::add_listener(const WlSeat::Listener &listener)
{
    this->_listener = listener;
    wl_seat_add_listener(this->_wl_seat,
        listener.wl_seat_listener(), NULL);
}

void WlSeat::add_listener(const WlSeat::Listener &listener, void *data)
{
    this->_listener = listener;
    wl_seat_add_listener(this->_wl_seat,
        listener.wl_seat_listener(), data);
}

std::shared_ptr<WlKeyboard> WlSeat::get_keyboard()
{
    struct wl_keyboard *wl_keyboard = wl_seat_get_keyboard(this->_wl_seat);

    return std::make_shared<WlKeyboard>(wl_keyboard);
}

std::shared_ptr<WlPointer> WlSeat::get_pointer()
{
    struct wl_pointer *wl_pointer = wl_seat_get_pointer(this->_wl_seat);

    return std::make_shared<WlPointer>(wl_pointer);
}

} // namespace bl
