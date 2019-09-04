#pragma once

#include <vector>
#include <bitstream.h>

template<typename T> inline
bitstream& operator>>(bitstream &bs, std::vector<T> &data)
{
    uint32_t size = 0;
    bs >> size;
    data.resize(size);

    for(uint32_t i = 0; i < size; ++i)
    {
        bs >> data[i];
    }
    return bs;
}

template<typename T> inline
bitstream& operator<<(bitstream &bs, const std::vector<T>& data)
{
    uint32_t size = data.size();
    bs << size;

    for(uint32_t i = 0; i < data.size(); ++i)
    {
        bs << data[i];
    }

    return bs;
}


