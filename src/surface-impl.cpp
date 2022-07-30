#include "surface-impl.h"

// C
#include <assert.h>
#include <stdio.h>

// Unix
#include <sys/mman.h>
#include <unistd.h>

// libinput
#include <linux/input.h>

// Qt
#include <QResizeEvent>

// Primer
#include <primer/vector.h>

// Blusher
#include <blusher/application.h>
#ifdef emit
#undef emit
#endif
#include <blusher/surface.h>
#ifndef emit
#define emit Q_EMIT
#endif
#include <blusher/utils.h>
#include <blusher/image.h>
#include <blusher/view.h>
#include <blusher/utils.h>
#include <blusher/size.h>

#include "application-impl.h"
#include "view-impl.h"
#include "egl-utils.h"

//================
// Manage buffers
//================

//=================
// EGL/OpenGL
//=================

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

    /*
    fprintf(stderr, "[LOG] texture_function() - width height: %ldx%ld\n",
        width, height);
    */
    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
    EGLint err = eglGetError();
    if (err != EGL_SUCCESS) {
        fprintf(stderr,
            "[WARN] texture_function() - eglMakeCurrent Error: %s %d\n",
            egl_error_to_string(err), err);
    }

    // Convert image format.
    bl::Image rgba32_image = image.converted(bl::Image::Format::Rgba32);

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
        rgba32_image.data()
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

    eglSwapBuffers(egl_display, egl_surface);

    eglMakeCurrent(egl_display, NULL, NULL, NULL);
}

