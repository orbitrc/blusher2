#include <blusher/svg.h>

// Blusher SVG
#include <blusher/svg/svg-core.h>

// Blusher
#include <blusher/application.h>
#include <blusher/resource.h>

namespace bl {

Svg::Svg(const pr::String& path)
{
    auto path_s = path.unicode_scalars();
    if (path_s.length() > 4 &&
            path.starts_with("brc:"_S)) {
        // Path is Blusher resource path.
        // TODO: Implementation.
        const char *data_path = path.c_str() + 4;
        const Resource::Data *data = app->resource()->data(data_path);
        if (data == nullptr) {
            //
            return;
        }
        this->_core = bl_svg_core_from_data(data->data(), data->size());
    } else {
        // Path is filesystem path.
        // TODO: Implementation.
    }
}

Image Svg::to_image(uint64_t width, uint64_t height) const
{
    uint32_t size;
    const unsigned char *png = bl_svg_core_png_data(this->_core,
        width, height, &size);

    if (png == nullptr) {
        //
    }

    return Image(png, size);
}

} // namespace bl
