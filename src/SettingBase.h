#pragma once

#include "CoreString.h"
#include "ErrorConverter.h"
#include "Array.h"
#include <zephyr/settings/settings.h>


class SettingBase
{
friend class SettingsBase;

public:
    virtual ReturnCode Reset() = 0;

protected:
    constexpr SettingBase(StringLiteral key, bool throttle) : _key(key), _throttle(throttle)
    {
    }

    ReturnCode Clear()
    {
        auto err = settings_delete(_key.GetData());
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        _dirty = false;

        return ReturnCode::Success;
    }

    ReturnCode Save(Span<const uint8_t> value)
    {
        if (_throttle)
        {
            auto rc = value.CopyTo(_valueBuffer.AsSpan());
            CHECK_RETURN_CODE(rc);

            _valueSpan = _valueBuffer.Take(value.GetLength());

            _dirty = true;

            return ReturnCode::Success;
        }

        return SaveImpl(value);
    }

    StringLiteral GetKey() const
    {
        return _key;
    }

    virtual uint32_t GetValueLength() = 0;
    virtual void* GetValuePointer() = 0;

private:
    static const uint32_t MaxValueLength = 64;

    StringLiteral _key;
    Array<uint8_t, MaxValueLength> _valueBuffer;
    Span<const uint8_t> _valueSpan;
    bool _throttle;
    bool _dirty = false;

    ReturnCode Flush()
    {
        if (!_dirty)
        {
            return ReturnCode::Success;
        }

        auto rc = SaveImpl(_valueSpan);
        CHECK_RETURN_CODE(rc);

        _dirty = false;

        return ReturnCode::Success;
    }

    ReturnCode SaveImpl(Span<const uint8_t> value)
    {
        auto err = settings_save_one(_key.GetData(), value.GetData(), value.GetLength());
        return ErrorConverter::Convert(err);
    }
};
