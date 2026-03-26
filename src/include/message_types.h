#pragma once

#include <cstdint>
#include <iostream>

namespace msg_type {
    //const uint16_t PACKET_HEADER = 0;
    ////const uint16_t PACKET_BEGIN = 1;
    //const uint16_t PACKET_END = 2;
    //const uint16_t INCREMENTAL_L3 = 3;
    const uint16_t INCREMENTAL_L2 = 4;
    const uint16_t BOOK_TICKER = 5;
    const uint16_t TRADE = 6;
    //const uint16_t TRIGGER = 7;
    //const uint16_t EXEC_ORDER_REQUEST = 8;
    //const uint16_t ORDER_REQUEST = 9;
    //const uint16_t ORDER_UPDATE = 10;
    //const uint16_t OPEN_ORDER_UPDATE = 11;
    //const uint16_t ORDER_TRADE = 12;
    //const uint16_t RESET = 13;
    //const uint16_t CLOCK = 14;
    //const uint16_t END_TIME = 15;
    //const uint16_t SYMBOL_TRADING_STATE= 16;
}

struct BufferWrapper {
    char* data;
    int64_t size;
    uint16_t topic = 0;
    uint16_t strategy_id = 0;
};
