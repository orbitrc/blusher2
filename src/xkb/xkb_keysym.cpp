#include <blusher/xkb/keysym.h>

namespace bl {

namespace xkb {

Keysym::Keysym(uint32_t keysym)
{
    this->_xkb_keysym = keysym;
}

Keysym::operator uint32_t() const
{
    return static_cast<uint32_t>(this->_xkb_keysym);
}

} // namespace xkb

} // namespace bl
