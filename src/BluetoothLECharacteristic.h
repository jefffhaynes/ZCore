#pragma once

#include "ReturnCode.h"
#include "MemoryMarshal.h"
#include "Clock.h"
#include "CoreString.h"
#include "EventHandler.h"
#include "BluetoothLEConnections.h"
#include "Temperature.h"
#include "Power.h"
#include "Illuminance.h"
#include "SignalStrength.h"
#include "Angle.h"
#include "Nullable.h"

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

class BluetoothLECharacteristicBase
{
public:
    typedef bt_gatt_attr* (*FindAttributeCallback)(const bt_uuid_128& uuid);

    static constexpr TimeSpan DefaultUpdateCooldown = TimeSpan::FromMilliseconds(50);

    BluetoothLECharacteristicBase(bt_uuid_128& uuid, TimeSpan updateCooldown = DefaultUpdateCooldown) : 
        _uuid(uuid), _updateCooldown(updateCooldown), _attribute(nullptr)
    {
    }

    static void SetFindAttributeCallback(FindAttributeCallback callback)
    {
        _findAttributeCallback = callback;
    }

    virtual ReturnCode Read(Span<uint8_t> data, uint32_t& read, uint16_t offset) = 0;
    virtual ReturnCode Write(Span<const uint8_t> data, const bt_conn* connection) = 0;

protected:
    static ReturnCode ReadData(Span<const uint8_t> source, Span<uint8_t> destination, uint32_t& read, uint16_t offset)
    {
        auto chunk = source.Skip(offset).Take(destination.GetLength());
        auto rc = chunk.CopyTo(destination);
        CHECK_RETURN_CODE(rc);

        read = chunk.GetLength();

        return ReturnCode::Success;
    }

    ReturnCode Notify(Span<const uint8_t> data, void* originator)
    {
        if(_attribute == nullptr)
        {
            _attribute = GetAttribute();

            if (_attribute == nullptr)
            {
                return ReturnCode::NotFound;
            }
        }

        if(_lastUpdate + _updateCooldown > Clock::GetUptime())
        {
            return ReturnCode::Success;
        }

        for(auto* connection : BluetoothLEConnections)
        {
            if(connection == originator || !bt_gatt_is_subscribed(connection, _attribute, BT_GATT_CCC_NOTIFY))
            {
                continue;
            }

            auto rc = NotifyImpl(data, connection);
            CHECK_RETURN_CODE(rc);
        }

        _lastUpdate = Clock::GetUptime();
        
        return ReturnCode::Success;
    }

private:
    static FindAttributeCallback _findAttributeCallback;

    bt_uuid_128& _uuid;
    TimeSpan _updateCooldown;
    TimeSpan _lastUpdate;
    bt_gatt_attr* _attribute;

    bt_gatt_attr* GetAttribute()
    {
        if (_findAttributeCallback == nullptr)
        {
            return nullptr;
        }

        return _findAttributeCallback(_uuid);
    }

    ReturnCode NotifyImpl(Span<const uint8_t> data, bt_conn* connection = nullptr)
    {
        bt_gatt_notify_params parameters
        {
            .uuid = nullptr,
            .attr = _attribute,
            .data = data.GetData(),
            .len = (uint16_t) data.GetLength(),
            .func = nullptr,
        };

        // there's a bit of a race condition that means this could fail, but that's ok
        bt_gatt_notify_cb(connection, &parameters);

        return ReturnCode::Success;
    }
};

inline BluetoothLECharacteristicBase::FindAttributeCallback BluetoothLECharacteristicBase::_findAttributeCallback = nullptr;

template<typename TValue>
class BluetoothLEValueCharacteristic : public BluetoothLECharacteristicBase
{
    typedef float TTemperature;
    typedef float TPower;
    typedef float TIlluminance;
    typedef float TSignalStrength;
    typedef float TAngle;
    typedef uint32_t TTime;

public:
    BluetoothLEValueCharacteristic(bt_uuid_128& uuid, TimeSpan updateCooldown = DefaultUpdateCooldown) : 
        BluetoothLECharacteristicBase(uuid, updateCooldown)
    {
    }

    ReturnCode Write(Span<const uint8_t> data, const bt_conn* connection) override
    {
        TValue value;
        auto rc = Convert(data, value);
        CHECK_RETURN_CODE(rc);

        rc = SetValue(value);
        CHECK_RETURN_CODE(rc);

        return Updated.Invoke(value, (void*) connection);
    }

