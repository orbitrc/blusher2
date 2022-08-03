#include <blusher/pointer-state.h>

namespace bl {

PointerState::PointerState()
{
    this->x = 0;
    this->y = 0;
    this->button = PointerButton::None;

    this->_pressed = false;
}

bool PointerState::pressed() const
{
    return this->_pressed;
}

void PointerState::set_pressed(bool value)
{
    this->_pressed = value;
}

} // namespace bl
