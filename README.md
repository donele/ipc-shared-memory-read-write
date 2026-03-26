# IPC Shared Memory Read Write

## How to build

```
cmake -S . -B ./build
cmake --build ./build
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

## Benchmark

```
./build/bin/benchmark_test_produce
```