    ReturnCode Read(Span<uint8_t> data, uint32_t& read, uint16_t offset) override
    {
        Nullable<TValue> value;
        auto rc = GetValue(value);
        CHECK_RETURN_CODE(rc);

        if (!value.HasValue())
        {
            return ReturnCode::NotFound;
        }

        return ConvertBack(value.GetValue(), data, read, offset);
    }

    EventHandler<TValue, void*> Updated;

protected:
    virtual ReturnCode GetValue(Nullable<TValue>& value) = 0;
    virtual ReturnCode SetValue(TValue value) = 0;


    ReturnCode Notify(float value, void* connection)
    {
        auto data = MemoryMarshal::AsConstBytes(value);
        return BluetoothLECharacteristicBase::Notify(data, connection);
    }

    ReturnCode Notify(bool value, void* connection)
    {
        auto data = MemoryMarshal::AsConstBytes(value);
        return BluetoothLECharacteristicBase::Notify(data, connection);
    }

    ReturnCode Notify(uint32_t value, void* connection)
    {
        auto data = MemoryMarshal::AsConstBytes(value);
        return BluetoothLECharacteristicBase::Notify(data, connection);
    }

    ReturnCode Notify(Power value, void* connection)
    {
        auto watts = (TPower) value.ToWatts();
        auto data = MemoryMarshal::AsConstBytes(watts);
        return BluetoothLECharacteristicBase::Notify(data, connection);
    }

    ReturnCode Notify(TimeSpan value, void* connection)
    {
	    auto seconds = (TTime) value.ToSeconds();
        auto data = MemoryMarshal::AsConstBytes(seconds);
        return BluetoothLECharacteristicBase::Notify(data, connection);
    }

    ReturnCode Notify(Angle value, void* connection)
    {
        auto degrees = value.ToDegrees();
        auto data = MemoryMarshal::AsConstBytes(degrees);
        return BluetoothLECharacteristicBase::Notify(data, connection);
    }

    ReturnCode Notify(Temperature value, void* connection)
    {
        auto celcius = (TTemperature) value.ToCelsius();
        auto data = MemoryMarshal::AsConstBytes(celcius);
        return BluetoothLECharacteristicBase::Notify(data, connection);
    }
    
    ReturnCode Notify(SignalStrength value, void* connection)
    {
        auto db = (TSignalStrength) value.ToDecibels();
        auto data = MemoryMarshal::AsConstBytes(db);
        return BluetoothLECharacteristicBase::Notify(data, connection);
    }

    ReturnCode Notify(Illuminance value, void* connection)
    {
        auto lux = (TIlluminance) value.ToLux();
        auto data = MemoryMarshal::AsConstBytes(lux);
        return BluetoothLECharacteristicBase::Notify(data, connection);
    }

    static constexpr ReturnCode Convert(Span<const uint8_t> data, float& value)
    {
        auto valueData = MemoryMarshal::AsBytes(value);
        return data.CopyTo(valueData);
    }

    static constexpr ReturnCode Convert(Span<const uint8_t> data, uint32_t& value)
    {
        auto valueData = MemoryMarshal::AsBytes(value);
        return data.CopyTo(valueData);
    }
    
    static constexpr ReturnCode Convert(Span<const uint8_t> data, bool& value)
    {
        auto valueData = MemoryMarshal::AsBytes(value);
        return data.CopyTo(valueData);
    }

    static constexpr ReturnCode Convert(Span<const uint8_t> data, Power& value)
    {
        TPower watts = 0;
        auto valueData = MemoryMarshal::AsBytes(watts);
        auto rc = data.CopyTo(valueData);
        CHECK_RETURN_CODE(rc);

        value = Power::FromWatts(watts);

        return ReturnCode::Success;
    }

    static constexpr ReturnCode Convert(Span<const uint8_t> data, Illuminance& value)
    {
        TIlluminance lux = 0;
        auto valueData = MemoryMarshal::AsBytes(lux);
        auto rc = data.CopyTo(valueData);
        CHECK_RETURN_CODE(rc);

        value = Illuminance::FromLux(lux);

        return ReturnCode::Success;
    }

    static constexpr ReturnCode Convert(Span<const uint8_t> data, Temperature& value)
    {
        TTemperature celsius = 0;
        auto valueData = MemoryMarshal::AsBytes(celsius);
        auto rc = data.CopyTo(valueData);
        CHECK_RETURN_CODE(rc);

        value = Temperature::FromCelsius(celsius);

        return ReturnCode::Success;
    }
    
