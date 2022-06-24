#include <stdio.h>

#include <blusher/application.h>
#include <blusher/window.h>
#include <blusher/surface.h>
#include <blusher/color.h>
#include <blusher/view.h>
#include <blusher/point.h>
#include <blusher/image.h>

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    // bl::Surface surface;
    bl::Window window;

    // surface.set_color(bl::Color::from_rgba(100, 100, 100, 100));
    window.show();
    // surface.show();

    bl::View view(window.root_view().get());
    // window.root_view()->update();
    view.set_width(160);
    view.set_height(160);
    view.set_color(bl::Color::from_rgb(255, 0, 0));

    bl::Image sample_image("brc:/io.orbitrc.blusher/sample.png");
    view.draw_image(bl::Point(0, 0), sample_image);
    fprintf(stderr, "-- update view\n");
    view.update();
    fprintf(stderr, "-- update view done.\n");
    window.root_view()->update();

    return app.exec();
}
