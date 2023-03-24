#ifndef _BL_DESKTOP_SURFACE_H
#define _BL_DESKTOP_SURFACE_H

#include <memory>

#include <blusher/wayland/xdg-surface.h>
#include <blusher/wayland/xdg-toplevel.h>
#include <blusher/surface.h>
#include <blusher/rect.h>

namespace bl {

class DesktopSurface : public Surface
{
public:
    enum class Role {
        Toplevel,
        Popup,
    };

public:
    DesktopSurface(DesktopSurface::Role role, DesktopSurface *parent = nullptr);

    DesktopSurface* parent() const;

    DesktopSurface::Role role() const;

    void close();

    /// Sets window geomtry hint for a compositor.
    void set_geometry_hint(const Rect& geometry);

    /// Move if role is toplevel. If the role is popup, do nothing.
    void toplevel_move();

    /// Resize if role is toplevel. If the role is popup, do nothing.
    void toplevel_resize(XdgToplevel::ResizeEdge edge);

    /// Maximize if role is toplevel. If the role is popup, do nothing.
    void toplevel_maximize();

    /// Restore from maximized toplevel. If the role is popup, do nothing.
    void toplevel_restore();

    void set_size(uint32_t width, uint32_t height);

private:
    std::shared_ptr<XdgSurface> _xdg_surface;
    std::shared_ptr<XdgToplevel> _xdg_toplevel;
    // std::shared_ptr<XdgPopup> _xdg_popup;

    DesktopSurface::Role _role;
    DesktopSurface *_parent;

    bool _toplevel_maximized;
};

} // namespace bl

#endif // _BL_DESKTOP_SURFACE_H
