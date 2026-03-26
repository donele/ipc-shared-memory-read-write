#include "ipc.h"
#include "message_types.h"
#include "incremental.h"
#include "ipc/ShmContext.h"
#include "ipc/ShmProducer.h"

#include <iostream>
#include <string>
#include <chrono>

using namespace std;

int main() {
    ShmContext context(shm_name);
    context.Attach();

    ShmProducer prod(context);
    Incremental incr;

    int niter = 1000000;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for(int i = 0; i < niter; ++i) {
        incr.type = msg_type::INCREMENTAL_L2;
        incr.price.SetValue(1000 + i);
        incr.qty.SetValue(100 + i);
        incr.side = Side::BID;
        prod.Produce(incr);
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    int64_t nanos = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
    std::cout << "Test producer: " << niter << " writes in " << nanos << " [ns], " << nanos/niter << " ns per write" << std::endl;

    cout << "Done test publisher\n";
    return 0;
}
