file(GLOB_RECURSE APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")
file(GLOB_RECURSE APP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h"
                               "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp")

# Platform-specific GUI type
if(WIN32)
    set(GUI_TYPE WIN32)
else()
    set(GUI_TYPE "")
endif()

add_executable(ChatSystemApp ${GUI_TYPE} ${APP_SOURCES} ${APP_HEADERS})

set_target_properties(ChatSystemApp PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "ChatSystem"
)

target_include_directories(ChatSystemApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(ChatSystemApp PRIVATE
    ChatSystem::Core
    ChatSystem::Dependencies
)

# Copy runtime DLLs for wxWidgets
if(WIN32)
    add_custom_command(TARGET ChatSystemApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:ChatSystemApp>
        $<TARGET_FILE_DIR:ChatSystemApp>
        COMMAND_EXPAND_LISTS
    )
endif()

# Source groups for IDEs
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include" 
    PREFIX "Header Files" FILES ${APP_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" 
    PREFIX "Source Files" FILES ${APP_SOURCES})

message(STATUS "Configured App executable")