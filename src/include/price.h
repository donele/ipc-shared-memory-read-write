#pragma once

#include "num.h"

#include <cmath>
#include <limits>

class Price {
public:
    explicit Price(int64_t v, double increment)
        :value_(v),
        increment_(increment)
    {}
    explicit Price(double increment)
        :Price(std::numeric_limits<int64_t>::max(), increment)
    {}
    Price()
        :Price(std::numeric_limits<int64_t>::max(), 1)
    {}

    // don't return a reference to a packed field https://gcc.gnu.org/bugzilla/show_bug.cgi?id=36566
    int64_t IntValue() const { return value_; }
    double Increment() const { return increment_; }
    double ToDouble() const { return increment_ * static_cast<double>(value_); }
    void SetValue(int64_t v) { value_ = v; }
    void SetValue(int64_t v, double increment) {
        value_ = v;
        increment_ = increment;
    }
    void SetIncrement(double increment) { increment_ = increment; }
    bool IsValid() const { return value_ != std::numeric_limits<int64_t>::max(); }

    Price operator-() const {
        Price p;
        p.value_ = -this->value_;
        return p;
    }

    Price operator++() {
        this->value_ += 1;
        return *this;
    }

    Price operator++(int) {
        Price ans(*this);
        ++(*this);
        return ans;
    }

    inline bool operator==(const Price& rhs) const {
        return this->value_ == rhs.value_ && fabs(this->increment_ - rhs.increment_) <= EPSILON;
    }
    inline bool operator!=(const Price& rhs) const { return !(*this == rhs); }
    friend inline std::ostream& operator<<(std::ostream& os, const Price& p) {
        return os << p.ToDouble();
    }

private:
    int64_t value_;
    double increment_;
}__attribute__((packed));

using Quantity = Price;
using PriceIncrement = double;
using QtyIncrement = double;
using Rate = Price;
using Notional = Price;
using DisplayFactor = Price;
using SeqNum = uint64_t;

const Price NAN_PRICE = Price(std::numeric_limits<int64_t>::max(), 1);
const Quantity ZERO_QTY = Quantity(0, 1);

struct PriceComparator {
    bool operator()(const Price& p1, const Price& p2) const {
        return p1.IntValue() < p2.IntValue();
    }
};
using QtyComparator = PriceComparator;
