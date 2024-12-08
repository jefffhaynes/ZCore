#pragma once

#include "Span.h"
#include "OutputStream.h"
#include "Array.h"
#include "Clock.h"

class InputStream
{
public:
    virtual ReturnCode Read(Span<uint8_t> data, uint32_t& read) = 0;

    ReturnCode CopyTo(OutputStream& stream)
    {
        ReturnCode rc;
        Array<uint8_t, 128> block;
        uint32_t read;

        while ((rc = Read(block, read)) == ReturnCode::Success && read > 0)
        {
            auto readBlock = block.Take(read);
            rc = stream.Write(readBlock);
            CHECK_RETURN_CODE(rc);
        }

        return rc;
    }

    ReturnCode CopyTo(OutputStream& stream, uint32_t length, TimeSpan timeout = DefaultTimeout)
    {
        ReturnCode rc;
        Array<uint8_t, 128> block;
        uint32_t read = 0;

        while(read < length)
        {
            auto remaining = length - read;
            auto readLength = remaining < block.GetLength() ? remaining : block.GetLength();
            auto readBlock = block.Take(readLength);

            rc = Read(readBlock, timeout);
            CHECK_RETURN_CODE(rc);

            rc = stream.Write(readBlock);
            CHECK_RETURN_CODE(rc);

            read += readBlock.GetLength();
        }

        return rc;
    }

    ReturnCode Read(Span<uint8_t> data, TimeSpan timeout = DefaultTimeout)
    {
        auto start = Clock::GetUptime();

        uint32_t totalRead = 0;

        while (totalRead < data.GetLength())
        {
            auto elapsed = Clock::GetUptime() - start;

            if (elapsed > timeout)
            {
                return ReturnCode::Timeout;
            }

            auto remainder = data.Skip(totalRead);

            uint32_t read;
            auto rc = Read(remainder, read);
            CHECK_RETURN_CODE(rc);

            totalRead += read;

            if (totalRead < data.GetLength())
            {
                Clock::Sleep(TimeSpan::FromMicroseconds(10));
            }
        }

        return ReturnCode::Success;
    }

private:
    static constexpr TimeSpan DefaultTimeout = TimeSpan::FromSeconds(1);
};