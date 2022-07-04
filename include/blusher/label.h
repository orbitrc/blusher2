#ifndef _BL_LABEL_H
#define _BL_LABEL_H

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/view.h>
#include <blusher/color.h>

namespace bl {

class Label : public View
{
public:
    Label(View *parent);

    const pr::String& text() const;

    void set_text(const pr::String& text);

private:
    pr::String _text;
    Color _color;
    Color _background_color;
};

} // namespace bl

#endif // _BL_LABEL_H
