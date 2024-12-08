#pragma once

#include "StorageStreamBase.h"
#include "OutputStream.h"
#include "Flags.h"

class StorageOutputStream : public StorageStreamBase, public OutputStream
{
public:
    StorageOutputStream(String path, FileMode mode) : StorageStreamBase(path, mode)
    {
    }

    ReturnCode Write(Span<const uint8_t> data) override
    {
        auto rc = Open(FileAccess::Write);
        CHECK_RETURN_CODE(rc);

    	auto err = fs_write(GetFile(), data.GetData(), data.GetLength());
        return ErrorConverter::Convert(err);
    }
};