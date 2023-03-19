#ifndef _BL_VIEW_H
#define _BL_VIEW_H

// C++
#include <memory>

// Primer
#include <primer/vector.h>

// Blusher
#include <blusher/color.h>
#include <blusher/point.h>
#include <blusher/size.h>
#include <blusher/rect.h>
#include <blusher/image.h>
#include <blusher/pointer-event.h>
#include <blusher/update-event.h>

namespace bl {

class Surface;

class SurfaceImpl;

class ViewImpl;

class EventDispatcher;

class View
{
    friend Surface;
    friend SurfaceImpl;
    friend ViewImpl;
    friend EventDispatcher;
public:
    enum class FillType {
        Color       = 0,
        Image       = 1,
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
    View(View *parent = nullptr);

    double x() const;
    double y() const;
    double width() const;
    double height() const;

    void set_x(double x);
    void set_y(double y);
    void set_position(const Point& pos);
    void set_width(double width);
    void set_height(double height);
    void set_size(const Size& size);

    FillType fill_type() const;
    void set_fill_type(FillType fill_type);

    Rect geometry() const;

    View::State state() const;

    void fill(const Color& color);

    /// Color of this view. Only valid when FillType::Color.
    Color color() const;

    void draw_image(const Point& pos, const Image& image);

    /// Get the underlying image of this view.
    /// Only Valid when FillType::Image.
    const Image& image() const;

    pr::Vector<View*> children() const;

    View* child_at(const Point& pos);

    /// Get surface of this view.
    Surface* surface();

    /// DO NOT USE THIS MANUALLY YET!
    void set_surface(Surface *surface);

    void update();

    virtual void paint();

    pr::String debug_id() const;

    void set_debug_id(const pr::String& debug_id);

protected:
    //===============
    // Events
    //===============
    virtual void pointer_enter_event(std::shared_ptr<PointerEvent> event);
    virtual void pointer_leave_event(std::shared_ptr<PointerEvent> event);
    virtual void pointer_press_event(std::shared_ptr<PointerEvent> event);
    virtual void pointer_release_event(std::shared_ptr<PointerEvent> event);
    virtual void pointer_click_event(std::shared_ptr<PointerEvent> event);
    virtual void pointer_double_click_event(std::shared_ptr<PointerEvent> event);
    virtual void pointer_move_event(std::shared_ptr<PointerEvent> event);
    virtual void update_event(std::shared_ptr<UpdateEvent> event);

private:
    ViewImpl *_impl;

    View *_parent;
    Surface *_surface;
    pr::Vector<View*> _children;
    View::State _state;
    /// If manually called paint method, then set this true. Otherwise set this
    /// false so it will be painted in next update.
    bool _painted;
    pr::String _debug_id;
};

} // namespace bl

#endif // _BL_VIEW_H
