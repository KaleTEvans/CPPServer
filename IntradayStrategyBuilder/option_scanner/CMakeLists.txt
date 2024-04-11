cmake_minimum_required(VERSION 3.10)

project(isb_option_scanner)

# Include files
include_directories(include)

# Source files
file (GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES})

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE tws_data_handler TwsSocketClient Threads::Threads)

option(BUILD_OPTION_SCANNER_TEST "Build the test file in option_scanner" ON)
if(BUILD_OPTION_SCANNER_TEST)
    add_executable(option_scanner_test option_scanner_test.cpp)
    target_link_libraries(option_scanner_test isb_option_scanner TwsSocketClient tws_data_handler)
endif()