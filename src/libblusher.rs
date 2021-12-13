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
}
