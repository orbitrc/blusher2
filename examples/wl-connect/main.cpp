#include <stdio.h>
#include <string.h>

#include <blusher/wayland/wl-display.h>
#include <blusher/wayland/wl-output.h>

static void global_registry_handler(void *data, struct wl_registry *registry,
        uint32_t id, const char *interface, uint32_t version)
{
    printf("Got a registry event for <%s>, id %d, version %d.\n",
        interface, id, version);

    if (strcmp(interface, "wl_compositor") == 0) {
        auto registry = bl::WlRegistry::instance();
        auto interface = bl::WlInterface<bl::WlInterfaceType::Compositor>();
        registry->bind(id,
            interface,
            version
        );
    }
}

static void global_registry_remove_handler(void *data,
        struct wl_registry *registry, uint32_t id)
{
}

static const bl::WlRegistry::Listener registry_listener =
    bl::WlRegistry::Listener(
        global_registry_handler,
        global_registry_remove_handler
    );

int main(int argc, char *argv[])
{
    bl::WlDisplay display = bl::WlDisplay::connect();

    bl::WlRegistry registry = display.get_registry();
    registry.add_listener(registry_listener);

    display.dispatch();
    display.roundtrip();
    //

    return 0;
}
