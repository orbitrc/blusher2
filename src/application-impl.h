#ifndef APPLICATIONIMPL_H
#define APPLICATIONIMPL_H

#include <wayland-client.h>

#include <QCoreApplication>
#include <QGuiApplication>

namespace bl {

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

    QCoreApplication *_q_core_application;
    QGuiApplication *_q_gui_application;
};

// Singleton object.
extern ApplicationImpl *app_impl;

} // namespace bl

#endif // APPLICATIONIMPL_H
