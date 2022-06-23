#include "surface-impl.h"

// C
#include <assert.h>
#include <stdio.h>

// Unix
#include <sys/mman.h>
#include <unistd.h>

// libinput
#include <linux/input.h>

#include <QBackingStore>
#include <QPainter>
#include <QMouseEvent>
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

#include "application-impl.h"
#include "view-impl.h"

//=========
// XDG
//=========

// XDG surface
static void xdg_surface_configure_handler(void *data,
        struct xdg_surface *xdg_surface, uint32_t serial)
{
    xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure_handler,
};

// XDG toplevel
static void xdg_toplevel_configure_handler(void *data,
        struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height,
        struct wl_array *states)
{
    bl::SurfaceImpl *surface_impl = static_cast<bl::SurfaceImpl*>(data);
    bl::Surface *surface = surface_impl->surface();

    // Check if same.
    // assert(xdg_toplevel == surface_impl->_xdg_toplevel);

    (void)xdg_toplevel;
    fprintf(stderr, "[LOG] xdg_toplevel_configure_handler - size: %dx%d\n",
        width, height);
    // TODO: implement

    pr::Vector<bl::XdgToplevel::State> states_v =
        bl::XdgToplevel::states_to_vector(states);

    // State is resizing.
    if (states_v.index(bl::XdgToplevel::State::Resizing) != std::nullopt) {
        if (surface->width() != width || surface->height() != height) {
            fprintf(stderr, "Resizing...\n");
            surface->set_geometry(0, 0, width, height);
            // surface->update();
        }
    }
}

static void xdg_toplevel_close_handler(void *data,
        struct xdg_toplevel *xdg_toplevel)
{
    fprintf(stderr, "Closing...\n");
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure_handler,
    .close = xdg_toplevel_close_handler,
};

//================
// Manage buffers
//================

//=================
// EGL/OpenGL
//=================

static EGLSurface last_swapped = nullptr;

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

    fprintf(stderr, "[LOG] texture_function() - width height: %ldx%ld\n",
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

    if (last_swapped != egl_surface) {
        eglSwapBuffers(egl_display, egl_surface);
        last_swapped = egl_surface;
    }

    eglMakeCurrent(egl_display, NULL, NULL, NULL);
}

static void init_egl(bl::SurfaceImpl::EglObject *egl_object)
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
        (EGLNativeDisplayType)bl::WlDisplay::instance()->wl_display()
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

SurfaceImpl::SurfaceImpl(QObject *parent)
    : QObject(parent)
{
    this->m_x = 0;
    this->m_y = 0;
    this->m_width = 100.0;
    this->m_height = 100.0;

    if (parent != nullptr) {
        this->m_clipX = 0;
        this->m_clipY = 0;
        this->m_clipWidth = this->m_width;
        this->m_clipHeight = this->m_height;
    } else {
        this->m_clipX = 0;
        this->m_clipY = 0;
        this->m_clipWidth = this->m_width;
        this->m_clipHeight = this->m_height;
    }

    this->_xdg_toplevel = nullptr;

    this->m_visible = false;
    this->m_color = Color::from_rgb(255, 255, 255);

    this->m_rootView = std::make_shared<View>(new View());

    this->m_pointerEnterHandler = nullptr;
    this->m_pointerLeaveHandler = nullptr;
    this->m_pointerPressHandler = nullptr;

    //===============
    // Wayland
    //===============
    this->_surface = wl_compositor_create_surface(bl::app_impl->compositor());
    this->_subsurface = NULL;
    this->_frame_callback = NULL;

    this->_shm_data = NULL;
    this->_shm_data_size = 0;

    if (this->toplevel() != true) {
        this->_subsurface = wl_subcompositor_get_subsurface(app_impl->subcompositor(),
            this->_surface,
            static_cast<SurfaceImpl*>(this->parent())->wlSurface()
        );
    }

    //=============
    // XDG shell
    //=============
    if (this->parent() == nullptr) {
        this->_xdg_surface = xdg_wm_base_get_xdg_surface(app_impl->xdgWmBase()->xdg_wm_base(),
            this->_surface);
        xdg_surface_add_listener(this->_xdg_surface, &xdg_surface_listener, NULL);

        this->_xdg_toplevel = xdg_surface_get_toplevel(this->_xdg_surface);
        xdg_toplevel_add_listener(this->_xdg_toplevel,
            &xdg_toplevel_listener,
            static_cast<void*>(this));

        // Signal that the surface is ready to be configured.
        wl_surface_commit(this->_surface);
        // Wait for the surface to be configured.
        app_impl->display()->roundtrip();

        xdg_surface_set_window_geometry(this->_xdg_surface, 0, 0, 200, 200);
    }

    //============
    // EGL
    //============
    this->_egl_object = EglObject();

    init_egl(&this->_egl_object);
    this->_egl_window = wl_egl_window_create(this->_surface,
        this->width(), this->height());
    this->_egl_object.egl_surface = eglCreateWindowSurface(
        this->_egl_object.egl_display, this->_egl_object.egl_config,
        this->_egl_window,
        NULL);

//    this->setGeometry(this->m_x, this->m_y, this->m_width, this->m_height);

    app_impl->addSurfaceImpl(this);

    QObject::connect(this, &SurfaceImpl::implXChanged, this, &SurfaceImpl::onImplXChanged);
    QObject::connect(this, &SurfaceImpl::implYChanged, this, &SurfaceImpl::onImplYChanged);
    QObject::connect(this, &SurfaceImpl::implWidthChanged, this, &SurfaceImpl::onImplWidthChanged);
    QObject::connect(this, &SurfaceImpl::implHeightChanged, this, &SurfaceImpl::onImplHeightChanged);
}

