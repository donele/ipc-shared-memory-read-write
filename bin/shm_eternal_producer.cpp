#include "ipc.h"
#include "message_types.h"
#include "incremental.h"
#include "ipc/ShmContext.h"
#include "ipc/ShmProducer.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    string name(shm_name);
    if(argc == 2) {
        name = name + argv[1];
    }

    int sleep_sec = 0;
    if(argc == 3) {
        sleep_sec = stoi(argv[2]);
    }

    ShmContext context(name);
    context.Attach();

    ShmProducer prod(context);
    Incremental incr;

    constexpr int max_price = 9998;
    int price = 0;
    int size = price % 100;
    while(true) {
        price = (price + 1) % max_price + 1;
        size = price / 10 + 1;
        incr.type = msg_type::INCREMENTAL_L2;
        incr.price.SetValue(price);
        incr.qty.SetValue(size);
        incr.side = Side::BID;
        prod.Produce(incr);
        if(sleep_sec > 0)
            sleep(sleep_sec);
    }

    cout << "Done test publisher\n";
    return 0;
}
