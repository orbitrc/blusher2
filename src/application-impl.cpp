#include "application-impl.h"

#include <stdio.h>

#include <linux/input.h>

#include <QEnterEvent>

#include "surface-impl.h"

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

//=============
// Seat
//=============

// Keyboard

// Pointer
static void pointer_enter_handler(void *data, struct wl_pointer *pointer,
        uint32_t serial, struct wl_surface *surface,
        wl_fixed_t sx, wl_fixed_t sy)
{
    fprintf(stderr, "pointer_enter_handler()\n");
    bl::ApplicationImpl *application_impl = static_cast<bl::ApplicationImpl*>(
        data);

    // Set pointer surface.
    application_impl->setPointerSurface(surface);

    bl::SurfaceImpl *surface_impl =
        application_impl->surfaceImplForWlSurface(surface);

    if (surface_impl != nullptr) {
        QPointF pos;
        QPointF winPos;
        QPointF scrPos;
        QEnterEvent event(pos, winPos, scrPos);
        QCoreApplication::sendEvent(surface_impl, &event);
    }
}

static void pointer_leave_handler(void *data, struct wl_pointer *pointer,
        uint32_t serial, struct wl_surface *surface)
{
    bl::ApplicationImpl *application_impl = static_cast<bl::ApplicationImpl*>(
        data);

    // Pop pointer surface.
    application_impl->setPointerSurface(nullptr);

    bl::SurfaceImpl *surface_impl =
        application_impl->surfaceImplForWlSurface(surface);

    if (surface_impl != nullptr) {
        QEvent event(QEvent::Type::Leave);
        QCoreApplication::sendEvent(surface_impl, &event);
    }
}

static void pointer_motion_handler(void *data, struct wl_pointer *pointer,
        uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
}

static void pointer_button_handler(void *data, struct wl_pointer *pointer,
        uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    bl::ApplicationImpl *application_impl = static_cast<bl::ApplicationImpl*>(
        data);

    struct wl_surface *surface = application_impl->pointerSurface();
    if (surface != nullptr) {
        bl::SurfaceImpl *surface_impl =
            application_impl->surfaceImplForWlSurface(surface);
        if (surface_impl != nullptr) {
            // Pointer press event.
            if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
                application_impl->setPointerPressSerial(serial);
                surface_impl->callPointerPressHandler(button, 0, 0);
            }
            // Pointer release event.
            if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
                surface_impl->callPointerReleaseHandler(0, 0, 0);
            }
        }
//        QMouseEvent event(QEvent::Type::MouseButtonPress, pos, )
    }
}

static void pointer_axis_handler(void *data, struct wl_pointer *wl_pointer,
        uint32_t time, uint32_t axis, wl_fixed_t value)
{
    fprintf(stderr, "Pointer handle axis\n");
}

static void pointer_frame_handler(void *data, struct wl_pointer *wl_pointer)
{
//    bl_log(BL_LOG_LEVEL_INFO, "%s()", __func__);
}

static void pointer_axis_source_handler(void *data,
        struct wl_pointer *wl_pointer, uint32_t axis_source)
{
}

static void pointer_axis_stop_handler(void *data,
        struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis)
{
}

static void pointer_axis_discrete_handler(void *data,
        struct wl_pointer *wl_pointer, uint32_t axis, int discrete)
{
}

static const struct wl_pointer_listener pointer_listener = {
    .enter = pointer_enter_handler,
    .leave = pointer_leave_handler,
    .motion = pointer_motion_handler,
    .button = pointer_button_handler,
    .axis = pointer_axis_handler,
    .frame = pointer_frame_handler,
    .axis_source = pointer_axis_source_handler,
    .axis_stop = pointer_axis_stop_handler,
    .axis_discrete = pointer_axis_discrete_handler,
};

// Capabilities
static void seat_capabilities_handler(void *data, struct wl_seat *seat,
        uint32_t caps)
{
    fprintf(stderr, "seat_capabilities_handler() - capabilities: %d\n", caps);

    bl::ApplicationImpl *application_impl = static_cast<bl::ApplicationImpl*>(data);
    if (caps & WL_SEAT_CAPABILITY_POINTER &&
            application_impl->pointer() == nullptr) {
        application_impl->setPointer(
            wl_seat_get_pointer(application_impl->seat()));
        wl_pointer_add_listener(application_impl->pointer(),
            &pointer_listener, (void*)application_impl);
    } else if (caps & WL_SEAT_CAPABILITY_KEYBOARD &&
            application_impl->keyboard() == nullptr) {
        application_impl->setKeyboard(
            wl_seat_get_keyboard(application_impl->seat()));
    }
}

static void seat_name_handler(void *data, struct wl_seat *seat,
        const char *name)
{
    fprintf(stderr, "seat_name_handler() - name: %s\n", name);
}

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_capabilities_handler,
    .name = seat_name_handler,
};