SurfaceImpl::~SurfaceImpl()
{
    // wl_shm_pool_destroy(this->_shm_pool);

    app_impl->removeSurfaceImpl(this);
}

//===================
// Public Methods
//===================

double SurfaceImpl::x() const
{
    return this->m_x;
}

double SurfaceImpl::y() const
{
    return this->m_y;
}

double SurfaceImpl::width() const
{
    return this->m_width;
}

double SurfaceImpl::height() const
{
    return this->m_height;
}

void SurfaceImpl::setX(double x)
{
    if (this->m_x != x) {
        this->m_x = x;

        if (!this->toplevel()) {
            wl_subsurface_set_position(this->_subsurface, x, this->y());
        }

        emit this->implXChanged(x);
    }
}

void SurfaceImpl::setY(double y)
{
    if (this->m_y != y) {
        this->m_y = y;

        if (!this->toplevel()) {
            wl_subsurface_set_position(this->_subsurface, this->x(), y);
        }

        emit this->implYChanged(y);
    }
}

void SurfaceImpl::setWidth(double width)
{
    if (this->m_width != width) {
        this->m_width = width;

        emit this->implWidthChanged(width);
    }
}

void SurfaceImpl::setHeight(double height)
{
    if (this->m_height != height) {
        this->m_height = height;

        emit this->implHeightChanged(height);
    }
}

void SurfaceImpl::setSize(double width, double height)
{
    // Pass if size is not changed.
    if (this->m_width == width && this->m_height == height) {
        return;
    }

    if (this->m_width != width) {
        this->m_width = width;
    }
    if (this->m_height != height) {
        this->m_height = height;
    }

    // Resize view's size.
    this->m_rootView->set_width(width);
    this->m_rootView->set_height(height);

    wl_egl_window_resize(this->_egl_window, width, height, 0, 0);
    // Re-create EGL window surface.
    EGLBoolean destroyed = eglDestroySurface(this->_egl_object.egl_display,
        this->_egl_object.egl_surface);
    if (!destroyed) {
        fprintf(stderr, "[WARN] EGL surface not destroyed!\n");
        return;
    }
    this->_egl_object.egl_surface = eglCreateWindowSurface(
        this->_egl_object.egl_display,
        this->_egl_object.egl_config,
        this->_egl_window,
        NULL
    );

    if (this->m_blSurface != nullptr) {
        fprintf(stderr, "[LOG] SurfaceImpl::setSize() - update.\n");
        this->m_blSurface->update();
    } else {
        fprintf(stderr, "[WARN] SurfaceImpl::setSize() - surface is null!\n");
    }
}

