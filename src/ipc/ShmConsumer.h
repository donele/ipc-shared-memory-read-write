#pragma once

#include "macros.h"
#include "spin_lock.h"
#include "message_types.h"
#include "ipc/ShmContext.h"

#include <atomic>
#include <cstdint>
#include <limits>
#include <thread>
#include <memory>
#include <iostream>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>

class ShmConsumer {
public:
    explicit ShmConsumer(ShmContext& context)
        :context_(context)
    {
        consumer_idx_ = context.GetCursor() + 1;
    }

    ~ShmConsumer() = default;
    NO_COPY(ShmConsumer);

    inline BufferWrapper Consume() {
        auto out = context_.Consume(consumer_idx_);
        consumer_idx_++;
        return out;
    }

private:
    ShmContext& context_;
    int64_t consumer_idx_ = -1;
};
