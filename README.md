# IPC Shared Memory Read Write

## What this repository does

This project implements a lightweight inter-process communication (IPC) queue
using System V shared memory. It is designed for low-latency, high-throughput
message passing between producer and consumer processes on the same machine.

Main components:
- `shm_manager`: creates and manages the shared memory region.
- `shm_test_producer` / `shm_eternal_producer`: write sample market-style
  messages into shared memory.
- `shm_test_consumer`: reads and decodes messages from shared memory.
- `unit_tests` and `benchmark_test_produce`: validate behavior and measure
  producer performance.

## Concurrency model and memory ordering

The queue uses a ring-buffer style layout in shared memory with:
- `claimed_sequence_num`: next sequence number to reserve for writing.
- `sequence_num`: highest sequence that is fully published and visible.
- `current_offset_`: current write offset in the data region.
- `ledger[]`: metadata per sequence (`offset`, `size`, `topic`, `strategy_id`).
- `storage[]`: raw payload bytes.

### Writer flow (multiple publishers)

Writers coordinate through atomics in `ShmContext::Produce`:
1. A writer reserves a unique sequence using
   `claimed_sequence_num.compare_exchange_weak(..., memory_order_acq_rel)`.
2. It waits until `sequence_num == claimed - 1` (`memory_order_acquire` load),
   which serializes the commit path so only one writer publishes at a time.
3. It writes metadata into `ledger[seq & (LEDGER_SIZE - 1)]` and payload bytes
   into `storage`.
4. It stores `current_offset_`.
5. It publishes completion with `sequence_num.store(seq, memory_order_release)`.

Effectively, many producers can contend and reserve sequence numbers concurrently,
but publication is in strict sequence order (single-file commit order).

### Reader flow (multiple readers)

Each `ShmConsumer` keeps its own local `consumer_idx_` cursor:
1. Reader waits until `sequence_num >= consumer_idx_`
   (`memory_order_acquire` load in `Consume`).
2. It reads ledger metadata for that sequence.
3. It reads payload from `storage[offset & (STORAGE_SIZE - 1)]`.
4. It increments its own local cursor.

This means multiple readers can read concurrently and independently, each with
its own progress. There is no central per-reader flow control currently enforced
in the active path.

### Lock-free note

The implementation is mostly atomic-and-spin based (no mutex in the hot path),
but it is not strictly wait-free:
- writers may spin waiting for prior sequence publication,
- readers spin waiting for new data.

So it behaves as a low-latency, lock-avoiding design with busy-wait sections.

## How to build

```
cmake -S . -B build
cmake --build build
```

## How to run

```
sudo mkdir /ipc_resource/shm
sudo ./build/bin/shm_manager
```

```
./build/bin/shm_test_consumer
```

```
./build/bin/shm_test_producer
```

## Unit Test

```
./build/bin/unit_tests
```

## Benchmark

```
./build/bin/benchmark_test_produce
```
