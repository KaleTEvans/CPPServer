cmake_minimum_required(VERSION 3.10)

project(isb_option_scanner)

# Include files
include_directories(include)

# Source files
file (GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} TwsSocketClient tws_data_handler Threads::Threads)

if(BUILD_OPTION_SCANNER_TEST)
    add_executable(option_scanner_test option_scanner_test.cpp)
    target_link_libraries(option_scanner_test ${PROJECT_NAME} TwsSocketClient tws_data_handler)
endif()