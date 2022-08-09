#include <blusher/application.h>
#include <blusher/window.h>
#include <blusher/desktop-surface.h>
#include <blusher/color.h>

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    bl::DesktopSurface desktop_surface(bl::DesktopSurface::Role::Toplevel);
    // bl::Window window;

    bl::View view(desktop_surface.root_view());
    view.fill(bl::Color::from_rgb(255, 0, 0));
    view.paint();
    view.update();

    // surface.set_color(bl::Color::from_rgba(100, 100, 100, 100));
    desktop_surface.show();

    /*
    bl::Surface subsurface(&surface);
    subsurface.set_color(bl::Color::from_rgba(255, 0, 0, 100));
    subsurface.set_geometry(50, 50, 150, 150);
    subsurface.show();
    surface.show();
    */

    return app.exec();
}
