#include <blusher/wayland/wl-output.h>

namespace bl {

WlOutput::WlOutput(struct wl_output *output)
{
    this->_wl_output = output;
}

} // namespace bl
