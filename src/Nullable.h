#pragma once

template<typename T>
class Nullable
{
public:
    constexpr Nullable() : _hasValue(false)
    {
    }

    constexpr Nullable(T value) : _hasValue(true), _value(value)
    {
    }

    constexpr Nullable& operator=(T value) 
    {
        _value = value;
        _hasValue = true;
        return *this;
    }

    constexpr bool operator==(T value) const
    {
        if(!_hasValue)
        {
            return false;
        }

        return _value == value;
    }

    constexpr bool operator!=(T value) const
    {
        if(!_hasValue)
        {
            return true;
        }

        return _value != value;
    }

    constexpr bool HasValue()
    {
        return _hasValue;
    }

    constexpr T& GetValue()
    {
        return _value;
    }

    constexpr bool TryGetValue(T& value)
    {
        if(!_hasValue)
        {
            return false;
        }

        value = _value;

        return true;
    }

    constexpr T GetValueOrDefault()
    {
        return _hasValue ? _value : T();
    }

private:
    bool _hasValue;
    T _value;
};