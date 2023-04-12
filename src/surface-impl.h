#ifndef SURFACEIMPL_H
#define SURFACEIMPL_H

// C
#include <stdint.h>

// C++
#include <functional>
#include <memory>
#include <optional>

// Wayland
#include <wayland-client.h>
#include <wayland-egl.h>

// EGL/OpenGL
#include <EGL/egl.h>
#include <GL/glew.h>

// Primer
#include <primer/vector.h>

// Blusher
#include <blusher/wayland/wl-surface.h>
#include <blusher/wayland/wl-subsurface.h>
#include <blusher/gl/context.h>
#include <blusher/gl/program.h>
#include <blusher/color.h>
#include <blusher/view.h>

namespace bl {

class Surface;

class SurfaceImpl
{
public:

    SurfaceImpl(Surface *surface);
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

    pr::Vector<Surface*> children() const;

    View* entered_view() const;
    void set_entered_view(View *view);

    void callPointerEnterHandler();
    void callPointerLeaveHandler();
    void callPointerPressHandler(uint32_t button, double x, double y);
    void callPointerReleaseHandler(uint32_t button, double x, double y);
    void callPointerMoveHandler(uint32_t button, double x, double y);
    void callResizeHandler(int32_t width, int32_t height,
            int32_t old_width, int32_t old_height);

    std::shared_ptr<gl::Context> context();

    void swapBuffers();

    void makeCurrent(bool nullSurface = false);

public:
    //==================
    // Wayland objects
    //==================
    std::shared_ptr<WlSurface> wl_surface() const;

    struct wl_surface* wlSurface() const;

public:
    void update();

protected:
    /*
    bool event(QEvent *event);
    void exposeEvent(QExposeEvent *event);
    */

private:
    //===============
    // Set Uniforms
    //===============

    /// Set uniform variable `fillType`.
    void _set_uniform_fillType(View::FillType fill_type);
    /// Set uniform variable `fillColor`.
    void _set_uniform_fillColor(Color color);
    /// Set uniform texture.
    void _set_uniform_textureIn(const Image& image);
    /// Set uniform variable `parentPosition`.
    void _set_uniform_parentPosition(Point position);
    /// Set uniform variable `parentSize`.
    void _set_uniform_parentSize(Size size);
    /// Set uniform variable `resolution`.
    void _set_uniform_resolution(Size size);
    /// Set uniform variable `validGeometry`.
    void _set_uniform_validGeometry(Rect geometry);
    /// Set uniform variable `validViewport`.
    void _set_uniform_validViewport(Rect viewport);
    /// Set uniform variable `viewport`.
    void _set_uniform_viewport(Rect viewport);

private:
    void _init_program();
    /// Using `_recursive` itself.
    void _recursive(View *view,
            std::optional<Rect> valid_geometry,
            Point relative_position);
    /// Using `_recursive`.
    void _draw_frame();
    void _egl_update(bool swap_buffers = false);

private:
    uint32_t m_x;
    uint32_t m_y;
    uint32_t m_width;
    uint32_t m_height;

    bool m_visible;

    View *m_rootView;

    bool _updating;

    Surface *m_blSurface;

    /// Currently pointer entered view.
    View *_entered_view;

    //=================
    // Wayland client
    //=================
    std::shared_ptr<WlSurface> _wl_surface;
    std::shared_ptr<WlSubsurface> _wl_subsurface;
    struct wl_callback *_frame_callback;

    //==================
    // EGL/OpenGL
    //==================
    struct wl_egl_window *_egl_window;
    std::shared_ptr<gl::Context> _context;
    EGLSurface _egl_surface;
    std::shared_ptr<gl::Program> _program;

    int _shm_fd;
    void *_shm_data;
    uint64_t _shm_data_size;
};

} // namespace bl

#endif // SURFACEIMPL_H
