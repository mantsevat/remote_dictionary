cmake_minimum_required(VERSION 3.14)
project(remote_dictionary)

set(CMAKE_CXX_STANDARD 17)
include(FetchContent)

set(BOOST_INCLUDE_LIBRARIES program_options asio)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.80.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.x
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

file(GLOB server_src "${PROJECT_SOURCE_DIR}/app.h"
    "${PROJECT_SOURCE_DIR}/app.cpp"
    "${PROJECT_SOURCE_DIR}/main.cpp"
	"${PROJECT_SOURCE_DIR}/network/*.h"
	"${PROJECT_SOURCE_DIR}/network/*.cpp"
	"${PROJECT_SOURCE_DIR}/cli_parser/*.h"
	"${PROJECT_SOURCE_DIR}/cli_parser/*.cpp"
	"${PROJECT_SOURCE_DIR}/dictionary/*.h"
	"${PROJECT_SOURCE_DIR}/dictionary/*.cpp")

add_executable(server ${server_src})

target_link_libraries(
    server
    Boost::program_options
    Boost::asio
    spdlog::spdlog)

option(BUILD_WITH_TESTS "Build with tests" OFF)

if(BUILD_WITH_TESTS)
    file(GLOB test_src "${PROJECT_SOURCE_DIR}/dictionary/*.h"
        "${PROJECT_SOURCE_DIR}/dictionary/*.cpp")  
    add_subdirectory(tests)
    add_library(test_lib ${test_src})
endif()
