#pragma once

#include "Rgb565.h"

namespace Rgb565Tests
{
    static_assert(Rgb565().Value == 0, "Default constructor failed");
    static_assert(Rgb565(0x1234).Value == 0x1234, "Value constructor failed");

    static_assert(Rgb565::FromRgb(255, 0, 0).Value == 0xF800, "FromRgb method failed for red");
    static_assert(Rgb565::FromRgb(0, 255, 0).Value == 0x07E0, "FromRgb method failed for green");
    static_assert(Rgb565::FromRgb(0, 0, 255).Value == 0x001F, "FromRgb method failed for blue");
    static_assert(Rgb565::FromRgb(255, 255, 255).Value == 0xFFFF, "FromRgb method failed for white");
    static_assert(Rgb565::FromRgb(0x80, 0x80, 0x80).Value == 0x8410, "FromRgb method failed for grey");
    static_assert(Rgb565::FromRgb(7, 3, 7).Value == 0, "FromRgb method kept bits below a channel's depth");

    constexpr Rgb565 white(0xFFFF);
    static_assert(white.GetRed() == 255 && white.GetGreen() == 255 && white.GetBlue() == 255,
        "Full scale didn't widen to full scale");

    constexpr Rgb565 grey(0x8410);
    static_assert(grey.GetRed() == 0x84 && grey.GetGreen() == 0x82 && grey.GetBlue() == 0x84,
        "Channel getters failed");

    static_assert(Rgb565(0xF800).GetRed() == 255 && Rgb565(0xF800).GetGreen() == 0 && Rgb565(0xF800).GetBlue() == 0,
        "Channel getters mixed channels");

    static_assert([]{
        for(uint32_t value = 0; value <= 0xFFFF; value += 251)
        {
            Rgb565 pixel(static_cast<uint16_t>(value));

            if(!(Rgb565::FromRgb(pixel.GetRed(), pixel.GetGreen(), pixel.GetBlue()) == pixel))
            {
                return false;
            }
        }

        return true;
    }(), "Widened channels didn't pack back to the same pixel");

    static_assert(Rgb565::FromRgb(1, 2, 3) == Rgb565::FromRgb(1, 2, 3)
        && !(Rgb565::FromRgb(255, 0, 0) == Rgb565::FromRgb(0, 0, 255)), "Equality operator failed");
}
