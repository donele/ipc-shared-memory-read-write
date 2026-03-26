#pragma once

#include <string>

enum class Side : uint8_t {
    BID = 0,
    ASK = 1,
    UNKNOWN = 2,
    COUNT
};

inline const std::string ToString(Side side) {
    switch (side) {
        case Side::BID: return "BID"; break;
        case Side::ASK: return "ASK"; break;
        default: return "UNKNWON";
    }
}

inline std::ostream& operator<<(std::ostream& os, const Side side) {
    os << ToString(side);
    return os;
}

enum class PositionSide : uint8_t {
    LONG = 0,
    SHORT,
    BOTH,
    UNKNOWN
};

inline const std::string ToString(PositionSide side) {
    switch (side) {
        case PositionSide::LONG: return "LONG";
        case PositionSide::SHORT: return "SHORT";
        case PositionSide::BOTH: return "BOTH";
        default: return "UNKNOWN";
    }
}
