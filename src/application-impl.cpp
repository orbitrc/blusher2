#include "application-impl.h"

//==============
// Shm
//==============
static void shm_format(void *data, struct wl_shm *wl_shm, uint32_t format)
{
    fprintf(stderr, "Format: %d\n", format);
}

struct wl_shm_listener shm_listener = {
    .format = shm_format,
};

//==============
// XDG
//==============
static void xdg_wm_base_ping_handler(void *data,
        struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping_handler,
};

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
    } else if (strcmp(interface, "wl_shm") == 0) {
        if (application_impl->shm() == NULL) {
            application_impl->setShm(static_cast<struct wl_shm*>(
                wl_registry_bind(registry, id, &wl_shm_interface, 1)));
        }
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        if (application_impl->xdgWmBase() == NULL) {
            application_impl->setXdgWmBase(static_cast<struct xdg_wm_base*>(
                wl_registry_bind(registry, id, &xdg_wm_base_interface, 4)));
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

void DisplayDispatchThread::run()
{
    fprintf(stderr, "before wl_display_dispatch %p\n", app_impl->display());
    int result = wl_display_dispatch(app_impl->display());
    fprintf(stderr, "result: %d\n", result);
    while (result != -1) {
        result = wl_display_dispatch(app_impl->display());
    }
}

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
    xdg_wm_base_add_listener(this->_xdg_wm_base,
        &xdg_wm_base_listener, (void*)this);

    wl_display_dispatch(this->_display);
    wl_display_roundtrip(this->_display);

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
    DisplayDispatchThread thr;
    thr.start();

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

//===========================
// Wayland XDG shell objects
//===========================

// Xdg wm base
struct xdg_wm_base* ApplicationImpl::xdgWmBase() const
{
    return this->_xdg_wm_base;
}

void ApplicationImpl::setXdgWmBase(struct xdg_wm_base *base)
{
    this->_xdg_wm_base = base;
}

} // namespace bl
