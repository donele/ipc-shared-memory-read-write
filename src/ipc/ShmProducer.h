#pragma once

#include "spin_lock.h"
#include "ipc/ShmContext.h"

#include <atomic>
#include <cstdint>
#include <limits>
#include <thread>
#include <memory>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>

class ShmProducer {
public:
    explicit ShmProducer(ShmContext& context)
        :context_(context)
    {}

    //DISALLOW_COPY(ShmProducer)

    template<typename T>
    int Produce(const T& obj) {
        return context_.Produce(reinterpret_cast<const char*>(&obj), sizeof(T));
    }

private:
    ShmContext& context_;
};