//==============
// Global
//==============

static void global_registry_handler(void *data, struct wl_registry *registry,
        uint32_t id, const char *interface, uint32_t version)
{
    bl::ApplicationImpl *application_impl = static_cast<bl::ApplicationImpl*>(
        data);

    fprintf(stderr, "<%s %d>\n", interface, version);
    if (strcmp(interface, "wl_seat") == 0) {
        if (application_impl->seat() == NULL) {
            application_impl->setSeat(static_cast<struct wl_seat*>(
                wl_registry_bind(registry, id, &wl_seat_interface, 7)));
            wl_seat_add_listener(application_impl->seat(),
                &seat_listener, (void*)application_impl);
        }
    } else if (strcmp(interface, "wl_compositor") == 0) {
        if (application_impl->compositor() == NULL) {
            application_impl->setCompositor(static_cast<struct wl_compositor*>(
                wl_registry_bind(registry, id, &wl_compositor_interface, 3)));
        }
    } else if (strcmp(interface, "wl_subcompositor") == 0) {
        if (application_impl->subcompositor() == NULL) {
            application_impl->setSubcompositor(static_cast<struct wl_subcompositor*>(
                wl_registry_bind(registry, id, &wl_subcompositor_interface, 1)));
        }
    } else if (strcmp(interface, "wl_shm") == 0) {
        if (application_impl->shm() == NULL) {
            application_impl->setShm(static_cast<struct wl_shm*>(
                wl_registry_bind(registry, id, &wl_shm_interface, 1)));
        }
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        if (application_impl->xdgWmBase() == NULL) {
            application_impl->setXdgWmBase(static_cast<struct xdg_wm_base*>(
                wl_registry_bind(registry, id, &xdg_wm_base_interface, 3)));
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
    this->_seat = NULL;
    this->_keyboard = NULL;
    this->_pointer = NULL;

    this->_xdg_wm_base = NULL;

    this->setRegistry(wl_display_get_registry(this->_display));

    wl_registry_add_listener(this->_registry,
        &registry_listener, (void*)this);

    wl_display_dispatch(this->_display);
    wl_display_roundtrip(this->_display);

    xdg_wm_base_add_listener(this->_xdg_wm_base,
        &xdg_wm_base_listener, (void*)this);

//    wl_seat_add_listener(this->_seat,
//        &seat_listener, (void*)this);

    app_impl = this;

    this->_q_core_application = new QCoreApplication(this->_argc, argv);
}

ApplicationImpl::~ApplicationImpl()
{
    delete this->_q_core_application;
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

// Seat
struct wl_seat* ApplicationImpl::seat() const
{
    return this->_seat;
}

void ApplicationImpl::setSeat(struct wl_seat *seat)
{
    this->_seat = seat;
}

struct wl_keyboard* ApplicationImpl::keyboard() const
{
    return this->_keyboard;
}

void ApplicationImpl::setKeyboard(struct wl_keyboard *keyboard)
{
    this->_keyboard = keyboard;
}

struct wl_pointer* ApplicationImpl::pointer() const
{
    return this->_pointer;
}

void ApplicationImpl::setPointer(struct wl_pointer *pointer)
{
    this->_pointer = pointer;
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


bool ApplicationImpl::addSurfaceImpl(SurfaceImpl *impl)
{
    this->_surface_impl_list.push_back(impl);

    return true;
}

bool ApplicationImpl::removeSurfaceImpl(SurfaceImpl *impl)
{
    bool ret = false;
    auto begin = this->_surface_impl_list.begin();
    auto end = this->_surface_impl_list.end();

    for (auto it = begin; it != end; ++it) {
        if (*it == impl) {
            ret = true;
            this->_surface_impl_list.erase(it);
            break;
        }
    }

    return ret;
}

SurfaceImpl* ApplicationImpl::surfaceImplForWlSurface(
        struct wl_surface *wl_surface)
{
    SurfaceImpl *ret = nullptr;
    auto begin = this->_surface_impl_list.begin();
    auto end = this->_surface_impl_list.end();

    for (auto it = begin; it != end; ++it) {
        if ((*it)->wlSurface() == wl_surface) {
            ret = *it;
            break;
        }
    }

    return ret;
}

struct wl_surface* ApplicationImpl::pointerSurface() const
{
    return this->_pointer_surface;
}

void ApplicationImpl::setPointerSurface(struct wl_surface *surface)
{
    this->_pointer_surface = surface;
}

uint32_t ApplicationImpl::pointerPressSerial() const
{
    return this->_pointer_press_serial;
}

void ApplicationImpl::setPointerPressSerial(uint32_t serial)
{
    this->_pointer_press_serial = serial;
}

} // namespace bl
