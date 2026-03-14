#pragma once

#include <Array.h>
#include <Aligned.h>
#include <zephyr/kernel.h>


template<uint32_t BlockSize, uint32_t BlockCount, uint32_t Alignment = 4>
class MemorySlab 
{
    static_assert((BlockSize % Alignment) == 0, "BlockSize must be a multiple of Alignment");
    
public:
    constexpr MemorySlab()
    {
        k_mem_slab_init(&_slab, _buffer.Value.GetData(), BlockSize, BlockCount);
    }

    MemorySlab(const MemorySlab&) = delete;
    MemorySlab& operator=(const MemorySlab&) = delete;
    MemorySlab(MemorySlab&&) = delete;
    MemorySlab& operator=(MemorySlab&&) = delete;

    k_mem_slab* GetSlab()
    {
        return &_slab;
    }

    void Free(void* block)
    {
        k_mem_slab_free(&_slab, block);
    }

private:
    Aligned<Array<uint8_t, BlockSize * BlockCount>, Alignment> _buffer;
    struct k_mem_slab _slab;
};
