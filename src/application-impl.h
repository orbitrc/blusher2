#ifndef APPLICATIONIMPL_H
#define APPLICATIONIMPL_H

#include <wayland-client.h>

#include <vector>
#include <memory>

#include <QCoreApplication>
#include <QThread>

#include <blusher/wayland/wl-display.h>
#include <blusher/wayland/wl-registry.h>
#include <blusher/wayland/wl-seat.h>
#include <blusher/wayland/xdg-wm-base.h>

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
    ApplicationImpl(int argc, char *argv[]);
    ~ApplicationImpl();

    int exec();

public:
    //========================
    // Wayland client objects
    //========================
    WlDisplay* display();

    struct wl_compositor* compositor() const;
    void setCompositor(struct wl_compositor*);

    struct wl_subcompositor* subcompositor() const;
    void setSubcompositor(struct wl_subcompositor*);

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
    struct wl_surface* pointerSurface() const;
    void setPointerSurface(struct wl_surface*);
    uint32_t pointerPressSerial() const;
    void setPointerPressSerial(uint32_t serial);

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
    struct wl_compositor *_compositor;
    struct wl_subcompositor *_subcompositor;
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
    /// Some pointer handlers has not surface info. So store it when enter
    /// and pop when leave.
    struct wl_surface *_pointer_surface;
    uint32_t _pointer_press_serial;
    double _pointer_event_x;
    double _pointer_event_y;

    QCoreApplication *_q_core_application;

    DisplayDispatchThread _display_dispatch;
};

// Singleton object.
extern ApplicationImpl *app_impl;

} // namespace bl

#endif // APPLICATIONIMPL_H
