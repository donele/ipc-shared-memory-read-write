add_library(core SHARED
    $<TARGET_OBJECTS:ipc>
)

find_package(Parquet REQUIRED)
target_link_libraries(core PUBLIC
    nlohmann_json::nlohmann_json
)

add_subdirectory(ipc)
