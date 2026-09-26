#pragma once

#include "Span.h"
#include "OutputStream.h"
#include "Array.h"
#include "Clock.h"

class InputStream
{
public:
    virtual ReturnCode Read(Span<uint8_t> data, uint32_t& read) = 0;

    // A live source such as a UART never ends: nothing read means nothing yet.
    virtual constexpr bool IsEndOfStream()
    {
        return false;
    }

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

    constexpr ReturnCode Read(Span<uint8_t> data, TimeSpan timeout = DefaultTimeout)
    {
        uint32_t totalRead = 0;
        bool waiting = false;
        auto start = TimeSpan::Zero();

        while (totalRead < data.GetLength())
        {
            auto remainder = data.Skip(totalRead);

            uint32_t read = 0;
            auto rc = Read(remainder, read);
            CHECK_RETURN_CODE(rc);

            totalRead += read;

            if (totalRead < data.GetLength())
            {
                if (IsEndOfStream())
                {
                    return ReturnCode::InvalidLength;
                }

                // The clock is read only when waiting, so reads that don't wait stay constexpr.
                if (!waiting)
                {
                    start = Clock::GetUptime();
                    waiting = true;
                }
                else if (Clock::GetUptime() - start > timeout)
                {
                    return ReturnCode::Timeout;
                }

                Clock::Sleep(TimeSpan::FromMicroseconds(10));
            }
        }

        return ReturnCode::Success;
    }

private:
    static constexpr TimeSpan DefaultTimeout = TimeSpan::FromSeconds(1);
};