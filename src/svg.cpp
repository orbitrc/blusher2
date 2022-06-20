#include <blusher/svg.h>

#include <blusher/svg/svg-core.h>

namespace bl {

Svg::Svg(const pr::String& path)
{
    auto path_s = path.unicode_scalars();
    if (path_s.length() > 4 &&
            path.starts_with("brc:"_S)) {
        // Path is Blusher resource path.
        // TODO: Implementation.
    } else {
        // Path is filesystem path.
        // TODO: Implementation.
    }
}

} // namespace bl
