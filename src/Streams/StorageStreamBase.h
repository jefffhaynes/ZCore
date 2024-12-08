#pragma once

#include "FileSystem.h"
#include "CoreString.h"
#include "FileMode.h"
#include "FileAccess.h"
#include "Flags.h"

class StorageStreamBase
{
public:
    virtual ~StorageStreamBase()
    {
        Close();
    }

    ReturnCode Close()
    {
        if(!IsOpen())
        {
            return ReturnCode::Success;
        }
        
        auto err = fs_close(&_file);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        Initialize();

        return ReturnCode::Success;
    }

    ReturnCode Delete()
    {
        auto rc = Close();
        CHECK_RETURN_CODE(rc);

        return FileSystem::Delete(_path);
    }

protected:
    StorageStreamBase(String path, FileMode mode) : _path(path), _mode(mode)
    {
        Initialize();
    }

    fs_file_t* GetFile()
    {
        return &_file;
    }

    ReturnCode Open(FileAccess access)
    {
        if(IsOpen())
        {
            return ReturnCode::Success;
        }

        if(_mode == FileMode::Create)
        {
            auto rc = Delete();
            CHECK_RETURN_CODE(rc);
        }

        fs_mode_t mode = FS_O_CREATE;

        if(Flags::HasFlag(access, FileAccess::Read))
        {
            mode |= FS_O_READ;
        }

        if(Flags::HasFlag(access, FileAccess::Write))
        {
            mode |= FS_O_WRITE;
        }
        
        auto err = fs_open(&_file, _path.GetData(), mode);
        return ErrorConverter::Convert(err);
    }

private:
    String _path;
    FileMode _mode;
    fs_file_t _file;
    
    void Initialize()
    {
	    fs_file_t_init(&_file);
    }

    bool IsOpen()
    {
        return _file.filep != nullptr;
    }
};