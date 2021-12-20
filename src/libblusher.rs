use std::ffi::c_void;

extern "C" {
    //================
    // Application
    //================
    pub fn bl_application_new() -> *mut c_void;

    pub fn bl_application_add_window(application: *mut c_void, window: *mut c_void);

    pub fn bl_application_remove_window(application: *mut c_void, window: *mut c_void);

    pub fn bl_application_exec(application: *mut c_void) -> i32;

    pub fn bl_application_free(application: *mut c_void);

    //===============
    // Window
    //===============
    pub fn bl_window_new() -> *mut c_void;

    pub fn bl_window_show(window: *mut c_void);

    pub fn bl_window_set_body(window: *mut c_void, surface: *mut c_void);

    //===============
    // Surface
    //===============
    pub fn bl_surface_new(parent: *mut c_void) -> *mut c_void;

    pub fn bl_surface_set_parent(surface: *mut c_void, parent: *mut c_void);

    pub fn bl_surface_set_geometry(surface: *mut c_void, x: f64, y: f64, width: f64, height: f64);

    pub fn bl_surface_set_color(surface: *mut c_void, color: bl_color);

    pub fn bl_surface_paint(surface: *mut c_void);

    pub fn bl_surface_show(surface: *mut c_void);

    pub fn bl_surface_set_pointer_press_event(surface: *mut c_void, event: fn(&dyn super::Surface, &super::PointerEvent));

    pub fn bl_surface_free(surface: *mut c_void);

    //==============
    // Color
    //==============
    pub fn bl_color_from_rgb(r: u32, g: u32, b: u32) -> bl_color;

    pub fn bl_color_from_rgba(r: u32, g: u32, b: u32, a: u32) -> bl_color;

    //================
    // Pointer Event
    //================
    pub fn bl_pointer_event_new() -> *mut c_void;

    pub fn bl_pointer_event_free(pointer_event: *mut c_void);
}

//=============
// Color
//=============
#[repr(C)]
pub struct bl_color {
    red: u32,
    green: u32,
    blue: u32,
    alpha: u32,
}
