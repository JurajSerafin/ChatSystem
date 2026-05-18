file(GLOB_RECURSE SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")
file(GLOB_RECURSE SERVER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp")

file(GLOB_RECURSE PRIVATE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
                                     "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp")

# Isolate main from the sources
set(SERVER_MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Source/server_main.cpp")
list(REMOVE_ITEM SERVER_SOURCES ${SERVER_MAIN_FILE})

# Target 1: The Server Library
add_library(ChatSystemServerLib STATIC ${SERVER_SOURCES} ${SERVER_HEADERS} ${PRIVATE_HEADERS})

set_target_properties(ChatSystemServerLib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# PUBLIC for including in Tests/
target_include_directories(ChatSystemServerLib
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
)

# PUBLIC so consumers (e.g. Tests) get them implicitly
target_link_libraries(ChatSystemServerLib PUBLIC
    ChatSystem::Core
    ChatSystem::Dependencies
)

# Target 2: The Server Executable
add_executable(ChatSystemServer ${SERVER_MAIN_FILE})

set_target_properties(ChatSystemServer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "ChatSystemServer"
)

# The executable needs only the server's 'library
target_link_libraries(ChatSystemServer PRIVATE
    ChatSystemServerLib
)

target_compile_definitions(ChatSystemServer PRIVATE
    ENV_FILE_PATH=\"${CMAKE_SOURCE_DIR}/.env\"
)

# Source groups for IDEs
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include" PREFIX "Header Files" FILES ${SERVER_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source Files" FILES ${SERVER_SOURCES})

message(STATUS "Configured Server Library and Executable")