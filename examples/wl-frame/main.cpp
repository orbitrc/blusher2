#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <memory>

#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <primer/string.h>

#include <blusher/image.h>
#include <blusher/wayland/wl-display.h>
#include <blusher/wayland/wl-compositor.h>
#include <blusher/wayland/wl-surface.h>
#include <blusher/wayland/wl-output.h>
#include <blusher/wayland/xdg-wm-base.h>
#include <blusher/wayland/xdg-toplevel.h>

std::shared_ptr<bl::WlCompositor> compositor = nullptr;
std::shared_ptr<bl::XdgWmBase> xdg_wm_base = nullptr;

struct wl_egl_window *egl_window = nullptr;

EGLDisplay egl_display;
EGLConfig egl_config;
EGLSurface egl_surface;
EGLContext egl_context;
GLuint program_object;

bl::Image image("sample.png"_S);

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 360

//=============
// GL/EGL
//=============

GLuint load_shader(const char *shader_src, GLenum type)
{
    GLuint shader;
    GLint compiled;

    // Create the shader object.
    shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }

    // Load the shader source.
    glShaderSource(shader, 1, &shader_src, NULL);

    // Compile the shader.
    glCompileShader(shader);

    // Check the compile status.
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint info_len = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 1) {
            char *info_log = (char*)malloc(sizeof(char));
            glGetShaderInfoLog(shader, info_len, NULL, info_log);
            fprintf(stderr, "Error compiling shader: %s\n", info_log);
            free(info_log);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int init_program(GLuint *program_object)
{
    GLbyte vertex_shader_str[] =
        "#version 300 es \n"
        "layout (location = 0) in vec3 aPos; \n"
        "layout (location = 1) in vec3 aColor; \n"
        "layout (location = 2) in vec2 aTexCoord; \n"
        "out vec3 ourColor; \n"
        "out vec2 TexCoord; \n"
        "void main()                         \n"
        "{                                   \n"
        "    gl_Position = vec4(aPos, 1.0);  \n"
        "    ourColor = aColor;              \n"
        "    TexCoord = aTexCoord;           \n"
        "}                                   \n";

    GLbyte fragment_shader_str[] =
        "#version 300 es  \n"
        "precision mediump float; \n"
        "out vec4 fragColor;      \n"
        "in vec3 ourColor;        \n"
        "in vec2 TexCoord;        \n"
        "uniform sampler2D ourTexture; \n"
        "void main()              \n"
        "{                        \n"
        "    fragColor = texture(ourTexture, TexCoord); \n"
        "}                        \n";

    GLuint vertex_shader;
    GLuint fragment_shader;
    GLint linked;

    vertex_shader = load_shader((const char*)vertex_shader_str, GL_VERTEX_SHADER);
    fragment_shader = load_shader((const char*)fragment_shader_str, GL_FRAGMENT_SHADER);

    *program_object = glCreateProgram();
    if (*program_object == 0) {
        fprintf(stderr, "glCreateProgram() - program_object is 0.\n");
        return 0;
    }

    glAttachShader(*program_object, vertex_shader);
    glAttachShader(*program_object, fragment_shader);

    // Link the program.
    glLinkProgram(*program_object);

    // Check the link status.
    glGetProgramiv(*program_object, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint info_len = 0;
        glGetProgramiv(*program_object, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 1) {
            char *info_log = (char*)malloc(sizeof(char) * info_len);

            glGetProgramInfoLog(*program_object, info_len, NULL, info_log);
            fprintf(stderr, "Error linking program: %s\n", info_log);
            free(info_log);
        }

        glDeleteProgram(*program_object);
        return 0;
    }

    return 1;
}

static void draw_function()
{
    GLfloat vVertices[] = {
        -1.0f, -1.0f,  0.0f,    0.0f, 0.0f, 0.0f,    0.0f,  1.0f,     // Top right
        -1.0f,  1.0f,  0.0f,    0.0f, 0.0f, 0.0f,    0.0f,  0.0f,     // Bottom right
         1.0f,  1.0f,  0.0f,    0.0f, 0.0f, 0.0f,    1.0f,  0.0f,     // Bottom left
         1.0f, -1.0f,  0.0f,    0.0f, 0.0f, 0.0f,    1.0f,  1.0f,     // Top left
    };
    GLuint indices[] = {
        0, 1, 3,    // First triangle
        1, 2, 3,    // Second triangle
    };

    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);

    // Set the viewport.
    glViewport(
        0, WINDOW_HEIGHT - image.height(),
        image.width(), image.height()
    );

    // Clear the color buffer.
    glClearColor(0.5, 0.5, 0.5, 0.8);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object.
    glUseProgram(program_object);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
        GL_STATIC_DRAW);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices,
        GL_STATIC_DRAW);

    // Position attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Texture coord attribute.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        image.width(),
        image.height(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image.data()
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

    eglSwapBuffers(egl_display, egl_surface);
}

static void init_egl()
{
    EGLint major, minor, count, n, size;
    EGLConfig *configs;
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE,
        EGL_WINDOW_BIT,
        EGL_RED_SIZE,
        8,
        EGL_GREEN_SIZE,
        8,
        EGL_BLUE_SIZE,
        8,
        EGL_ALPHA_SIZE,
        8,
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_ES2_BIT,
        EGL_NONE,
    };

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION,
        2,
        EGL_NONE,
    };

    egl_display = eglGetDisplay(
        (EGLNativeDisplayType)bl::WlDisplay::instance()->c_ptr()
    );
    if (egl_display == EGL_NO_DISPLAY) {
        fprintf(stderr, "Can't create egl display.\n");
        exit(1);
    }

    if (eglInitialize(egl_display, &major, &minor) != EGL_TRUE) {
        fprintf(stderr, "Can't initialise egl display.\n");
        exit(1);
    }
    printf("EGL major: %d, minor %d\n", major, minor);

    eglGetConfigs(egl_display, NULL, 0, &count);

    configs = (EGLConfig*)calloc(count, sizeof *configs);

    eglChooseConfig(egl_display, config_attribs, configs, count, &n);

    for (int i = 0; i < n; ++i) {
        eglGetConfigAttrib(egl_display, configs[i], EGL_BUFFER_SIZE, &size);
        eglGetConfigAttrib(egl_display, configs[i], EGL_RED_SIZE, &size);

        // Just choose the first one.
        egl_config = configs[i];
        break;
    }

    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT,
        context_attribs);
}

