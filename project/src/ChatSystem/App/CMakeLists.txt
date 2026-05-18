file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")
file(GLOB_RECURSE APP_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp")
file(GLOB_RECURSE PRIVATE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp")


# Isolate main from the sources
set(APP_MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Source/app_main.cpp")
list(REMOVE_ITEM APP_SOURCES ${APP_MAIN_FILE})


# Target 1: The App Library
add_library(ChatSystemAppLib STATIC ${APP_SOURCES} ${APP_HEADERS} ${PRIVATE_HEADERS})

set_target_properties(ChatSystemAppLib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# PUBLIC for including in Tests/
target_include_directories(ChatSystemAppLib 
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
)

# PUBLIC so consumers (Tests) get them implicitly
target_link_libraries(ChatSystemAppLib PUBLIC
    ChatSystem::Core
    ChatSystem::Dependencies
)

# Platform-specific GUI type
if(WIN32)
    set(GUI_TYPE WIN32)
else()
    set(GUI_TYPE "")
endif()

# Target 2: The App Executable
add_executable(ChatSystemApp ${GUI_TYPE} ${APP_MAIN_FILE})

set_target_properties(ChatSystemApp PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "ChatSystem"
)

# The executable needs only the server's 'library
target_link_libraries(ChatSystemApp PRIVATE
    ChatSystemAppLib
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
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include" PREFIX "Header Files" FILES ${APP_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source Files" FILES ${APP_SOURCES})

message(STATUS "Configured App executable")