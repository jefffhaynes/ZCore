#pragma once

#include "Span.h"
#include "SettingBase.h"
#include "Timer.h"

class SettingsBase
{
public:
    static ReturnCode ResetAll()
    {
        for (auto* setting : _settings)
        {
            auto rc = setting->Reset();
            CHECK_RETURN_CODE(rc);
        }

        return ReturnCode::Success;
    }
    
    static int OnSet(const char* name, size_t len, 
        settings_read_cb read_cb, void *cb_arg)
    {
        for (auto* setting : _settings)
        {
            auto nameValue = String::FromNullTerminated(name);
            auto keyValue = setting->GetKey();

            auto index = keyValue.LastIndexOf('/');

            if(index == -1)
            {
                continue;
            }

            auto nameSpan = keyValue.Substring(index + 1);

            if(nameSpan == nameValue)
            {
                return read_cb(cb_arg, setting->GetValuePointer(), setting->GetValueLength());
            }
        }

        return 0;
    }


protected:
    constexpr SettingsBase()
    {
    }

    static ReturnCode Load(Span<SettingBase*> settings)
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        _settings = settings;

        auto err = settings_load();
        return ErrorConverter::Convert(err);
    }

private:
    static bool _initialized;
    static Span<SettingBase*> _settings;
    
    static ReturnCode Initialize()
    {
        if(_initialized)
        {
            return ReturnCode::Success;
        }
        
        auto err = settings_subsys_init();
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        _initialized = true;

        return ReturnCode::Success;
    }
};

inline bool SettingsBase::_initialized = false;
inline Span<SettingBase*> SettingsBase::_settings;
