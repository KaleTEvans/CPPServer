cmake_minimum_required(VERSION 3.10)

project(isb_option_scanner)

# Include files
include_directories(include)

# Add protobuf generated files
include_directories(${CMAKE_SOURCE_DIR}/generated)

# Source files
file (GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)

target_link_libraries(${PROJECT_NAME} TwsSocketClient tws_data_handler isb_database_handler Threads::Threads ${Protobuf_LIBRARIES})

if(BUILD_OPTION_SCANNER_TEST)
    add_executable(option_scanner_test option_scanner_test.cpp)
    target_include_directories(option_scanner_test PRIVATE include ${CMAKE_SOURCE_DIR}/generated)
    target_link_libraries(option_scanner_test ${PROJECT_NAME} TwsSocketClient tws_data_handler isb_database_handler ${Protobuf_LIBRARIES})
endif()