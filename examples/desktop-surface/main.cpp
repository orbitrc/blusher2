#include <blusher/application.h>
#include <blusher/view.h>
#include <blusher/desktop-surface.h>
#include <blusher/color.h>

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    bl::DesktopSurface desktop_surface(bl::DesktopSurface::Role::Toplevel);

    desktop_surface.set_size(640, 360);

    bl::View view(desktop_surface.root_view());
    view.set_size({50, 50});
    view.fill(bl::Color::from_rgba(255, 0, 0, 150));
    view.paint();
    view.update();

    desktop_surface.show();

    return app.exec();
}
