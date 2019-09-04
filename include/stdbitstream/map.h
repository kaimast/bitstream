#pragma once

#include <map>
#include <bitstream.h>

template<typename K, typename V> inline
bitstream& operator>>(bitstream &bs, std::map<K, V> &data)
{
    uint32_t size = 0;
    bs >> size;

    for(uint32_t i = 0; i < size; ++i)
    {
        K k;
        V v;

        bs >> k >> v;

        data.emplace(k, std::move(v));
    }

    return bs;
}

template<typename K, typename V> inline
bitstream& operator<<(bitstream &bs, const std::map<K, V> &data)
{
    uint32_t size = data.size();
    bs << size;

    for(auto &[k,v] : data)
    {
        bs << k << v;
    }

    return bs;
}


