#include <blusher/wayland/wl-output.h>

namespace bl {

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

} // namespace bl
