#ifndef _BL_XKB_CONTEXT_H
#define _BL_XKB_CONTEXT_H

#include <xkbcommon/xkbcommon.h>

namespace bl {

namespace xkb {

class Context
{
public:
    Context();
    Context(const Context& other);
    ~Context();

    struct xkb_context* c_ptr();

private:
    struct xkb_context *_xkb_context;
};

} // namespace xkb

} // namespace bl

#endif /* _BL_XKB_CONTEXT_H */
