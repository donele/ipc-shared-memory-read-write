set(LIBNAME ipc)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)
add_library(${LIBNAME} OBJECT ${SOURCES})
target_link_libraries(${LIBNAME} PUBLIC nlohmann_json)
