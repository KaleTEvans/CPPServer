cmake_minimum_required(VERSION 3.10)

project(tws_data_handler)

# Include files
include_directories(include)

# Source files
file (GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE TwsSocketClient Threads::Threads)

option(BUILD_TWS_DATA_TEST "Build the test file in option_scanner" ON)
if(BUILD_TWS_DATA_TEST)
    add_executable(TwsData_test tws_data_test.cpp)
    target_link_libraries(TwsData_test ${PROJECT_NAME} TwsSocketClient)
endif()