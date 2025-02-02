#pragma once

#include "OutputStream.h"
#include "MemoryMarshal.h"

class StreamWriter
{
public:
    constexpr StreamWriter(OutputStream& stream) : _stream(stream)
    {
    }

    template <Arithmetic T>
    constexpr ReturnCode Write(T value)
    {
        for (uint32_t i = 0; i < sizeof(T); i++)
        {
            auto byte = static_cast<uint8_t>(value >> (i * 8));
            auto rc = _stream.Write(Span(&byte, 1));
            CHECK_RETURN_CODE(rc);
        }

        return ReturnCode::Success;
    }

    template <Enum T>
    constexpr ReturnCode Write(T value)
    {
        auto underlying = static_cast<std::underlying_type_t<T>>(value);
        return Write(underlying);
    }

    template <ComplexSafe T>
    constexpr ReturnCode Write(T& value)
    {
        auto span = MemoryMarshal::AsConstBytes(value);
        return _stream.Write(span);
    }

    constexpr ReturnCode Write(Span<const uint8_t> data)
    {
        return _stream.Write(data);
    }

private:
    OutputStream& _stream;
};
