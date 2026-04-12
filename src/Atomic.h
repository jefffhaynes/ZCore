#pragma once

#include <atomic>

template<typename T>
class Atomic
{
public:
    Atomic() : _value()
    {
    }

    Atomic(T value) : _value(value)
    {
    }

    Atomic(const Atomic&) = delete;
    Atomic& operator=(const Atomic&) = delete;

    void Store(T value)
    {
        _value.store(value);
    }

    T Load() const
    {
        return _value.load();
    }

    T Exchange(T value)
    {
        return _value.exchange(value);
    }

    bool CompareExchange(T& expected, T desired)
    {
        return _value.compare_exchange_strong(expected, desired);
    }

private:
    std::atomic<T> _value;
};