use std::process;

use blusher::{Surface};
use blusher::{Application, Window, PlainSurface, Color};

fn main() {
    let app = Application::new();
    let window = Window::new();
    app.add_window(&window);

    window.show();

    let mut surface = PlainSurface::new(Some(window.body()));
    surface.set_geometry(0.0, 0.0, 100.0, 100.0);
    surface.show();
    let mut surface2 = PlainSurface::new(Some(&surface));
    surface2.set_geometry(10.0, 10.0, 50.0, 50.0);
    surface2.set_color(Color::from_rgb(255, 0, 0));
    surface2.show();

    let ret = app.exec();
    if ret != 0 {
        process::exit(ret);
    }
}