bool SurfaceImpl::clip() const
{
    return this->m_clip;
}

void SurfaceImpl::setClip(bool clip)
{
    this->m_clip = clip;
}

double SurfaceImpl::clipX() const
{
    return this->m_clipX;
}

double SurfaceImpl::clipY() const
{
    return this->m_clipY;
}

double SurfaceImpl::clipWidth() const
{
    return this->m_clipWidth;
}

double SurfaceImpl::clipHeight() const
{
    return this->m_clipHeight;
}

std::shared_ptr<View> SurfaceImpl::rootView()
{
    return this->m_rootView;
}

void SurfaceImpl::paint()
{
    if (this->m_visible == false) {
        return;
    }
    fprintf(stderr, "SurfaceImpl::paint() - width: %f, height: %f\n",
        this->width(), this->height());

    /*
    uint32_t *pixel = static_cast<uint32_t*>(this->shmData());

    const uint32_t color = this->m_color.to_argb();
    for (int n = 0; n < (this->width() * this->height()); ++n) {
        *pixel++ = color;
    }
    */
}

void SurfaceImpl::show()
{
    if (this->m_visible != true) {
        this->m_visible = true;
        fprintf(stderr, "visible set to true. width: %f, height: %f\n",
            this->width(), this->height());

        // wl_surface_attach(this->_surface, this->_buffer, 0, 0);
        wl_surface_commit(this->_surface);

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

void SurfaceImpl::setColor(const Color &color)
{
    this->m_color = color;

    this->m_rootView->set_color(color);
}

void SurfaceImpl::placeAbove(SurfaceImpl *surface_impl)
{
    if (this->_subsurface != nullptr) {
        wl_subsurface_place_above(this->_subsurface,
            surface_impl->wlSurface());
    }
}

void SurfaceImpl::placeBelow(SurfaceImpl *surface_impl)
{
    if (this->_subsurface != nullptr) {
        wl_subsurface_place_below(this->_subsurface,
            surface_impl->wlSurface());
    }
}

bool SurfaceImpl::toplevel() const
{
    return this->parent() == nullptr;
}

Surface* SurfaceImpl::surface()
{
    return this->m_blSurface;
}

void SurfaceImpl::setBlSurface(Surface *blSurface)
{
    this->m_blSurface = blSurface;
    this->m_rootView->set_surface(blSurface);
}

void SurfaceImpl::moveIfToplevel()
{
    if (this->parent() == nullptr) {
        xdg_toplevel_move(this->_xdg_toplevel,
            app_impl->seat()->wl_seat(), app_impl->pointerPressSerial());
    }
}

void SurfaceImpl::resizeIfToplevel()
{
    if (this->parent() == nullptr) {
        xdg_toplevel_resize(this->_xdg_toplevel,
            bl::app_impl->seat()->wl_seat(),
            bl::app_impl->pointerPressSerial(),
            XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM
        );
    }
}

//===================
// Event handlers
//===================
void SurfaceImpl::setPointerEnterHandler(void (Surface::*handler)())
{
    this->m_pointerEnterHandler = handler;
}

void SurfaceImpl::setPointerLeaveHandler(void (Surface::*handler)())
{
    this->m_pointerLeaveHandler = handler;
}

void SurfaceImpl::setPointerPressHandler(void (Surface::*handler)(uint32_t, double, double))
{
    this->m_pointerPressHandler = handler;
}

void SurfaceImpl::setPointerReleaseHandler(void (Surface::*handler)(uint32_t, double, double))
{
    this->m_pointerReleaseHandler = handler;
}

void SurfaceImpl::callPointerEnterHandler()
{
    if (this->m_pointerEnterHandler != nullptr) {
        auto handler = this->m_pointerEnterHandler;
        (this->m_blSurface->*handler)();
    }
}

void SurfaceImpl::callPointerLeaveHandler()
{
    if (this->m_pointerLeaveHandler != nullptr) {
        auto handler = this->m_pointerLeaveHandler;
        (this->m_blSurface->*handler)();
    }
}

void SurfaceImpl::callPointerPressHandler(uint32_t button, double x, double y)
{
    if (this->m_pointerPressHandler != nullptr) {
        auto handler = this->m_pointerPressHandler;
        (this->m_blSurface->*handler)(button, x, y);
    }
}

void SurfaceImpl::callPointerReleaseHandler(uint32_t button, double x, double y)
{
    if (this->m_pointerReleaseHandler != nullptr) {
        auto handler = this->m_pointerReleaseHandler;
        (this->m_blSurface->*handler)(button, x, y);
    }
}

//==================
// Wayland objects
//==================
struct wl_surface* SurfaceImpl::wlSurface() const
{
    return this->_surface;
}


//=================
// Public Slots
//=================

void SurfaceImpl::update()
{
    fprintf(stderr, " - update() SurfaceImpl: %p\n", this);
    eglMakeCurrent(this->_egl_object.egl_display,
        this->_egl_object.egl_surface,
        this->_egl_object.egl_surface,
        this->_egl_object.egl_context);
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
    texture_function(
        this->_egl_object.egl_display,
        this->_egl_object.egl_surface,
        this->_egl_object.egl_context,
        &this->_egl_object.program_object,
        *this->m_rootView->_impl->m_composedImage,
        this->width(), this->height()
    );
}

//=================
// Private Slots
//=================

void SurfaceImpl::onImplXChanged(double x)
{
    if (this->parent() != nullptr) {
//        QWindow::setX(x);
    }
//    emit this->xChanged(static_cast<int>(x));
}

void SurfaceImpl::onImplYChanged(double y)
{
    if (this->parent() != nullptr) {
//        QWindow::setY(y);
    }
//    emit this->yChanged(static_cast<int>(y));
}

void SurfaceImpl::onImplWidthChanged(double width)
{
//    QWindow::setWidth(width);
//    emit this->widthChanged(static_cast<int>(width));
}

void SurfaceImpl::onImplHeightChanged(double height)
{
//    QWindow::setHeight(height);
//    emit this->heightChanged(static_cast<int>(height));
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
    if (event->type() == QEvent::Enter) {
        if (this->m_pointerEnterHandler != nullptr) {
            auto handler = this->m_pointerEnterHandler;
            (this->m_blSurface->*handler)();

            return true;
        }
    } else if (event->type() == QEvent::Leave) {
        if (this->m_pointerLeaveHandler != nullptr) {
            auto handler = this->m_pointerLeaveHandler;
            (this->m_blSurface->*handler)();

            return true;
        }
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
//    fprintf(stderr, "(%f, %f)\n", event->localPos().x(), event->localPos().y());
}

void SurfaceImpl::mousePressEvent(QMouseEvent *event)
{
    if (this->m_pointerPressHandler != nullptr) {
        int button = 0;
        switch (event->button()) {
        case Qt::LeftButton:
            button = BTN_LEFT;
            break;
        case Qt::RightButton:
            button = BTN_RIGHT;
            break;
        case Qt::MiddleButton:
            button = BTN_MIDDLE;
            break;
        default:
            break;
        }

        auto handler = this->m_pointerPressHandler;
        (this->m_blSurface->*handler)(button,
            event->localPos().x(),
            event->localPos().y());
    }
}

void SurfaceImpl::mouseReleaseEvent(QMouseEvent *event)
{
    if (this->m_pointerPressHandler != nullptr) {
        int button = 0;
        switch (event->button()) {
        case Qt::LeftButton:
            button = BTN_LEFT;
            break;
        case Qt::RightButton:
            button = BTN_RIGHT;
            break;
        case Qt::MiddleButton:
            button = BTN_MIDDLE;
            break;
        default:
            break;
        }

        auto handler = this->m_pointerReleaseHandler;
        (this->m_blSurface->*handler)(button,
            event->localPos().x(),
            event->localPos().y());
    }
}

void SurfaceImpl::resizeEvent(QResizeEvent *event)
{
//    this->m_backingStore->resize(event->size());
}

} // namespace bl
