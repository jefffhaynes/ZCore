#pragma once

enum class FileAccess
{
    Read = 0b01,
    Write = 0b10,
    ReadWrite = Read | Write
};