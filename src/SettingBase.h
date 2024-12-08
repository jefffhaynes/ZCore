#pragma once

#include "CoreString.h"
#include "ErrorConverter.h"
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
        return ErrorConverter::Convert(err);
    }

    ReturnCode Save(Span<const uint8_t> value)
    {
        auto err = settings_save_one(_key.GetData(), value.GetData(), value.GetLength());
        return ErrorConverter::Convert(err);
    }

    StringLiteral GetKey() const
    {
        return _key;
    }

    virtual uint32_t GetValueLength() = 0;
    virtual void* GetValuePointer() = 0;

private:
    StringLiteral _key;
};
