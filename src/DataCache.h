#pragma once

#include <stdint.h>
#include <zephyr/cache.h>
#include "Span.h"
#include "ReturnCode.h"
#include "ErrorConverter.h"

#if defined(CONFIG_DCACHE) && !defined(CONFIG_CACHE_MANAGEMENT)
#error "The D-cache is on but CONFIG_CACHE_MANAGEMENT is off, so maintenance would silently do nothing"
#endif

// Keeps the D-cache coherent with DMA. Invalidate discards whole lines, so it takes whole lines only.
class DataCache
{
public:
#if defined(CONFIG_DCACHE)
    static constexpr uint32_t LineSize = CONFIG_DCACHE_LINE_SIZE;
    static_assert(LineSize > 0, "Needs a fixed CONFIG_DCACHE_LINE_SIZE");
#else
    static constexpr uint32_t LineSize = 1;
#endif

    static constexpr uint32_t RoundUp(uint32_t length)
    {
        return (length + LineSize - 1) / LineSize * LineSize;
    }

    // Before the CPU reads what DMA wrote.
    template<typename T>
    static ReturnCode Invalidate(Span<T> span)
    {
        if(!span.IsAligned(LineSize))
        {
            return ReturnCode::InvalidArgument;
        }

#if defined(CONFIG_DCACHE)
        auto err = sys_cache_data_invd_range(GetAddress(span), GetSize(span));
        return ErrorConverter::Convert(err);
#else
        return ReturnCode::Success;
#endif
    }

    // Before DMA reads what the CPU wrote.
    template<typename T>
    static ReturnCode Flush(Span<T> span)
    {
#if defined(CONFIG_DCACHE)
        auto err = sys_cache_data_flush_range(GetAddress(span), GetSize(span));
        return ErrorConverter::Convert(err);
#else
        return ReturnCode::Success;
#endif
    }

private:
    template<typename T>
    static void* GetAddress(Span<T> span)
    {
        return const_cast<void*>(static_cast<const void*>(span.GetData()));
    }

    template<typename T>
    static size_t GetSize(Span<T> span)
    {
        return span.GetLength() * sizeof(T);
    }
};
