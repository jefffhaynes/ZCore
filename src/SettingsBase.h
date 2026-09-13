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
                // A stored value of a different size (the setting's type changed
                // between firmware versions, or the entry is damaged) would only
                // partially overwrite the default. Leave the default in place.
                if(len != setting->GetValueLength())
                {
                    return 0;
                }

                auto read = read_cb(cb_arg, setting->GetValuePointer(), setting->GetValueLength());

                if(read >= 0)
                {
                    setting->Sanitize();
                }

                return read;
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

        rc = _autoSaveTimer.Expired.Subscribe(AutoSave);
        CHECK_RETURN_CODE(rc);

        rc = _autoSaveTimer.Start(TimeSpan::FromSeconds(3), TimerMode::Repeating);
        CHECK_RETURN_CODE(rc);

        auto err = settings_load();
        return ErrorConverter::Convert(err);
    }

private:
    static bool _initialized;
    static Span<SettingBase*> _settings;
    static Timer _autoSaveTimer;
    
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

    static ReturnCode AutoSave(void* context)
    {
        for (auto* setting : _settings)
        {
            auto rc = setting->Flush();
            CHECK_RETURN_CODE(rc);
        }

        return ReturnCode::Success;
    }
};

inline bool SettingsBase::_initialized = false;
inline Span<SettingBase*> SettingsBase::_settings;
inline Timer SettingsBase::_autoSaveTimer = Timer(TimerOptions::Scheduled);