#pragma once

#include <zephyr/dfu/mcuboot.h>
#include "ErrorConverter.h"
#include "Debug.h"

class Bootloader
{
public:
    static bool IsPresent()
    {
        return IS_ENABLED(CONFIG_BOOTLOADER_MCUBOOT);
    }

    static bool IsImageConfirmed()
    {
        if(_isConfirmed)
        {
            return true;
        }

        _isConfirmed = boot_is_img_confirmed();
        
        return _isConfirmed;
    }

    static ReturnCode ConfirmImage()
    {
        auto err = boot_write_img_confirmed();
        return ErrorConverter::Convert(err);
    }

    static ReturnCode EnsureImageConfirmed()
    {
        if(IsImageConfirmed())
        {
            Debug::WriteLine("Image already confirmed");
            return ReturnCode::Success;
        }

        auto rc = ConfirmImage();
        CHECK_RETURN_CODE(rc);

        Debug::WriteLine("Image confirmed");

        return ReturnCode::Success;
    }

private:
    static bool _isConfirmed;
};

inline bool Bootloader::_isConfirmed = false;