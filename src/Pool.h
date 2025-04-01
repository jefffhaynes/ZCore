#pragma once

#include "Array.h"

class IPool
{
public:
    virtual ~IPool() = default;

    virtual void Release(int index) = 0;
};

template<typename T>
class PoolItemHandle
{
public:
    constexpr PoolItemHandle() : _item(nullptr), _pool(nullptr)
    {
    }

    constexpr PoolItemHandle(T* item, IPool* pool, int index)
        : _item(item), _pool(pool), _index(index)
    {
    }

    constexpr ~PoolItemHandle()
    {
        Release();
    }

    constexpr PoolItemHandle(const PoolItemHandle&) = delete;
    constexpr PoolItemHandle& operator=(const PoolItemHandle&) = delete;

    constexpr PoolItemHandle(PoolItemHandle&& other)
        : _item(other._item), _pool(other._pool), _index(other._index)
    {
        other._item = nullptr;
        other._pool = nullptr;
    }

    constexpr PoolItemHandle& operator=(PoolItemHandle&& other)
    {
        if (this != &other)
        {
            Release();
            _item = other._item;
            _pool = other._pool;
            _index = other._index;
            other._item = nullptr;
            other._pool = nullptr;
        }

        return *this;
    }

    constexpr T& Get() { return *_item; }
    constexpr const T& Get() const { return *_item; }
    constexpr bool IsValid() const { return _item != nullptr; }

private:
    constexpr void Release()
    {
        if (_pool != nullptr)
        {
            _pool->Release(_index);
            _item = nullptr;
            _pool = nullptr;
        }
    }

    T* _item;
    IPool* _pool;
    int _index;
};

template<typename T, int Size>
class Pool : public IPool
{
public:
    using Handle = PoolItemHandle<T>;

    constexpr ~Pool() = default;

    constexpr ReturnCode Acquire(PoolItemHandle<T>& outHandle)
    {
        for (int i = 0; i < Size; ++i)
        {
            if (!_inUse[i])
            {
                _inUse[i] = true;
                outHandle = PoolItemHandle<T>(&_pool[i], this, i);
                return ReturnCode::Success;
            }
        }

        return ReturnCode::OutOfMemory;
    }

    constexpr void Release(int index) override
    {
        _inUse.TrySet(index, false);
    }

private:
    friend class PoolItemHandle<T>;

    Array<T, Size> _pool;
    Array<bool, Size> _inUse;
};
