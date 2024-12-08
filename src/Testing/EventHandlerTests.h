#pragma once

#include "EventHandler.h"
#include "TestSupport.h"

namespace EventHandlerTests
{
    static_assert([]{
        EventHandler<int> handler;
        int value = 0;
        auto rc = handler.Subscribe([](int v, void* ctx)
        {
            auto& value = *reinterpret_cast<int*>(ctx);
            value = v;
            return ReturnCode::Success;
        }, &value);

        if(rc != ReturnCode::Success)
        {
            return false;
        }

        rc = handler.Invoke(5);

        if(rc != ReturnCode::Success)
        {
            return false;
        }

        return value == 5;
    }(), "EventHandler failed");

    static_assert([]{
        EventHandler<int> handler;
        auto rc = handler.Subscribe([](int v, void* ctx)
        {
            return ReturnCode::InvalidOperation;
        });

        if(rc != ReturnCode::Success)
        {
            return false;
        }

        rc = handler.Subscribe([](int v, void* ctx)
        {
            return ReturnCode::Success;
        });
        
        if(rc != ReturnCode::Success)
        {
            return false;
        }

        rc = handler.Invoke(5);
        return rc == ReturnCode::InvalidOperation;
    }(), "EventHandler failed");

    static_assert([]{
        EventHandler<int> handler;

        auto rc = ReturnCode::Success;

        bool allSubscribedSuccessfully = constexpr_for<0, handler.MaxCallbackCount, 1>([&](auto i)
        {
            return handler.Subscribe([](int v, void* ctx)
            {
                return ReturnCode::Success;
            }) == ReturnCode::Success;
        });

        if(!allSubscribedSuccessfully)
        {
            return false;
        }

        if(rc != ReturnCode::Success)
        {
            return false;
        }

        rc = handler.Subscribe([](int v, void* ctx)
        {
            return ReturnCode::Success;
        });

        return rc == ReturnCode::OutOfMemory;
    }(), "EventHandler allowed over-subscription");


    static_assert([]{
        EventHandler<int> handler;
        EventHandlerDelegate<int> delegate(handler);

        int value = 0;
        auto rc = delegate.Subscribe([](int v, void* ctx)
        {
            auto& value = *reinterpret_cast<int*>(ctx);
            value = v;
            return ReturnCode::Success;
        }, &value);

        if(rc != ReturnCode::Success)
        {
            return false;
        }

        rc = handler.Invoke(5);

        if(rc != ReturnCode::Success)
        {
            return false;
        }

        return value == 5;
    }(), "EventHandlerDelegate failed");
}