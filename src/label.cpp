#include <blusher/label.h>

// Pango
#include <pango/pangocairo.h>

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/color.h>
#include <blusher/point.h>
#include <blusher/image.h>
#include <blusher/utils.h>

static void draw_text(cairo_t *cr, const char *text, const bl::Color& color,
        double font_size)
{
    PangoLayout *layout;
    PangoFontDescription *desc;

    layout = pango_cairo_create_layout(cr);

    pango_layout_set_text(layout, text, -1);

    desc = pango_font_description_from_string("Noto Serif CJK KR");

    pango_font_description_set_absolute_size(desc,
        pixel_to_pango_absolute_size(font_size));

    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);

    cairo_save(cr);

    cairo_set_source_rgba(cr,
        color.red_f(), color.green_f(), color.blue_f(), color.alpha_f());

    pango_cairo_update_layout(cr, layout);

    cairo_move_to(cr, 0, 0);
    pango_cairo_show_layout(cr, layout);

    cairo_restore(cr);

    g_object_unref(layout);
}

namespace bl {

Label::Label(View *parent)
    : View(parent)
{
    // View.
    this->set_width(100);
    this->set_height(32);

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

    // Set background.
    this->fill(this->_background_color);

    // Draw text.
    cairo_surface_t *surface;

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
        this->width(), this->height());

    cairo_t *cr = cairo_create(surface);

    draw_text(cr, text.c_str(), this->_color, 12);

    // Add image.
    Image image(cairo_image_surface_get_data(surface),
        this->width(),
        this->height()
    );
    this->draw_image(Point(0, 0), image);

    // Free resources.
    cairo_surface_destroy(surface);
    cairo_destroy(cr);

    // Update.
    this->update();
}

} // namespace bl
