#pragma once

#include "Span.h"
#include "SettingBase.h"


class SettingsBase
{
public:
    static ReturnCode Load()
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        auto err = settings_load();
        return ErrorConverter::Convert(err);
    }

protected:
    constexpr SettingsBase()
    {
    }

    static ReturnCode Reset(Span<SettingBase*> settings)
    {
        for (auto* setting : settings)
        {
            auto rc = setting->Reset();
            CHECK_RETURN_CODE(rc);
        }

        return ReturnCode::Success;
    }

    static int OnSet(Span<SettingBase*> settings, 
        const char* name, size_t len, 
        settings_read_cb read_cb, void *cb_arg)
    {
        for (auto* setting : settings)
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

private:
    static bool _initialized;
    
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