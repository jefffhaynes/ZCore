#pragma once

template<typename TCallbackContext>
struct CallbackContainer
{
    constexpr CallbackContainer(void* context) : Context(context)
    {
    }

    TCallbackContext CallbackContext;
    void* Context;
};