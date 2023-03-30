#ifndef _BL_XKB_STATE_H
#define _BL_XKB_STATE_H

// C++
#include <memory>

#include <xkbcommon/xkbcommon.h>

// Blusher
#include <blusher/xkb/keymap.h>

namespace bl {

namespace xkb {

class State
{
public:
    State(std::shared_ptr<Keymap> keymap);
    ~State();

private:
    std::shared_ptr<Keymap> _keymap;

    struct xkb_state *_xkb_state;
};

} // namespace xkb

} // namespace bl

#endif /* _BL_XKB_STATE_H */
