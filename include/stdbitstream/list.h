#pragma once

#include <list>
#include <bitstream.h>

template<typename T>
inline bitstream& operator<<(bitstream &bs, const std::list<T> &l)
{
    bs << l.size();
    for(auto &e: l)
    {
        bs << e;
    }
    return bs;
}

template<typename T>
inline bitstream& operator>>(bitstream &bs, std::list<T> &l)
{
    size_t s;
    bs >> s;

    for(size_t i = 0; i < s; ++i)
    {
        T e;
        bs >> e;
        l.emplace_back(std::move(e));
    }

    return bs;
}