//====================
// Callback Handlers
//====================
static void wl_callback_done_handler(void *data, struct wl_callback *cb,
        uint32_t time)
{
    fprintf(stderr, "wl_callback_done_handler()\n");
}

static const wl_callback_listener wl_callback_listener = {
    wl_callback_done_handler,
};

//======================
// XDG Shell Handlers
//======================

static void xdg_wm_base_ping_handler(void *data,
        struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    fprintf(stderr, "Ping!\n");
    ::xdg_wm_base->pong(serial);
}

static const bl::XdgWmBase::Listener xdg_wm_base_listener =
    bl::XdgWmBase::Listener(
        xdg_wm_base_ping_handler
    );

//========================
// XDG Surface Handlers
//========================

static void xdg_surface_configure_handler(void *data,
        struct xdg_surface *xdg_surface, uint32_t serial)
{
    bl::XdgSurface::ack_configure(xdg_surface, serial);
}

static const bl::XdgSurface::Listener xdg_surface_listener =
    bl::XdgSurface::Listener(
        xdg_surface_configure_handler
    );

//=========================
// XDG Toplevel Handlers
//=========================

static void xdg_toplevel_configure_handler(void *data,
        struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height,
        struct wl_array *states)
{
}

static void xdg_toplevel_close_handler(void *data,
        struct xdg_toplevel *xdg_toplevel)
{
}

static const bl::XdgToplevel::Listener xdg_toplevel_listener =
    bl::XdgToplevel::Listener(
        xdg_toplevel_configure_handler,
        xdg_toplevel_close_handler
    );

//============================
// Global Registry Handlers
//============================
static void global_registry_handler(void *data, struct wl_registry *registry,
        uint32_t id, const char *interface, uint32_t version)
{
    if (strcmp(interface, "wl_compositor") == 0) {
        auto registry = bl::WlRegistry::instance();
        auto interface = bl::WlInterface<bl::WlInterfaceType::Compositor>();
        compositor = registry->bind(id,
            interface,
            version
        );
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        auto registry = bl::WlRegistry::instance();
        auto interface = bl::WlInterface<bl::WlInterfaceType::XdgWmBase>();
        xdg_wm_base = registry->bind(id,
            interface,
            1
        );
    }
}

static void global_registry_remove_handler(void *data,
        struct wl_registry *registry, uint32_t id)
{
}

static const bl::WlRegistry::Listener registry_listener =
    bl::WlRegistry::Listener(
        global_registry_handler,
        global_registry_remove_handler
    );

//=============
// Main
//=============

int main(int argc, char *argv[])
{
    bl::WlDisplay display = bl::WlDisplay::connect();

    bl::WlRegistry registry = display.get_registry();
    registry.add_listener(registry_listener);

    display.dispatch();
    display.roundtrip();
    //

    // Create a surface.
    if (bl::WlCompositor::instance() == nullptr) {
        fprintf(stderr, "WlCompositor singleton not ready.\n");
    }
    fprintf(stderr, "Before create surface. wl_compositor: %p\n",
        bl::WlCompositor::instance()->c_ptr());
    bl::WlSurface surface = bl::WlCompositor::instance()->create_surface();
    fprintf(stderr, "Surface created!\n");

    fprintf(stderr, "xdg_wm_base: %p\n", xdg_wm_base->c_ptr());
    xdg_wm_base->add_listener(xdg_wm_base_listener);
    fprintf(stderr, "xdg_wm_base listener added.\n");

    // Get XDG surface.
    std::shared_ptr<bl::XdgSurface> xdg_surface =
        xdg_wm_base->get_xdg_surface(surface);
    fprintf(stderr, "Got XDG surface.\n");
    xdg_surface->add_listener(xdg_surface_listener);

    // Get XDG toplevel.
    std::shared_ptr<bl::XdgToplevel> xdg_toplevel =
        xdg_surface->get_toplevel();
    xdg_toplevel->add_listener(xdg_toplevel_listener);

    // Commit before roundtrip.
    fprintf(stderr, "Before commit.\n");
    surface.commit();

    display.roundtrip();

    // Commit the surface.
    surface.commit();

    // Callback.
    struct wl_callback *cb = wl_surface_frame(surface.c_ptr());
    wl_callback_add_listener(cb, &wl_callback_listener, NULL);

    // GL/EGL
    init_egl();
    //== CREATE WINDOW ==//
    egl_window = wl_egl_window_create(surface.c_ptr(),
        WINDOW_WIDTH, WINDOW_HEIGHT);
    egl_surface = eglCreateWindowSurface(egl_display, egl_config, egl_window,
        NULL);
    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
    glFlush();
    eglSwapBuffers(egl_display, egl_surface);
    //===================//
    if (init_program(&program_object) == 0) {
        fprintf(stderr, "Error init program!\n");
        exit(1);
    }
    draw_function();

    while (true) {
        display.dispatch();
    }

    return 0;
}
