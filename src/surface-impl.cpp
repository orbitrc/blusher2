#include "surface-impl.h"

// C
#include <assert.h>
#include <string.h>
#include <stdio.h>

// Unix
#include <sys/mman.h>
#include <unistd.h>

// libinput
#include <linux/input.h>

// Primer
#include <primer/vector.h>

// Blusher
#include <blusher/application.h>
#include <blusher/resource.h>
#include <blusher/surface.h>
#include <blusher/utils.h>
#include <blusher/image.h>
#include <blusher/view.h>
#include <blusher/utils.h>
#include <blusher/size.h>
#include <blusher/gl/shader.h>

#include "application-impl.h"
#include "egl-utils.h"

//================
// Manage buffers
//================

//=================
// EGL/OpenGL
//=================

GLuint indices[] = {
    0, 1, 3,    // First triangle
    1, 2, 3,    // Second triangle
};
GLfloat vertices[] = {
    -1.0f, -1.0f,  0.0f,    // Top right
    -1.0f,  1.0f,  0.0f,    // Bottom right
     1.0f,  1.0f,  0.0f,    // Bottom left
     1.0f, -1.0f,  0.0f,    // Top left
};
GLfloat tex_coord[] = {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
};

static void frame_callback_handler(void *data,
        struct wl_callback *wl_callback, uint32_t time);

static const struct wl_callback_listener frame_callback_listener = {
    .done = frame_callback_handler,
};

static void frame_callback_handler(void *data,
        wl_callback *wl_callback, uint32_t time)
{
    bl::SurfaceImpl *surface_impl = static_cast<bl::SurfaceImpl*>(data);

    wl_callback_destroy(wl_callback);
    wl_callback = wl_surface_frame(surface_impl->wl_surface()->c_ptr());
    wl_callback_add_listener(wl_callback, &frame_callback_listener, data);

    // surface_impl->swapBuffers();
    surface_impl->surface()->request_update();

    surface_impl->wl_surface()->commit();
}

namespace bl {

SurfaceImpl::SurfaceImpl(Surface *surface)
{
    this->_wl_surface = app->wl_compositor()->create_surface();

    this->m_x = 0;
    this->m_y = 0;
    this->m_width = 100.0;
    this->m_height = 100.0;

    // Initialize.
    this->_wl_subsurface = nullptr;

    this->m_blSurface = surface;

    this->_entered_view = nullptr;

    this->m_visible = false;

    this->m_rootView = new View();
    this->m_rootView->set_surface(surface);

    this->_context = nullptr;
    this->_program = nullptr;

    this->_updating = false;

    //===============
    // Wayland
    //===============
    this->_frame_callback = wl_surface_frame(this->_wl_surface->c_ptr());
    wl_callback_add_listener(this->_frame_callback,
        &frame_callback_listener, this);

    this->_shm_data = NULL;
    this->_shm_data_size = 0;

    if (this->isToplevel() != true) {
        /*
        this->_wl_subsurface = app_impl->subcompositor()->get_subsurface(
            this->m_blSurface->wl_surface(),
            static_cast<SurfaceImpl*>(this->parent())->m_blSurface->wl_surface()
        );
        */
    }

    //============
    // EGL
    //============
    EGLDisplay display = eglGetDisplay(
        (EGLNativeDisplayType)WlDisplay::instance()->c_ptr());
    this->_context = std::make_shared<gl::Context>(display);

    fprintf(stderr, "[DEBUG] wl_surface: %p\n", this->_wl_surface->c_ptr());
    this->_egl_window = wl_egl_window_create(
        this->_wl_surface->c_ptr(),
        this->width(), this->height());

    this->_egl_surface = eglCreateWindowSurface(
        this->_context->egl_display(),
        this->_context->egl_config(),
        (EGLNativeWindowType)this->_egl_window,
        NULL
    );
    if (this->_egl_surface == EGL_NO_SURFACE) {
        fprintf(stderr, "[WARN] SurfaceImpl::SurfaceImpl() - EGL_NO_SURFACE\n");
    }
    // this->_context->make_current(this->_egl_surface, false);

    glewInit();

    this->_program = std::make_shared<gl::Program>();

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
        // this->m_blSurface->request_update();
    } else {
        fprintf(stderr, "[WARN] SurfaceImpl::setSize() - surface is null!\n");
    }

