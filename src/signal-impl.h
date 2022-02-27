#ifndef SIGNALIMPL_H
#define SIGNALIMPL_H

#include <functional>

#include <QObject>

#ifdef emit
#undef emit
#endif
#include <blusher/surface.h>

namespace bl {

class ConnectionImpl
{
public:
    ConnectionImpl(QMetaObject::Connection& conn);

    bool disconnect();

private:
    QMetaObject::Connection _q_meta_object_connection;
};


class SignalImpl : public QObject
{
    Q_OBJECT
public:
    SignalImpl();

    ConnectionImpl* connect(std::function<void()> slot);
    ConnectionImpl* connect(std::function<void(int)> slot);
    ConnectionImpl* connect(std::function<void(Surface::State)> slot);

    void emitSignal();
    void emitSignal(int);
    void emitSignal(Surface::State);

signals:
    void signalVoid();
    void signalInt(int);
    void signalSurfaceState(Surface::State);

private:
};

} // namespace bl

#endif // SIGNALIMPL_H
