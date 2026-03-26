find_package(benchmark QUIET)
if(NOT benchmark_FOUND)
    # Do not build benchmark's own tests; avoids requiring googletest sources.
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_USE_BUNDLED_GTEST OFF CACHE BOOL "" FORCE)
    include(FetchContent)
    FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.7.1
    )
    FetchContent_MakeAvailable(googlebenchmark)
endif()

add_executable(benchmark_test_produce benchmark_test_produce.cpp)
target_link_libraries(benchmark_test_produce benchmark::benchmark core)
