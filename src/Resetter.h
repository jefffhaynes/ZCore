#pragma once

template <typename T>
class Resetter
{
public:
    constexpr Resetter(T& reference, T resetValue = T()) : _reference(reference), _resetValue(resetValue)
    {
    }

    ~Resetter()
    {
        _reference = _resetValue;
    }
    
private:
    T& _reference;
    T _resetValue;
};
