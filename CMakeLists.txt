cmake_minimum_required(VERSION 3.21)

project(ChatSystem 
    VERSION 1.0.0
    DESCRIPTION "A chatting application with user authentication."
    LANGUAGES CXX
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    include(CTest)
endif()

option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_SERVER "Build server component" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(Dependencies.cmake)

add_subdirectory(Core)
add_subdirectory(App)

if(BUILD_SERVER)
    add_subdirectory(Server)
endif()

if(BUILD_TESTS AND BUILD_TESTING)
    add_subdirectory(Tests)
endif()

include(GNUInstallDirs)

install(TARGETS ChatSystemApp
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUILD_SERVER)
    install(TARGETS ChatSystemServer
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

message(STATUS "")
message(STATUS "========== Build Configuration ==========")
message(STATUS "Project:         ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:    ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler:        ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "System:          ${CMAKE_SYSTEM_NAME}")
message(STATUS "Build tests:     ${BUILD_TESTS}")
message(STATUS "Build server:    ${BUILD_SERVER}")
message(STATUS "=========================================")
message(STATUS "")