#ifndef _BL_IMAGE_H
#define _BL_IMAGE_H

// C
#include <stdint.h>

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/color.h>

namespace bl {

class Image
{
public:
    enum class Format {
        Invalid,
        /// Most format is Argb32.
        Argb32,
        Rgb32,
        /// OpenGL uses Rgba32 format.
        Rgba32,
    };

    enum class FileFormat {
        Invalid,
        Png,
    };

    enum class Scale {
        NoScale,
        NoScaleRepeat,
        Fit,
    };

public:
    Image(const pr::String& path);

    /// Construct Image from data. The data is a file data, not a pixel data.
    Image(const uint8_t *data, uint64_t size);

    /// Construct empty image with size and format.
    Image(uint64_t width, uint64_t height,
            Image::Format format = Image::Format::Argb32);

    ~Image();

    uint64_t width() const;

    uint64_t height() const;

    Image::Format format() const;

    const uint8_t* data() const;

    /// Fill every pixels as given color.
    void fill(const Color& color);

    /// Resize image size. If `scale` is `NoScale` and new size is larger,
    /// fill transparent pixels.
    void resize(uint64_t width, uint64_t height,
            Image::Scale scale = Image::Scale::NoScale);

    /// Adds an image.
    void add(const Image& image, uint64_t x, uint64_t y);

    /// Returns new image that format changed.
    Image converted(Image::Format format) const;

private:
    uint64_t _width;
    uint64_t _height;
    Image::Format _format;
    uint8_t *_data; // This should be free in destructor.
};

} // namespace bl

#endif // _BL_IMAGE_H
