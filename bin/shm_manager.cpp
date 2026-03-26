#include "ipc.h"
#include "ipc/ShmContext.h"

#include <sys/shm.h>
#include <csignal>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

bool running = true;

void SignalHandler(int signum) {
    cout << "Interrupt " << signum << endl;
    running = false;
}

int main(int argc, char* argv[]) {
    string name = shm_name;
    cout << argc << endl;
    if(argc == 2) {
        name = name + argv[1];
    }
    signal(SIGINT, SignalHandler);
    vector<ShmContext> shm_segments;
    vector<int> shm_ids;

    ofstream memfile(name);
    if(memfile.fail()) {
        cout << "Invalid shm path\n";
        for(auto& context : shm_segments)
            context.Destroy();
        for(auto id : shm_ids)
            shmctl(id, IPC_RMID, NULL);
        return 1;
    }
    shm_segments.emplace_back(name);

    int size_mb = 1 * 1024;
    int64_t queue_size = size_mb * 1024UL * 1024UL;
    int num_consumers = 32;
    int obj_size_hint = 1024;
    cout << "Creating " << name << " " << queue_size << endl;
    int success = shm_segments[shm_segments.size() - 1].Create(queue_size, num_consumers, obj_size_hint);
    if(success == 0) {
        cout << "Successfully created\n";
        cout << "  \'ipcs -m\' shows the information\n";
        cout << "  \'ipcrm -m <id>\' removes\n";
        cout << "  \'lsof | egrep \"<id>|COMMAND\"\' lists attached processes\n";
    } else {
        running = false;
    }

    while(running) sleep(3);
    for(auto& context : shm_segments)
        context.Destroy();
    for(auto id : shm_ids)
        shmctl(id, IPC_RMID, NULL);

    return 0;
}
