cmake_minimum_required(VERSION 3.10)
project(IntradayStrategyBuilder)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64 -O0 -fPIC")

# Protobuf integration
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/generated)

add_subdirectory(third_party_libs)
add_subdirectory(tws_data_handler)
add_subdirectory(option_scanner)
add_subdirectory(database)
add_subdirectory(http_server)

add_executable(TwsStrategyCppServer main.cpp ${CMAKE_SOURCE_DIR}/generated/messages.pb.cc)

option(BUILD_TWS_DATA_TEST "Build the test file in tws_data_handler" OFF)
option(BUILD_OPTION_SCANNER_TEST "Build the test file in option_scanner" OFF)
option(BUILD_HTTP_SERVER_TEST "Build the test file in http server" OFF)

set(CPPCOMMON_MODULE "Set to on to build only the core cppserver library" ON)

target_link_libraries(TwsStrategyCppServer PUBLIC 
    pthread
    ${Protobuf_LIBRARIES}
    TwsSocketClient
    cppserver
    tws_data_handler
    isb_option_scanner
    isb_http_server
    isb_database_handler
)

target_include_directories(TwsStrategyCppServer PUBLIC 
    ${OPTION_SCANNER_INCLUDE_DIR}
    ${HTTP_SERVER_INCLUDE_DIR}
)

# set_target_properties(
#     TwsStrategyCppServer
#     option_scanner_test
#     http_server_test
#     TwsData_test
#     PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Executables
# )