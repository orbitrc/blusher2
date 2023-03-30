#include <blusher/xkb/context.h>

namespace bl {

namespace xkb {

Context::Context()
{
    this->_xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
}

Context::Context(const Context& other)
{
    this->_xkb_context = other._xkb_context;
    xkb_context_ref(other._xkb_context);
}

Context::~Context()
{
    xkb_context_unref(this->_xkb_context);
}

struct xkb_context* Context::c_ptr()
{
    return this->_xkb_context;
}

} // namespace xkb

} // namespace bl
