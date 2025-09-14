find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_custom_target(test_cppchroma
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_cppchroma.py
)

add_test(
    NAME cppchroma_tests
    COMMAND ${Python3_EXECUTABLE} -m unittest discover ${CMAKE_CURRENT_SOURCE_DIR}
)
