#ifndef _BL_LABEL_H
#define _BL_LABEL_H

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/view.h>
#include <blusher/color.h>
#include <blusher/text.h>

namespace bl {

class Label : public View
{
public:
    Label(View *parent);

    const pr::String& text() const;

    void set_text(const pr::String& text);

    TextAlignment horizontal_alignment() const;

    void set_horizontal_alignment(TextAlignment alignment);

    TextAlignment vertical_alignment() const;

    void set_vertical_alignment(TextAlignment alignment);

    //===================
    // Override Methods
    //===================
    void paint() override;

private:
    pr::String _text;
    Color _color;
    Color _background_color;
    TextAlignment _horizontal_alignment;
    TextAlignment _vertical_alignment;
};

} // namespace bl

#endif // _BL_LABEL_H
