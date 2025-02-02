#pragma once

#include "OutputStream.h"
#include "SpanWriter.h"

class BufferedOutputStream : public OutputStream
{
public:
    constexpr BufferedOutputStream(OutputStream& stream, Span<uint8_t> buffer) : 
        _stream(stream), _buffer(buffer), _writer(buffer)
    {
    }

    constexpr ~BufferedOutputStream()
    {
        Flush();
    }

    constexpr ReturnCode Write(Span<const uint8_t> data) override
    {
        auto offset = 0;
        auto remaining = data.GetLength();

        while (remaining > 0)
        {
            auto available = _buffer.GetLength() - _writer.GetWritten();
            auto write = std::min(remaining, available);
            auto block = data.Skip(offset).Take(write);
            auto rc = _writer.Write(block);
            CHECK_RETURN_CODE(rc);

            offset += write;
            remaining -= write;

            if (_writer.IsFull())
            {
                rc = Flush();
                CHECK_RETURN_CODE(rc);
            }
        }

        return ReturnCode::Success;
    }

    constexpr ReturnCode Flush() override
    {
        auto span = _writer.GetWrittenSpan();
        auto rc = _stream.Write(span);
        CHECK_RETURN_CODE(rc);

        _writer.Reset();

        return ReturnCode::Success;
    }

private:
    OutputStream& _stream;
    Span<uint8_t> _buffer;
    SpanWriter<uint8_t> _writer;
};