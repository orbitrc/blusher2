#include <blusher/signal.h>

#include <stdint.h>

#include "signal-impl.h"

#ifdef emit
#undef emit
#endif

#define BLUSHER_SIGNAL_CONSTRUCTOR { this->_impl = new SignalImpl(); }
#define BLUSHER_CONNECTION_CONSTRUCTOR { this->_impl = impl; }
#define BLUSHER_CONNECTION_CONSTRUCTOR_ARG(type) template<> \
    Signal<type>::Connection::Connection(ConnectionImpl *impl) \
    { this->_impl = impl; }
#define BLUSHER_CONNECTION_DISCONNECT_VOID template<> \
    bool Signal<>::Connection::disconnect() \
    { return this->_impl->disconnect(); }
#define BLUSHER_CONNECTION_DISCONNECT_ARG(type) template<> \
    bool Signal<type>::Connection::disconnect() \
    { return this->_impl->disconnect(); }

namespace bl {

//===============
// void
//===============

template<>
Signal<>::Signal()
BLUSHER_SIGNAL_CONSTRUCTOR

template<>
Signal<>::Connection::Connection(ConnectionImpl *impl)
BLUSHER_CONNECTION_CONSTRUCTOR

BLUSHER_CONNECTION_DISCONNECT_VOID

template<>
void Signal<>::emit()
{
    this->_impl->emitSignal();
}

template<>
Signal<>::Connection Signal<>::connect(std::function<void()> slot)
{
    return Connection(this->_impl->connect(slot));
}

//===============
// int
//===============

template<>
Signal<int>::Signal()
BLUSHER_SIGNAL_CONSTRUCTOR

BLUSHER_CONNECTION_CONSTRUCTOR_ARG(int)

BLUSHER_CONNECTION_DISCONNECT_ARG(int)

template<>
void Signal<int>::emit(int arg)
{
    this->_impl->emitSignal(arg);
}

template<>
Signal<int>::Connection Signal<int>::connect(std::function<void(int)> slot)
{
    return Connection(this->_impl->connect(slot));
}

//==================
// Surface::State
//==================

template<>
Signal<Surface::State>::Signal()
BLUSHER_SIGNAL_CONSTRUCTOR

BLUSHER_CONNECTION_CONSTRUCTOR_ARG(Surface::State)

BLUSHER_CONNECTION_DISCONNECT_ARG(Surface::State)

template<>
void Signal<Surface::State>::emit(Surface::State arg)
{
    this->_impl->emitSignal(arg);
}

template<>
Signal<Surface::State>::Connection Signal<Surface::State>::connect(
        std::function<void(Surface::State)> slot)
{
    return Connection(this->_impl->connect(slot));
}

} // namespace bl
