if(NOT BUILD_TESTS)
    return()
endif()

include(GoogleTest)

function(add_unit_test TEST_NAME)
    cmake_parse_arguments(TEST "" "" "SOURCES" ${ARGN})
    
    add_executable(${TEST_NAME} ${TEST_SOURCES})
    
    set_target_properties(${TEST_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    
    target_link_libraries(${TEST_NAME} PRIVATE
        ChatSystem::Core
        ChatSystem::Dependencies
        GTest::gtest
        GTest::gtest_main
    )
    
    gtest_discover_tests(${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# Add test categories
file(GLOB CORE_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp")
if(CORE_TESTS)
    add_unit_test(CoreTests SOURCES ${CORE_TESTS})
endif()

message(STATUS "Configured unit tests")