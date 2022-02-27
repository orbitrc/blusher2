#ifndef _BL_TITLE_BAR_H
#define _BL_TITLE_BAR_H

#include "surface.h"

namespace bl {

class TitleBar : public Surface
{
public:
    TitleBar(Surface *parent);

protected:
    void pointer_press_event(std::shared_ptr<PointerEvent> event) override;
};

} // namespace bl

#endif /* _BL_TITLE_BAR_H */
