#include "application-impl.h"

#include <stdio.h>
#include <assert.h>

#include <linux/input.h>

#include <QEnterEvent>

#include <blusher/application.h>
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
// Output
//==============
static void output_geometry_handler(void *data,
        struct wl_output *wl_output,
        int32_t x,
        int32_t y,
        int32_t physical_width,
        int32_t physical_height,
        int32_t subpixel,
        const char *make,
        const char *model,
        int32_t transform)
{
    (void)data;
    fprintf(stderr, "output_geometry_handler() - output: %p\n", wl_output);
}

static void output_mode_handler(void *data,
        struct wl_output *wl_output,
        uint32_t flags,
        int32_t width,
        int32_t height,
        int32_t refresh)
{
    (void)data;
    fprintf(stderr, "output_mode_handler() - output: %p\n", wl_output);
}

static void output_done_handler(void *data,
        struct wl_output *wl_output)
{
    (void)data;
    fprintf(stderr, "output_done_handler() - output: %p\n", wl_output);
}

static void output_scale_handler(void *data,
        struct wl_output *wl_output,
        int32_t scale)
{
    (void)data;
    fprintf(stderr, "output_scale_handler() - output: %p\n", wl_output);
}

static const bl::WlOutput::Listener output_listener =
    bl::WlOutput::Listener(
        output_geometry_handler,
        output_mode_handler,
        output_done_handler,
        output_scale_handler
    );

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
        surface_impl->callPointerEnterHandler();
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
        surface_impl->callPointerLeaveHandler();
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
    bl::app_impl->pointer_state.x = x;
    bl::app_impl->pointer_state.y = y;

    struct wl_surface *active_wl_surface = bl::app_impl->pointer_state.wl_surface;
    if (active_wl_surface != nullptr) {
        bl::SurfaceImpl *surface_impl =
            bl::app_impl->surfaceImplForWlSurface(active_wl_surface);

        if (surface_impl != nullptr) {
            surface_impl->callPointerMoveHandler(
                bl::app_impl->pointer_state.button, x, y);
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
                // Pressed x, y.
                application_impl->pointer_state.pressed_x =
                    application_impl->pointer_state.x;
                application_impl->pointer_state.pressed_y =
                    application_impl->pointer_state.y;

                surface_impl->callPointerPressHandler(button,
                    application_impl->pointer_state.x,
                    application_impl->pointer_state.y
                );
            }
            // Pointer release event.
            if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
                application_impl->pointer_state.button = 0;
                // Static click count.
                double x = application_impl->pointer_state.x;
                double y = application_impl->pointer_state.y;
                if (application_impl->pointer_state.pressed_x == x &&
                        application_impl->pointer_state.pressed_y == y) {
                    application_impl->pointer_state.static_click_count += 1;
                } else {
                    application_impl->pointer_state.static_click_count = 0;
                }

                surface_impl->callPointerReleaseHandler(0,
                    application_impl->pointer_state.x,
                    application_impl->pointer_state.y);

                // Double click.
                if (application_impl->pointer_state.static_click_count == 2) {
                    fprintf(stderr, "DOUBLE CLICK!\n");
                    application_impl->pointer_state.static_click_count = 0;
                }
            }
        }
//        QMouseEvent event(QEvent::Type::MouseButtonPress, pos, )
    }
}

static void pointer_axis_handler(void *data, struct wl_pointer *wl_pointer,
        uint32_t time, uint32_t axis, wl_fixed_t value)
{
    double val = wl_fixed_to_double(value);
    fprintf(stderr,
        "pointer_axis_handler() - axis: %d, value: %f\n", axis, val);
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

static const bl::WlPointer::Listener pointer_listener =
    bl::WlPointer::Listener(
        pointer_enter_handler,
        pointer_leave_handler,
        pointer_motion_handler,
        pointer_button_handler,
        pointer_axis_handler,
        pointer_frame_handler,
        pointer_axis_source_handler,
        pointer_axis_stop_handler,
        pointer_axis_discrete_handler
    );

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
        auto wl_pointer = application_impl->seat()->get_pointer();
        application_impl->setPointer(wl_pointer);
        application_impl->pointer()->add_listener(
            pointer_listener, (void*)application_impl);
    } else if (caps & WL_SEAT_CAPABILITY_KEYBOARD &&
            application_impl->keyboard() == nullptr) {
        application_impl->setKeyboard(
            wl_seat_get_keyboard(application_impl->seat()->c_ptr()));
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
    assert(reg->c_ptr() == registry);

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
    } else if (strcmp(interface, "wl_output") == 0) {
        if (application_impl->output() == nullptr) {
            auto interface = bl::WlInterface<bl::WlInterfaceType::Output>();
            application_impl->setOutput(reg->bind(id, interface, 2));
            application_impl->output()->add_listener(output_listener);
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
    int result = wl_display_dispatch(app_impl->display()->c_ptr());
    fprintf(stderr, "result: %d\n", result);
    while (result != -1) {
        result = wl_display_dispatch(app_impl->display()->c_ptr());
        if (app->desktop_surfaces().length() == 0) {
            fprintf(stderr, "All desktop surfaces gone. Quit.\n");
            break;
        }
    }

    app_impl->quit();
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
    this->_pointer = nullptr;
    this->_output = nullptr;

    this->pointer_state.x = 0;
    this->pointer_state.y = 0;
    this->pointer_state.pressed_x = 0;
    this->pointer_state.pressed_y = 0;
    this->pointer_state.static_click_count = 0;

    // Toy cursor. Change this later.
    this->_cursor = nullptr;

    this->_xdg_wm_base = nullptr;

    fprintf(stderr, "!!! get_registry()\n");
    this->_registry = this->_display.get_registry();

    wl_registry_add_listener(this->_registry.c_ptr(),
        &registry_listener, (void*)this);

    this->_display.dispatch();
    this->_display.roundtrip();

    this->_xdg_wm_base->add_listener(xdg_wm_base_listener);

//    wl_seat_add_listener(this->_seat,
//        &seat_listener, (void*)this);

    // Singleton.
    app_impl = this;

    // OpenGL.
    if (!GL_VERSION_4_6) {
        fprintf(stderr, "No GL_VERSION_4_6!\n");
    }
    fprintf(stderr, "Has GL_VERSION_4_6\n");

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

void ApplicationImpl::quit()
{
    this->_q_core_application->quit();
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

std::shared_ptr<WlPointer> ApplicationImpl::pointer() const
{
    return this->_pointer;
}

void ApplicationImpl::setPointer(std::shared_ptr<WlPointer> pointer)
{
    this->_pointer = pointer;
}

std::shared_ptr<WlOutput> ApplicationImpl::output()
{
    return this->_output;
}

void ApplicationImpl::setOutput(std::shared_ptr<WlOutput> output)
{
    this->_output = output;
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

Cursor* ApplicationImpl::cursor()
{
    return this->_cursor;
}

} // namespace bl