    static constexpr ReturnCode Convert(Span<const uint8_t> data, SignalStrength& value)
    {
        TSignalStrength db = 0;
        auto valueData = MemoryMarshal::AsBytes(db);
        auto rc = data.CopyTo(valueData);
        CHECK_RETURN_CODE(rc);

        value = SignalStrength::FromDecibels(db);

        return ReturnCode::Success;
    }

    static constexpr ReturnCode Convert(Span<const uint8_t> data, TimeSpan& value)
    {
        TTime seconds = 0;
        auto valueData = MemoryMarshal::AsBytes(seconds);
        auto rc = data.CopyTo(valueData);
        CHECK_RETURN_CODE(rc);

        value = TimeSpan::FromSeconds(seconds);

        return ReturnCode::Success;
    }

    static constexpr ReturnCode Convert(Span<const uint8_t> data, Angle& value)
    {
        TAngle degrees = 0;
        auto valueData = MemoryMarshal::AsBytes(degrees);
        auto rc = data.CopyTo(valueData);
        CHECK_RETURN_CODE(rc);

        value = Angle::FromDegrees(degrees);

        return ReturnCode::Success;
    }

    static ReturnCode ConvertBack(float value, Span<uint8_t> data, uint32_t& read, uint16_t offset)
    {
        auto valueData = MemoryMarshal::AsConstBytes(value);
        return BluetoothLECharacteristicBase::ReadData(valueData, data, read, offset);
    }

    static ReturnCode ConvertBack(uint32_t value, Span<uint8_t> data, uint32_t& read, uint16_t offset)
    {
        auto valueData = MemoryMarshal::AsConstBytes(value);
        return BluetoothLECharacteristicBase::ReadData(valueData, data, read, offset);
    }

    static ReturnCode ConvertBack(bool value, Span<uint8_t> data, uint32_t& read, uint16_t offset)
    {
        auto valueData = MemoryMarshal::AsConstBytes(value);
        return BluetoothLECharacteristicBase::ReadData(valueData, data, read, offset);
    }
    
    static ReturnCode ConvertBack(Temperature value, Span<uint8_t> data, uint32_t& read, uint16_t offset)
    {
        auto celsius = (TTemperature) value.ToCelsius();
        auto valueData = MemoryMarshal::AsConstBytes(celsius);
        return BluetoothLECharacteristicBase::ReadData(valueData, data, read, offset);
    }

    static ReturnCode ConvertBack(Power value, Span<uint8_t> data, uint32_t& read, uint16_t offset)
    {
        auto watts = (TPower) value.ToWatts();
        auto valueData = MemoryMarshal::AsConstBytes(watts);
        return BluetoothLECharacteristicBase::ReadData(valueData, data, read, offset);
    }
    
    static ReturnCode ConvertBack(SignalStrength value, Span<uint8_t> data, uint32_t& read, uint16_t offset)
    {
        auto db = (TSignalStrength) value.ToDecibels();
        auto valueData = MemoryMarshal::AsConstBytes(db);
        return BluetoothLECharacteristicBase::ReadData(valueData, data, read, offset);
    }

    static ReturnCode ConvertBack(Illuminance value, Span<uint8_t> data, uint32_t& read, uint16_t offset)
    {
        auto lux = (TIlluminance) value.ToLux();
        auto valueData = MemoryMarshal::AsConstBytes(lux);
        return BluetoothLECharacteristicBase::ReadData(valueData, data, read, offset);
    }
    
    static ReturnCode ConvertBack(TimeSpan value, Span<uint8_t> data, uint32_t& read, uint16_t offset)
    {
        auto seconds = (TTime) value.ToSeconds();
        auto valueData = MemoryMarshal::AsConstBytes(seconds);
        return BluetoothLECharacteristicBase::ReadData(valueData, data, read, offset);
    }
    
    static ReturnCode ConvertBack(Angle value, Span<uint8_t> data, uint32_t& read, uint16_t offset)
    {
        auto degrees = (TAngle) value.ToDegrees();
        auto valueData = MemoryMarshal::AsConstBytes(degrees);
        return BluetoothLECharacteristicBase::ReadData(valueData, data, read, offset);
    }
};

