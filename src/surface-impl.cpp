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
#include <blusher/utils.h>
#include <blusher/size.h>

#include "application-impl.h"
#include "view-impl.h"
#include "egl-utils.h"

//=========
// XDG
//=========

// XDG surface
static void xdg_surface_configure_handler(void *data,
        struct xdg_surface *xdg_surface, uint32_t serial)
{
    (void)data;
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
    /*
    fprintf(stderr, "[LOG] xdg_toplevel_configure_handler - size: %dx%d\n",
        width, height);
    */
    // TODO: implement

    pr::Vector<bl::XdgToplevel::State> states_v =
        bl::XdgToplevel::states_to_vector(states);

    // State is resizing.
    if (states_v.index(bl::XdgToplevel::State::Resizing) != std::nullopt) {
        // Sometimes width and height could be zero.
        if (width == 0 && height == 0) {
            return;
        }
        if (surface->width() != width || surface->height() != height) {
            // fprintf(stderr, "Resizing...\n");
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
    : QObject(parent),
      _surface(bl::app_impl->compositor()->create_surface())
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

    this->_xdg_surface = nullptr;
    this->_xdg_toplevel = nullptr;

    this->m_visible = false;

    this->m_rootView = new View();

    this->m_pointerEnterHandler = nullptr;
    this->m_pointerLeaveHandler = nullptr;
    this->m_pointerPressHandler = nullptr;
    this->m_resizeHandler = nullptr;

    //===============
    // Wayland
    //===============
    this->_subsurface = NULL;
    this->_frame_callback = NULL;

    this->_shm_data = NULL;
    this->_shm_data_size = 0;

    if (this->toplevel() != true) {
        this->_subsurface = wl_subcompositor_get_subsurface(app_impl->subcompositor(),
            this->_surface.wl_surface(),
            static_cast<SurfaceImpl*>(this->parent())->wlSurface()
        );
    }

    //=============
    // XDG shell
    //=============
    if (this->parent() == nullptr) {
        this->_xdg_surface =
            xdg_wm_base_get_xdg_surface(app_impl->xdgWmBase()->xdg_wm_base(),
                this->_surface.wl_surface());
        xdg_surface_add_listener(this->_xdg_surface, &xdg_surface_listener, NULL);

        this->_xdg_toplevel = xdg_surface_get_toplevel(this->_xdg_surface);
        xdg_toplevel_add_listener(this->_xdg_toplevel,
            &xdg_toplevel_listener,
            static_cast<void*>(this));

        // Signal that the surface is ready to be configured.
        this->_surface.commit();
        // Wait for the surface to be configured.
        app_impl->display()->roundtrip();

        xdg_surface_set_window_geometry(this->_xdg_surface, 0, 0, 200, 200);
    }

    //============
    // EGL
    //============
    this->_egl_object = EglObject();

    init_egl(&this->_egl_object);
    this->_egl_window = wl_egl_window_create(this->_surface.wl_surface(),
        this->width(), this->height());
    this->_egl_object.egl_surface = eglCreateWindowSurface(
        this->_egl_object.egl_display, this->_egl_object.egl_config,
        this->_egl_window,
        NULL);

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

        if (!this->toplevel()) {
            wl_subsurface_set_position(this->_subsurface, x, this->y());
        }

        emit this->implXChanged(x);
    }
}

void SurfaceImpl::setY(uint32_t y)
{
    if (this->m_y != y) {
        this->m_y = y;

        if (!this->toplevel()) {
            wl_subsurface_set_position(this->_subsurface, this->x(), y);
        }

        emit this->implYChanged(y);
    }
}

void SurfaceImpl::setWidth(uint32_t width)
{
    if (this->m_width != width) {
        this->m_width = width;

        emit this->implWidthChanged(width);
    }
}

void SurfaceImpl::setHeight(uint32_t height)
{
    if (this->m_height != height) {
        this->m_height = height;

        emit this->implHeightChanged(height);
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

    // Set xdg-surface window geometry.
    if (this->_xdg_surface != nullptr) {
        xdg_surface_set_window_geometry(this->_xdg_surface, -1, -1, width, height);
    }

    // Fire resize event.
    QResizeEvent event(QSize(width, height), QSize(old_width, old_height));
    this->resizeEvent(&event);

    if (this->m_blSurface != nullptr) {
        // fprintf(stderr, "[LOG] SurfaceImpl::setSize() - update.\n");
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
        this->_surface.commit();

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
    fprintf(stderr,
        "[LOG] SurfaceImpl::setBlSurface() - %p, surface: %p, root_view: %p\n",
        this, blSurface, this->m_rootView);
    this->m_rootView->set_surface(blSurface);
}

void SurfaceImpl::moveIfToplevel()
{
    if (this->parent() == nullptr) {
        xdg_toplevel_move(this->_xdg_toplevel,
            app_impl->seat()->wl_seat(), app_impl->pointer_state.serial);
        // Manually release button pressed state because after
        // xdg_toplevel_move() call, xdg_toplevel.button event not called.
        app_impl->pointer_state.button = 0;
    }
}

void SurfaceImpl::resizeIfToplevel(XdgToplevel::ResizeEdge edge)
{
    uint32_t xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
    switch (edge) {
    case XdgToplevel::ResizeEdge::TopLeft:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
        break;
    case XdgToplevel::ResizeEdge::Top:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
        break;
    case XdgToplevel::ResizeEdge::TopRight:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
        break;
    case XdgToplevel::ResizeEdge::Left:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
        break;
    case XdgToplevel::ResizeEdge::Right:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
        break;
    case XdgToplevel::ResizeEdge::BottomLeft:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
        break;
    case XdgToplevel::ResizeEdge::Bottom:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
        break;
    case XdgToplevel::ResizeEdge::BottomRight:
        xdg_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
        break;
    default:
        break;
    }

    if (this->parent() == nullptr) {
        xdg_toplevel_resize(this->_xdg_toplevel,
            bl::app_impl->seat()->wl_seat(),
            bl::app_impl->pointer_state.serial,
            xdg_edge
        );
        // Manually release button pressed state because after
        // xdg_toplevel_resize() call, xdg_toplevel.button event not called.
        app_impl->pointer_state.button = 0;
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

void SurfaceImpl::setPointerMoveHandler(void (Surface::*handler)(uint32_t, double, double))
{
    this->m_pointerMoveHandler = handler;
}

void SurfaceImpl::setResizeHandler(void (Surface::*handler)(int32_t, int32_t, int32_t, int32_t))
{
    this->m_resizeHandler = handler;
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

void SurfaceImpl::callPointerMoveHandler(uint32_t button, double x, double y)
{
    if (this->m_pointerMoveHandler != nullptr) {
        auto handler = this->m_pointerMoveHandler;
        (this->m_blSurface->*handler)(button, x, y);
    }
}

void SurfaceImpl::callResizeHandler(int32_t width, int32_t height,
        int32_t old_width, int32_t old_height)
{
    if (this->m_resizeHandler != nullptr) {
        auto handler = this->m_resizeHandler;
        (this->m_blSurface->*handler)(width, height, old_width, old_height);
    }
}

//==================
// Wayland objects
//==================
struct wl_surface* SurfaceImpl::wlSurface() const
{
    return const_cast<WlSurface&>(this->_surface).wl_surface();
}


//=================
// Public Slots
//=================

void SurfaceImpl::update()
{
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
    } else if (event->type() == QEvent::MouseMove) {
        auto mouse_event = static_cast<QMouseEvent*>(event);
        if (this->m_pointerMoveHandler != nullptr) {
            auto handler = this->m_pointerMoveHandler;
            uint32_t button =
                qt_mouse_button_to_libinput_button(mouse_event->button());
            (this->m_blSurface->*handler)(button,
                mouse_event->pos().x(),
                mouse_event->pos().y());
        }
        return true;
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
    if (this->m_pointerPressHandler != nullptr) {
        int button = qt_mouse_button_to_libinput_button(event->button());

        auto handler = this->m_pointerPressHandler;
        (this->m_blSurface->*handler)(button,
            event->localPos().x(),
            event->localPos().y());
    }
}

void SurfaceImpl::mouseReleaseEvent(QMouseEvent *event)
{
    if (this->m_pointerPressHandler != nullptr) {
        int button = qt_mouse_button_to_libinput_button(event->button());

        auto handler = this->m_pointerReleaseHandler;
        (this->m_blSurface->*handler)(button,
            event->localPos().x(),
            event->localPos().y());
    }
}

void SurfaceImpl::resizeEvent(QResizeEvent *event)
{
    if (this->m_resizeHandler != nullptr) {
        auto handler = this->m_resizeHandler;
        (this->m_blSurface->*handler)(
            event->size().width(),
            event->size().height(),
            event->oldSize().width(),
            event->oldSize().height()
        );
    }
}

} // namespace bl
