use std::ffi::c_void;
use blusher::{Application, Window};

fn main() {
    let app = Application::new();
    let window = Window::new();
    app.add_window(&window);

    window.show();

    app.exec();
}
