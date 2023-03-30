#include <blusher/xkb/keymap.h>

namespace bl {

namespace xkb {

Keymap::Keymap(std::shared_ptr<Context> context, const pr::String& string)
{
    this->_context = context;
    this->_xkb_keymap = xkb_keymap_new_from_string(context->c_ptr(),
        string.c_str(),
        XKB_KEYMAP_FORMAT_TEXT_V1,
        XKB_KEYMAP_COMPILE_NO_FLAGS);
}

Keymap::~Keymap()
{
    xkb_keymap_unref(this->_xkb_keymap);
}

struct xkb_keymap* Keymap::c_ptr()
{
    return this->_xkb_keymap;
}

} // namespace xkb

} // namespace bl