template<typename TValue>
class BluetoothLECharacteristic : public BluetoothLEValueCharacteristic<TValue>
{
public:
    BluetoothLECharacteristic(bt_uuid_128& uuid, TValue updateThreshold, TValue initialValue, 
        TimeSpan updateCooldown = BluetoothLECharacteristicBase::DefaultUpdateCooldown) :
        BluetoothLEValueCharacteristic<TValue>(uuid, updateCooldown), _updateThreshold(updateThreshold), _value(initialValue)
    {
    }

    ReturnCode Update(TValue value, void* originator = nullptr)
    {
        Nullable<TValue> current;
        auto rc = GetValue(current);
        CHECK_RETURN_CODE(rc);

        TValue currentValue;
        if (current.TryGetValue(currentValue))
        {
            if constexpr(std::is_same_v<TValue, bool>)
            {
                if (currentValue == value)
                {
                    return ReturnCode::Success;
                }
            }
            else if (currentValue < value + _updateThreshold && value < currentValue + _updateThreshold)
            {
                return ReturnCode::Success;
            }
        }

        rc = BluetoothLEValueCharacteristic<TValue>::Notify(value, originator);
        CHECK_RETURN_CODE(rc);

        return SetValue(value);
    }

protected:
    ReturnCode GetValue(Nullable<TValue>& value) override
    {
        value = _value;
        return ReturnCode::Success;
    }

    ReturnCode SetValue(TValue value) override
    {
        _value = value;
        return ReturnCode::Success;
    }

private:
    TValue _updateThreshold;
    Nullable<TValue> _value;
};

template<typename TValue>
class BluetoothLESettingCharacteristic : public BluetoothLEValueCharacteristic<TValue>
{
public:
    typedef TValue (*SettingGetter)();
    typedef ReturnCode (*SettingSetter)(TValue);

    BluetoothLESettingCharacteristic(bt_uuid_128& uuid,
        SettingGetter getter, SettingSetter setter = nullptr, 
        TimeSpan updateCooldown = BluetoothLECharacteristicBase::DefaultUpdateCooldown) : 
            BluetoothLEValueCharacteristic<TValue>(uuid, updateCooldown), _getter(getter), _setter(setter)
    {
    }

    ReturnCode TriggerUpdate(void* connection = nullptr)
    {
        TValue value = _getter();
        return BluetoothLEValueCharacteristic<TValue>::Notify(value, connection);
    }

protected:
    ReturnCode GetValue(Nullable<TValue>& value) override
    {
        value = _getter();
        return ReturnCode::Success;
    }

    ReturnCode SetValue(TValue value) override
    {
        if(_setter == nullptr)
        {
            return ReturnCode::Success;
        }

        return _setter(value);
    }

private:
    SettingGetter _getter;
    SettingSetter _setter;

    ReturnCode Notify(TValue value)
    {
        return BluetoothLEValueCharacteristic<TValue>::Notify(value, nullptr);
    }
};


class BluetoothLENameCharacteristic : public BluetoothLECharacteristicBase
{
public:
    BluetoothLENameCharacteristic(bt_uuid_128& uuid) : 
        BluetoothLECharacteristicBase(uuid)
    {
    }

    ReturnCode TriggerUpdate(void* connection = nullptr)
    {
        auto name = String::FromNullTerminated(bt_get_name());
        auto data = name.AsConstBytes();
        return BluetoothLECharacteristicBase::Notify(data, connection);
    }
    
    ReturnCode Write(Span<const uint8_t> data, const bt_conn* connection) override
    {
        Array<char, CONFIG_BT_DEVICE_NAME_MAX + 1> nameData;
        
        if(data.GetLength() > CONFIG_BT_DEVICE_NAME_MAX)
        {
            return ReturnCode::InvalidLength;
        }
        
        auto span = nameData.AsBytes();
        auto rc = data.CopyTo(span);
        CHECK_RETURN_CODE(rc);

                rc = SetName(nameData);
                CHECK_RETURN_CODE(rc);

                return TriggerUpdate((void*) connection);
    }

    ReturnCode Read(Span<uint8_t> data, uint32_t& read, uint16_t offset) override
    {
        auto name = String::FromNullTerminated(bt_get_name());
	    auto nameSpan = name.AsConstBytes();
	    return ReadData(nameSpan, data, read, offset);
    }

private:
    ReturnCode SetName(StringLiteral name)
    {
        auto err = bt_set_name(name.GetData());
        return ErrorConverter::Convert(err);
    }
};