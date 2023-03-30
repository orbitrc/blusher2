#ifndef _BL_XKB_STATE_H
#define _BL_XKB_STATE_H

// C++
#include <memory>

#include <xkbcommon/xkbcommon.h>

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/xkb/keymap.h>
#include <blusher/xkb/keysym.h>

namespace bl {

namespace xkb {

class State
{
public:
    State(std::shared_ptr<Keymap> keymap);
    ~State();

    Keysym keycode_to_keysym(uint32_t keycode);

    pr::Unicode::Scalar keycode_to_unicode_scalar(uint32_t keycode);

    bool is_mod_active(const pr::String& mod_name);

    void update(uint32_t depressed, uint32_t latched, uint32_t locked,
            uint32_t layout);

private:
    std::shared_ptr<Keymap> _keymap;

    struct xkb_state *_xkb_state;
};

} // namespace xkb

} // namespace bl

#endif /* _BL_XKB_STATE_H */
