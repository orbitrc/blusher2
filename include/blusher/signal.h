#ifndef _BL_SIGNAL_H
#define _BL_SIGNAL_H

#include <memory>
#include <functional>

#include <primer/vector.h>

namespace bl {

template <typename... Args>
class Signal {
public:
    class Connection
    {
        friend Signal;

    private:
        Connection(Signal *signal, std::function<void(Args...)>&& slot)
        {
            this->_signal = signal;
            this->_slot = slot;
        }

        void call(Args... args)
        {
            this->_slot(args...);
        }

    public:
        ~Connection()
        {
        }

        bool disconnect()
        {
            if (this->_signal == nullptr) {
                return false;
            }
            this->_signal = nullptr;
            this->_slot = [](Args...){};

            return true;
        }

    private:
        Signal<Args...> *_signal;
        std::function<void(Args...)> _slot;
    };

public:
    Signal()
    {
    }

    ~Signal()
    {
    }

    std::shared_ptr<Connection> connect(std::function<void(Args...)>&& slot)
    {
        auto conn = std::shared_ptr<Connection>(
            new Connection(this, std::move(slot)));

        this->_connections.push(conn);

        return conn;
    }

    template<typename T>
    std::shared_ptr<Connection> connect(T *receiver,
            void(T::* slot)(Args... args))
    {
        std::function<void(Args...)> f{[receiver, slot](Args... args){
            (receiver->*slot)(std::forward<Args...>(args)...);
        }};

        return this->connect(std::move(f));
    }

    void emit(Args... args)
    {
        for (auto& conn: this->_connections) {
            conn->call(args...);
        }
    }

private:
    pr::Vector<std::shared_ptr<Connection>> _connections;
};

} // namespace bl

#endif /* _BL_SIGNAL_H */
