#include <blusher/wayland/wl-subcompositor.h>

namespace bl {

WlSubcompositor::WlSubcompositor(struct wl_subcompositor *subcompositor)
{
    this->_wl_subcompositor = subcompositor;
}

WlSubcompositor::~WlSubcompositor()
{
    // TODO
}

struct wl_subcompositor* WlSubcompositor::wl_subcompositor()
{
    return this->_wl_subcompositor;
}

} // namespace bl
