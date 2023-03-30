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
#include <blusher/wayland/wl-pointer.h>
#include <blusher/wayland/wl-keyboard.h>
#include <blusher/wayland/wl-output.h>
#include <blusher/wayland/xdg-wm-base.h>
#include <blusher/xkb/state.h>
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
    /// Pointer button event not contains x, y position. So, store position
    /// in pointer motion event.
    class PointerState
    {
    public:
        struct wl_surface *wl_surface;
        double x;
        double y;
        int button;
        uint32_t serial;
        double pressed_x;
        double pressed_y;
        /// Static click consists of pointer press and pointer release events
        /// without moving the position.
        uint32_t static_click_count;
    };

public:
    ApplicationImpl(int argc, char *argv[]);
    ~ApplicationImpl();

    int exec();

    void quit();

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

    std::shared_ptr<WlKeyboard> keyboard() const;
    void setKeyboard(std::shared_ptr<WlKeyboard> keyboard);

    std::shared_ptr<WlPointer> pointer() const;
    void setPointer(std::shared_ptr<WlPointer> pointer);

    std::shared_ptr<WlOutput> output();
    void setOutput(std::shared_ptr<WlOutput> output);

    //============================
    // Wayland XDG shell objects
    //============================
    std::shared_ptr<XdgWmBase> xdgWmBase();
    void setXdgWmBase(std::shared_ptr<XdgWmBase> xdg_wm_base);


    std::shared_ptr<xkb::State> xkb_state();
    void set_xkb_state(std::shared_ptr<xkb::State> state);

    bool addSurfaceImpl(SurfaceImpl*);
    bool removeSurfaceImpl(SurfaceImpl*);
    SurfaceImpl* surfaceImplForWlSurface(struct wl_surface*);

    Cursor* cursor();

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
    std::shared_ptr<WlKeyboard> _keyboard;
    std::shared_ptr<WlPointer> _pointer;
    std::shared_ptr<WlOutput> _output;

    //====================
    // Wayland XDG shell
    //====================
    std::shared_ptr<XdgWmBase> _xdg_wm_base;

    std::vector<SurfaceImpl*> _surface_impl_list;

    std::shared_ptr<xkb::State> _xkb_state;

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
