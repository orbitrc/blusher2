#include <blusher/label.h>

// Pango
#include <pango/pango.h>

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/color.h>
#include <blusher/utils.h>

namespace bl {

Label::Label(View *parent)
    : View(parent)
{
    // View.
    this->set_width(100);
    this->set_height(16);

    // Initialize.
    this->_text = ""_S;
    this->_color = Color::from_rgb(0, 0, 0);
    this->_background_color = Color::from_rgba(0, 0, 0, 0);
}

const pr::String& Label::text() const
{
    return this->_text;
}

void Label::set_text(const pr::String& text)
{
    this->_text = text;
}

} // namespace bl
