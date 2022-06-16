#ifndef _BL_XDG_TOPLEVEL_H
#define _BL_XDG_TOPLEVEL_H

struct xdg_toplevel;

namespace bl {

class XdgToplevel
{
public:
    XdgToplevel(struct xdg_toplevel *xdg_toplevel);

    ~XdgToplevel();

private:
    struct xdg_toplevel *_xdg_toplevel;
};

} // namespace bl

#endif // _BL_XDG_TOPLEVEL_H
