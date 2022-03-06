#ifndef APPLICATIONIMPL_H
#define APPLICATIONIMPL_H

#include <wayland-client.h>

#include <vector>

#include <QCoreApplication>
#include <QThread>

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
    struct wl_display* display() const;
    void setDisplay(struct wl_display*);

    struct wl_compositor* compositor() const;
    void setCompositor(struct wl_compositor*);

    struct wl_subcompositor* subcompositor() const;
    void setSubcompositor(struct wl_subcompositor*);

    struct wl_registry* registry() const;
    void setRegistry(struct wl_registry*);

    struct wl_shm* shm() const;
    void setShm(struct wl_shm*);

    struct wl_seat* seat() const;
    void setSeat(struct wl_seat*);

    struct wl_keyboard* keyboard() const;
    void setKeyboard(struct wl_keyboard*);

    struct wl_pointer* pointer() const;
    void setPointer(struct wl_pointer*);

    //============================
    // Wayland XDG shell objects
    //============================
    struct xdg_wm_base* xdgWmBase() const;
    void setXdgWmBase(struct xdg_wm_base*);


    bool addSurfaceImpl(SurfaceImpl*);
    bool removeSurfaceImpl(SurfaceImpl*);
    SurfaceImpl* surfaceImplForWlSurface(struct wl_surface*);
    struct wl_surface* pointerSurface() const;
    void setPointerSurface(struct wl_surface*);

private:
    int _argc;

    //=================
    // Wayland client
    //=================
    struct wl_display *_display;
    struct wl_compositor *_compositor;
    struct wl_subcompositor *_subcompositor;
    struct wl_registry *_registry;
    struct wl_shm *_shm;
    struct wl_seat *_seat;
    struct wl_keyboard *_keyboard;
    struct wl_pointer *_pointer;

    //====================
    // Wayland XDG shell
    //====================
    struct xdg_wm_base *_xdg_wm_base;

    std::vector<SurfaceImpl*> _surface_impl_list;
    /// Some pointer handlers has not surface info. So store it when enter
    /// and pop when leave.
    struct wl_surface *_pointer_surface;

    QCoreApplication *_q_core_application;

    DisplayDispatchThread _display_dispatch;
};

// Singleton object.
extern ApplicationImpl *app_impl;

} // namespace bl

#endif // APPLICATIONIMPL_H
