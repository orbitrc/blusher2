#include <blusher/wayland/wl-keyboard.h>

namespace bl {

//====================
// WlKeyboard::Listener
//====================
WlKeyboard::Listener::Listener()
{
    struct wl_keyboard_listener *listener = new struct wl_keyboard_listener();
    listener->keymap = nullptr;
    listener->enter = nullptr;
    listener->leave = nullptr;
    listener->key = nullptr;
    listener->modifiers = nullptr;
    listener->repeat_info = nullptr;

    this->_wl_keyboard_listener = listener;
}

WlKeyboard::Listener::Listener(WlKeyboard::Listener::KeymapHandler keymap,
        WlKeyboard::Listener::EnterHandler enter,
        WlKeyboard::Listener::LeaveHandler leave,
        WlKeyboard::Listener::KeyHandler key,
        WlKeyboard::Listener::ModifiersHandler modifiers,
        WlKeyboard::Listener::RepeatInfoHandler repeat_info)
{
    struct wl_keyboard_listener *listener = new struct wl_keyboard_listener();
    listener->keymap = keymap;
    listener->enter = enter;
    listener->leave = leave;
    listener->key = key;
    listener->modifiers = modifiers;
    listener->repeat_info = repeat_info;

    this->_wl_keyboard_listener = listener;
}

WlKeyboard::Listener::~Listener()
{
    delete this->_wl_keyboard_listener;
}

struct wl_keyboard_listener* WlKeyboard::Listener::wl_keyboard_listener() const
{
    return this->_wl_keyboard_listener;
}

//================
// WlKeyboard
//================

WlKeyboard::WlKeyboard(struct wl_keyboard *wl_keyboard)
{
    this->_wl_keyboard = wl_keyboard;
}

WlKeyboard::~WlKeyboard()
{
//    wl_pointer_destroy(this->_wl_pointer);
//    wl_pointer_release(this->_wl_pointer);
}

struct wl_keyboard* WlKeyboard::c_ptr()
{
    return this->_wl_keyboard;
}

void WlKeyboard::add_listener(const WlKeyboard::Listener &listener)
{
    this->_listener = listener;
    wl_keyboard_add_listener(this->_wl_keyboard,
        listener.wl_keyboard_listener(), NULL);
}

void WlKeyboard::add_listener(const WlKeyboard::Listener& listener, void *data)
{
    this->_listener = listener;
    wl_keyboard_add_listener(this->_wl_keyboard,
        listener.wl_keyboard_listener(), data);
}

} // namespace bl
