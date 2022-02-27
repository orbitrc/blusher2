#ifndef _BL_APPLICATION_H
#define _BL_APPLICATION_H

namespace bl {

class ApplicationImpl;

class Resource;

class Application
{
public:
    Application(int argc, char *argv[]);
    ~Application();

    Resource* resource();

    int exec();

private:
    ApplicationImpl *_impl;

    Resource *_resource;
};

// Singleton object.
extern Application *app;

} // namespace bl

#endif /* _BL_APPLICATION_H */
