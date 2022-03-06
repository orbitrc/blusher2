#include "surface-impl.h"

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

#include <blusher/application.h>
#include <blusher/utils.h>

#include "application-impl.h"

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
    // TODO: implement
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
static struct wl_shm_pool* create_shm_pool(bl::SurfaceImpl *surface_impl,
        int width, int height)
{
    struct wl_shm *shm = bl::app_impl->shm();
    struct wl_shm_pool *pool;
    int stride = width * 4; // 4 bytes per pixel.
    int size = stride * height;
    int fd;

    fd = os_create_anonymous_file(size);
    if (fd < 0) {
        fprintf(stderr, "Creating a buffer file for %d B failed: %m\n",
            size);
        return NULL;
    }

    surface_impl->setShmData(
        mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)
    );
    if (surface_impl->shmData() == MAP_FAILED) {
        surface_impl->setShmData(NULL);
        close(fd);
        return NULL;
    }
    surface_impl->setShmDataSize(size);

    pool = wl_shm_create_pool(shm, fd, size);

    surface_impl->setShmFd(fd);
//    close(fd);

    return pool;
}

static void resize_shm_pool(bl::SurfaceImpl *surface_impl,
        int width, int height)
{
    int origin_size = surface_impl->shmDataSize();
    int stride = width * 4;
    int size = stride * height;

    fprintf(stderr, "resize_shm_pool() - origin_size: %d, size: %d\n", origin_size, size);

    if (size > origin_size) {
        fprintf(stderr, "resize_shm_pool() - size is greater than origin size.\n");
        truncate_fd(surface_impl->shmFd(), size);

        surface_impl->setShmData(
            mremap(surface_impl->shmData(), origin_size, size, MREMAP_MAYMOVE)
        );
        surface_impl->setShmDataSize(size);
        wl_shm_pool_resize(surface_impl->shmPool(), size);
    }
}

static struct wl_buffer* create_buffer(bl::SurfaceImpl *surface_impl,
        int width, int height)
{
    fprintf(stderr, "create_buffer: %dx%d\n", width, height);
    struct wl_shm_pool *pool = surface_impl->shmPool();
    int stride = width * 4; // 4 bytes per pixel.
    struct wl_buffer *buff;

    buff = wl_shm_pool_create_buffer(pool, 0, width, height, stride,
        WL_SHM_FORMAT_ARGB8888);

//    wl_shm_pool_destroy(pool);

    return buff;
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

    this->m_visible = false;
    this->m_color = Color::from_rgb(255, 255, 255);

    this->m_pointerEnterHandler = nullptr;
    this->m_pointerLeaveHandler = nullptr;
    this->m_pointerPressHandler = nullptr;

    //===============
    // Wayland
    //===============
    this->_surface = wl_compositor_create_surface(bl::app_impl->compositor());
    this->_subsurface = NULL;
    this->_frame_callback = NULL;
    this->_shm_pool = NULL;
    this->_buffer = NULL;

    this->_shm_data = NULL;
    this->_shm_data_size = 0;

    this->_shm_pool = create_shm_pool(this, this->m_width, this->m_height);
    this->_buffer = create_buffer(this, this->m_width, this->m_height);

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
        this->_xdg_surface = xdg_wm_base_get_xdg_surface(app_impl->xdgWmBase(),
            this->_surface);
        xdg_surface_add_listener(this->_xdg_surface, &xdg_surface_listener, NULL);

        this->_xdg_toplevel = xdg_surface_get_toplevel(this->_xdg_surface);
        xdg_toplevel_add_listener(this->_xdg_toplevel, &xdg_toplevel_listener, NULL);

        // Signal that the surface is ready to be configured.
        wl_surface_commit(this->_surface);
        // Wait for the surface to be configured.
        wl_display_roundtrip(app_impl->display());
    }

//    this->setGeometry(this->m_x, this->m_y, this->m_width, this->m_height);

    app_impl->addSurfaceImpl(this);

    QObject::connect(this, &SurfaceImpl::implXChanged, this, &SurfaceImpl::onImplXChanged);
    QObject::connect(this, &SurfaceImpl::implYChanged, this, &SurfaceImpl::onImplYChanged);
    QObject::connect(this, &SurfaceImpl::implWidthChanged, this, &SurfaceImpl::onImplWidthChanged);
    QObject::connect(this, &SurfaceImpl::implHeightChanged, this, &SurfaceImpl::onImplHeightChanged);
}

SurfaceImpl::~SurfaceImpl()
{
    wl_shm_pool_destroy(this->_shm_pool);

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
    if (this->m_width != width) {
        this->m_width = width;
    }
    if (this->m_height != height) {
        this->m_height = height;
    }

    resize_shm_pool(this, width, height);
    wl_buffer_destroy(this->_buffer);
    this->_buffer = create_buffer(this, width, height);
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

void SurfaceImpl::paint()
{
    if (this->m_visible == false) {
        return;
    }
    fprintf(stderr, "SurfaceImpl::paint() - width: %f, height: %f\n", this->width(), this->height());

    uint32_t *pixel = static_cast<uint32_t*>(this->shmData());

    const uint32_t color = this->m_color.to_argb();
    for (int n = 0; n < (this->width() * this->height()); ++n) {
        *pixel++ = color;
    }
}

void SurfaceImpl::show()
{
    if (this->m_visible != true) {
        this->m_visible = true;
        fprintf(stderr, "visible set to true\n");

        wl_surface_attach(this->_surface, this->_buffer, 0, 0);
        wl_surface_commit(this->_surface);

        if (this->parent() != nullptr) {
            wl_surface_commit(static_cast<SurfaceImpl*>(this->parent())->wlSurface());
        }

        QRegion q_region;
        QExposeEvent event(q_region);
        QCoreApplication::sendEvent(this, &event);
    }
}

void SurfaceImpl::setColor(const Color &color)
{
    this->m_color = color;
}

bool SurfaceImpl::toplevel() const
{
    return this->parent() == nullptr;
}

void SurfaceImpl::setBlSurface(Surface *blSurface)
{
    this->m_blSurface = blSurface;
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
// Shm objects
//=================
int SurfaceImpl::shmFd() const
{
    return this->_shm_fd;
}

void SurfaceImpl::setShmFd(int fd)
{
    this->_shm_fd = fd;
}

struct wl_shm_pool* SurfaceImpl::shmPool()
{
    return this->_shm_pool;
}

void SurfaceImpl::setShmPool(struct wl_shm_pool *shmPool)
{
    this->_shm_pool = shmPool;
}

void* SurfaceImpl::shmData()
{
    return this->_shm_data;
}

void SurfaceImpl::setShmData(void *shmData)
{
    this->_shm_data = shmData;
}

uint64_t SurfaceImpl::shmDataSize() const
{
    return this->_shm_data_size;
}

void SurfaceImpl::setShmDataSize(uint64_t size)
{
    this->_shm_data_size = size;
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
