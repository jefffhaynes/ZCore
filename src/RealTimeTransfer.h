#pragma once

#include <SEGGER_RTT.h>
#include "CoreString.h"
#include "ErrorConverter.h"
#include "Array.h"
#include "Timer.h"
#include "MemoryMarshal.h"

class RealTimeTransfer
{
public:
    static ReturnCode Initialize(String name = String())
    {
        if(IsInitialized)
        {
            return ReturnCode::Success;
        }

        auto err = SEGGER_RTT_ConfigUpBuffer(Channel, name.GetData(), 
            TxBuffer.GetData(), TxBuffer.GetLength(), SEGGER_RTT_MODE_NO_BLOCK_SKIP);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        err = SEGGER_RTT_ConfigDownBuffer(Channel, name.GetData(), 
            RxBuffer.GetData(), RxBuffer.GetLength(), SEGGER_RTT_MODE_NO_BLOCK_SKIP);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);
        
        rc = PollTimer.Expired.Subscribe(Poll);
        CHECK_RETURN_CODE(rc);

        rc = PollTimer.Start(PollInterval, TimerMode::Repeating);
        CHECK_RETURN_CODE(rc);

        IsInitialized = true;

        return ReturnCode::Success;
    }

    static ReturnCode Send(Span<const uint8_t> data)
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);
        
        auto err = SEGGER_RTT_Write(Channel, data.GetData(), data.GetLength());
        return ErrorConverter::Convert(err);
    }
    
    static EventHandler<Span<const uint8_t>> Received;

private:
    static const uint32_t Channel = 1;
    static constexpr String Name = "Control";
    static const uint32_t TxBufferSize = 256;
    static const uint32_t RxBufferSize = 16;
    static constexpr TimeSpan PollInterval = TimeSpan::FromMilliseconds(1);

    static bool IsInitialized;
    static Array<char, TxBufferSize> TxBuffer;
    static Array<char, RxBufferSize> RxBuffer;
    static Timer PollTimer;


    static ReturnCode Poll(void* context)
    {
        Array<char, RxBufferSize> buffer;
        auto read = SEGGER_RTT_Read(Channel, buffer.GetData(), buffer.GetLength());

        if(read == 0)
        {
            return ReturnCode::Success;
        }

        auto readBlock = buffer.Take(read);
        auto data = readBlock.AsConstBytes();
        return Received.Invoke(data);
    }
};

inline EventHandler<Span<const uint8_t>> RealTimeTransfer::Received;
inline bool RealTimeTransfer::IsInitialized = false;
inline Array<char, RealTimeTransfer::TxBufferSize> RealTimeTransfer::TxBuffer;
inline Array<char, RealTimeTransfer::RxBufferSize> RealTimeTransfer::RxBuffer;
inline Timer RealTimeTransfer::PollTimer = Timer(TimerOptions::Scheduled);