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
    constexpr SettingBase(StringLiteral key) : _key(key)
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
        auto rc = value.CopyTo(_valueBuffer.AsSpan());
        CHECK_RETURN_CODE(rc);

        _valueLength = value.GetLength();

        _dirty = true;

        return ReturnCode::Success;
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
    uint32_t _valueLength = 0;
    bool _dirty = false;

    ReturnCode Flush()
    {
        if (!_dirty)
        {
            return ReturnCode::Success;
        }

        auto err = settings_save_one(_key.GetData(), _valueBuffer.GetData(), _valueLength);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        _dirty = false;

        return ReturnCode::Success;
    }
};
