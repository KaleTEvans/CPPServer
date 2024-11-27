cmake_minimum_required(VERSION 3.10)

project(isb_option_scanner)

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)

# Include files
include_directories(include)

# Add protobuf generated files
include_directories(${CMAKE_SOURCE_DIR}/generated)

# Source files
file (GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} 
    ${Protobuf_LIBRARIES}
    TwsSocketClient 
    cppserver 
    tws_data_handler 
    isb_database_handler 
    isb_http_server 
    Threads::Threads 
)

if(BUILD_OPTION_SCANNER_TEST)
    add_executable(option_scanner_test option_scanner_test.cpp ${CMAKE_SOURCE_DIR}/generated/messages.pb.cc)
    target_include_directories(option_scanner_test PRIVATE include ${CMAKE_SOURCE_DIR}/generated)
    target_link_libraries(option_scanner_test ${PROJECT_NAME} TwsSocketClient cppserver tws_data_handler isb_http_server isb_database_handler ${Protobuf_LIBRARIES})
endif()