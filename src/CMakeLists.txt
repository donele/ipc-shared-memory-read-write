add_subdirectory(ipc)

add_library(core SHARED
    $<TARGET_OBJECTS:ipc>
)

target_compile_features(core PUBLIC cxx_std_20)
target_compile_options(core PRIVATE
    $<$<CONFIG:Debug>:-g;-fmodules-ts;-O0;-fPIC>
    $<$<CONFIG:Release>:-g;-fmodules-ts;-flto>
)
target_include_directories(core PUBLIC
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/include"
)
target_link_libraries(core PUBLIC
    nlohmann_json::nlohmann_json
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
