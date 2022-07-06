#ifndef APPLICATIONIMPL_H
#define APPLICATIONIMPL_H

#include <wayland-client.h>

#include <vector>
#include <memory>

#include <QCoreApplication>
#include <QThread>

#include <blusher/wayland/wl-display.h>
#include <blusher/wayland/wl-registry.h>
#include <blusher/wayland/wl-compositor.h>
#include <blusher/wayland/wl-seat.h>
#include <blusher/wayland/xdg-wm-base.h>
#include <blusher/cursor.h>

// Wayland protocols
#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

class SurfaceImpl;

class DisplayDispatchThread : public QThread
{
    Q_OBJECT
public:
    void run() override;
};

class ApplicationImpl
{
public:
    class PointerState
    {
    public:
        struct wl_surface *wl_surface;
        double x;
        double y;
        int button;
        uint32_t serial;
    };

public:
    ApplicationImpl(int argc, char *argv[]);
    ~ApplicationImpl();

    int exec();

public:
    //========================
    // Wayland client objects
    //========================
    WlDisplay* display();

    std::shared_ptr<WlCompositor> compositor() const;
    void setCompositor(std::shared_ptr<WlCompositor>);

    std::shared_ptr<WlSubcompositor> subcompositor() const;
    void setSubcompositor(std::shared_ptr<WlSubcompositor>);

    WlRegistry* registry();

    struct wl_shm* shm() const;
    void setShm(struct wl_shm*);

    std::shared_ptr<WlSeat> seat();
    void setSeat(std::shared_ptr<WlSeat> seat);

    struct wl_keyboard* keyboard() const;
    void setKeyboard(struct wl_keyboard*);

    struct wl_pointer* pointer() const;
    void setPointer(struct wl_pointer*);

    //============================
    // Wayland XDG shell objects
    //============================
    std::shared_ptr<XdgWmBase> xdgWmBase();
    void setXdgWmBase(std::shared_ptr<XdgWmBase> xdg_wm_base);


    bool addSurfaceImpl(SurfaceImpl*);
    bool removeSurfaceImpl(SurfaceImpl*);
    SurfaceImpl* surfaceImplForWlSurface(struct wl_surface*);

    Cursor* cursor();

    /// Pointer x position stored in pointer motion event.
    double pointerEventX() const;
    /// Pointer y position stored in pointer motion event.
    double pointerEventY() const;
    /// Pointer button event not contains x, y position. So, store position
    /// in pointer motion event.
    void setPointerEventX(double x);
    /// Pointer button event not contains x, y position. So, store position
    /// in pointer motion event.
    void setPointerEventY(double y);

private:
    int _argc;

    //=================
    // Wayland client
    //=================
    WlDisplay _display;
    std::shared_ptr<WlCompositor> _compositor;
    std::shared_ptr<WlSubcompositor> _subcompositor;
    WlRegistry _registry;
    struct wl_shm *_shm;
    std::shared_ptr<WlSeat> _seat;
    struct wl_keyboard *_keyboard;
    struct wl_pointer *_pointer;

    //====================
    // Wayland XDG shell
    //====================
    std::shared_ptr<XdgWmBase> _xdg_wm_base;

    std::vector<SurfaceImpl*> _surface_impl_list;

public:
    Cursor *_cursor;

    /// Store global pointer event informations.
    PointerState pointer_state;

private:
    QCoreApplication *_q_core_application;

    DisplayDispatchThread _display_dispatch;
};

// Singleton object.
extern ApplicationImpl *app_impl;

} // namespace bl

#endif // APPLICATIONIMPL_H
