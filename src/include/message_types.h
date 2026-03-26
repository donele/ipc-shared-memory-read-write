#pragma once

#include <cstdint>
#include <iostream>

namespace msg_type {
    const uint16_t INCREMENTAL_L2 = 4;
    const uint16_t BOOK_TICKER = 5;
    const uint16_t TRADE = 6;
}

struct BufferWrapper {
    char* data;
    int64_t size;
    uint16_t topic = 0;
    uint16_t strategy_id = 0;
};
