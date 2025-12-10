file(GLOB_RECURSE SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")
file(GLOB_RECURSE SERVER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp")

add_executable(ChatSystemServer ${SERVER_SOURCES} ${SERVER_HEADERS})

set_target_properties(ChatSystemServer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "ChatSystemServer"
)

target_include_directories(ChatSystemServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(ChatSystemServer PRIVATE
    ChatSystem::Core
    ChatSystem::Dependencies
)

# Source groups for IDEs
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include" 
    PREFIX "Header Files" FILES ${SERVER_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" 
    PREFIX "Source Files" FILES ${SERVER_SOURCES})

message(STATUS "Configured Server executable")