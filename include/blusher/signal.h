#ifndef _BL_SIGNAL_H
#define _BL_SIGNAL_H

#include <vector>
#include <functional>

namespace bl {

class ConnectionImpl;
class SignalImpl;

template <typename... Args>
class Signal {
public:
    class Connection
    {
        friend Signal;

    public:
        Connection(ConnectionImpl *impl);

    private:
        bool disconnect();

    private:
        ConnectionImpl *_impl;

        Signal<Args...> *_signal;
    };

public:
    Signal();

    void emit(Args... args);

    Connection connect(std::function<void(Args...)> slot);

private:
    SignalImpl *_impl;

    std::vector<Connection> _connections;
};

} // namespace bl

#endif /* _BL_SIGNAL_H */
