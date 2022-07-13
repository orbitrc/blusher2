#ifndef _BL_APPLICATION_H
#define _BL_APPLICATION_H

// C++
#include <memory>

// Primer
#include <primer/vector.h>

// Blusher
#include <blusher/wayland/wl-compositor.h>
#include <blusher/wayland/xdg-wm-base.h>
#include <blusher/event-dispatcher.h>
#include <blusher/output.h>

namespace bl {

class ApplicationImpl;

class Resource;

class DesktopSurface;

class Application
{
    friend class ApplicationImpl;
public:
    Application(int argc, char *argv[]);
    ~Application();

    Resource* resource();

    std::shared_ptr<EventDispatcher> event_dispatcher();

    //==================
    // Wayland Native
    //==================
    std::shared_ptr<WlCompositor> wl_compositor();

    std::shared_ptr<XdgWmBase> xdg_wm_base();


    const pr::Vector<DesktopSurface*> desktop_surfaces() const;

    void add_desktop_surface(DesktopSurface *desktop_surface);

    void remove_desktop_surface(DesktopSurface *desktop_surface);

    int exec();

private:
    ApplicationImpl *_impl;

    Resource *_resource;

    std::shared_ptr<EventDispatcher> _event_dispatcher;

    pr::Vector<DesktopSurface*> _desktop_surfaces;

    pr::Vector<std::shared_ptr<Output>> _outputs;
};

// Singleton object.
extern Application *app;

} // namespace bl

#endif /* _BL_APPLICATION_H */
