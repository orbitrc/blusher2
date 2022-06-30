#ifndef _BL_APPLICATION_H
#define _BL_APPLICATION_H

// C++
#include <memory>

// Blusher
#include <blusher/wayland/wl-compositor.h>
#include <blusher/event-dispatcher.h>

namespace bl {

class ApplicationImpl;

class Resource;

class Application
{
public:
    Application(int argc, char *argv[]);
    ~Application();

    Resource* resource();

    std::shared_ptr<EventDispatcher> event_dispatcher();

    //==================
    // Wayland Native
    //==================
    std::shared_ptr<WlCompositor> compositor();


    int exec();

private:
    ApplicationImpl *_impl;

    Resource *_resource;

    std::shared_ptr<EventDispatcher> _event_dispatcher;
};

// Singleton object.
extern Application *app;

} // namespace bl

#endif /* _BL_APPLICATION_H */
