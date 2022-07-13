#include <blusher/wayland/wl-output.h>

namespace bl {

//====================
// WlOutput::Listener
//====================

WlOutput::Listener::Listener()
{
    struct wl_output_listener *listener = new struct wl_output_listener();
    listener->geometry = nullptr;
    listener->mode = nullptr;
    listener->done = nullptr;
    listener->scale = nullptr;

    this->_wl_output_listener = listener;
}

WlOutput::Listener::Listener(
        WlOutput::Listener::GeometryHandler geometry,
        WlOutput::Listener::ModeHandler mode,
        WlOutput::Listener::DoneHandler done,
        WlOutput::Listener::ScaleHandler scale)
{
    struct wl_output_listener *listener = new struct wl_output_listener();
    listener->geometry = geometry;
    listener->mode = mode;
    listener->done = done;
    listener->scale = scale;

    this->_wl_output_listener = listener;
}

WlOutput::Listener::~Listener()
{
    // TODO.
}

const struct wl_output_listener* WlOutput::Listener::wl_output_listener() const
{
    return this->_wl_output_listener;
}

//============
// WlOutput
//============

WlOutput::WlOutput(struct wl_output *output)
{
    this->_wl_output = output;
}

WlOutput::~WlOutput()
{
    // TODO
}

struct wl_output* WlOutput::c_ptr()
{
    return this->_wl_output;
}

void WlOutput::add_listener(const WlOutput::Listener& listener)
{
    this->_listener = listener;
    wl_output_add_listener(this->_wl_output,
        listener.wl_output_listener(), NULL);
}

} // namespace bl
