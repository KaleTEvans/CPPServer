cmake_minimum_required(VERSION 3.10)
project(IntradayStrategyBuilder)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64 -O0 -fPIC")

add_subdirectory(option_scanner)
add_subdirectory(http_server)
add_subdirectory(third_party_libs)

add_executable(TwsStrategyCppServer main.cpp)

# Path to TwsApiIBKR
# set(TWSAPIIBKR_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/TwsApiIBKR/source/cppclient/client/libTwsSocketClient.so")
# set(TWSAPIIBKR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/TwsApiIBKR/source/cppclient/client")

target_link_libraries(TwsStrategyCppServer PUBLIC 
    isb_option_scanner
    isb_http_server
    cppserver
    #twsapicpp
    pthread
    TwsSocketClient
)

target_include_directories(TwsStrategyCppServer PUBLIC 
    ${OPTION_SCANNER_INCLUDE_DIR}
    ${HTTP_SERVER_INCLUDE_DIR}
)

#target_compile_options(twsapicpp PRIVATE -Wno-switch)

set_target_properties(
    TwsStrategyCppServer
    option_scanner_test
    http_server_test
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Executables
)