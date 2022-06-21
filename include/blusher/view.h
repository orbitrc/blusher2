#ifndef _BL_VIEW_H
#define _BL_VIEW_H

// Primer
#include <primer/vector.h>

namespace bl {

class ViewImpl;

class View
{
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

private:
    ViewImpl *_impl;

    View *_parent;
    pr::Vector<View*> _children;
};

} // namespace bl

#endif // _BL_VIEW_H
