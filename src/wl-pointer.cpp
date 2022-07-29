#include <blusher/wayland/wl-pointer.h>

namespace bl {

//====================
// WlPointer::Listener
//====================
WlPointer::Listener::Listener()
{
    struct wl_pointer_listener *listener = new struct wl_pointer_listener();
    listener->enter = nullptr;
    listener->leave = nullptr;
    listener->motion = nullptr;
    listener->button = nullptr;
    listener->axis = nullptr;
    listener->frame = nullptr;
    listener->axis_source = nullptr;
    listener->axis_stop = nullptr;
    listener->axis_discrete = nullptr;

    this->_wl_pointer_listener = listener;
}

WlPointer::Listener::Listener(WlPointer::Listener::EnterHandler enter,
        WlPointer::Listener::LeaveHandler leave,
        WlPointer::Listener::MotionHandler motion,
        WlPointer::Listener::ButtonHandler button,
        WlPointer::Listener::AxisHandler axis,
        WlPointer::Listener::FrameHandler frame,
        WlPointer::Listener::AxisSourceHandler axis_source,
        WlPointer::Listener::AxisStopHandler axis_stop,
        WlPointer::Listener::AxisDiscreteHandler axis_discrete)
{
    struct wl_pointer_listener *listener = new struct wl_pointer_listener();
    listener->enter = enter;
    listener->leave = leave;
    listener->motion = motion;
    listener->button = button;
    listener->axis = axis;
    listener->frame = frame;
    listener->axis_source = axis_source;
    listener->axis_stop = axis_stop;
    listener->axis_discrete = axis_discrete;

    this->_wl_pointer_listener = listener;
}

WlPointer::Listener::~Listener()
{
    delete this->_wl_pointer_listener;
}

const wl_pointer_listener* WlPointer::Listener::wl_pointer_listener() const
{
    return this->_wl_pointer_listener;
}

//================
// WlPointer
//================

WlPointer::WlPointer(struct wl_pointer *wl_pointer)
{
    this->_wl_pointer = wl_pointer;
}

WlPointer::~WlPointer()
{
//    wl_pointer_destroy(this->_wl_pointer);
//    wl_pointer_release(this->_wl_pointer);
}

struct wl_pointer* WlPointer::c_ptr()
{
    return this->_wl_pointer;
}

void WlPointer::add_listener(const WlPointer::Listener &listener)
{
    this->_listener = listener;
    wl_pointer_add_listener(this->_wl_pointer,
        listener.wl_pointer_listener(), NULL);
}

void WlPointer::add_listener(const WlPointer::Listener& listener, void *data)
{
    this->_listener = listener;
    wl_pointer_add_listener(this->_wl_pointer,
        listener.wl_pointer_listener(), data);
}

} // namespace bl
