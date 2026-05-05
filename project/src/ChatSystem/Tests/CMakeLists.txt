if(NOT BUILD_TESTS)
    return()
endif()

include(GoogleTest)

function(add_unit_test TEST_NAME)
    cmake_parse_arguments(TEST "" "" "SOURCES;EXTRA_LIBS" ${ARGN})
    
    add_executable(${TEST_NAME} 
        ${TEST_SOURCES} 
        "${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp"
    )
    
    set_target_properties(${TEST_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
    )
    
    target_link_libraries(${TEST_NAME} PRIVATE
        ChatSystem::Core
        ChatSystem::Dependencies
        ${TEST_EXTRA_LIBS}
        GTest::gtest
        GTest::gmock
    )
    
    target_compile_definitions(${TEST_NAME} PRIVATE
        GTEST_LINKED_AS_SHARED_LIBRARY=1
    )
    
    gtest_discover_tests(${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    if(WIN32)
        add_custom_command(TARGET ${TEST_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${TEST_NAME}>
            $<TARGET_FILE_DIR:${TEST_NAME}>
            COMMAND_EXPAND_LISTS
        )
    endif()
    
endfunction()

# Add test categories
file(GLOB_RECURSE CORE_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp")
if(CORE_TESTS)
    add_unit_test(CoreTests SOURCES ${CORE_TESTS})
endif()

file(GLOB_RECURSE SERVER_TESTS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Server/*.cpp")
if(SERVER_TESTS)
    add_unit_test(ServerTests SOURCES ${SERVER_TESTS} EXTRA_LIBS ChatSystemServerLib)
endif()

message(STATUS "Configured unit tests")