#ifndef _BL_IMAGE_H
#define _BL_IMAGE_H

// C
#include <stdint.h>

// Primer
#include <primer/string.h>

namespace bl {

class Image
{
public:
    enum class Format {
        Invalid,
        Argb32,
        Rgb32,
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

    Image(const uint8_t *data, uint64_t size);

    ~Image();

    uint64_t width() const;

    uint64_t height() const;

    Image::Format format() const;

    const uint8_t* data() const;

private:
    uint64_t _width;
    uint64_t _height;
    Image::Format _format;
    uint8_t *_data; // This should be free in destructor.
};

} // namespace bl

#endif // _BL_IMAGE_H