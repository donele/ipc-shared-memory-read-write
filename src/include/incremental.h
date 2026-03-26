#pragma once

#include "message_types.h"
#include "price.h"
#include "side.h"
#include "time_types.h"

#include <cstdint>
#include <ostream>

struct Incremental {
    uint16_t type = msg_type::INCREMENTAL_L2;
    uint64_t exchange_time;
    Price price;
    Quantity qty = ZERO_QTY;
    uint64_t num_orders_at_level;
    Side side;
} __attribute__((packed));

inline std::ostream& operator<<(std::ostream& os, const Incremental& incr) {
    os << "PacketType=IncrementalL2"
       << " ExchangeTime=" << TimeToStr(incr.exchange_time)
       << " Price=" << incr.price.IntValue()
       << " Qty=" << incr.qty.IntValue()
       << " NumOrdersAtLevel=" << incr.num_orders_at_level
       << " Side=" << incr.side;
    return os;
}
