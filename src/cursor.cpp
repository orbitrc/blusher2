#include <blusher/cursor.h>

// C
#include <stdio.h>

// Wayland
#include <wayland-egl.h>

// EGL/OpenGL
#include <EGL/egl.h>
#include <GLES3/gl3.h>

// Blusher
#include <blusher/wayland/wl-display.h>
#include <blusher/wayland/wl-surface.h>
#include <blusher/application.h>
#include <blusher/image.h>
#include <blusher/color.h>

#include "egl-utils.h"

class EglObject {
public:
    EglObject()
    {
        this->egl_display = nullptr;
        this->egl_config = nullptr;
        this->egl_surface = nullptr;
        this->egl_context = nullptr;
        this->program_object = 0;
    }

    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLSurface egl_surface;
    EGLContext egl_context;
    GLuint program_object;
};

//==============
// EGL/OpenGL
//==============
static GLuint load_shader(const char *shader_src, GLenum type)
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

static int init_program(GLuint *program_object)
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

static void texture_function(EGLDisplay egl_display, EGLSurface egl_surface,
        EGLContext egl_context,
        GLuint *program_object,
        const bl::Image& image,
        uint64_t width, uint64_t height)
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

    fprintf(stderr,
        "[LOG] In Cursor. texture_function() - width height: %ldx%ld\n",
        width, height);
    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
    EGLint err = eglGetError();
    if (err != EGL_SUCCESS) {
        fprintf(stderr,
            "[WARN] texture_function() - eglMakeCurrent Error: %s %d\n",
            egl_error_to_string(err), err);
    }

    // Set the viewport.
    glViewport(
        0, 0,
        width, height
    );

    // Clear the color buffer.
    glClearColor(0.5, 0.5, 0.5, 0.8);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object.
    glUseProgram(*program_object);

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
        image.width(),      // width.
        image.height(),     // height.
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

    eglMakeCurrent(egl_display, NULL, NULL, NULL);
}

static void init_egl(EglObject *egl_object)
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

    egl_object->egl_display = eglGetDisplay(
        (EGLNativeDisplayType)bl::WlDisplay::instance()->c_ptr()
    );
    if (egl_object->egl_display == EGL_NO_DISPLAY) {
        fprintf(stderr, "Can't create egl display.\n");
        exit(1);
    }

    if (eglInitialize(egl_object->egl_display, &major, &minor) != EGL_TRUE) {
        fprintf(stderr, "Can't initialise egl display.\n");
        exit(1);
    }
    printf("EGL major: %d, minor %d\n", major, minor);

    eglGetConfigs(egl_object->egl_display, NULL, 0, &count);

    configs = (EGLConfig*)calloc(count, sizeof *configs);

    eglChooseConfig(egl_object->egl_display,
        config_attribs, configs, count, &n);

    for (int i = 0; i < n; ++i) {
        eglGetConfigAttrib(egl_object->egl_display,
            configs[i], EGL_BUFFER_SIZE, &size);
        eglGetConfigAttrib(egl_object->egl_display,
            configs[i], EGL_RED_SIZE, &size);

        // Just choose the first one.
        egl_object->egl_config = configs[i];
        break;
    }

    egl_object->egl_context = eglCreateContext(egl_object->egl_display,
        egl_object->egl_config, EGL_NO_CONTEXT,
        context_attribs);
}

namespace bl {

//===============
// CursorImpl
//===============

class CursorImpl
{
public:
    CursorImpl(Cursor *cursor)
        : _surface(bl::app->wl_compositor()->create_surface())
    {
        this->_cursor = cursor;

        this->_image = new Image(24, 24);
        this->_image->fill(Color::from_rgb(255, 0, 0));

        //================
        // EGL/OpenGL
        //================
        init_egl(&this->_egl_object);
        this->_egl_window = wl_egl_window_create(this->_surface->c_ptr(),
            24, 24);
        this->_egl_object.egl_surface = eglCreateWindowSurface(
            this->_egl_object.egl_display, this->_egl_object.egl_config,
            this->_egl_window,
            NULL);

        eglMakeCurrent(this->_egl_object.egl_display,
            this->_egl_object.egl_surface,
            this->_egl_object.egl_surface,
            this->_egl_object.egl_context);

        init_program(&this->_egl_object.program_object);

        texture_function(
            this->_egl_object.egl_display,
            this->_egl_object.egl_surface,
            this->_egl_object.egl_context,
            &this->_egl_object.program_object,
            *this->_image,
            24,
            24
        );
    }

    ~CursorImpl()
    {
        delete this->_image;
    }

    struct wl_surface* wl_surface()
    {
        return this->_surface->c_ptr();
    }

private:
    Cursor *_cursor;

    std::shared_ptr<WlSurface> _surface;

    Image *_image;

    struct wl_egl_window *_egl_window;
    EglObject _egl_object;
};

//==============
// Cursor
//==============

Cursor::Cursor(Cursor::Shape shape)
{
    this->_shape = shape;

    this->_hot_spot_x = 0;
    this->_hot_spot_y = 0;

    this->_impl = new CursorImpl(this);
}

Cursor::~Cursor()
{
    delete this->_impl;
}

Cursor::Shape Cursor::shape() const
{
    return this->_shape;
}

uint32_t Cursor::hot_spot_x() const
{
    return this->_hot_spot_x;
}

uint32_t Cursor::hot_spot_y() const
{
    return this->_hot_spot_y;
}

struct wl_surface* Cursor::wl_surface()
{
    return this->_impl->wl_surface();
}

} // namespace bl
