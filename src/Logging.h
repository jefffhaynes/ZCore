#pragma once

#include "CoreString.h"
#include "MemoryMarshal.h"
#include "Streams/StorageOutputStream.h"
#include "Streams/StorageInputStream.h"

#include "Debug.h"

class Logging
{
public:
    static ReturnCode WriteLine(String message)
    {
        auto length = (uint16_t) message.GetLength();
        auto lengthData = MemoryMarshal::AsBytes(length);

        auto rc = _stream.Write(lengthData);
        CHECK_RETURN_CODE(rc);

        auto messageData = message.AsConstBytes();
        rc = _stream.Write(messageData);
        CHECK_RETURN_CODE(rc);

        auto terminator = (uint16_t) 0;
        auto terminatorData = MemoryMarshal::AsBytes(terminator);

        rc = _stream.Write(terminatorData);
        CHECK_RETURN_CODE(rc);

        return _stream.Flush();
    }

    static ReturnCode PrintAll()
    {
        // read through the file and print all the lines to debug
        StorageInputStream stream(LogFile, FileMode::Open);

        Array<uint8_t, 256> buffer;

        while(true)
        {
            uint16_t length;
            auto readData = MemoryMarshal::AsBytes(length);

            uint32_t read;
            auto rc = stream.Read(readData, read);

            if(rc == ReturnCode::NotFound)
            {
                return ReturnCode::Success;
            }
            
            CHECK_RETURN_CODE(rc);

            if(length == 0)
            {
                break;
            }
            
            auto readBlock = buffer.Take(length);

            rc = stream.Read(readBlock, read);
            CHECK_RETURN_CODE(rc);

            auto message = String::FromNullTerminated(reinterpret_cast<char*>(readBlock.GetData()));

            Debug::WriteLine(message);
        }

        return ReturnCode::Success;
    }

private:
    static constexpr StringLiteral LogFile = "/log.txt";

    static StorageOutputStream _stream;
    static bool _initialized;
};

inline StorageOutputStream Logging::_stream = StorageOutputStream(Logging::LogFile, FileMode::Create);
inline bool Logging::_initialized = false;