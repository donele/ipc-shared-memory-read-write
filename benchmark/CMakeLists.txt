include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
    )
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.7.1
    )
FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(googlebenchmark)

add_executable(benchmark_test_produce benchmark_test_produce.cpp)
target_link_libraries(benchmark_test_produce benchmark::benchmark ipc)

