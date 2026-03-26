add_executable(unit_tests
    test_basic.cpp
)

target_link_libraries(unit_tests PRIVATE core)

add_test(NAME unit_tests COMMAND unit_tests)
