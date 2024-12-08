#pragma once

#include "StorageStreamBase.h"
#include "InputStream.h"
#include "Flags.h"

class StorageInputStream : public StorageStreamBase, public InputStream
{
public:
    StorageInputStream(String path, FileMode mode) : StorageStreamBase(path, mode)
    {
    }

    ReturnCode Read(Span<uint8_t> data, uint32_t& read) override
    {
        auto rc = Open(FileAccess::Read);
        CHECK_RETURN_CODE(rc);

    	auto result = fs_read(GetFile(), data.GetData(), data.GetLength());
        rc = ErrorConverter::Convert(result);
        CHECK_RETURN_CODE(rc);

        read = result;

        return ReturnCode::Success;
    }
};