#pragma once

#include <set>
#include <bitstream.h>

template<typename T> inline
bitstream& operator>>(bitstream &bs, std::set<T> &data)
{
    uint32_t size = 0;
    bs >> size;

    for(uint32_t i = 0; i < size; ++i)
    {
        T t;
        bs >> t;
        data.emplace(std::move(t));
    }

    return bs;
}

template<typename T> inline
bitstream& operator<<(bitstream &bs, const std::set<T> &s)
{
    uint32_t size = s.size();
    bs << size;

    for(auto &e: s)
    {
        bs << e;
    }

    return bs;
}


