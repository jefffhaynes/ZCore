#pragma once

#include "SettingBase.h"
#include <Range.h>
#include <Nullable.h>
#include "CoreString.h"
#include "UnitHelper.h"

// A persisted value. An optional Range is enforced at every boundary the value
// can cross: Set() clamps before persisting (so a bad write can never be stored)
// and Sanitize() clamps what was loaded from storage (so a stale or corrupt
// stored value can never reach the code that applies it). Keep the range in
// sync with whatever consumes the setting, ideally by sharing one constant.

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

    constexpr Setting(StringLiteral key, T defaultValue, Range<T> range, bool throttle = false) : SettingBase(key, throttle),
        _value(defaultValue), _defaultValue(defaultValue), _range(range)
    {
    }

    constexpr T Get() const
    {
        return _value;
    }

    constexpr ReturnCode Set(T value)
    {
        value = Constrain(value);
        auto data = MemoryMarshal::AsConstBytes(value);
        auto rc = SettingBase::Save(data);
        CHECK_RETURN_CODE(rc);

        _value = value;

        return ReturnCode::Success;
    }

    constexpr ReturnCode Reset() override
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

    void Sanitize() override
    {
        _value = Constrain(_value);
    }

private:
    T _value;
    T _defaultValue;
    Nullable<Range<T>> _range;

    constexpr T Constrain(T value) const
    {
        return _range.HasValue() ? _range.GetValue().Clamp(value) : value;
    }
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

    constexpr Setting(StringLiteral key, T defaultValue, Range<T> range, bool throttle = false) : SettingBase(key, throttle),
        _value(defaultValue), _defaultValue(defaultValue), _range(range)
    {
    }

    constexpr T Get() const
    {
        return _value;
    }

    constexpr ReturnCode Set(T value)
    {
        value = Constrain(value);
        auto rawValue = UnitHelper::ToRawValue(value);
        auto data = MemoryMarshal::AsConstBytes(rawValue);
        auto rc = SettingBase::Save(data);
        CHECK_RETURN_CODE(rc);

        _value = value;

        return ReturnCode::Success;
    }

    constexpr ReturnCode Reset() override
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

    void Sanitize() override
    {
        _value = Constrain(_value);
    }

private:
    T _value;
    T _defaultValue;
    Nullable<Range<T>> _range;

    constexpr T Constrain(T value) const
    {
        return _range.HasValue() ? _range.GetValue().Clamp(value) : value;
    }
};
