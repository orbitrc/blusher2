#include "surface-impl.h"

#include <stdio.h>

// Unix
#include <sys/mman.h>
#include <unistd.h>

#include <QBackingStore>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>

#include <blusher/application.h>
#include <blusher/utils.h>

#include "application-impl.h"

//=============
// Drawing
//=============
static struct wl_buffer* create_buffer(bl::SurfaceImpl *surface_impl,
        int width, int height)
{
    fprintf(stderr, "create_buffer: %dx%d\n", width, height);
    struct wl_shm *shm = bl::app_impl->shm();
    struct wl_shm_pool *pool;
    int stride = width * 4; // 4 bytes per pixel.
    int size = stride * height;
    int fd;
    struct wl_buffer *buff;

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
    buff = wl_shm_pool_create_buffer(pool, 0, width, height, stride,
        WL_SHM_FORMAT_ARGB8888);

    close(fd);
    wl_shm_pool_destroy(pool);

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
    this->_buffer = NULL;

    this->_shm_data = NULL;
    this->_shm_data_size = 0;

    this->_buffer = create_buffer(this, this->m_width, this->m_height);

//    this->setGeometry(this->m_x, this->m_y, this->m_width, this->m_height);

    QObject::connect(this, &SurfaceImpl::implXChanged, this, &SurfaceImpl::onImplXChanged);
    QObject::connect(this, &SurfaceImpl::implYChanged, this, &SurfaceImpl::onImplYChanged);
    QObject::connect(this, &SurfaceImpl::implWidthChanged, this, &SurfaceImpl::onImplWidthChanged);
    QObject::connect(this, &SurfaceImpl::implHeightChanged, this, &SurfaceImpl::onImplHeightChanged);
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

        emit this->implXChanged(x);
    }
}

void SurfaceImpl::setY(double y)
{
    if (this->m_y != y) {
        this->m_y = y;

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

        this->paint();
        wl_surface_attach(this->_surface, this->_buffer, 0, 0);
        wl_surface_commit(this->_surface);

        QRegion q_region;
        QExposeEvent event(q_region);
        QCoreApplication::sendEvent(this, &event);
    }
}

void SurfaceImpl::setColor(const Color &color)
{
    this->m_color = color;
}

void SurfaceImpl::setBlSurface(Surface *blSurface)
{
    this->m_blSurface = blSurface;
}

void SurfaceImpl::setPointerEnterHandler(void (Surface::*handler)())
{
    this->m_pointerEnterHandler = handler;
}

void SurfaceImpl::setPointerLeaveHandler(void (Surface::*handler)())
{
    this->m_pointerLeaveHandler = handler;
}

void SurfaceImpl::setPointerPressHandler(void (Surface::*handler)(int, double, double))
{
    this->m_pointerPressHandler = handler;
}

void SurfaceImpl::setPointerReleaseHandler(void (Surface::*handler)(int, double, double))
{
    this->m_pointerReleaseHandler = handler;
}

//=================
// Shm objects
//=================
void* SurfaceImpl::shmData()
{
    return this->_shm_data;
}

void SurfaceImpl::setShmData(void *shmData)
{
    if (this->_shm_data != NULL) {
        munmap(this->_shm_data, this->_shm_data_size);
    }

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
            button = SurfaceImplButtonLeft;
            break;
        case Qt::RightButton:
            button = SurfaceImplButtonRight;
            break;
        case Qt::MiddleButton:
            button = SurfaceImplButtonMiddle;
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
            button = SurfaceImplButtonLeft;
            break;
        case Qt::RightButton:
            button = SurfaceImplButtonRight;
            break;
        case Qt::MiddleButton:
            button = SurfaceImplButtonMiddle;
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