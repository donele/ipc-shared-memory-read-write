#include "ipc.h"
#include "incremental.h"
#include "ipc/ShmContext.h"
#include "ipc/ShmConsumer.h"

#include <iostream>
#include <string>

using namespace std;

int main() {
    cout << shm_name << '\n';
    ShmContext context{std::string(shm_name)};
    context.Attach();

    ShmConsumer consumer(context);

    int count = 0;
    std::chrono::steady_clock::time_point last_consume = std::chrono::steady_clock::now();
    while(true) {
        auto buf = consumer.Consume();

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        int microseconds_since_consume = std::chrono::duration_cast<std::chrono::microseconds> (end - last_consume).count();
        if(count > 0 && microseconds_since_consume  > 1e6) {
            cout << count << " consumed." << endl;
            count = 0;
        }

        auto msgtype = *reinterpret_cast<uint16_t*>(buf.data);
        switch(msgtype) {
            case msg_type::INCREMENTAL_L2: {
                auto msg = reinterpret_cast<Incremental*>(buf.data);
                cout << *msg << '\n';
                last_consume = std::chrono::steady_clock::now();
                ++count;
                break;
            }
            default:
                cout << "Unknown message type\n";
        }

    }
    cout << "Finished consumer\n";
}
