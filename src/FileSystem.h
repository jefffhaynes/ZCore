#pragma once

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>

#include "ErrorConverter.h"
#include "CoreString.h"

FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(logging);

class FileSystem
{
public: 
    static ReturnCode Mount()
    {
        auto err = fs_mount(&_mount);
        return ErrorConverter::Convert(err);
    }

    static ReturnCode Unmount()
    {
        auto err = fs_unmount(&_mount);
        return ErrorConverter::Convert(err);
    }
    
    static ReturnCode Delete(String path)
    {
        bool exists;
        auto rc = Exists(path, exists);
        CHECK_RETURN_CODE(rc);

        if(!exists)
        {
            return ReturnCode::Success;
        }

        auto err = fs_unlink(path.GetData());
        return ErrorConverter::Convert(err);
    }

    static ReturnCode Exists(String path, bool& exists)
    {
        uint32_t size;
        bool isDirectory;
        auto rc = GetStatus(path, size, isDirectory);

        if(rc == ReturnCode::NotFound)
        {
            exists = false;
        }
        else
        {
            CHECK_RETURN_CODE(rc);
        }

        exists = true;

        return ReturnCode::Success;
    }

private:
    static struct fs_mount_t _mount;

    static ReturnCode GetStatus(String path, uint32_t& size, bool& isDirectory)
    {
        fs_dirent entry;
        auto err = fs_stat(path.GetData(), &entry);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        size = entry.size;
        isDirectory = entry.type == FS_DIR_ENTRY_DIR;

        return ReturnCode::Success;
    }
};

inline fs_mount_t FileSystem::_mount = {
            .type = FS_LITTLEFS,
            .mnt_point = "/logging",
            .fs_data = &logging,
            .storage_dev = (void *)FIXED_PARTITION_ID(storage_partition)
        };