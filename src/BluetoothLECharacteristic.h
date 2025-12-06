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

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

class BluetoothLECharacteristicBase
{
public:
    BluetoothLECharacteristicBase(bt_uuid_128& uuid, TimeSpan updateCooldown = TimeSpan::FromMilliseconds(50)) : 
        _uuid(uuid), _updateCooldown(updateCooldown), _attribute(nullptr)
    {
    }

    static void SetFirstServiceAttribute(const bt_gatt_attr* attribute)
    {
        _firstServiceAttribute = attribute;
    }

    virtual ReturnCode Read(Span<uint8_t> data, uint32_t& read) = 0;
    virtual ReturnCode Write(Span<const uint8_t> data, const bt_conn* connection) = 0;

protected:
    ReturnCode Notify(Span<const uint8_t> data, void* originator)
    {
        if(_attribute == nullptr)
        {
            _attribute = GetAttribute();
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
    bt_uuid_128& _uuid;
    TimeSpan _updateCooldown;
    TimeSpan _lastUpdate;
    static const bt_gatt_attr* _firstServiceAttribute;
    bt_gatt_attr* _attribute;

    bt_gatt_attr* GetAttribute()
    {
        return bt_gatt_find_by_uuid(_firstServiceAttribute, 1, &_uuid.uuid);
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

inline const bt_gatt_attr* BluetoothLECharacteristicBase::_firstServiceAttribute = nullptr;

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
    BluetoothLEValueCharacteristic(bt_uuid_128& uuid, TValue updateThreshold) : 
        BluetoothLECharacteristicBase(uuid), _updateThreshold(updateThreshold)
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

    ReturnCode Read(Span<uint8_t> data, uint32_t& read) override
    {
        TValue value;
        auto rc = GetValue(value);
        CHECK_RETURN_CODE(rc);

        return ConvertBack(value, data, read);
    }

    ReturnCode Update(TValue value, void* originator = nullptr)
    {
        TValue currentValue;
        auto rc = GetValue(currentValue);
        CHECK_RETURN_CODE(rc);

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

        return Notify(value, originator);
    }

    EventHandler<TValue, void*> Updated;

protected:
    virtual ReturnCode GetValue(TValue& value) = 0;
    virtual ReturnCode SetValue(TValue value) = 0;

private:
    TValue _updateThreshold;

    ReturnCode Notify(float value, void* connection)
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

    static constexpr ReturnCode ConvertBack(float value, Span<uint8_t> data, uint32_t& read)
    {
        auto valueData = MemoryMarshal::AsConstBytes(value);
        auto rc = valueData.CopyTo(data);
        CHECK_RETURN_CODE(rc);

        read = valueData.GetLength();

        return ReturnCode::Success;
    }

    static constexpr ReturnCode ConvertBack(uint32_t value, Span<uint8_t> data, uint32_t& read)
    {
        auto valueData = MemoryMarshal::AsConstBytes(value);
        auto rc = valueData.CopyTo(data);
        CHECK_RETURN_CODE(rc);

        read = valueData.GetLength();

        return ReturnCode::Success;
    }

    static constexpr ReturnCode ConvertBack(bool value, Span<uint8_t> data, uint32_t& read)
    {
        auto valueData = MemoryMarshal::AsConstBytes(value);
        auto rc = valueData.CopyTo(data);
        CHECK_RETURN_CODE(rc);

        read = valueData.GetLength();

        return ReturnCode::Success;
    }
    
    static constexpr ReturnCode ConvertBack(Temperature value, Span<uint8_t> data, uint32_t& read)
    {
        auto celsius = (TTemperature) value.ToCelsius();
        auto valueData = MemoryMarshal::AsConstBytes(celsius);
        auto rc = valueData.CopyTo(data);
        CHECK_RETURN_CODE(rc);

        read = valueData.GetLength();

        return ReturnCode::Success;
    }

    static constexpr ReturnCode ConvertBack(Power value, Span<uint8_t> data, uint32_t& read)
    {
        auto watts = (TPower) value.ToWatts();
        auto valueData = MemoryMarshal::AsConstBytes(watts);
        auto rc = valueData.CopyTo(data);
        CHECK_RETURN_CODE(rc);

        read = valueData.GetLength();

        return ReturnCode::Success;
    }
    
    static constexpr ReturnCode ConvertBack(SignalStrength value, Span<uint8_t> data, uint32_t& read)
    {
        auto db = (TSignalStrength) value.ToDecibels();
        auto valueData = MemoryMarshal::AsConstBytes(db);
        auto rc = valueData.CopyTo(data);
        CHECK_RETURN_CODE(rc);

        read = valueData.GetLength();

        return ReturnCode::Success;
    }

    static constexpr ReturnCode ConvertBack(Illuminance value, Span<uint8_t> data, uint32_t& read)
    {
        auto lux = (TIlluminance) value.ToLux();
        auto valueData = MemoryMarshal::AsConstBytes(lux);
        auto rc = valueData.CopyTo(data);
        CHECK_RETURN_CODE(rc);

        read = valueData.GetLength();

        return ReturnCode::Success;
    }
    
    static constexpr ReturnCode ConvertBack(TimeSpan value, Span<uint8_t> data, uint32_t& read)
    {
        auto seconds = (TTime) value.ToSeconds();
        auto valueData = MemoryMarshal::AsConstBytes(seconds);
        auto rc = valueData.CopyTo(data);
        CHECK_RETURN_CODE(rc);

        read = valueData.GetLength();

        return ReturnCode::Success;
    }
    
    static constexpr ReturnCode ConvertBack(Angle value, Span<uint8_t> data, uint32_t& read)
    {
        auto degrees = (TTime) value.ToDegrees();
        auto valueData = MemoryMarshal::AsConstBytes(degrees);
        auto rc = valueData.CopyTo(data);
        CHECK_RETURN_CODE(rc);

        read = valueData.GetLength();

        return ReturnCode::Success;
    }
};

template<typename TValue>
class BluetoothLECharacteristic : public BluetoothLEValueCharacteristic<TValue>
{
public:
    BluetoothLECharacteristic(bt_uuid_128& uuid, TValue updateThreshold, TValue initialValue) :
        BluetoothLEValueCharacteristic<TValue>(uuid, updateThreshold), _value(initialValue)
    {
    }

protected:
    ReturnCode GetValue(TValue& value) override
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
    TValue _value;
};

template<typename TValue>
class BluetoothLESettingCharacteristic : public BluetoothLEValueCharacteristic<TValue>
{
public:
    typedef TValue (*SettingGetter)();
    typedef ReturnCode (*SettingSetter)(TValue);

    BluetoothLESettingCharacteristic(bt_uuid_128& uuid, TValue updateThreshold,
        SettingGetter getter, SettingSetter setter = nullptr) :
        BluetoothLEValueCharacteristic<TValue>(uuid, updateThreshold), 
        _getter(getter), _setter(setter)
    {
    }

protected:
    ReturnCode GetValue(TValue& value) override
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
};


class BluetoothLENameCharacteristic : public BluetoothLECharacteristicBase
{
public:
    BluetoothLENameCharacteristic(bt_uuid_128& uuid) : 
        BluetoothLECharacteristicBase(uuid)
    {
    }
    
    ReturnCode Write(Span<const uint8_t> data, const bt_conn* connection) override
    {
        Array<char, CONFIG_BT_DEVICE_NAME_MAX> nameData;
        
        if(data.GetLength() >= nameData.GetLength())
        {
            return ReturnCode::InvalidLength;
        }
        
        auto span = nameData.AsBytes();
        auto rc = data.CopyTo(span);
        CHECK_RETURN_CODE(rc);

        return SetName(nameData);
    }

    ReturnCode Read(Span<uint8_t> data, uint32_t& read) override
    {
        auto name = String::FromNullTerminated(bt_get_name());
	    auto nameSpan = name.AsConstBytes();
	    auto rc = nameSpan.CopyTo(data);
        CHECK_RETURN_CODE(rc);

        read = nameSpan.GetLength();

        return ReturnCode::Success;
    }

private:
    ReturnCode SetName(StringLiteral name)
    {
        auto err = bt_set_name(name.GetData());
        return ErrorConverter::Convert(err);
    }
};