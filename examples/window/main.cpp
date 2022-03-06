#include <blusher/application.h>
#include <blusher/window.h>
#include <blusher/surface.h>
#include <blusher/color.h>

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    // bl::Surface surface;
    bl::Window window;

    // surface.set_color(bl::Color::from_rgba(100, 100, 100, 100));
    window.show();
    // surface.show();

    return app.exec();
}
