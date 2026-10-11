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

    StringLiteral GetKey() const
    {
        return _key;
    }

    // True once the value is known to be in storage: one was loaded from there
    // whole, or this one was saved there. False while it's only the default,
    // as when storage holds nothing for the key, or holds something of another
    // size, or couldn't be read. A value that must never start over from its
    // default, such as a counter, is checked with this before it's used.
    bool IsStored() const
    {
        return _stored;
    }

protected:
    constexpr SettingBase(StringLiteral key, bool throttle) : _key(key), _throttle(throttle)
    {
    }

    // Called after a value has been loaded from storage into GetValuePointer().
    // Implementations bring the value back into their valid range; storage is
    // not rewritten (the next Set() will).
    virtual void Sanitize()
    {
    }

    ReturnCode Clear()
    {
        auto err = settings_delete(_key.GetData());
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        _dirty = false;
        _stored = false;

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

    virtual uint32_t GetValueLength() = 0;
    virtual void* GetValuePointer() = 0;

private:
    static const uint32_t MaxValueLength = 64;

    StringLiteral _key;
    Array<uint8_t, MaxValueLength> _valueBuffer;
    Span<const uint8_t> _valueSpan;
    bool _throttle;
    bool _dirty = false;
    bool _stored = false;

    // Takes the stored value, read through Zephyr's callback. It's read aside
    // first and taken only if all of it came, so that a read that fails or
    // falls short leaves the value as it was, whole, and no longer known to
    // be in storage. Zephyr's error, or 0.
    int Load(settings_read_cb read, void* argument)
    {
        _stored = false;

        auto length = GetValueLength();
        Span<uint8_t> value(static_cast<uint8_t*>(GetValuePointer()), length);
        Array<uint8_t, MaxValueLength> buffer;
        auto aside = buffer.Take(length);

        // A value too long to read aside is read where it lies, as it always was.
        auto destination = aside.GetLength() == length ? aside : value;
        auto count = read(argument, destination.GetData(), length);

        if(count < 0)
        {
            return static_cast<int>(count);
        }

        if(static_cast<uint32_t>(count) != length)
        {
            return 0;
        }

        if(aside.GetLength() == length && aside.CopyTo(value) != ReturnCode::Success)
        {
            return 0;
        }

        Sanitize();
        _stored = true;

        return 0;
    }

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
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        _stored = true;

        return ReturnCode::Success;
    }
};
