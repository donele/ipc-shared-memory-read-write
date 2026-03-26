#include "ipc.h"
#include "incremental.h"
#include "ipc/ShmContext.h"
#include "ipc/ShmConsumer.h"

#include <iostream>

using namespace std;

int main() {
    cout << shm_name << '\n';
    ShmContext context(shm_name);
    context.Attach();

    ShmConsumer consumer(context);

    int count = 0;
    while(true) {
        auto buf = consumer.Consume();
        if(buf.size == -1)
            continue;

        auto ph = reinterpret_cast<PacketHeader*>(buf.data);
        auto batch_size = ph->batch_size;
        auto offset = sizeof(PacketHeader);
        
        while(offset < batch_size) {
            auto msgtype = *reinterpret_cast<uint16_t*>(buf.data + offset);
            switch(msgtype) {
                case msg_type::INCREMENTAL_L2: {
                    auto msg = reinterpret_cast<Incremental*>(buf.data + offset);
                    cout << *msg << '\n';
                    offset += sizeof(Incremental);
                    break;
                }
                default:
                    cout << "Unknown message type\n";
            }
        }
    }
    cout << "Finished consumer\n";
}
