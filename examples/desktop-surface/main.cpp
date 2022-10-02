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

    desktop_surface.append_view(&view);

    // surface.set_color(bl::Color::from_rgba(100, 100, 100, 100));
    desktop_surface.show();

    /*
    bl::Surface subsurface(&desktop_surface);
    subsurface.set_geometry(25, 25, 150, 150);

    bl::View view2(subsurface.root_view());
    view2.fill(bl::Color::from_rgb(0, 0, 255));
    view2.paint();
    view2.update();

    subsurface.show();
    */

    return app.exec();
}
