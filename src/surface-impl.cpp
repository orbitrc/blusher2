#include "surface-impl.h"

#include <stdio.h>

#include <QBackingStore>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>

#include <blusher/application.h>

#include "application-impl.h"

namespace bl {

SurfaceImpl::SurfaceImpl(QWindow *parent)
    : QWindow(parent)
{
    this->m_backingStore = new QBackingStore(this);

    this->setFlag(Qt::FramelessWindowHint, true);

    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    this->setFormat(format);

    this->m_x = 0;
    this->m_y = 0;
    this->m_width = 100.0;
    this->m_height = 100.0;

    this->m_color = QColor(255, 255, 255);

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

    this->setGeometry(this->m_x, this->m_y, this->m_width, this->m_height);

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
    if (!isExposed()) {
        return;
    }
    fprintf(stderr, "SurfaceImpl::paint() - width: %f, height: %f\n", this->width(), this->height());

    QRect rect(0, 0, this->width(), this->height());
    this->m_backingStore->beginPaint(rect);

    QPaintDevice *device = this->m_backingStore->paintDevice();
    QPainter painter(device);

    painter.fillRect(0, 0, this->width(), this->height(), this->m_color);

    painter.end();

    this->m_backingStore->endPaint();
    this->m_backingStore->flush(rect);
}

void SurfaceImpl::setColor(const Color &color)
{
    this->m_color = QColor(color.red(), color.green(), color.blue(), color.alpha());
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
// Private Slots
//=================

void SurfaceImpl::onImplXChanged(double x)
{
    if (this->parent() != nullptr) {
        QWindow::setX(x);
    }
//    emit this->xChanged(static_cast<int>(x));
}

void SurfaceImpl::onImplYChanged(double y)
{
    if (this->parent() != nullptr) {
        QWindow::setY(y);
    }
//    emit this->yChanged(static_cast<int>(y));
}

void SurfaceImpl::onImplWidthChanged(double width)
{
    QWindow::setWidth(width);
//    emit this->widthChanged(static_cast<int>(width));
}

void SurfaceImpl::onImplHeightChanged(double height)
{
    QWindow::setHeight(height);
//    emit this->heightChanged(static_cast<int>(height));
}

//===========
// Events
//===========

bool SurfaceImpl::event(QEvent *event)
{
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

    return QWindow::event(event);
}

void SurfaceImpl::exposeEvent(QExposeEvent *event)
{
    (void)event;

    if (this->isExposed()) {
        this->paint();
    }
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
    this->m_backingStore->resize(event->size());
}

} // namespace bl
