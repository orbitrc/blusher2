#ifndef _BL_WL_KEYBOARD_H
#define _BL_WL_KEYBOARD_H

// C
#include <stdint.h>

// Wayland
#include <wayland-client.h>

namespace bl {

class WlKeyboard
{
public:
    class Listener
    {
    public:
        using KeymapHandler = void (void *data,
            struct wl_keyboard *wl_keyboard,
            uint32_t format,
            int32_t fd,
            uint32_t size);
        using EnterHandler = void (void *data,
            struct wl_keyboard *wl_keyboard,
            uint32_t serial,
            struct wl_surface *surface,
            struct wl_array *keys);
        using LeaveHandler = void (void *data,
            struct wl_keyboard *wl_keyboard,
            uint32_t serial,
            struct wl_surface *surface);
        using KeyHandler = void (void *data,
            struct wl_keyboard *wl_keyboard,
            uint32_t serial,
            uint32_t time,
            uint32_t key,
            uint32_t state);
        using ModifiersHandler = void (void *data,
            struct wl_keyboard *wl_keyboard,
            uint32_t serial,
            uint32_t mods_depressed,
            uint32_t mods_latched,
            uint32_t mods_locked,
            uint32_t group);
        using RepeatInfoHandler = void (void *data,
            struct wl_keyboard *wl_keyboard,
            int32_t rate,
            int32_t delay);

    public:
        Listener();

        Listener(KeymapHandler keymap,
                EnterHandler enter,
                LeaveHandler leave,
                KeyHandler key,
                ModifiersHandler modifiers,
                RepeatInfoHandler repeat_info);

        ~Listener();

        struct wl_keyboard_listener* wl_keyboard_listener() const;

    private:
        struct wl_keyboard_listener *_wl_keyboard_listener;
    };

public:
    WlKeyboard(struct wl_keyboard *wl_keyboard);

    ~WlKeyboard();

    struct wl_keyboard* c_ptr();

    void add_listener(const WlKeyboard::Listener& listener);

    void add_listener(const WlKeyboard::Listener& listener, void *data);

private:
    struct wl_keyboard *_wl_keyboard;
    WlKeyboard::Listener _listener;
};

} // namespace bl

#endif // _BL_WL_KEYBOARD_H
