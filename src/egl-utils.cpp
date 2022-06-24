#include "egl-utils.h"

const char* egl_error_to_string(EGLint err)
{
    switch (err) {
    case EGL_BAD_ACCESS:
        return "EGL_BAD_ACCESS";
    case EGL_BAD_SURFACE:
        return "EGL_BAD_SURFACE";
    case EGL_BAD_NATIVE_WINDOW:
        return "EGL_BAD_NATIVE_WINDOW";
    case EGL_BAD_MATCH:
        return "EGL_BAD_MATCH";
    case EGL_BAD_CURRENT_SURFACE:
        return "EGL_BAD_CURRENT_SURFACE";
    case EGL_BAD_ALLOC:
        return "EGL_BAD_ALLOC";
    case EGL_CONTEXT_LOST:
        return "EGL_CONTEXT_LOST";
    case EGL_NOT_INITIALIZED:
        return "EGL_NOT_INITIALIZED";
    case EGL_SUCCESS:
        return "EGL_SUCCESS";
    default:
        return "Error Not Catched!";
    }
}
