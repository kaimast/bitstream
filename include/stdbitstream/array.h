#pragma once

#include <bitstream.h>
#include <array>

template<typename T, size_t size> inline
bitstream& operator>>(bitstream &bs, std::array<T, size> &data)
{
    for(uint32_t i = 0; i < size; ++i)
    {
        bs >> data[i];
    }
    return bs;
}

template<typename T, size_t size> inline
bitstream& operator<<(bitstream &bs, const std::array<T, size>& data)
{
    for(uint32_t i = 0; i < size; ++i)
    {
        bs << data[i];
    }

    return bs;
}
