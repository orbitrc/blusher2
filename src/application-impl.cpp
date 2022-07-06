#include "application-impl.h"

#include <stdio.h>
#include <assert.h>

#include <linux/input.h>

#include <QEnterEvent>

#include <blusher/utils.h>

#include "surface-impl.h"

//==============
// Shm
//==============
static void shm_format(void *data, struct wl_shm *wl_shm, uint32_t format)
{
    (void)data;
    (void)wl_shm;
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
    (void)data;
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const bl::XdgWmBase::Listener xdg_wm_base_listener =
    bl::XdgWmBase::Listener(
        xdg_wm_base_ping_handler
    );

//=============
// Keyboard
//=============


//=============
// Pointer
//=============
static void pointer_enter_handler(void *data, struct wl_pointer *pointer,
        uint32_t serial, struct wl_surface *surface,
        wl_fixed_t sx, wl_fixed_t sy)
{
    bl::ApplicationImpl *application_impl = static_cast<bl::ApplicationImpl*>(
        data);

    // Set pointer surface.
    application_impl->pointer_state.wl_surface = surface;

    bl::SurfaceImpl *surface_impl =
        application_impl->surfaceImplForWlSurface(surface);

    // Set toy cursor.
    if (application_impl->cursor() == nullptr) {
        application_impl->_cursor = new bl::Cursor(bl::Cursor::Shape::Arrow);
    }
    wl_pointer_set_cursor(pointer, serial,
        application_impl->cursor()->wl_surface(),
        application_impl->cursor()->hot_spot_x(),
        application_impl->cursor()->hot_spot_y());

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
    application_impl->pointer_state.wl_surface = nullptr;

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
    (void)data;
    (void)pointer;
    (void)time;
    double x = wl_fixed_to_double(sx);
    double y = wl_fixed_to_double(sy);
    bl::app_impl->setPointerEventX(x);
    bl::app_impl->setPointerEventY(y);

    struct wl_surface *active_wl_surface = bl::app_impl->pointer_state.wl_surface;
    if (active_wl_surface != nullptr) {
        bl::SurfaceImpl *surface_impl =
            bl::app_impl->surfaceImplForWlSurface(active_wl_surface);

        // Set button.
        Qt::MouseButton q_btn = libinput_button_to_qt_mouse_button(
            bl::app_impl->pointer_state.button);

        if (surface_impl != nullptr) {
            QPoint pos(x, y);
            Qt::KeyboardModifiers mod;
            QMouseEvent event(QEvent::MouseMove, pos, q_btn, 0, mod);
            QCoreApplication::sendEvent(surface_impl, &event);
        }
    }
}

static void pointer_button_handler(void *data, struct wl_pointer *pointer,
        uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    (void)pointer;
    (void)time;
    bl::ApplicationImpl *application_impl = static_cast<bl::ApplicationImpl*>(
        data);

    struct wl_surface *surface = application_impl->pointer_state.wl_surface;
    if (surface != nullptr) {
        bl::SurfaceImpl *surface_impl =
            application_impl->surfaceImplForWlSurface(surface);
        if (surface_impl != nullptr) {
            // Pointer press event.
            if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
                application_impl->pointer_state.serial = serial;
                application_impl->pointer_state.button = button;
                surface_impl->callPointerPressHandler(button,
                    application_impl->pointerEventX(),
                    application_impl->pointerEventY()
                );
            }
            // Pointer release event.
            if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
                application_impl->pointer_state.button = 0;
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

//=============
// Seat
//=============

static void seat_capabilities_handler(void *data, struct wl_seat *seat,
        uint32_t caps)
{
    (void)data;
    (void)seat;
    fprintf(stderr, "seat_capabilities_handler() - capabilities: %d\n", caps);

    bl::ApplicationImpl *application_impl =
        static_cast<bl::ApplicationImpl*>(data);
    assert(application_impl != nullptr);

    if (caps & WL_SEAT_CAPABILITY_POINTER &&
            application_impl->pointer() == nullptr) {
        application_impl->setPointer(
            wl_seat_get_pointer(application_impl->seat()->wl_seat()));
        wl_pointer_add_listener(application_impl->pointer(),
            &pointer_listener, (void*)application_impl);
    } else if (caps & WL_SEAT_CAPABILITY_KEYBOARD &&
            application_impl->keyboard() == nullptr) {
        application_impl->setKeyboard(
            wl_seat_get_keyboard(application_impl->seat()->wl_seat()));
    }
}

static void seat_name_handler(void *data, struct wl_seat *seat,
        const char *name)
{
    (void)data;
    (void)seat;
    fprintf(stderr, "seat_name_handler() - name: %s\n", name);
}

static const bl::WlSeat::Listener seat_listener =
    bl::WlSeat::Listener(
        seat_capabilities_handler,
        seat_name_handler
    );

//==============
// Global
//==============

static void global_registry_handler(void *data, struct wl_registry *registry,
        uint32_t id, const char *interface, uint32_t version)
{
    bl::ApplicationImpl *application_impl = static_cast<bl::ApplicationImpl*>(
        data);

    bl::WlRegistry *reg = bl::WlRegistry::instance();
    // Check singleton registry is correct.
    assert(reg->wl_registry() == registry);

    fprintf(stderr, "<%s %d>\n", interface, version);
    if (strcmp(interface, "wl_seat") == 0) {
        if (application_impl->seat() == nullptr) {
            auto interface = bl::WlInterface<bl::WlInterfaceType::Seat>();
            application_impl->setSeat(reg->bind(id, interface, 5));
            application_impl->seat()->add_listener(seat_listener,
                static_cast<void*>(application_impl));
        }
    } else if (strcmp(interface, "wl_compositor") == 0) {
        if (application_impl->compositor() == nullptr) {
            auto interface = bl::WlInterface<bl::WlInterfaceType::Compositor>();
            application_impl->setCompositor(reg->bind(id, interface, 3));
        }
    } else if (strcmp(interface, "wl_subcompositor") == 0) {
        if (application_impl->subcompositor() == nullptr) {
            auto interface = bl::WlInterface<bl::WlInterfaceType::Subcompositor>();
            application_impl->setSubcompositor(
                reg->bind(id, interface, 1));
        }
    } else if (strcmp(interface, "wl_shm") == 0) {
        if (application_impl->shm() == NULL) {
            application_impl->setShm(static_cast<struct wl_shm*>(
                wl_registry_bind(registry, id, &wl_shm_interface, 1)));
        }
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        if (application_impl->xdgWmBase() == nullptr) {
            auto interface = bl::WlInterface<bl::WlInterfaceType::XdgWmBase>();
            application_impl->setXdgWmBase(reg->bind(id, interface, 3));
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
    int result = wl_display_dispatch(app_impl->display()->wl_display());
    fprintf(stderr, "result: %d\n", result);
    while (result != -1) {
        result = wl_display_dispatch(app_impl->display()->wl_display());
        // fprintf(stderr, "result: %d\n", result);
    }
}

ApplicationImpl *app_impl = nullptr;

ApplicationImpl::ApplicationImpl(int argc, char *argv[])
    : _display(WlDisplay::connect())
{
    this->_argc = argc;

    //===============
    // Wayland
    //===============
//    this->_display = WlDisplay::connect();

    this->_compositor = nullptr;
    this->_subcompositor = NULL;
    this->_shm = NULL;
    this->_seat = nullptr;
    this->_keyboard = NULL;
    this->_pointer = NULL;

    this->pointer_state.x = 0;
    this->pointer_state.y = 0;

    // Toy cursor. Change this later.
    this->_cursor = nullptr;

    this->_xdg_wm_base = nullptr;

    fprintf(stderr, "!!! get_registry()\n");
    this->_registry = this->_display.get_registry();

    wl_registry_add_listener(this->_registry.wl_registry(),
        &registry_listener, (void*)this);

    this->_display.dispatch();
    this->_display.roundtrip();

    this->_xdg_wm_base->add_listener(xdg_wm_base_listener);

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
WlDisplay* ApplicationImpl::display()
{
    return &this->_display;
}

// Compositor
std::shared_ptr<WlCompositor> ApplicationImpl::compositor() const
{
    return this->_compositor;
}

void ApplicationImpl::setCompositor(std::shared_ptr<WlCompositor> compositor)
{
    this->_compositor =  compositor;
}

// Subcompositor
std::shared_ptr<WlSubcompositor> ApplicationImpl::subcompositor() const
{
    return this->_subcompositor;
}

void ApplicationImpl::setSubcompositor(
        std::shared_ptr<WlSubcompositor> subcompositor)
{
    this->_subcompositor = subcompositor;
}

// Registry
WlRegistry* ApplicationImpl::registry()
{
    return &this->_registry;
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
std::shared_ptr<WlSeat> ApplicationImpl::seat()
{
    return this->_seat;
}

void ApplicationImpl::setSeat(std::shared_ptr<WlSeat> seat)
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

std::shared_ptr<XdgWmBase> ApplicationImpl::xdgWmBase()
{
    return this->_xdg_wm_base;
}

void ApplicationImpl::setXdgWmBase(std::shared_ptr<XdgWmBase> xdg_wm_base)
{
    this->_xdg_wm_base = xdg_wm_base;
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

double ApplicationImpl::pointerEventX() const
{
    return this->pointer_state.x;
}

double ApplicationImpl::pointerEventY() const
{
    return this->pointer_state.y;
}

void ApplicationImpl::setPointerEventX(double x)
{
    if (this->pointer_state.x != x) {
        this->pointer_state.x = x;
    }
}

void ApplicationImpl::setPointerEventY(double y)
{
    if (this->pointer_state.y != y) {
        this->pointer_state.y = y;
    }
}

Cursor* ApplicationImpl::cursor()
{
    return this->_cursor;
}

} // namespace bl
