#ifndef _BL_DESKTOP_SURFACE_H
#define _BL_DESKTOP_SURFACE_H

#include <memory>

#include <blusher/wayland/xdg-surface.h>
#include <blusher/wayland/xdg-toplevel.h>
#include <blusher/surface.h>

namespace bl {

class DesktopSurface : public Surface
{
public:
    enum class Role {
        Toplevel,
        Popup,
    };

public:
    DesktopSurface(Surface *surface);

    DesktopSurface(DesktopSurface *parent);

private:
    std::shared_ptr<XdgSurface> _xdg_surface;
    std::shared_ptr<XdgToplevel> _xdg_toplevel;
    // std::shared_ptr<XdgPopup> _xdg_popup;

    DesktopSurface::Type _type;
};

} // namespace bl

#endif // _BL_DESKTOP_SURFACE_H
