#pragma once

#include <cstdint>

namespace libcron
{
    struct DateTime
    {
        int year = 0;
        unsigned month = 0;
        unsigned day = 0;
        uint8_t hour = 0;
        uint8_t min = 0;
        uint8_t sec = 0;
    

    DateTime(int y, 
             unsigned m, 
             unsigned d, 
             uint8_t h = 0,
             uint8_t min = 0,
             uint8_t s = 0) 
            : 
             year(y), 
             month(m), 
             day(d), 
             hour(h), 
             min(min), 
             sec(s)
             {};
    };
}
