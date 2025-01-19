#pragma once

#include "StringFormatter.h"
#include "ErrorConverter.h"
#include "Array.h"
#include "DebugColor.h"

#if defined(CONFIG_USE_SEGGER_RTT)
#include <SEGGER_RTT.h>
#endif

#include <stdio.h>
#include <algorithm>


class Debug
{
public:
    template<typename... Args>
    static constexpr ReturnCode Write(String message, Args... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            return WriteImpl(message);
        }
        else // (constexpr)
        {
            auto tuple = std::forward_as_tuple(args...);

            // get last arg
            auto last = std::get<sizeof...(args) - 1>(tuple);

            // check if last arg is a DebugColor
            if constexpr (std::is_same_v<DebugColor, std::decay_t<decltype(last)>>)
            {
                auto rc = SetColor(last);
                CHECK_RETURN_CODE(rc);

                auto allExceptLast = forward_except_last(
                    std::make_index_sequence<sizeof...(Args) - 1>{}, tuple);

                rc = std::apply([&](auto&&... args){ return Write(message, args...); }, allExceptLast);
                CHECK_RETURN_CODE(rc);

                return ResetColor();
            }
            else // (constexpr)
            {
                Array<char, 256> buffer;
                auto string = StringFormatter::Format(buffer, message, args...);
                return WriteImpl(string);
            }
        }
    }

    template<typename... Args>
    static constexpr ReturnCode WriteLine(String line, Args... args)
    {
        auto rc = Write(line, args...);
        CHECK_RETURN_CODE(rc);

        return WriteLine();
    }

    static constexpr ReturnCode WriteLine()
    {
        return Write("\r\n");
    }

    static constexpr ReturnCode WriteIfError(ReturnCode code)
    {
        return code == ReturnCode::Success ? 
            ReturnCode::Success : 
            WriteLine("Error: %s", GetErrorMessage(code), DebugColor::Red);
    }

    template<typename T>
    static constexpr ReturnCode WriteData(Span<T> data)
    {
        for(auto& value : data)
        {
            if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>)
            {
                auto rc = Write("0x%02hx ", value);
                CHECK_RETURN_CODE(rc);
            }
            else if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t>)
            {
                auto rc = Write("%05d ", value);
                CHECK_RETURN_CODE(rc);
            }
            else if constexpr (std::is_same_v<T, float>)
            {
                auto rc = Write("%.2f, ", value);
                CHECK_RETURN_CODE(rc);
            }
            else
            {
                return ReturnCode::NotSupported;
            }
        }

        return WriteLine();
    }

    static constexpr ReturnCode Clear()
    {
#if defined(CONFIG_USE_SEGGER_RTT)
        return Write(RTT_CTRL_CLEAR);
#else
        return ReturnCode::NotImplemented;
#endif
    }

    static constexpr ReturnCode SetColor(DebugColor color)
    {
        auto code = GetColorCode(color);
        return Write(code);
    }

    static constexpr ReturnCode ResetColor()
    {
#if defined(CONFIG_USE_SEGGER_RTT)
        return Write(RTT_CTRL_RESET);
#else
        return ReturnCode::NotImplemented;
#endif
    }

    static constexpr StringLiteral GetErrorMessage(ReturnCode code)
    {
        switch(code)
        {
            case ReturnCode::InvalidOperation: return "Invalid op";
            case ReturnCode::NotFound: return "Not found";
            case ReturnCode::NotSupported: return "Not supported";
            case ReturnCode::NotImplemented: return "Not implemented";
            case ReturnCode::InvalidArgument: return "Invalid argument";
            case ReturnCode::OutOfRange: return "Out of range";
            case ReturnCode::NullArgument: return "Null argument";
            case ReturnCode::InvalidLength: return "Invalid length";
            case ReturnCode::InvalidData: return "Invalid data";
            case ReturnCode::InvalidState: return "Invalid state";
            case ReturnCode::OutOfMemory: return "Out of memory";
            case ReturnCode::Timeout: return "Timeout";
            case ReturnCode::Busy: return "Busy";
            case ReturnCode::Success: return StringLiteral();
        }

        return StringLiteral();
    }
    
private:
    static const uint32_t Channel = 0;

    static ReturnCode WriteImpl(Span<const char> data)
    {
        // print to UART console
        // auto err = printf("%.*s", data.GetLength(), data.GetData());
        // auto rc = ErrorConverter::Convert(err);
        // CHECK_RETURN_CODE(rc);

#if defined(CONFIG_USE_SEGGER_RTT)
        // print to RTT
        auto err = SEGGER_RTT_Write(Channel, data.GetData(), data.GetLength());
        return ErrorConverter::Convert(err);
#else
        return ReturnCode::NotImplemented;
#endif
    }

    static constexpr StringLiteral GetColorCode(DebugColor color)
    {
#if defined(CONFIG_USE_SEGGER_RTT)
        switch(color)
        {
            case DebugColor::Black: return RTT_CTRL_TEXT_BLACK;
            case DebugColor::Red: return RTT_CTRL_TEXT_RED;
            case DebugColor::Green: return RTT_CTRL_TEXT_GREEN;
            case DebugColor::Yellow: return RTT_CTRL_TEXT_YELLOW;
            case DebugColor::Blue: return RTT_CTRL_TEXT_BLUE;
            case DebugColor::Magenta: return RTT_CTRL_TEXT_MAGENTA;
            case DebugColor::Cyan: return RTT_CTRL_TEXT_CYAN;
            case DebugColor::White: return RTT_CTRL_TEXT_WHITE;
            case DebugColor::BrightBlack: return RTT_CTRL_TEXT_BRIGHT_BLACK;
            case DebugColor::BrightRed: return RTT_CTRL_TEXT_BRIGHT_RED;
            case DebugColor::BrightGreen: return RTT_CTRL_TEXT_BRIGHT_GREEN;
            case DebugColor::BrightYellow: return RTT_CTRL_TEXT_BRIGHT_YELLOW;
            case DebugColor::BrightBlue: return RTT_CTRL_TEXT_BRIGHT_BLUE;
            case DebugColor::BrightMagenta: return RTT_CTRL_TEXT_BRIGHT_MAGENTA;
            case DebugColor::BrightCyan: return RTT_CTRL_TEXT_BRIGHT_CYAN;
            case DebugColor::BrightWhite: return RTT_CTRL_TEXT_BRIGHT_WHITE;
        }
#endif

        return StringLiteral();
    }

    template <typename Tuple, std::size_t... I>
    static auto forward_except_last(std::index_sequence<I...>, Tuple&& tuple) {
        return std::forward_as_tuple(std::get<I>(std::forward<Tuple>(tuple))...);
    }
};