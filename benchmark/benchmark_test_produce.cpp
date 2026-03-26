#include "benchmark/benchmark.h"
#include "ipc.h"
#include "message_types.h"
#include "incremental.h"
#include "ipc/ShmContext.h"
#include "ipc/ShmProducer.h"

#include <iostream>
#include <string>

static void bm_test_produce_1mm(benchmark::State& state) {
    ShmContext context(std::string(shm_name));
    context.Attach();

    ShmProducer prod(context);
    Incremental incr;

    for(auto _ : state) {
        for(int i = 0; i < 1000000; ++i) {
            incr.type = msg_type::INCREMENTAL_L2;
            incr.price.SetValue(1000 + i);
            incr.qty.SetValue(100 + i);
            incr.side = Side::BID;
            prod.Produce(incr);
        }
    }
}
BENCHMARK(bm_test_produce_1mm)->Iterations(1);

static void bm_test_produce_1k(benchmark::State& state) {
    ShmContext context(std::string(shm_name));
    context.Attach();

    ShmProducer prod(context);
    Incremental incr;

    for(auto _ : state) {
        for(int i = 0; i < 1000; ++i) {
            incr.type = msg_type::INCREMENTAL_L2;
            incr.price.SetValue(1000 + i);
            incr.qty.SetValue(100 + i);
            incr.side = Side::BID;
            prod.Produce(incr);
        }
    }
}
BENCHMARK(bm_test_produce_1k)->Iterations(1);

static void bm_test_produce_single(benchmark::State& state) {
    ShmContext context(std::string(shm_name));
    context.Attach();

    ShmProducer prod(context);
    Incremental incr;

    for(auto _ : state) {
        for(int i = 0; i < 1; ++i) {
            incr.type = msg_type::INCREMENTAL_L2;
            incr.price.SetValue(1000 + i);
            incr.qty.SetValue(100 + i);
            incr.side = Side::BID;
            prod.Produce(incr);
        }
    }
}
BENCHMARK(bm_test_produce_single);

BENCHMARK_MAIN();
