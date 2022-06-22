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

    bool clip() const;
    void setClip(bool clip);

    double clipX() const;
    double clipY() const;
    double clipWidth() const;
    double clipHeight() const;

    std::shared_ptr<View> rootView();

    void paint();

    void show();

    void setColor(const Color& color);

    void placeAbove(SurfaceImpl*);
    void placeBelow(SurfaceImpl*);

    bool toplevel() const;

    void moveIfToplevel();

    void setBlSurface(Surface *blSurface);
    void setPointerEnterHandler(void (Surface::*)());
    void setPointerLeaveHandler(void (Surface::*)());
    void setPointerPressHandler(void (Surface::*)(uint32_t button, double x, double y));
    void setPointerReleaseHandler(void (Surface::*)(uint32_t button, double x, double y));

    void callPointerEnterHandler();
    void callPointerLeaveHandler();
    void callPointerPressHandler(uint32_t button, double x, double y);
    void callPointerReleaseHandler(uint32_t button, double x, double y);

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

    std::shared_ptr<View> m_rootView;

    //===========
    // Clipping
    //===========
    bool m_clip;
    double m_clipX;
    double m_clipY;
    double m_clipWidth;
    double m_clipHeight;

    Surface *m_blSurface;
    void (Surface::*m_pointerEnterHandler)();
    void (Surface::*m_pointerLeaveHandler)();
    void (Surface::*m_pointerPressHandler)(uint32_t button, double x, double y);
    void (Surface::*m_pointerReleaseHandler)(uint32_t button, double x, double y);

    //=================
    // Wayland client
    //=================
    struct wl_surface *_surface;
    struct wl_subsurface *_subsurface;
    struct wl_callback *_frame_callback;

    //============================
    // Wayland XDG shell objects
    //============================
    struct xdg_surface *_xdg_surface;
    struct xdg_toplevel *_xdg_toplevel;

    //==================
    // EGL/OpenGL
    //==================
    struct wl_egl_window *_egl_window;
    EglObject _egl_object;

    int _shm_fd;
    void *_shm_data;
    uint64_t _shm_data_size;
};

} // namespace bl

#endif // SURFACEIMPL_H
