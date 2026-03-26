cmake_minimum_required(VERSION 3.20)
project(shmcas LANGUAGES CXX)


set(BUILD_SHARED_LIBS ON)
option(BUILD_BENCHMARKS "Build benchmark targets" ON)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib64")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

include(CTest)

find_package(nlohmann_json REQUIRED)
set(LIBNAME ipc)
add_library(${LIBNAME} INTERFACE)
target_compile_features(${LIBNAME} INTERFACE cxx_std_20)
target_compile_options(${LIBNAME} INTERFACE
    $<$<CONFIG:Debug>:-g;-fmodules-ts;-O0;-fPIC>
    $<$<CONFIG:Release>:-g;-fmodules-ts;-flto>
)
target_include_directories(${LIBNAME} INTERFACE
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/include"
)
target_link_libraries(${LIBNAME} INTERFACE nlohmann_json::nlohmann_json)

add_library(core INTERFACE)
target_compile_features(core INTERFACE cxx_std_20)
target_compile_options(core INTERFACE
    $<$<CONFIG:Debug>:-g;-fmodules-ts;-O0;-fPIC>
    $<$<CONFIG:Release>:-g;-fmodules-ts;-flto>
)
target_include_directories(core INTERFACE
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/include"
)
target_link_libraries(core INTERFACE
    ipc
    nlohmann_json::nlohmann_json
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

add_subdirectory(bin)
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()
