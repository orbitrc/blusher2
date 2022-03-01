#include <blusher/application.h>
#include <blusher/window.h>
#include <blusher/surface.h>
#include <blusher/color.h>

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    bl::Surface surface;
    // bl::Window window;

    // surface.set_color(bl::Color::from_rgba(100, 100, 100, 100));
    // window.show();
    surface.show();

    bl::Surface subsurface(&surface);
    subsurface.set_color(bl::Color::from_rgba(255, 0, 0, 100));
    subsurface.set_geometry(50, 50, 50, 50);
    subsurface.show();
    surface.show();

    return app.exec();
}
