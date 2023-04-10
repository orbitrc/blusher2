#ifndef VIEW_IMPL_H
#define VIEW_IMPL_H

// C++
#include <memory.h>

// Blusher
#include <blusher/view.h>
#include <blusher/color.h>
#include <blusher/image.h>

namespace bl {

class SurfaceImpl;

class View;

class ViewImpl
{
    friend View;
public:
    ViewImpl();
    ~ViewImpl();

    double x() const;
    double y() const;
    double width() const;
    double height() const;

    void setX(double x);
    void setY(double y);
    void setPosition(double x, double y);
    void setWidth(double width);
    void setHeight(double height);
    void setSize(double width, double height);

    Rect content_geometry() const;
    /// If content policy is Fit, do nothing.
    void set_content_geometry(const Rect& geometry);

    View::FillType fill_type() const;
    void set_fill_type(View::FillType fill_type);

    View::ContentPolicy content_policy() const;
    void set_content_policy(View::ContentPolicy policy);

    void fill(const Color& color);

    Color color() const;

    void drawImage(uint64_t x, uint64_t y, const Image& image);

    const Image& image() const;

    void appendChild(View *view);

    void process_pointer_press_event(std::shared_ptr<PointerEvent>);
    void process_pointer_move_event(std::shared_ptr<PointerEvent>);

private:
    /// Resize composed image and compose it's children views.
    void update();

private:
    double m_x;
    double m_y;
    double m_width;
    double m_height;

    Rect _content_geometry;

    View *_view;

    View::FillType _fill_type;
    View::ContentPolicy _content_policy;

    Color _color;

    Image *m_image;
    Image *m_composedImage;
};

} // namespace bl

#endif // VIEW_IMPL_H
