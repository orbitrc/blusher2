#include "signal-impl.h"

namespace bl {

ConnectionImpl::ConnectionImpl(QMetaObject::Connection& conn)
{
    this->_q_meta_object_connection = conn;
}

bool ConnectionImpl::disconnect()
{
    return QObject::disconnect(this->_q_meta_object_connection);
}

SignalImpl::SignalImpl()
{
}

//================
// void
//================
ConnectionImpl* SignalImpl::connect(std::function<void()> slot)
{
    auto qConn = QObject::connect(this, &SignalImpl::signalVoid, this, [slot]() {
        slot();
    });

    return new ConnectionImpl(qConn);
}

void SignalImpl::emitSignal()
{
    Q_EMIT this->signalVoid();
}

//==================
// int
//==================
ConnectionImpl* SignalImpl::connect(std::function<void(int)> slot)
{
    auto qConn = QObject::connect(this, &SignalImpl::signalInt, this, [slot](int arg) {
        slot(arg);
    });

    return new ConnectionImpl(qConn);
}

void SignalImpl::emitSignal(int arg)
{
    Q_EMIT this->signalInt(arg);
}

//===================
// Surface::State
//===================
ConnectionImpl* SignalImpl::connect(std::function<void(Surface::State)> slot)
{
    auto qConn = QObject::connect(this, &SignalImpl::signalSurfaceState,
                                  this, [slot](Surface::State arg) {
        slot(arg);
    });

    return new ConnectionImpl(qConn);
}

void SignalImpl::emitSignal(Surface::State arg)
{
    Q_EMIT this->signalSurfaceState(arg);
}

}
