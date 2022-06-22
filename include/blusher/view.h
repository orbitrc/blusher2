#ifndef _BL_VIEW_H
#define _BL_VIEW_H

// Primer
#include <primer/vector.h>

// Blusher
#include <blusher/color.h>

namespace bl {

class Surface;

class SurfaceImpl;

class ViewImpl;

class View
{
    friend SurfaceImpl;
    friend ViewImpl;
public:
    View(View *parent = nullptr);

    double x() const;
    double y() const;
    double width() const;
    double height() const;

    void set_x(double x);
    void set_y(double y);
    void set_width(double width);
    void set_height(double height);

    Color color() const;
    void set_color(const Color& color);

    /// DO NOT USE THIS MANUALLY YET!
    void set_surface(Surface *surface);

    void update();

private:
    ViewImpl *_impl;

    View *_parent;
    Surface *_surface;
    pr::Vector<View*> _children;
};

} // namespace bl

#endif // _BL_VIEW_H
