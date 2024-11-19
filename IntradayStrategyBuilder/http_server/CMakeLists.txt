cmake_minimum_required(VERSION 3.10)

project(isb_http_server)

# Include files
include_directories(include)

# Add protobuf generated files
include_directories(${CMAKE_SOURCE_DIR}/generated)

# Source Files
file(GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE cppserver isb_option_scanner Threads::Threads ${Protobuf_LIBRARIES})

option(BUILD_HTTP_SERVER_TEST "Build the test file in http_server" ON)
if(BUILD_HTTP_SERVER_TEST)
    add_executable(http_server_test http_server_test.cpp ${CMAKE_SOURCE_DIR}/generated/messages.pb.cc)
    target_include_directories(option_scanner_test PRIVATE include ${CMAKE_SOURCE_DIR}/generated)
    target_link_libraries(http_server_test isb_http_server cppserver isb_option_scanner ${Protobuf_LIBRARIES})
endif()