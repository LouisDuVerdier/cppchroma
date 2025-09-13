include(../cmake/gtest.cmake)

macro(register_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_compile_definitions(${TESTNAME} PRIVATE UNIT_TESTING=1)
    target_link_libraries(${TESTNAME} PRIVATE gtest re2::re2 yaml-cpp::yaml-cpp absl::log absl::log_initialize)
    gtest_discover_tests(${TESTNAME}
        WORKING_DIRECTORY ${PROJECT_DIR}
        DISCOVERY_MODE PRE_TEST
    )
endmacro()

set(COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/cppchroma.cpp
)

register_test(test_main test_main.cpp ${COMMON_SOURCES})
