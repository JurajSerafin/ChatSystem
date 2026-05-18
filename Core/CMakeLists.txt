file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h" 
                                            "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp")

add_library(ChatSystemCore STATIC ${CORE_SOURCES} ${CORE_HEADERS})
add_library(ChatSystem::Core ALIAS ChatSystemCore)

set_target_properties(ChatSystemCore PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(ChatSystemCore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(ChatSystemCore PUBLIC ChatSystem::Dependencies)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include" 
    PREFIX "Header Files" FILES ${CORE_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" 
    PREFIX "Source Files" FILES ${CORE_SOURCES})

message(STATUS "Configured Core library")