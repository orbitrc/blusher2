#ifndef _BL_WL_REGISTRY_H
#define _BL_WL_REGISTRY_H

#include <wayland-client.h>

// C
#include <stdint.h>

// Blusher
#include <blusher/wayland/wl-interface.h>

namespace bl {

extern class WlRegistry* wl_registry_singleton;

class WlDisplay;

class WlRegistry
{
    friend WlDisplay;
public:
    //============
    // Listener
    //============
    class Listener
    {
    public:
        using GlobalHandler = void (void *data,
            struct wl_registry *wl_registry,
            uint32_t name,
            const char *interface,
            uint32_t version);
        using GlobalRemoveHandler = void (void *data,
            struct wl_registry *wl_registry,
            uint32_t name);

    public:
        Listener();

        Listener(GlobalHandler global, GlobalRemoveHandler remove);

        const struct wl_registry_listener* wl_registry_listener() const;

    private:
        struct wl_registry_listener _listener;
    };

public:
    WlRegistry();

    ~WlRegistry();

    struct wl_registry* wl_registry();

    void add_listener(const WlRegistry::Listener& listener);

    WlInterface<WlInterfaceType::Output>::BindType bind(uint32_t id,
            WlInterface<WlInterfaceType::Output>& interface,
            uint32_t version);

    WlInterface<WlInterfaceType::Compositor>::BindType bind(uint32_t id,
            WlInterface<WlInterfaceType::Compositor>& interface,
            uint32_t version);

    /// Returns singleton instance.
    static WlRegistry* instance();

private:
    struct wl_registry *_wl_registry;
    WlRegistry::Listener _listener;
};

} // namespace bl

#endif // _BL_WL_REGISTRY_H
