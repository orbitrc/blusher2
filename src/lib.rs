mod libblusher;

use std::ffi::c_void;
use std::ptr::null_mut;

//================
// Application
//================
pub struct Application {
    application: *mut c_void,
}

impl Application {
    pub fn new() -> Application {
        unsafe {
            let application = Application {
                application: libblusher::bl_application_new(),
            };

            application
        }
    }

    pub fn add_window(&self, window: &Window) {
        unsafe {
            libblusher::bl_application_add_window(self.application, window.window);
        }
    }

    pub fn exec(&self) -> i32 {
        unsafe {
            libblusher::bl_application_exec(self.application)
        }
    }
}

impl Drop for Application {
    fn drop(&mut self) {
        unsafe {
            libblusher::bl_application_free(self.application);
        }
    }
}

//=============
// Window
//=============
pub struct Window {
    window: *mut c_void,
    body: PlainSurface,
}

impl Window {
    pub fn new() -> Window {
        unsafe {
            let bl_window = libblusher::bl_window_new();
            let body = PlainSurface::new(None);
            libblusher::bl_window_set_body(bl_window, body.bl_surface);

            let window = Window {
                window: bl_window,
                body: body,
            };

            window
        }
    }

    pub fn body(&self) -> &PlainSurface {
        &self.body
    }

    pub fn show(&self) {
        unsafe {
            libblusher::bl_window_show(self.window);
        }
    }
}

//=================
// Trait Surface
//=================
pub trait Surface {
    /// Show the surface.
    fn show(&self);

    /// Set geometry of surface.
    fn set_geometry(&mut self, x: f64, y: f64, width: f64, height: f64);

    fn pointer_press_event(&self, event: &PointerEvent) {
        println!("This is Default pointer_press_event!");
    }

    /// Get underlying C pointer.
    ///
    /// DO NOT USE THIS METHOD IN YOUR APPLICATION!
    fn surface_ptr(&self) -> *mut c_void;
}

//=================
// PlainSurface
//=================
pub struct PlainSurface {
    bl_surface: *mut c_void,
}

impl PlainSurface {
    pub fn new(parent: Option<&dyn Surface>) -> PlainSurface {
        let bl_surface = match parent {
            Some(surface) => {
                surface.surface_ptr()
            }
            None => {
                null_mut()
            }
        };
        let surface = PlainSurface {
            bl_surface: unsafe { libblusher::bl_surface_new(bl_surface) },
        };
        unsafe {
            libblusher::bl_surface_set_pointer_press_event(
                surface.bl_surface,
                |s, evt| Surface::pointer_press_event(s, evt)
            );
        }

        surface
    }

    pub fn set_color(&mut self, color: Color) {
        unsafe {
            let bl_color = libblusher::bl_color_from_rgba(
                color.red() as u32,
                color.green() as u32,
                color.blue() as u32,
                color.alpha() as u32
            );
            libblusher::bl_surface_set_color(self.bl_surface, bl_color);
        }
    }
}

impl Surface for PlainSurface {
    fn show(&self) {
        unsafe {
            libblusher::bl_surface_paint(self.bl_surface);
            libblusher::bl_surface_show(self.bl_surface);
        }
    }

    fn set_geometry(&mut self, x: f64, y: f64, width: f64, height: f64) {
        unsafe {
            libblusher::bl_surface_set_geometry(self.bl_surface, x, y, width, height);
        }
    }

    fn surface_ptr(&self) -> *mut c_void {
        self.bl_surface
    }
}

impl Drop for PlainSurface {
    fn drop(&mut self) {
        unsafe {
            libblusher::bl_surface_free(self.surface_ptr());
        }
    }
}

//============
// Color
//============
pub struct Color {
    red: u8,
    green: u8,
    blue: u8,
    alpha: u8,
}

impl Color {
    pub fn from_rgb(r: u8, g: u8, b: u8) -> Color {
        Color {
            red: r,
            green: g,
            blue: b,
            alpha: 255,
        }
    }

    pub fn from_rgba(r: u8, g: u8, b: u8, a: u8) -> Color {
        Color {
            red: r,
            green: g,
            blue: b,
            alpha: a,
        }
    }

    pub fn red(&self) -> u8 {
        self.red
    }

    pub fn green(&self) -> u8 {
        self.green
    }

    pub fn blue(&self) -> u8 {
        self.blue
    }

    pub fn alpha(&self) -> u8 {
        self.alpha
    }
}

//===============
// PointerEvent
//===============
pub struct PointerEvent {
    bl_pointer_event: *mut c_void,
}

impl PointerEvent {
    pub fn new() -> PointerEvent {
        unsafe {
            PointerEvent {
                bl_pointer_event: libblusher::bl_pointer_event_new(),
            }
        }
    }
}

impl Drop for PointerEvent {
    fn drop(&mut self) {
        unsafe {
            libblusher::bl_pointer_event_free(self.bl_pointer_event);
        }
    }
}
