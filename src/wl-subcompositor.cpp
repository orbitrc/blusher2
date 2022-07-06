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

std::shared_ptr<WlSubsurface> WlSubcompositor::get_subsurface(
        const WlSurface& surface, const WlSurface& parent)
{
    struct wl_subsurface *subsurface = wl_subcompositor_get_subsurface(
        this->_wl_subcompositor,
        const_cast<WlSurface&>(surface).wl_surface(),
        const_cast<WlSurface&>(parent).wl_surface());

    return std::shared_ptr<WlSubsurface>(new WlSubsurface(subsurface));
}

} // namespace bl
