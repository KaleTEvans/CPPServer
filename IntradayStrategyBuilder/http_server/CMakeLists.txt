cmake_minimum_required(VERSION 3.10)

project(isb_http_server)

# Include files
include_directories(include)

# Source Files
file(GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE cppserver)

option(BUILD_HTTP_SERVER_TEST "Build the test file in http_server" ON)
if(BUILD_HTTP_SERVER_TEST)
    add_executable(http_server_test http_server_test.cpp)
    target_link_libraries(http_server_test isb_http_server cppserver)
endif()