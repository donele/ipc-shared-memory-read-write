set(LIBNAME ipc)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)
add_library(${LIBNAME} OBJECT ${SOURCES})
target_compile_features(${LIBNAME} PUBLIC cxx_std_20)
target_compile_options(${LIBNAME} PRIVATE
    $<$<CONFIG:Debug>:-g;-fmodules-ts;-O0;-fPIC>
    $<$<CONFIG:Release>:-g;-fmodules-ts;-flto>
)
target_include_directories(${LIBNAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/include"
)
target_link_libraries(${LIBNAME} PUBLIC nlohmann_json::nlohmann_json)
