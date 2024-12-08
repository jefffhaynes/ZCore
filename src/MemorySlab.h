#pragma once

#include <kernel.h>

template<typename T, uint32_t Count, uint32_t Alignment>
class MemorySlab
{
public:
    constexpr MemorySlab()
    {
        k_mem_slab_init(&_slab, _buffer.Value, sizeof(T), Count);
    }

    constexpr ReturnCode Alloc(T*& item, TimeSpan timeout = TimeSpan::Zero)
    {
        auto err = k_mem_slab_alloc(&_slab, reinterpret_cast<void**>(&item),
            K_MSEC(timeout.ToMilliseconds()));
        return ErrorConverter::Convert(err);
    }

    constexpr ReturnCode Free(T* item)
    {
        auto err = k_mem_slab_free(&_slab, reinterpret_cast<void*>(item));
        return ErrorConverter::Convert(err);
    }

private:
    Aligned<T, Alignment> _buffer[Count * sizeof(T)];
    struct k_mem_slab _slab;
};


// #define K_MEM_SLAB_DEFINE_STATIC(name, slab_block_size, slab_num_blocks, slab_align) 
	static char __noinit_named(k_mem_slab_buf_my_slab) 
                __aligned(WB_UP(slab_align)) 
                _k_mem_slab_buf_my_slab[(slab_num_blocks) * WB_UP(slab_block_size)];


	static STRUCT_SECTION_ITERABLE(k_mem_slab, name) = \
		Z_MEM_SLAB_INITIALIZER(name, _k_mem_slab_buf_##name, \
					WB_UP(slab_block_size), slab_num_blocks)