    this->update();
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
        this->_wl_surface->commit();

        if (this->m_blSurface->parent() != nullptr) {
            this->m_blSurface->parent()->wl_surface()->commit();
        }

        fprintf(stderr, "EGL and OpenGL\n");
        this->m_blSurface->request_update();

        // QRegion q_region;
        // QExposeEvent event(q_region);
        // QCoreApplication::sendEvent(this, &event);
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
            *surface_impl->surface()->wl_surface());
    }
}

void SurfaceImpl::placeBelow(SurfaceImpl *surface_impl)
{
    if (this->_wl_subsurface != nullptr) {
        this->_wl_subsurface->place_below(
            *surface_impl->surface()->wl_surface());
    }
}

bool SurfaceImpl::toplevel() const
{
    return false; // this->parent() == nullptr;
}

bool SurfaceImpl::isToplevel() const
{
    return false; // this->parent() == nullptr;
}

Surface* SurfaceImpl::surface()
{
    return this->m_blSurface;
}

pr::Vector<Surface*> SurfaceImpl::children() const
{
    pr::Vector<Surface*> v;

    for (auto surface: app->surfaces()) {
        if (surface->parent() == this->m_blSurface) {
            v.push(surface);
        }
    }

    return v;
}

std::shared_ptr<gl::Context> SurfaceImpl::context()
{
    return this->_context;
}

void SurfaceImpl::swapBuffers()
{
    fprintf(stderr, "[DEBUG] SurfaceImpl::swapBuffers()\n");
    eglSwapBuffers(this->_context->egl_display(), this->_egl_surface);
}

