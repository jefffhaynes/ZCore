#pragma once

#include "ReturnCode.h"
#include "Array.h"


template<typename ...Args>
class EventHandler
{
public:
    typedef ReturnCode (*EventHandlerCallback)(Args... args, void* context);

    static constexpr uint32_t MaxCallbackCount = 2;
    
    constexpr EventHandler()
    {
    }

    constexpr ReturnCode Subscribe(EventHandlerCallback callback, void* context = nullptr)
    {
        for(auto& entry : _callbacks)
        {
            if(entry.callback == nullptr)
            {
                entry.callback = callback;
                entry.context = context;
                return ReturnCode::Success;
            }
        }
        
        return ReturnCode::OutOfMemory;
    }

    template<typename T, ReturnCode (T::*F)(Args...)>
    constexpr ReturnCode Subscribe(void* subscriber)
    {
        return Subscribe([](Args... args, void* context)
            { 
                return (static_cast<T*>(context)->*F)(args...); 
            }, subscriber);
    }

    constexpr ReturnCode Invoke(Args... args)
    {   
        for(auto& entry : _callbacks)
        {
            if(entry.callback != nullptr)
            {
                auto rc = entry.callback(args..., entry.context);
                CHECK_RETURN_CODE(rc);
            }
        }

        return ReturnCode::Success;
    }

private:
    struct CallbackEntry
    {
        EventHandlerCallback callback;
        void* context;
    };

    Array<CallbackEntry, MaxCallbackCount> _callbacks = {};
};


template<typename ...Args>
class EventHandlerDelegate
{
public:
    constexpr EventHandlerDelegate(EventHandler<Args...>& handler) : 
        _handler(handler)
    {
    }

    constexpr ReturnCode Subscribe(typename EventHandler<Args...>::EventHandlerCallback callback, 
        void* context = nullptr)
    {
        return _handler.Subscribe(callback, context);
    }
    
    template<typename T, ReturnCode (T::*F)(Args...)>
    constexpr ReturnCode Subscribe(void* context)
    {
        return _handler.template Subscribe<T, F>(context);
    }

private:
    EventHandler<Args...>& _handler;
};