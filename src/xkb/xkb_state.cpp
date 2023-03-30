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

} // namespace xkb

} // namespace bl
