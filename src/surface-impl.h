#ifndef SURFACEIMPL_H
#define SURFACEIMPL_H

// C
#include <stdint.h>

// C++
#include <functional>
#include <memory>

// Qt
#include <QObject>
#include <QExposeEvent>
#include <QMouseEvent>

// Wayland
#include <wayland-client.h>
#include <wayland-egl.h>

// EGL/OpenGL
#include <EGL/egl.h>
#include <GLES3/gl3.h>

// Blusher
#include <blusher/wayland/wl-surface.h>
#include <blusher/wayland/wl-subsurface.h>
#include <blusher/wayland/xdg-surface.h>
#include <blusher/wayland/xdg-toplevel.h>
#include <blusher/color.h>
#include <blusher/view.h>

// Wayland protocols
#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

class Surface;

class SurfaceImpl : public QObject
{
    Q_OBJECT

public:
    class EglObject {
    public:
        EglObject()
        {
            this->egl_display = nullptr;
            this->egl_config = nullptr;
            this->egl_surface = nullptr;
            this->egl_context = nullptr;
            this->program_object = 0;
        }

        EGLDisplay egl_display;
        EGLConfig egl_config;
        EGLSurface egl_surface;
        EGLContext egl_context;
        GLuint program_object;
    };

public:

    SurfaceImpl(Surface *surface, QObject *parent = nullptr);
    ~SurfaceImpl();

    uint32_t x() const;
    uint32_t y() const;
    uint32_t width() const;
    uint32_t height() const;

    void setX(uint32_t x);
    void setY(uint32_t y);
    void setWidth(uint32_t width);
    void setHeight(uint32_t height);
    void setSize(uint32_t width, uint32_t height);

    View* rootView();

    void paint();

    void show();
    void hide();

    bool visible() const;

    void placeAbove(SurfaceImpl*);
    void placeBelow(SurfaceImpl*);

    bool toplevel() const;
    bool isToplevel() const;

    Surface* surface();

    void callPointerEnterHandler();
    void callPointerLeaveHandler();
    void callPointerPressHandler(uint32_t button, double x, double y);
    void callPointerReleaseHandler(uint32_t button, double x, double y);
    void callPointerMoveHandler(uint32_t button, double x, double y);
    void callResizeHandler(int32_t width, int32_t height,
            int32_t old_width, int32_t old_height);

public:
    //==================
    // Wayland objects
    //==================
    struct wl_surface* wlSurface() const;

signals:
    void implXChanged(double x);
    void implYChanged(double y);
    void implWidthChanged(double width);
    void implHeightChanged(double height);

public slots:
    void update();

private slots:

protected:
    bool event(QEvent *event) override;
    void exposeEvent(QExposeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    void _egl_update(bool hide = false);

private:
    uint32_t m_x;
    uint32_t m_y;
    uint32_t m_width;
    uint32_t m_height;

    bool m_visible;

    View *m_rootView;

    bool _updating;

    Surface *m_blSurface;

    //=================
    // Wayland client
    //=================
    std::shared_ptr<WlSubsurface> _wl_subsurface;
    struct wl_callback *_frame_callback;

    //==================
    // EGL/OpenGL
    //==================
    struct wl_egl_window *_egl_window;
    EglObject _egl_object;

    int _shm_fd;
    void *_shm_data;
    uint64_t _shm_data_size;

    //===========================
    // Toplevel surface states
    //===========================
    bool _toplevel_maximized;
};

} // namespace bl

#endif // SURFACEIMPL_H
