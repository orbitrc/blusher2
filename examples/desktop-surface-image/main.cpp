#include <blusher/application.h>
#include <blusher/view.h>
#include <blusher/desktop-surface.h>
#include <blusher/color.h>
#include <blusher/image.h>

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    bl::DesktopSurface desktop_surface(bl::DesktopSurface::Role::Toplevel);

    desktop_surface.set_size(640, 360);

    bl::View view(desktop_surface.root_view());
    view.set_position({20, 20});
    view.set_size({200, 200});
    view.fill(bl::Color::from_rgba(255, 0, 0, 150));
    view.set_debug_id("RedBox"_S);

    bl::Image image("brc:/io.orbitrc.blusher/sample.png"_S);

    bl::View image_view(&view);
    image_view.set_size({150, 150});
    image_view.set_fill_type(bl::View::FillType::Image);
    image_view.draw_image({0, 0}, image);
    image_view.set_debug_id("ImageView"_S);

    desktop_surface.show();

    return app.exec();
}
