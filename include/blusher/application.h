#ifndef _BL_APPLICATION_H
#define _BL_APPLICATION_H

// C++
#include <memory>

// Blusher
#include <blusher/wayland/wl-compositor.h>

namespace bl {

class ApplicationImpl;

class Resource;

class Application
{
public:
    Application(int argc, char *argv[]);
    ~Application();

    Resource* resource();

    //==================
    // Wayland Native
    //==================
    std::shared_ptr<WlCompositor> compositor();


    int exec();

private:
    ApplicationImpl *_impl;

    Resource *_resource;
};

// Singleton object.
extern Application *app;

} // namespace bl

#endif /* _BL_APPLICATION_H */
