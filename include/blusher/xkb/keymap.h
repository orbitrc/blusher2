#ifndef _BL_XKB_KEYMAP_H
#define _BL_XKB_KEYMAP_H

// C++
#include <memory>

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/xkb/context.h>

namespace bl {

namespace xkb {

class Keymap
{
public:
    Keymap(std::shared_ptr<Context> context, const pr::String& string);
    ~Keymap();

    struct xkb_keymap* c_ptr();

private:
    std::shared_ptr<Context> _context;

    struct xkb_keymap *_xkb_keymap;
};

} // namespace xkb

} // namespace bl

#endif /* _BL_XKB_KEYMAP_H */
