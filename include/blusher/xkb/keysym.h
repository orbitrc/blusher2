#ifndef _BL_XKB_KEYSYM_H
#define _BL_XKB_KEYSYM_H

#include <stdint.h>

#include <xkbcommon/xkbcommon.h>

namespace bl {

namespace xkb {

class Keysym
{
public:
    Keysym(uint32_t keysym);

    operator uint32_t() const;

private:
    xkb_keysym_t _xkb_keysym;
};

} // namespace xkb

} // namespace bl

#endif /* _BL_XKB_KEYSYM_H */
