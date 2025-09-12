include(../cmake/gtest.cmake)

macro(register_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_compile_definitions(${TESTNAME} PRIVATE UNIT_TESTING=1)
    target_link_libraries(${TESTNAME} PRIVATE gtest)
    gtest_discover_tests(${TESTNAME}
        WORKING_DIRECTORY ${PROJECT_DIR}
        DISCOVERY_MODE PRE_TEST
    )
endmacro()

register_test(test_main test_main.cpp)
