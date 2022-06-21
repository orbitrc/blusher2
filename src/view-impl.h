#ifndef VIEW_IMPL_H
#define VIEW_IMPL_H

// Qt
#include <QObject>

// Blusher
#include <blusher/color.h>
#include <blusher/image.h>

namespace bl {

class ViewImpl : public QObject
{
    Q_OBJECT
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

private:
    double m_x;
    double m_y;
    double m_width;
    double m_height;

    Color m_color;

    Image *m_image;
};

} // namespace bl

#endif // VIEW_IMPL_H
