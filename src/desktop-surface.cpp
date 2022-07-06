#include <blusher/desktop-surface.h>

namespace bl {

DesktopSurface::DesktopSurface(DesktopSurface::Role role,
        DesktopSurface *parent)
{
    // Initialize.
    this->_xdg_surface = nullptr;
    this->_xdg_toplevel = nullptr;

    this->_surface = nullptr;
    this->_role = role;
    this->_parent = parent;

    if (parent == nullptr) {
        //
    } else {
        //
    }
}

} // namespace bl
