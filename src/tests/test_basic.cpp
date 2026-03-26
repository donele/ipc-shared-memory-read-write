#include "incremental.h"
#include "message_types.h"
#include "price.h"
#include "side.h"

#include <iostream>
#include <sstream>
#include <string>

namespace {
int g_failed = 0;

void Expect(bool cond, const std::string& msg) {
    if (!cond) {
        ++g_failed;
        std::cerr << "FAIL: " << msg << '\n';
    }
}
} // namespace

int main() {
    {
        Price p(123, 0.01);
        Expect(p.IntValue() == 123, "Price.IntValue");
        Expect(p.Increment() == 0.01, "Price.Increment");
        Expect(p.ToDouble() == 1.23, "Price.ToDouble");
    }

    {
        Quantity q = ZERO_QTY;
        Expect(q.IntValue() == 0, "ZERO_QTY value");
        Expect(q.Increment() == 1.0, "ZERO_QTY increment");
    }

    {
        Expect(ToString(Side::BID) == "BID", "Side BID string");
        Expect(ToString(Side::ASK) == "ASK", "Side ASK string");
        Expect(ToString(Side::UNKNOWN) == "UNKNOWN", "Side UNKNOWN string");

        std::ostringstream os;
        os << Side::ASK;
        Expect(os.str() == "ASK", "Side ostream");
    }

    {
        Incremental incr{};
        Expect(incr.type == msg_type::INCREMENTAL_L2, "Incremental default type");
        Expect(incr.qty == ZERO_QTY, "Incremental default qty");
    }

    if (g_failed != 0) {
        std::cerr << g_failed << " test(s) failed\n";
        return 1;
    }
    std::cout << "All unit tests passed\n";
    return 0;
}
