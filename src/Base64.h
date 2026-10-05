#pragma once

#include <stdint.h>
#include "Span.h"
#include "SpanWriter.h"
#include "ReturnCode.h"

// RFC 4648's standard alphabet, padded with '='.
class Base64
{
public:
    static constexpr uint32_t GetEncodedLength(uint32_t length)
    {
        return (length + GroupBytes - 1) / GroupBytes * GroupSymbols;
    }

    // InvalidLength if `text` can't hold it.
    static constexpr ReturnCode Encode(Span<const uint8_t> data, Span<char> text, uint32_t& written)
    {
        SpanWriter<char> writer(text);

        written = 0;

        for(auto group = data; !group.IsEmpty(); group = group.Skip(GroupBytes))
        {
            uint32_t bits = 0;
            uint32_t count = 0;

            for(auto value : group.Take(GroupBytes))
            {
                bits |= static_cast<uint32_t>(value) << (BitsPerByte * (GroupBytes - 1 - count));
                count++;
            }

            for(uint32_t i = 0; i < GroupSymbols; i++)
            {
                auto symbol = i <= count ? ToSymbol((bits >> (BitsPerSymbol * (GroupSymbols - 1 - i))) & SymbolMask) : Padding;

                auto rc = writer.Write(symbol);
                CHECK_RETURN_CODE(rc);
            }
        }

        written = writer.GetWritten();

        return ReturnCode::Success;
    }

private:
    static constexpr uint32_t GroupBytes = 3;
    static constexpr uint32_t GroupSymbols = 4;
    static constexpr uint32_t BitsPerByte = 8;
    static constexpr uint32_t BitsPerSymbol = 6;
    static constexpr uint32_t SymbolMask = 0x3F;
    static constexpr uint32_t Letters = 26;
    static constexpr uint32_t Digits = 10;
    static constexpr char Padding = '=';

    // A-Z, a-z, 0-9, then + and /. Computed, so there's no table to read past.
    static constexpr char ToSymbol(uint32_t value)
    {
        if(value < Letters)
        {
            return static_cast<char>('A' + value);
        }

        if(value < 2 * Letters)
        {
            return static_cast<char>('a' + (value - Letters));
        }

        if(value < 2 * Letters + Digits)
        {
            return static_cast<char>('0' + (value - 2 * Letters));
        }

        return value == 2 * Letters + Digits ? '+' : '/';
    }
};
