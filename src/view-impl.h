#ifndef VIEW_IMPL_H
#define VIEW_IMPL_H

// Qt
#include <QObject>

// Blusher
#include <blusher/color.h>
#include <blusher/image.h>

namespace bl {

class SurfaceImpl;

class View;

class ViewImpl : public QObject
{
    Q_OBJECT
    friend SurfaceImpl;
    friend View;
public:
    ViewImpl(QObject *parent = nullptr);
    ~ViewImpl();

    double x() const;
    double y() const;
    double width() const;
    double height() const;

    void setX(double x);
    void setY(double y);
    void setWidth(double width);
    void setHeight(double height);

    Color color() const;
    void setColor(const Color& color);

    void appendChild(View *view);

signals:
    void colorChanged();

private slots:
    /// Resize composed image and compose it's children views.
    void update();

private:
    double m_x;
    double m_y;
    double m_width;
    double m_height;

    View *m_view;

    Color m_color;

    Image *m_image;
    Image *m_composedImage;
};

} // namespace bl

#endif // VIEW_IMPL_H