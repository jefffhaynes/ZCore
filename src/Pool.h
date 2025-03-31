#pragma once

#include "Array.h"

class IPool
{
public:
    virtual ~IPool() = default;

    virtual void Release(int index) = 0;
};

template<typename T>
class PooledHandle
{
public:
    constexpr PooledHandle() : _item(nullptr), _pool(nullptr)
    {
    }

    constexpr PooledHandle(T* item, IPool* pool, int index)
        : _item(item), _pool(pool), _index(index)
    {
    }

    constexpr ~PooledHandle()
    {
        Release();
    }

    constexpr PooledHandle(const PooledHandle&) = delete;
    constexpr PooledHandle& operator=(const PooledHandle&) = delete;

    constexpr PooledHandle(PooledHandle&& other)
        : _item(other._item), _pool(other._pool), _index(other._index)
    {
        other._item = nullptr;
        other._pool = nullptr;
    }

    constexpr PooledHandle& operator=(PooledHandle&& other)
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
    using Handle = PooledHandle<T>;

    constexpr ~Pool() = default;

    constexpr ReturnCode Acquire(PooledHandle<T>& outHandle)
    {
        for (int i = 0; i < Size; ++i)
        {
            if (!_inUse[i])
            {
                _inUse[i] = true;
                outHandle = PooledHandle<T>(&_pool[i], this, i);
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
    friend class PooledHandle<T>;

    Array<T, Size> _pool;
    Array<bool, Size> _inUse;
};