void SurfaceImpl::makeCurrent(bool nullSurface)
{
    EGLBoolean result;

    EGLSurface egl_surface = !nullSurface
        ? this->_egl_surface
        : EGL_NO_SURFACE;
    EGLContext egl_context = !nullSurface
        ? this->_context->egl_context()
        : NULL;
    result = eglMakeCurrent(this->_context->egl_display(),
        egl_surface, egl_surface,
        egl_context);
    if (result != EGL_TRUE) {
        fprintf(stderr, "[WARN] eglMakeCurrent failed!\n");
        fprintf(stderr, " - egl surface: %p\n", egl_surface);
        fprintf(stderr, " - egl context: %p\n", egl_context);
    }
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

std::shared_ptr<WlSurface> SurfaceImpl::wl_surface() const
{
    return this->_wl_surface;
}

struct wl_surface* SurfaceImpl::wlSurface() const
{
    return this->m_blSurface->wl_surface()->c_ptr();
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

    // Update children.
    for (auto child: this->children()) {
        child->update();
    }

    fprintf(stderr, "[DEBUG] SurfaceImpl::update - _update_requested: %d\n",
        this->m_blSurface->_update_requested);
    if (this->m_blSurface->_update_requested) {
        this->_egl_update(true);
    }
    this->_wl_surface->commit();

    this->_updating = false;
}

//=================
// Private Slots
//=================

//===========================
// Private Methods(Uniforms)
//===========================

void SurfaceImpl::_set_uniform_fillType(View::FillType fill_type)
{
    glUniform1i(
        glGetUniformLocation(this->_program->id(), "fillType"),
        static_cast<int>(fill_type)
    );
}

void SurfaceImpl::_set_uniform_fillColor(Color color)
{
    float colour[4] = {
        (float)color.red_f(),
        (float)color.green_f(),
        (float)color.blue_f(),
        (float)color.alpha_f(),
    };
    glUniform4fv(
        glGetUniformLocation(this->_program->id(), "fillColor"),
        1,
        colour
    );
}

void SurfaceImpl::_set_uniform_textureIn(const Image& image)
{
    auto rgba32_image = image.converted(Image::Format::Rgba32);

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
}

void SurfaceImpl::_set_uniform_parentPosition(Point position)
{
    float pos[2] = {(float)position.x(), (float)position.y()};
    glUniform2fv(
        glGetUniformLocation(this->_program->id(), "parentPosition"),
        1,
        pos
    );
}

void SurfaceImpl::_set_uniform_parentSize(Size size)
{
    float siz[2] = {(float)size.width(), (float)size.height()};
    glUniform2fv(
        glGetUniformLocation(this->_program->id(), "parentSize"),
        1,
        siz
    );
}

void SurfaceImpl::_set_uniform_resolution(Size size)
{
    float siz[2] = {(float)size.width(), (float)size.height()};
    glUniform2fv(
        glGetUniformLocation(this->_program->id(), "resolution"),
        1,
        siz
    );
}

void SurfaceImpl::_set_uniform_validGeometry(Rect geometry)
{
    float geo[4] = {
        (float)geometry.x(),
        (float)geometry.y(),
        (float)geometry.width(),
        (float)geometry.height()
    };
    glUniform4fv(
        glGetUniformLocation(this->_program->id(), "validGeometry"),
        1,
        geo
    );
}

void SurfaceImpl::_set_uniform_validViewport(Rect viewport)
{
    float vp[4] = {
        (float)viewport.x(),
        (float)viewport.y(),
        (float)viewport.width(),
        (float)viewport.height()
    };
    glUniform4fv(
        glGetUniformLocation(this->_program->id(), "validViewport"),
        1,
        vp
    );
}

void SurfaceImpl::_set_uniform_viewport(Rect viewport)
{
    float vp[4] = {
        (float)viewport.x(),
        (float)viewport.y(),
        (float)viewport.width(),
        (float)viewport.height()
    };
    glUniform4fv(
        glGetUniformLocation(this->_program->id(), "viewport"),
        1,
        vp
    );
}

//==================
// Private Methods
//==================

void SurfaceImpl::_init_program()
{
    using namespace bl::gl;

    auto vertex_shader = std::make_shared<Shader>(Shader::Type::Vertex);
    auto fragment_shader = std::make_shared<Shader>(Shader::Type::Fragment);

    // Append null character to the end.
    auto vert = app->resource()->data("/io.orbitrc.blusher/shaders/shader.vert");
    auto frag = app->resource()->data("/io.orbitrc.blusher/shaders/shader.frag");

    char *vertex_shader_str = new char[vert->size() + 1];
    char *fragment_shader_str = new char[frag->size() + 1];

    strncpy(vertex_shader_str, (char*)vert->data(), vert->size());
    vertex_shader_str[vert->size()] = '\0';
    strncpy(fragment_shader_str, (char*)frag->data(), frag->size());
    fragment_shader_str[frag->size()] = '\0';

    // Compile.
    vertex_shader->compile((const char*)vertex_shader_str);
    fragment_shader->compile((const char*)fragment_shader_str);

    // Free.
    delete[] vertex_shader_str;
    delete[] fragment_shader_str;

    // Attach shaders.
    this->_program->attach_shader(vertex_shader);
    this->_program->attach_shader(fragment_shader);

    // Link the program.
    this->_program->link();
}

void SurfaceImpl::_recursive(View *view,
        std::optional<Rect> valid_viewport,
        Point relative_position,
        Point translation)
{
    for (auto& child: view->children()) {
        // fprintf(stderr, " = recursive: %s\n", child->debug_id().c_str());

        // Set uniforms.
        this->_set_uniform_fillType(child->fill_type());

        if (child->fill_type() == View::FillType::Color) {
            this->_set_uniform_fillColor(child->color());
        } else if (child->fill_type() == View::FillType::Image) {
            this->_set_uniform_textureIn(child->image());
        }

        this->_set_uniform_parentPosition({view->x(), view->y()});
        this->_set_uniform_parentSize(view->geometry().size());

        int parent_x = relative_position.x();
        int parent_y = relative_position.y();

        int viewport_x = parent_x + child->x();
        int viewport_y = this->height()
            - (parent_y + child->y())
            - (child->height() * 1 /*this->scale() */);
        Rect viewport = Rect{
            static_cast<double>(viewport_x), static_cast<double>(viewport_y),
            child->width(), child->height()
        };
        this->_set_uniform_viewport(viewport);
        fprintf(stderr, "Viewport - %s: (%f, %f) %fx%f\n",
            child->debug_id().c_str(),
            viewport.x(), viewport.y(), viewport.width(), viewport.height());

        if (!valid_viewport.has_value()) {
            valid_viewport = view->geometry();
            // Set valid geometry as all zeros. It means no limit(not clip).
            // Child of root view not need to clip.
            this->_set_uniform_validViewport({0.0, 0.0, 0.0, 0.0});
        } else {
            fprintf(stderr, "Valid viewport - %s: (%f, %f) %fx%f\n",
                child->debug_id().c_str(),
                valid_viewport.value().x(), valid_viewport.value().y(),
                valid_viewport.value().width(), valid_viewport.value().height());
            this->_set_uniform_validViewport(valid_viewport.value());
        }
        glViewport(viewport.x(), viewport.y(),
            viewport.width(), viewport.height());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // Call recursive.
        if (child->children().length() != 0) {
            this->_recursive(child,
                valid_viewport.value().intersection(viewport),
                relative_position + Point(child->x(), child->y()),
                {0, 0});
        }
    }
}

void SurfaceImpl::_draw_frame()
{
    // Set the surface viewport.
    glViewport(
        0, 0,
        this->width(), this->height()
    );

    // Clear the color buffer.
    glClearColor(0.5, 0.5, 0.5, 0.8);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object.
    glUseProgram(this->_program->id());

    // Alpha channel blend.
    glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
        GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
        GL_STATIC_DRAW);

    GLuint vbo[2];
    glGenBuffers(2, vbo);

    glBindVertexArray(vao);

    // Position attribute.
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coord attribute.
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(tex_coord),
        tex_coord,
        GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    // Set uniform resolution as the surface's size.
    this->_set_uniform_resolution(this->m_rootView->geometry().size());
    this->_recursive(this->m_rootView, std::nullopt, {0.0, 0.0}, {0.0, 0.0});

    // this->swapBuffers();

    // Free.
    glDeleteBuffers(2, vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vao);
}

