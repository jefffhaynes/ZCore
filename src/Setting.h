#pragma once

#include "SettingBase.h"
#include "CoreString.h"
#include "TimeSpan.h"
#include "Units/SignalStrength.h"

template <typename T>
class Setting : public SettingBase
{
public:
    constexpr Setting(StringLiteral key, T defaultValue) : SettingBase(key),
        _value(defaultValue), _defaultValue(defaultValue)
    {
    }

    constexpr T Get() const
    {
        return _value;
    }

    constexpr ReturnCode Set(T value)
    {
        auto data = MemoryMarshal::AsConstBytes(value);
        auto rc = SettingBase::Save(data);
        CHECK_RETURN_CODE(rc);

        _value = value;

        return ReturnCode::Success;
    }

    constexpr ReturnCode Reset()
    {
        auto rc = SettingBase::Clear();
        CHECK_RETURN_CODE(rc);

        _value = _defaultValue;

        return ReturnCode::Success;
    }

protected:
    uint32_t GetValueLength() override
    {
        return sizeof(T);
    }

    void* GetValuePointer() override
    {
        return &_value;
    }

private:
    T _value;
    T _defaultValue;
};