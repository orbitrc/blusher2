#include "application-impl.h"

//==============
// Global
//==============

static void global_registry_handler(void *data, struct wl_registry *registry,
        uint32_t id, const char *interface, uint32_t version)
{
    bl::ApplicationImpl *application_impl = static_cast<bl::ApplicationImpl*>(
        data);

    if (strcmp(interface, "wl_seat") == 0) {
        // TODO
    } else if (strcmp(interface, "wl_compositor") == 0) {
        if (application_impl->compositor() == NULL) {
            application_impl->setCompositor(static_cast<struct wl_compositor*>(
                wl_registry_bind(registry, id, &wl_compositor_interface, 3)));
        }
    }
}

static void global_registry_remover(void *data, struct wl_registry *registry,
        uint32_t id)
{
    (void)data;
    (void)registry;
    (void)id;
    fprintf(stderr, "global_registry_remover()\n");
}

static const struct wl_registry_listener registry_listener = {
    .global = global_registry_handler,
    .global_remove = global_registry_remover,
};

namespace bl {

ApplicationImpl *app_impl = nullptr;

ApplicationImpl::ApplicationImpl(int argc, char *argv[])
{
    this->_argc = argc;

    //===============
    // Wayland
    //===============
    this->setDisplay(wl_display_connect(NULL));

    this->_compositor = NULL;
    this->_subcompositor = NULL;
    this->_registry = NULL;
    this->_shm = NULL;

    this->setRegistry(wl_display_get_registry(this->_display));
    wl_registry_add_listener(this->_registry,
        &registry_listener, (void*)this);

    app_impl = this;

    this->_q_core_application = new QCoreApplication(this->_argc, argv);
    this->_q_gui_application = new QGuiApplication(this->_argc, argv);
}

ApplicationImpl::~ApplicationImpl()
{
    delete this->_q_core_application;
    delete this->_q_gui_application;
}

int ApplicationImpl::exec()
{
    return this->_q_core_application->exec();
//    return this->_q_gui_application->exec();
}

//=========================
// Wayland client objects
//=========================

// Display
struct wl_display* ApplicationImpl::display() const
{
    return this->_display;
}

void ApplicationImpl::setDisplay(struct wl_display *display)
{
    this->_display = display;
}

// Compositor
struct wl_compositor* ApplicationImpl::compositor() const
{
    return this->_compositor;
}

void ApplicationImpl::setCompositor(struct wl_compositor *compositor)
{
    this->_compositor =  compositor;
}

// Subcompositor
struct wl_subcompositor* ApplicationImpl::subcompositor() const
{
    return this->_subcompositor;
}

void ApplicationImpl::setSubcompositor(struct wl_subcompositor *subcompositor)
{
    this->_subcompositor = subcompositor;
}

// Registry
struct wl_registry* ApplicationImpl::registry() const
{
    return this->_registry;
}

void ApplicationImpl::setRegistry(struct wl_registry *registry)
{
    this->_registry = registry;
}

// Shm
struct wl_shm* ApplicationImpl::shm() const
{
    return this->_shm;
}

void ApplicationImpl::setShm(struct wl_shm *shm)
{
    this->_shm = shm;
}

} // namespace bl
