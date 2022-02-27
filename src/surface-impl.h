#ifndef SURFACEIMPL_H
#define SURFACEIMPL_H

// C++
#include <functional>

// Qt
#include <QWindow>

// Wayland
#include <wayland-client.h>

// Blusher
#include <blusher/color.h>

namespace bl {

class Surface;

static const int SurfaceImplButtonNone = 0;
static const int SurfaceImplButtonLeft = 1;
static const int SurfaceImplButtonRight = 2;
static const int SurfaceImplButtonMiddle = 3;

class SurfaceImpl : public QWindow
{
    Q_OBJECT

public:
    SurfaceImpl(QWindow *parent = nullptr);

    double x() const;
    double y() const;
    double width() const;
    double height() const;

    void setX(double x);
    void setY(double y);
    void setWidth(double width);
    void setHeight(double height);

    void paint();

    void setColor(const Color& color);

    void setBlSurface(Surface *blSurface);
    void setPointerEnterHandler(void (Surface::*)());
    void setPointerLeaveHandler(void (Surface::*)());
    void setPointerPressHandler(void (Surface::*)(int button, double x, double y));
    void setPointerReleaseHandler(void (Surface::*)(int button, double x, double y));

signals:
    void implXChanged(double x);
    void implYChanged(double y);
    void implWidthChanged(double width);
    void implHeightChanged(double height);

private slots:
    void onImplXChanged(double x);
    void onImplYChanged(double y);
    void onImplWidthChanged(double width);
    void onImplHeightChanged(double height);

protected:
    bool event(QEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    double m_x;
    double m_y;
    double m_width;
    double m_height;

    QColor m_color;

    QBackingStore *m_backingStore;

    Surface *m_blSurface;
    void (Surface::*m_pointerEnterHandler)();
    void (Surface::*m_pointerLeaveHandler)();
    void (Surface::*m_pointerPressHandler)(int button, double x, double y);
    void (Surface::*m_pointerReleaseHandler)(int button, double x, double y);

    //=================
    // Wayland client
    //=================
    struct wl_surface *_surface;
    struct wl_subsurface *_subsurface;
    struct wl_callback *_frame_callback;
    struct wl_buffer *_buffer;
};

} // namespace bl

#endif // SURFACEIMPL_H
