#pragma once

#include <string>
#include <bitstream.h>

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


