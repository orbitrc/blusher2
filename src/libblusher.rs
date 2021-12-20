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

    pub fn bl_surface_paint(surface: *mut c_void);

    pub fn bl_surface_show(surface: *mut c_void);

    pub fn bl_surface_free(surface: *mut c_void);
}
