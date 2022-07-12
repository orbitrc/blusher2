#ifndef _BL_SURFACE_H
#define _BL_SURFACE_H

#include <stdint.h>

#include <memory>

#include <blusher/resize-event.h>
#include <blusher/wayland/wl-surface.h>
#include <blusher/wayland/wl-subsurface.h>

#include "color.h"
#include "signal.h"
#include "pointer-event.h"

namespace bl {

class View;

class SurfaceImpl;

class Surface
{
    friend class SurfaceImpl;
public:
    enum class Type {
        Normal,
        Subsurface,
    };

    enum class State {
        /// State is normal.
        Normal = 0,
        /// Pointer hovered.
        Hover = 1,
        /// Pointer pressed and pointer is in the surface.
        Active = 2,
        /// Pointer pressed but pointer is out of the surface area.
        ActiveOut = 3,
        /// Keyboard focus.
        Focus = 4,
    };

public:
    Surface(Surface *parent = nullptr);

    /// Height of the surface.
    uint32_t height() const;

    void paint();

    Surface* parent();

    void place_above(Surface *surface);

    void place_below(Surface *surface);

    void set_geometry(double x, double y, double width, double height);

    void show();

    /// Returns type of this surface.
    Surface::Type type() const;

    /// Returns true if this surface is a toplevel.
    bool is_toplevel() const; // Deprecated.

    /// Returns true if this surface is a subsurface of other.
    bool is_subsurface() const;

    /// Width of the surface.
    uint32_t width() const;

    View* root_view();

    void update();

    //=================
    // Wayland Access
    //=================
    const WlSurface& wl_surface() const;

    std::shared_ptr<WlSubsurface> wl_subsurface();

public:
    Signal<> color_changed;
    Signal<> clicked;

protected:
    //===============
    // Events
    //===============
    virtual void pointer_enter_event(std::shared_ptr<PointerEvent> event);
    virtual void pointer_leave_event(std::shared_ptr<PointerEvent> event);
    virtual void pointer_press_event(std::shared_ptr<PointerEvent> event);
    virtual void pointer_release_event(std::shared_ptr<PointerEvent> event);
    virtual void resize_event(std::shared_ptr<ResizeEvent> event);

    //===============
    // Protected
    //===============

private:
    void on_clicked();
    void pointer_enter_handler();
    void pointer_leave_handler();
    void pointer_press_handler(uint32_t impl_button, double x, double y);
    void pointer_release_handler(uint32_t impl_button, double x, double y);
    void pointer_move_handler(uint32_t impl_button, double x, double y);
    void resize_handler(int32_t width, int32_t height,
            int32_t old_width, int32_t old_height);

private:
    SurfaceImpl *_impl;
    WlSurface _wl_surface;
    std::shared_ptr<WlSubsurface> _wl_subsurface;

    Surface *_parent;
    State _state;
    Type _type;

    View *_current_view;
};

} // namespace bl

#endif /* _BL_SURFACE_H */
