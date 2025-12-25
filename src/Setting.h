#pragma once

#include "SettingBase.h"
#include "CoreString.h"
#include "UnitHelper.h"

template <typename T, bool UnitLike = IsUnit<T>>
class Setting;

template <typename T>
class Setting<T, false> : public SettingBase
{
public:
    constexpr Setting(StringLiteral key, T defaultValue, bool throttle = false) : SettingBase(key, throttle),
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


// specialization for Unit types

template <typename T>
class Setting<T, true> : public SettingBase
{
public:
    constexpr Setting(StringLiteral key, T defaultValue, bool throttle = false) : SettingBase(key, throttle),
        _value(defaultValue), _defaultValue(defaultValue)
    {
    }

    constexpr T Get() const
    {
        return _value;
    }

    constexpr ReturnCode Set(T value)
    {
        auto rawValue = UnitHelper::ToRawValue(value);
        auto data = MemoryMarshal::AsConstBytes(rawValue);
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
