#ifndef SURFACEIMPL_H
#define SURFACEIMPL_H

// C
#include <stdint.h>

// C++
#include <functional>

// Qt
#include <QObject>
#include <QExposeEvent>
#include <QMouseEvent>

// Wayland
#include <wayland-client.h>

// Blusher
#include <blusher/color.h>

// Wayland protocols
#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

class Surface;

static const int SurfaceImplButtonNone = 0;
static const int SurfaceImplButtonLeft = 1;
static const int SurfaceImplButtonRight = 2;
static const int SurfaceImplButtonMiddle = 3;

class SurfaceImpl : public QObject
{
    Q_OBJECT

public:
    SurfaceImpl(QObject *parent = nullptr);
    ~SurfaceImpl();

    double x() const;
    double y() const;
    double width() const;
    double height() const;

    void setX(double x);
    void setY(double y);
    void setWidth(double width);
    void setHeight(double height);
    void setSize(double width, double height);

    void paint();

    void show();

    void setColor(const Color& color);

    bool toplevel() const;

    void setBlSurface(Surface *blSurface);
    void setPointerEnterHandler(void (Surface::*)());
    void setPointerLeaveHandler(void (Surface::*)());
    void setPointerPressHandler(void (Surface::*)(int button, double x, double y));
    void setPointerReleaseHandler(void (Surface::*)(int button, double x, double y));

public:
    //==================
    // Wayland objects
    //==================
    struct wl_surface* wlSurface() const;

public:
    //================
    // Shm objects
    //================
    struct wl_shm_pool* shmPool();
    void setShmPool(struct wl_shm_pool *shmPool);

    void* shmData();
    void setShmData(void *shmData);

    uint64_t shmDataSize() const;
    void setShmDataSize(uint64_t size);

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
    void exposeEvent(QExposeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    double m_x;
    double m_y;
    double m_width;
    double m_height;

    bool m_visible;
    Color m_color;

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
    struct wl_shm_pool *_shm_pool;
    struct wl_buffer *_buffer;

    //============================
    // Wayland XDG shell objects
    //============================
    struct xdg_surface *_xdg_surface;
    struct xdg_toplevel *_xdg_toplevel;

    void *_shm_data;
    uint64_t _shm_data_size;
};

} // namespace bl

#endif // SURFACEIMPL_H
