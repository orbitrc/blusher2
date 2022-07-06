#ifndef _BL_SURFACE_H
#define _BL_SURFACE_H

#include <stdint.h>

#include <memory>

#include <blusher/wayland/xdg-toplevel.h>
#include <blusher/resize-event.h>

#include "color.h"
#include "signal.h"
#include "pointer-event.h"

namespace bl {

class View;

class SurfaceImpl;

class Surface
{
public:
    enum class Type {
        Normal,
        Toplevel,
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
    bool is_toplevel() const;

    /// Width of the surface.
    uint32_t width() const;

    /// This is implementation specific method. DO NOT USE THIS in an application.
    void move_if_window();
    /// This is implementation specific method. DO NOT USE THIS in an application.
    void resize_if_window(XdgToplevel::ResizeEdge edge);
    /// This is implementation specific method. DO NOT USE THIS in an application.
    void maximize_if_window();
    /// This is implementation specific method. DO NOT USE THIS in an application.
    void restore_if_window();

    View* root_view();

    void update();

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
    virtual void pointer_move_event(std::shared_ptr<PointerEvent> event);
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

    Surface *_parent;
    State _state;
    Type _type;

    View *_current_view;
};

} // namespace bl

#endif /* _BL_SURFACE_H */
