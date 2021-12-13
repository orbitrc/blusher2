use std::ffi::c_void;
use blusher::{Surface};
use blusher::{Application, Window, PlainSurface};

fn main() {
    let app = Application::new();
    let window = Window::new();
    app.add_window(&window);

    window.show();

    let surface = PlainSurface::new(None);
    surface.show();
    let surface2 = PlainSurface::new(Some(&surface));
    surface2.show();

    app.exec();
}
