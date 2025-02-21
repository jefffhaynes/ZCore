#pragma once

#include "CoreString.h"
#include <SEGGER_RTT.h>

class DebugColors
{
public:
    static constexpr StringLiteral Black = RTT_CTRL_TEXT_BLACK;
    static constexpr StringLiteral Red = RTT_CTRL_TEXT_RED;
    static constexpr StringLiteral Green = RTT_CTRL_TEXT_GREEN;
    static constexpr StringLiteral Yellow = RTT_CTRL_TEXT_YELLOW;
    static constexpr StringLiteral Blue = RTT_CTRL_TEXT_BLUE;
    static constexpr StringLiteral Magenta = RTT_CTRL_TEXT_MAGENTA;
    static constexpr StringLiteral Cyan = RTT_CTRL_TEXT_CYAN;
    static constexpr StringLiteral White = RTT_CTRL_TEXT_WHITE;
    static constexpr StringLiteral BrightBlack = RTT_CTRL_TEXT_BRIGHT_BLACK;
    static constexpr StringLiteral BrightRed = RTT_CTRL_TEXT_BRIGHT_RED;
    static constexpr StringLiteral BrightGreen = RTT_CTRL_TEXT_BRIGHT_GREEN;
    static constexpr StringLiteral BrightYellow = RTT_CTRL_TEXT_BRIGHT_YELLOW;
    static constexpr StringLiteral BrightBlue = RTT_CTRL_TEXT_BRIGHT_BLUE;
    static constexpr StringLiteral BrightMagenta = RTT_CTRL_TEXT_BRIGHT_MAGENTA;
    static constexpr StringLiteral BrightCyan = RTT_CTRL_TEXT_BRIGHT_CYAN;
    static constexpr StringLiteral BrightWhite = RTT_CTRL_TEXT_BRIGHT_WHITE;
    static constexpr StringLiteral Reset = RTT_CTRL_RESET;
};
