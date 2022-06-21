#include <blusher/image.h>

// C
#include <stdlib.h>
#include <string.h>

// Cairo
#include <cairo.h>

//===================
// Helper Functions
//===================
static bl::Image::FileFormat get_image_file_format(const uint8_t *data,
        uint32_t min_length)
{
    if (min_length > 8) {
        if (data[0] == 0x89 && data[1] == 0x50 &&
                data[2] == 0x4e && data[3] == 0x47 &&
                data[4] == 0x0d && data[5] == 0x0a &&
                data[6] == 0x1a && data[7] == 0x0a) {
            return bl::Image::FileFormat::Png;
        }
    }

    return bl::Image::FileFormat::Invalid;
}

typedef struct bl_cairo_png_closure {
    unsigned char const* data;
    unsigned int read;
} bl_cairo_png_closure;

static cairo_status_t cairo_read_png_func(void *closure,
        unsigned char *data, unsigned int length)
{
    bl_cairo_png_closure *c = static_cast<bl_cairo_png_closure*>(closure);

    for (unsigned int i = 0; i < length; ++i) {
        data[i] = c->data[i + c->read];
    }
    c->read += length;

    return CAIRO_STATUS_SUCCESS;
}

static bl::Image::Format cairo_format_to_bl_image_format(cairo_format_t c_f)
{
    switch (c_f) {
    case CAIRO_FORMAT_ARGB32:
        return bl::Image::Format::Argb32;
    case CAIRO_FORMAT_RGB24:
        return bl::Image::Format::Rgb32;
    default:
        return bl::Image::Format::Invalid;
    }
}

uint8_t* load_png_from_data(const uint8_t *data, uint64_t *size,
        uint64_t *width, uint64_t *height, bl::Image::Format *format)
{
    uint8_t *ret = nullptr;

    bl_cairo_png_closure closure = {
        data,
        0,
    };
    // Create Cairo surface.
    cairo_surface_t *cairo_surface =
        cairo_image_surface_create_from_png_stream(
            cairo_read_png_func,
            &closure
        );
    // Create Cairo.
    cairo_t *cr = cairo_create(cairo_surface);
    if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
        // TODO: Error.
        return nullptr;
    }
    // Get image width and height.
    *width = cairo_image_surface_get_width(cairo_surface);
    *height = cairo_image_surface_get_height(cairo_surface);

    // Get Cairo image format.
    cairo_format_t cairo_format =
        cairo_image_surface_get_format(cairo_surface);
    *format = cairo_format_to_bl_image_format(cairo_format);

    // Set data.
    // !! sizeof(uint32_t) is only for Argb32 format.
    uint64_t data_size = sizeof(uint32_t) * (*width * *height);
    *size = data_size;
    ret = new uint8_t[data_size];
    memcpy(
        ret,
        cairo_image_surface_get_data(cairo_surface),
        data_size
    );

    // Free the Cairo resources.
    cairo_surface_destroy(cairo_surface);
    cairo_destroy(cr);

    return ret;
}

uint8_t* load_png_from_path(const char *path, uint64_t *size,
        uint64_t *width, uint64_t *height, bl::Image::Format *format)
{
    uint8_t *ret = nullptr;

    cairo_surface_t *cairo_surface = cairo_image_surface_create_from_png(
        path);

    // Get image width and height.
    *width = cairo_image_surface_get_width(cairo_surface);
    *height = cairo_image_surface_get_height(cairo_surface);

    // Get Cairo image format.
    cairo_format_t cairo_format =
        cairo_image_surface_get_format(cairo_surface);
    *format = cairo_format_to_bl_image_format(cairo_format);

    // Set data.
    uint64_t data_size = sizeof(uint32_t) * (*width * *height);
    *size = data_size;
    ret = new uint8_t[data_size];
    memcpy(
        ret,
        cairo_image_surface_get_data(cairo_surface),
        data_size
    );

    // Free the Cairo resources.
    cairo_surface_destroy(cairo_surface);

    return ret;
}


namespace bl {

Image::Image(const pr::String& path)
{
    this->_width = 0;
    this->_height = 0;
    this->_format = Image::Format::Invalid;
    this->_data = nullptr;

    const auto path_s = path.unicode_scalars();
    if (path_s.length() > 4 &&
            path.starts_with("brc:"_S)) {
        // Path is blusher resource.
    } else {
        // Path is file system path.

        // TODO: Check if PNG.
        uint64_t image_size = 0;
        this->_data = load_png_from_path(path.c_str(), &image_size,
            &this->_width, &this->_height,
            &this->_format);
    }
}

Image::Image(const uint8_t *data, uint64_t size)
{
    this->_width = 0;
    this->_height = 0;
    this->_format = Image::Format::Invalid;
    this->_data = nullptr;

    Image::FileFormat file_format = get_image_file_format(data, size);

    if (file_format == Image::FileFormat::Png) {
        uint64_t image_size = 0;
        this->_data = load_png_from_data(data, &image_size,
            &this->_width, &this->_height,
            &this->_format);
    } else if (file_format == Image::FileFormat::Invalid) {
        // Do nothing.
    }
}

Image::Image(uint64_t width, uint64_t height, Image::Format format)
{
    this->_width = width;
    this->_height = height;
    this->_format = format;

    if (format == Image::Format::Argb32) {
        auto pixel_size = sizeof(uint8_t) * 4;
        this->_data = (uint8_t*)malloc(
            pixel_size * (width * height));
        auto data_size = pixel_size * (width * height);
        // Fill transparent.
        for (uint64_t i = 0; i < data_size; ++i) {
            this->_data[i] = 0x00;
        }
    } else {
        this->_data = nullptr;
    }
}

Image::~Image()
{
    // TODO: Delete data.
}

uint64_t Image::width() const
{
    return this->_width;
}

uint64_t Image::height() const
{
    return this->_height;
}

Image::Format Image::format() const
{
    return this->_format;
}

const uint8_t* Image::data() const
{
    return this->_data;
}

void Image::fill(const Color& color)
{
    if (this->_format == Image::Format::Argb32) {
        uint32_t *pixel = (uint32_t*)this->_data;
        for (uint64_t i = 0; i < this->_width * this->_height; ++i) {
            *pixel = color.to_argb();
            ++pixel;
        }
    } else {
        // TODO.
    }
}

} // namespace bl
