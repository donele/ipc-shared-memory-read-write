#pragma once

#include "spin_lock.h"
#include "message_types.h"

#include <atomic>
#include <cstdint>
#include <limits>
#include <thread>
#include <cstring>
#include <chrono>
#include <iostream>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>

class ShmContext {
    struct Header {
        alignas(64) std::atomic_int64_t sequence_num;
        alignas(64) std::atomic_int64_t claimed_sequence_num;
        alignas(64) std::atomic_int64_t current_offset_;
        alignas(64) AtomicFlagSpinlock reader_index_lock;
        int64_t LEDGER_SIZE;
        int64_t STORAGE_SIZE;
        int64_t CONSUMERS;
        std::atomic_bool initialized_ = false;
    };

    struct alignas(64) ConsumerSequence {
        std::atomic_int64_t sequence_num = 0;
        std::atomic_int64_t offset = 0;
        pid_t pid = 0;
        std::atomic_bool done = false;
        std::atomic_bool occupied = false;
    };

    struct Metadata {
        int64_t offset;
        int64_t size;
        uint16_t topic;
        uint16_t strategy_id;
    };

public:
    explicit ShmContext(std::string name)
        :name_(name),
        memory_size_(0),
        header_(nullptr),
        reader_sequence_numbers_(nullptr),
        ledger_(nullptr),
        storage_(nullptr)
    {}

    ~ShmContext() {
        if(queue_ptr_ == nullptr) return;
        shmdt(queue_ptr_);
    }

    int Create(int64_t queue_size, int64_t num_consumers, int64_t obj_size_hint) {
        auto actual_queue_size = GetNextPowerOfTwo(queue_size);
        int64_t actual_ledger_size = actual_queue_size / GetNextPowerOfTwo(obj_size_hint);
        int64_t total_size = sizeof(Header) + (sizeof(ConsumerSequence) * num_consumers)
            + (actual_ledger_size * sizeof(Metadata)) + actual_queue_size;
        key_t shm_key = ftok(name_.c_str(), 8);
        auto shm_id = shmget(shm_key, total_size, IPC_CREAT | 0666);
        if(shm_id < 0)  {
            perror("shmget");
            return -1;
        }

        queue_ptr_ = shmat(shm_id, NULL, 0);
        memset(queue_ptr_, 0, total_size);

        header_ = reinterpret_cast<Header*>(queue_ptr_);
        header_->LEDGER_SIZE = actual_ledger_size;
        header_->STORAGE_SIZE = actual_queue_size;
        header_->CONSUMERS = num_consumers;

        reader_sequence_numbers_ = reinterpret_cast<ConsumerSequence*>(header_ + 1);
        ledger_ = reinterpret_cast<Metadata*>(reader_sequence_numbers_ + num_consumers);
        storage_ = reinterpret_cast<char*>(ledger_ + actual_ledger_size);

        header_->initialized_.store(true);
        return 0;
    }

    int Attach() {
        key_t shm_key = ftok(name_.c_str(), 8);
        auto shm_id = shmget(shm_key, 0, 0);
        if(shm_id < 0) {
            perror("shmget");
            return -1;
        }
        queue_ptr_ = shmat(shm_id, NULL, 0);
        if(queue_ptr_ == MAP_FAILED) {
            perror("shmat");
            return -1;
        }
        header_ = reinterpret_cast<Header*>(queue_ptr_);
        while(!header_->initialized_)
            ;
        reader_sequence_numbers_ = reinterpret_cast<ConsumerSequence*>(header_ + 1);
        ledger_ = reinterpret_cast<Metadata*>(reader_sequence_numbers_ + header_->CONSUMERS);
        storage_ = reinterpret_cast<char*>(ledger_ + header_->LEDGER_SIZE);
        return 0;
    }

    int Destroy() {
        key_t shm_key = ftok(name_.c_str(), 8);
        auto shmid = shmget(shm_key, 0, 0);
        if(shmid < 0) {
            perror("shmget");
        }
        return shmctl(shmid, IPC_RMID, NULL);
    }

    int64_t GetCursor() const {
        return header_->sequence_num.load(std::memory_order_relaxed);
    }

