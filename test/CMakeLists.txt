enable_testing()
include(GoogleTest)
file(GLOB TEST_FILES_CPP "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB TEST_FILES_CX  "${CMAKE_CURRENT_LIST_DIR}/*.cx")
file(GLOB TEST_FILES     "${CMAKE_CURRENT_LIST_DIR}/*.c")
list(APPEND TEST_FILES ${TEST_FILES_CPP} ${TEST_FILES_CX})
foreach (test_file IN LISTS TEST_FILES)
    get_filename_component(filenamewe ${test_file} NAME_WE)
    string(CONCAT execname "test_" ${filenamewe})
    add_executable(${execname} ${test_file})
    set_property(TARGET ${execname} PROPERTY CXX_STANDARD ${CXX_VERSION})
    target_link_libraries(${execname} PRIVATE gtest_main sffs)
    gtest_discover_tests(${execname})
endforeach()
