#ifndef _BL_SURFACE_H
#define _BL_SURFACE_H

#include <stdint.h>

#include <memory>

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/resize-event.h>
#include <blusher/pointer-state.h>
#include <blusher/wayland/wl-surface.h>
#include <blusher/wayland/wl-subsurface.h>

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

    /// Deprecated: Subsurface.
    void place_above(Surface *surface);

    /// Deprecated: Subsurface.
    void place_below(Surface *surface);

    void set_geometry(double x, double y, double width, double height);

    void set_size(uint32_t width, uint32_t height);

    void show();

    void hide();

    bool visible() const;

    /// Returns type of this surface.
    Surface::Type type() const;

    /// Returns true if this surface is a toplevel.
    bool is_toplevel() const; // Deprecated.

    /// Returns true if this surface is a subsurface of other.
    bool is_subsurface() const;

    /// Width of the surface.
    uint32_t width() const;

    View* root_view();

    void process_pointer();

    void request_update();

    void update();

    pr::String debug_id() const;

    void set_debug_id(const pr::String& id);

    //=================
    // Input States
    //=================
    std::shared_ptr<PointerState> pointer_state();

    //=================
    // Wayland Access
    //=================
    std::shared_ptr<WlSurface> wl_surface() const;

    std::shared_ptr<WlSubsurface> wl_subsurface();

public:
    /// Pointer entered view. For Surface and View internal only.
    View* entered_view() const;

    /// Set pointer entered view. For Surface and View internal only.
    void set_entered_view(View *view);

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
    void pointer_enter_handler();
    void pointer_leave_handler();
    void pointer_press_handler(uint32_t impl_button, double x, double y);
    void pointer_release_handler(uint32_t impl_button, double x, double y);
    void pointer_move_handler(uint32_t impl_button, double x, double y);
    void resize_handler(int32_t width, int32_t height,
            int32_t old_width, int32_t old_height);

private:
    SurfaceImpl *_impl;
    std::shared_ptr<WlSubsurface> _wl_subsurface;

    Surface *_parent;
    State _state;
    Type _type;

    View *_current_view;

    std::shared_ptr<PointerState> _pointer_state;

    bool _update_requested;
    pr::String _debug_id;
};

} // namespace bl

#endif /* _BL_SURFACE_H */
