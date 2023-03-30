#include <blusher/xkb/state.h>

namespace bl {

namespace xkb {

State::State(std::shared_ptr<Keymap> keymap)
{
    this->_keymap = keymap;
    this->_xkb_state = xkb_state_new(keymap->c_ptr());
}

State::~State()
{
    xkb_state_unref(this->_xkb_state);
}

Keysym State::keycode_to_keysym(uint32_t keycode)
{
    return xkb_state_key_get_one_sym(this->_xkb_state, keycode);
}

pr::Unicode::Scalar State::keycode_to_unicode_scalar(uint32_t keycode)
{
    uint32_t utf32 = xkb_state_key_get_utf32(this->_xkb_state, keycode);
    return pr::Unicode::Scalar(utf32);
}

bool State::is_mod_active(const pr::String& mod_name)
{
    int ret = xkb_state_mod_name_is_active(this->_xkb_state, mod_name.c_str(),
        XKB_STATE_MODS_DEPRESSED);

    return (ret == 1) ? true : false;
}

void State::update(uint32_t depressed, uint32_t latched, uint32_t locked,
        uint32_t layout)
{
    auto component = xkb_state_update_mask(this->_xkb_state,
        depressed, latched, locked, layout, layout, layout);
    (void)component;
}

} // namespace xkb

} // namespace bl