    int Produce(const char* data, int64_t size, uint16_t topic = 0, uint16_t strategy_id = 0) {
        //bool verbose = true;
        // claimed_sequence_num
        //   Get the sequence_num for the intended write.
        auto claimed_expected = header_->claimed_sequence_num.load(std::memory_order_acquire); // A
        //if(verbose) {std::cout << "########## Read claimed_expected " << claimed_expected << std::endl;}
        auto claimed_desired = claimed_expected + 1;
        const auto LEDGER_SIZE = header_->LEDGER_SIZE;
        const auto STORAGE_SIZE = header_->STORAGE_SIZE;
        while(!header_->claimed_sequence_num.compare_exchange_weak(
                    claimed_expected, claimed_desired, std::memory_order_acq_rel)) { // /A
            claimed_desired = claimed_expected + 1;
        }
        //if(verbose) {std::cout << "Updated Header.claimed_sequence_num" << claimed_desired << '\n';
            //std::cout << ", waiting for Header.sequence_num to become " << claimed_desired - 1 << std::endl;}

        // sequence_num
        //   Wait for the other thread to be done writing.
        int64_t expected = claimed_desired - 1;
        int64_t count = 0;
        while(header_->sequence_num.load(std::memory_order_acquire) != expected) { // B
            ++count;
            if(count >= NUM_ITER) [[unlikely]] {
                std::this_thread::yield();
                count = 0;
            }
        }
        //if(verbose) {std::cout << "Confirmed Header.sequence_num = " << expected << std::endl;}

        // current_offset_
        auto curr_offset = header_->current_offset_.load(std::memory_order_acquire); // C
        auto remaining_size = STORAGE_SIZE - (curr_offset & STORAGE_SIZE - 1) - 1;
        int64_t offset_start = 0;
        int64_t offset_end = 0;
        if(size > remaining_size) [[unlikely]] {
            offset_start = curr_offset + remaining_size + 1;
            offset_end = offset_start + size - 1;
        } else {
            offset_start = curr_offset + 1;
            offset_end = offset_start + size - 1;
        }

        // First index is 1.
        // If the number exceeds LEDGER_SIZE, recycle ledger_.
        ledger_[claimed_desired & (LEDGER_SIZE - 1)].offset = offset_start;
        ledger_[claimed_desired & (LEDGER_SIZE - 1)].size = size;
        ledger_[claimed_desired & (LEDGER_SIZE - 1)].topic = topic;
        ledger_[claimed_desired & (LEDGER_SIZE - 1)].strategy_id = strategy_id;

        //if(verbose) {std::cout << "Start memcpy" << std::endl;}
        //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        std::memcpy(&storage_[offset_start & STORAGE_SIZE - 1], data, size);
        //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
        //if(verbose) {std::cout << "End memcpy" << std::endl;}

        header_->current_offset_.store(offset_end, std::memory_order_relaxed); // /B
        //if(verbose) {std::cout << "Updated Header.current_offset " << offset_end << std::endl;}
        header_->sequence_num.store(claimed_desired, std::memory_order_release); // /C
        //if(verbose) {std::cout << "Updated Header.sequence_num " << claimed_desired << std::endl;}
        return 0;
    }

    BufferWrapper Consume(int64_t consumer_sequence) {
        while(header_->sequence_num.load(std::memory_order_acquire) < consumer_sequence) {}
        int64_t idx = consumer_sequence & (header_->LEDGER_SIZE - 1); // ledger index
        auto offset = ledger_[idx].offset;
        auto size = ledger_[idx].size;
        auto topic = ledger_[idx].topic;
        auto strategy_id = ledger_[idx].strategy_id;
        auto storage_idx = offset & (header_->STORAGE_SIZE - 1);
        return BufferWrapper(storage_ + storage_idx, size, topic, strategy_id);
    }

private:
    static int64_t GetNextPowerOfTwo(int64_t size) {
        int64_t i{0};
        if(size > std::numeric_limits<int64_t>::max() / 2) {
            return -1;
        }
        for(; (1L << i) < size; ++i)
            ;
        return 1L << i;
    }

    std::string name_;
    uint64_t memory_size_;
    void* queue_ptr_ = nullptr;
    Header* header_;
    ConsumerSequence* reader_sequence_numbers_;
    Metadata* ledger_;
    char* storage_;

    static constexpr int64_t NUM_ITER = 1024;
};
