#ifndef _BL_SURFACE_H
#define _BL_SURFACE_H

#include <memory>

#include "color.h"
#include "signal.h"
#include "pointer-event.h"

namespace bl {

class View;

class SurfaceImpl;

class Surface
{
public:
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
    double height() const;

    void paint();

    Surface* parent();

    void place_above(Surface *surface);

    void place_below(Surface *surface);

    void set_color(const Color& color);

    void set_geometry(double x, double y, double width, double height);

    void show();

    /// Returns true if this surface is a toplevel.
    bool toplevel() const;

    /// Width of the surface.
    double width() const;

    /// This is implementation specific method. DO NOT USE THIS in an application.
    void move_if_window();

    std::shared_ptr<View> root_view();

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

    //===============
    // Protected
    //===============

private:
    void on_clicked();
    void pointer_enter_handler();
    void pointer_leave_handler();
    void pointer_press_handler(uint32_t impl_button, double x, double y);
    void pointer_release_handler(uint32_t impl_button, double x, double y);

private:
    SurfaceImpl *_impl;

    Surface *_parent;
    Color _color;
    State _state;
};

} // namespace bl

#endif /* _BL_SURFACE_H */
