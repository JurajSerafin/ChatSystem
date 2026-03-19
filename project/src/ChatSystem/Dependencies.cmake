message(STATUS "Finding dependencies...")

# wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base net aui adv)
if(wxWidgets_FOUND)
    message(STATUS "Found wxWidgets ${wxWidgets_VERSION}")
endif()

message(STATUS "CMAKE_SIZEOF_VOID_P=${CMAKE_SIZEOF_VOID_P}")
find_package(Boost REQUIRED CONFIG COMPONENTS system thread asio)

# Boost
cmake_policy(SET CMP0167 NEW)
find_package(Boost REQUIRED CONFIG COMPONENTS system thread asio)
if(Boost_FOUND)
    message(STATUS "Found Boost ${Boost_VERSION}")
endif()

# OpenSSL
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    message(STATUS "Found OpenSSL ${OPENSSL_VERSION}")
endif()

# libpqxx
find_package(libpqxx REQUIRED)
if(libpqxx_FOUND)
    message(STATUS "Found libpqxx")
endif()

# SQLite3
find_package(SQLite3 REQUIRED)
if(SQLite3_FOUND)
    message(STATUS "Found SQLite3 ${SQLite3_VERSION}")
endif()

# nlohmann_json
find_package(nlohmann_json REQUIRED)
if(nlohmann_json_FOUND)
    message(STATUS "Found nlohmann_json ${nlohmann_json_VERSION}")
endif()

# spdlog
find_package(spdlog REQUIRED)
if(spdlog_FOUND)
    message(STATUS "Found spdlog ${spdlog_VERSION}")
endif()

# Google Test
if(BUILD_TESTS)
    find_package(GTest CONFIG REQUIRED)
    if(GTest_FOUND)
        message(STATUS "Found GTest ${GTest_VERSION}")
    else()
        message(WARNING "GTest not found. Tests will not be built.")
        set(BUILD_TESTS OFF CACHE BOOL "Build unit tests" FORCE)
    endif()
endif()

add_library(ChatSystem_Dependencies INTERFACE)
add_library(ChatSystem::Dependencies ALIAS ChatSystem_Dependencies)

target_link_libraries(ChatSystem_Dependencies INTERFACE
    ${wxWidgets_LIBRARIES}
    Boost::system
    Boost::thread
    Boost::asio
    OpenSSL::SSL
    OpenSSL::Crypto
    libpqxx::pqxx
    SQLite::SQLite3
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

target_include_directories(ChatSystem_Dependencies INTERFACE
    ${wxWidgets_INCLUDE_DIRS}
)

target_compile_definitions(ChatSystem_Dependencies INTERFACE
    ${wxWidgets_DEFINITIONS}
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

if(WIN32)
    target_compile_definitions(ChatSystem_Dependencies INTERFACE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        UNICODE
        _UNICODE
    )
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(ChatSystem_Dependencies INTERFACE
        _GNU_SOURCE
    )
endif()

find_package(Threads REQUIRED)
target_link_libraries(ChatSystem_Dependencies INTERFACE Threads::Threads)

message(STATUS "All dependencies configured successfully")