namespace bl {

SurfaceImpl::SurfaceImpl(Surface *surface, QObject *parent)
    : QObject(parent)
{
    this->m_x = 0;
    this->m_y = 0;
    this->m_width = 100.0;
    this->m_height = 100.0;

    // Initialize.
    this->_toplevel_maximized = false;

    this->_wl_subsurface = nullptr;

    this->m_blSurface = surface;

    this->m_visible = false;

    this->m_rootView = new View();
    this->m_rootView->set_surface(surface);

    this->_context = nullptr;

    this->_updating = false;

    //===============
    // Wayland
    //===============
    this->_frame_callback = NULL;

    this->_shm_data = NULL;
    this->_shm_data_size = 0;

    if (this->isToplevel() != true) {
        this->_wl_subsurface = app_impl->subcompositor()->get_subsurface(
            this->m_blSurface->wl_surface(),
            static_cast<SurfaceImpl*>(this->parent())->m_blSurface->wl_surface()
        );
    }

    //============
    // EGL
    //============
    this->_egl_object = EglObject();
    EGLDisplay display = eglGetDisplay(
        (EGLNativeDisplayType)WlDisplay::instance()->c_ptr());
    this->_context = std::make_shared<gl::Context>(display);

    this->_egl_window = wl_egl_window_create(
        const_cast<WlSurface&>(this->surface()->wl_surface()).c_ptr(),
        this->width(), this->height());

    this->_egl_surface = eglCreateWindowSurface(
        this->_context->egl_display(),
        this->_context->egl_config(),
        (EGLNativeWindowType)this->_egl_window,
        NULL
    );

//    this->setGeometry(this->m_x, this->m_y, this->m_width, this->m_height);

    app_impl->addSurfaceImpl(this);
}

SurfaceImpl::~SurfaceImpl()
{
    // wl_shm_pool_destroy(this->_shm_pool);

    app_impl->removeSurfaceImpl(this);

//    delete this->m_rootView;
}

//===================
// Public Methods
//===================

uint32_t SurfaceImpl::x() const
{
    return this->m_x;
}

uint32_t SurfaceImpl::y() const
{
    return this->m_y;
}

uint32_t SurfaceImpl::width() const
{
    return this->m_width;
}

uint32_t SurfaceImpl::height() const
{
    return this->m_height;
}

void SurfaceImpl::setX(uint32_t x)
{
    if (this->m_x != x) {
        this->m_x = x;

        if (!this->isToplevel()) {
            wl_subsurface_set_position(this->_wl_subsurface->c_ptr(),
                x, this->y());
        }
    }
}

void SurfaceImpl::setY(uint32_t y)
{
    if (this->m_y != y) {
        this->m_y = y;

        if (!this->isToplevel()) {
            wl_subsurface_set_position(this->_wl_subsurface->c_ptr(),
                this->x(), y);
        }
    }
}

void SurfaceImpl::setWidth(uint32_t width)
{
    if (this->m_width != width) {
        this->m_width = width;
    }
}

void SurfaceImpl::setHeight(uint32_t height)
{
    if (this->m_height != height) {
        this->m_height = height;
    }
}

void SurfaceImpl::setSize(uint32_t width, uint32_t height)
{
    // Pass if size is not changed.
    if (this->m_width == width && this->m_height == height) {
        return;
    }

    uint32_t old_width = this->m_width;
    uint32_t old_height = this->m_height;

    if (this->m_width != width) {
        this->m_width = width;
    }
    if (this->m_height != height) {
        this->m_height = height;
    }

    // Resize view's size.
    this->m_rootView->set_size(Size(width, height));

    wl_egl_window_resize(this->_egl_window, width, height, 0, 0);
    // Re-create EGL window surface.
    EGLBoolean destroyed = eglDestroySurface(this->_context->egl_display(),
        this->_egl_surface);
    if (!destroyed) {
        fprintf(stderr, "[WARN] EGL surface not destroyed!\n");
        return;
    }
    this->_egl_surface = eglCreateWindowSurface(
        this->_context->egl_display(),
        this->_context->egl_config(),
        this->_egl_window,
        NULL
    );

    // Set xdg-surface window geometry.
    /*
    if (this->_xdg_surface != nullptr) {
        this->_xdg_surface->set_window_geometry(-1, -1, width, height);
    }
    */

    // Fire resize event.
    this->surface()->resize_handler(width, height, old_width, old_height);

    if (this->m_blSurface != nullptr) {
        // fprintf(stderr, "[LOG] SurfaceImpl::setSize() - update.\n");
        this->m_blSurface->update();
    } else {
        fprintf(stderr, "[WARN] SurfaceImpl::setSize() - surface is null!\n");
    }
}

View* SurfaceImpl::rootView()
{
    return this->m_rootView;
}

void SurfaceImpl::paint()
{
    if (this->m_visible == false) {
        return;
    }
}

void SurfaceImpl::show()
{
    if (this->m_visible != true) {
        this->m_visible = true;
        fprintf(stderr, "visible set to true. width: %d, height: %d\n",
            this->width(), this->height());

        // wl_surface_attach(this->_surface, this->_buffer, 0, 0);
        const_cast<WlSurface&>(this->m_blSurface->wl_surface()).commit();

        if (this->parent() != nullptr) {
            wl_surface_commit(static_cast<SurfaceImpl*>(this->parent())->wlSurface());
        }

        fprintf(stderr, "EGL and OpenGL\n");
        this->update();

        QRegion q_region;
        QExposeEvent event(q_region);
        QCoreApplication::sendEvent(this, &event);
    }
}

void SurfaceImpl::hide()
{
    if (this->m_visible == true) {
        this->m_visible = false;

        this->_egl_update(true);
    }
}

bool SurfaceImpl::visible() const
{
    return this->m_visible;
}

void SurfaceImpl::placeAbove(SurfaceImpl *surface_impl)
{
    if (this->_wl_subsurface != nullptr) {
        this->_wl_subsurface->place_above(
            surface_impl->surface()->wl_surface());
    }
}

void SurfaceImpl::placeBelow(SurfaceImpl *surface_impl)
{
    if (this->_wl_subsurface != nullptr) {
        this->_wl_subsurface->place_below(
            surface_impl->surface()->wl_surface());
    }
}

bool SurfaceImpl::toplevel() const
{
    return this->parent() == nullptr;
}

bool SurfaceImpl::isToplevel() const
{
    return this->parent() == nullptr;
}

Surface* SurfaceImpl::surface()
{
    return this->m_blSurface;
}

std::shared_ptr<gl::Context> SurfaceImpl::context()
{
    return this->_context;
}

//===================
// Event handlers
//===================

void SurfaceImpl::callPointerEnterHandler()
{
    this->m_blSurface->pointer_enter_handler();
}

void SurfaceImpl::callPointerLeaveHandler()
{
    this->m_blSurface->pointer_leave_handler();
}

void SurfaceImpl::callPointerPressHandler(uint32_t button, double x, double y)
{
    this->m_blSurface->pointer_press_handler(button, x, y);
}

void SurfaceImpl::callPointerReleaseHandler(uint32_t button, double x, double y)
{
    this->m_blSurface->pointer_release_handler(button, x, y);
}

void SurfaceImpl::callPointerMoveHandler(uint32_t button, double x, double y)
{
    this->m_blSurface->pointer_move_handler(button, x, y);
}

void SurfaceImpl::callResizeHandler(int32_t width, int32_t height,
        int32_t old_width, int32_t old_height)
{
    this->m_blSurface->resize_handler(width, height, old_width, old_height);
}

//==================
// Wayland objects
//==================
struct wl_surface* SurfaceImpl::wlSurface() const
{
    return const_cast<WlSurface&>(
        this->m_blSurface->wl_surface()).c_ptr();
}


//=================
// Public Slots
//=================

void SurfaceImpl::update()
{
    if (this->_updating) {
        return;
    }
    this->_updating = true;

    this->_egl_update();

    this->_updating = false;
}

//=================
// Private Slots
//=================

//==================
// Private Methods
//==================

void SurfaceImpl::_egl_update(bool hide)
{
    // Re-create EGL window surface.
    EGLBoolean destroyed = eglDestroySurface(this->_context->egl_display(),
        this->_egl_surface);
    if (!destroyed) {
        fprintf(stderr, "[WARN] EGL surface not destroyed!\n");
        return;
    }
    this->_egl_surface = eglCreateWindowSurface(
        this->_context->egl_display(),
        this->_context->egl_config(),
        this->_egl_window,
        NULL
    );

    eglMakeCurrent(this->_context->egl_display(),
        this->_egl_surface,
        this->_egl_surface,
        this->_context->egl_context());
    EGLint err = eglGetError();
    if (err != EGL_SUCCESS) {
        fprintf(stderr, "[WARN] eglMakeCurrent Error: %s\n",
            egl_error_to_string(err));
        return;
    }
    glFlush();
    // Below makes hang call eglSwapBuffers() in fill_function().
    // eglSwapBuffers(this->_egl_object.egl_display, this->_egl_object.egl_surface);
    //===================//
    if (init_program(&this->_egl_object.program_object) == 0) {
        fprintf(stderr, "Error init program!\n");
        // exit(1);
        return;
    }
    uint64_t width = !hide ? this->width() : 0;
    uint64_t height = !hide ? this->height() : 0;
    texture_function(
        this->_context->egl_display(),
        this->_egl_surface,
        this->_context->egl_context(),
        &this->_egl_object.program_object,
        *this->m_rootView->_impl->m_composedImage,
        width, height
    );
}

//===========
// Events
//===========

bool SurfaceImpl::event(QEvent *event)
{
    if (event->type() == QEvent::Expose) {
        fprintf(stderr, "event is Expose!\n");
        this->paint();
    }

    return QObject::event(event);
}

void SurfaceImpl::exposeEvent(QExposeEvent *event)
{
    (void)event;

    fprintf(stderr, "exposeEvent\n");
//    if (this->isExposed()) {
//        this->paint();
//    }
}

void SurfaceImpl::mouseMoveEvent(QMouseEvent *event)
{
    (void)event;
}

void SurfaceImpl::mousePressEvent(QMouseEvent *event)
{
    (void)event;
}

void SurfaceImpl::mouseReleaseEvent(QMouseEvent *event)
{
    (void)event;
}

} // namespace bl
