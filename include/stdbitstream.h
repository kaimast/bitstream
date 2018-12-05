#pragma once

#include <set>
#include <unordered_set>
#include <list>
#include <vector>

#include "bitstream.h"

/// std::unordered_set
template<typename T> inline
bitstream& operator>>(bitstream &bs, std::unordered_set<T> &data)
{
    uint32_t size = 0;
    bs >> size;

    for(uint32_t i = 0; i < size; ++i)
    {
        T t;
        bs >> t;
        data.insert(t);
    }

    return bs;
}

template<typename T> inline
bitstream& operator<<(bitstream &bs, const std::unordered_set<T> &s)
{
    uint32_t size = s.size();
    bs << size;

    for(auto &e: s)
    {
        bs << e;
    }

    return bs;
}


/// std::set
template<typename T> inline
bitstream& operator>>(bitstream &bs, std::set<T> &data)
{
    uint32_t size = 0;
    bs >> size;

    for(uint32_t i = 0; i < size; ++i)
    {
        T t;
        bs >> t;
        data.insert(t);
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

/// std::string
inline bitstream& operator<<(bitstream &bs, const std::string& str)
{
    uint32_t length = str.size();
    bs << length;

    if(length > 0)
    {
        bs.write_raw_data(reinterpret_cast<const uint8_t*>(str.c_str()), length);
    }

    return bs;
}

inline bitstream& operator>>(bitstream &bs, std::string& data)
{
    data = "";

    uint32_t length;
    bs >> length;

    for(uint32_t i = 0; i < length; ++i)
    {
        char c;
        bs >> c;

        data += c;
    }

    return bs;
}

/// std::list
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

/// std::vector
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
