#include <blusher/wayland/wl-registry.h>

// C
#include <assert.h>

// Wayland
#include <wayland-client.h>

// Blusher
#include <blusher/wayland/wl-output.h>
#include <blusher/wayland/wl-compositor.h>

namespace bl {

//=======================
// WlRegistry::Listener
//=======================

WlRegistry::Listener::Listener()
{
    this->_listener = {
        nullptr,
        nullptr,
    };
}

WlRegistry::Listener::Listener(
        WlRegistry::Listener::GlobalHandler global,
        WlRegistry::Listener::GlobalRemoveHandler global_remove)
{
    this->_listener = {
        global,
        global_remove,
    };
}

using Listener = WlRegistry::Listener;
const struct wl_registry_listener* WlRegistry::Listener::wl_registry_listener() const
{
    return &this->_listener;
}

//===============
// WlRegistry
//===============
WlRegistry *wl_registry_singleton = nullptr;

WlRegistry::WlRegistry()
{
    this->_wl_registry = nullptr;
}

WlRegistry::WlRegistry(struct wl_registry *wl_registry)
{
    assert(wl_registry_singleton == nullptr);

    this->_wl_registry = wl_registry;

    wl_registry_singleton = this;
}

WlRegistry::~WlRegistry()
{
}

struct wl_registry* WlRegistry::wl_registry()
{
    return this->_wl_registry;
}

void WlRegistry::add_listener(const WlRegistry::Listener& listener)
{
    this->_listener = listener;
    wl_registry_add_listener(this->_wl_registry,
        listener.wl_registry_listener(),
        NULL);
}

//==========
// Binds
//==========
std::shared_ptr<WlOutputInterface::BindType> WlRegistry::bind(uint32_t id,
        WlOutputInterface& interface,
        uint32_t version)
{
    struct wl_output *wl_output =
        static_cast<struct wl_output*>(
            wl_registry_bind(
                this->_wl_registry,
                id,
                interface.wl_interface(),
                version
            ));
    std::shared_ptr<WlOutput> output(new WlOutput(wl_output));

    return output;
}

std::shared_ptr<WlCompositorInterface::BindType> WlRegistry::bind(uint32_t id,
        WlCompositorInterface& interface,
        uint32_t version)
{
    struct wl_compositor *wl_compositor =
        static_cast<struct wl_compositor*>(
            wl_registry_bind(
                this->_wl_registry,
                id,
                interface.wl_interface(),
                version
            ));
    std::shared_ptr<WlCompositor> compositor(new WlCompositor(wl_compositor));

    return compositor;
}

std::shared_ptr<WlSubcompositorInterface::BindType> WlRegistry::bind(uint32_t id,
        WlSubcompositorInterface& interface,
        uint32_t version)
{
    struct wl_subcompositor *wl_subcompositor =
        static_cast<struct wl_subcompositor*>(
            wl_registry_bind(
                this->_wl_registry,
                id,
                interface.wl_interface(),
                version
            ));
    std::shared_ptr<WlSubcompositor> subcompositor(new WlSubcompositor(wl_subcompositor));

    return subcompositor;
}

std::shared_ptr<XdgWmBaseInterface::BindType> WlRegistry::bind(uint32_t id,
        XdgWmBaseInterface& interface,
        uint32_t version)
{
    struct xdg_wm_base *xdg_wm_base =
        static_cast<struct xdg_wm_base*>(
            wl_registry_bind(
                this->_wl_registry,
                id,
                interface.wl_interface(),
                version
            ));
    std::shared_ptr<XdgWmBase> base(new XdgWmBase(xdg_wm_base));

    return base;
}

WlRegistry* WlRegistry::instance()
{
    return wl_registry_singleton;
}

//====================
// Bind Methods
//====================


} // namespace bl