void SurfaceImpl::_egl_update(bool swap_buffers)
{
    this->makeCurrent();
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
    if (this->_program->vertex_shader() == nullptr &&
            this->_program->fragment_shader() == nullptr) {
        this->_init_program();
        /*
        if (init_program(this->_program) == 0) {
            fprintf(stderr, "Error init program!\n");
            // exit(1);
            return;
        }
        */
    }

    fprintf(stderr, "[DEBUG] SurfaceImpl::_egl_update - draw_frame\n");
    this->_draw_frame();
    if (swap_buffers == true) {
        this->swapBuffers();
    }

    if (this->m_blSurface->_update_requested) {
        ;
    }

    this->makeCurrent(true);

    this->m_blSurface->_update_requested = false;
}

View* SurfaceImpl::entered_view() const
{
    return this->_entered_view;
}

void SurfaceImpl::set_entered_view(View *view)
{
    this->_entered_view = view;
}

//===========
// Events
//===========

/*
bool SurfaceImpl::event(QEvent *event)
{
    if (event->type() == QEvent::Expose) {
        fprintf(stderr, "event is Expose!\n");
        this->paint();
    }

    return false; // QObject::event(event);
}

void SurfaceImpl::exposeEvent(QExposeEvent *event)
{
    (void)event;

    fprintf(stderr, "exposeEvent\n");
//    if (this->isExposed()) {
//        this->paint();
//    }
}
*/

} // namespace bl
