cmake_minimum_required(VERSION 3.10)

project(IntradayStrategyBuilder)

set(OPTION_SCANNER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/option_scanner)

file(GLOB OPTION_SCANNER_INCLUDES ${OPTION_SCANNER_INCLUDE_DIR}/*.h)

add_subdirectory(third_party_libs/TwsApiC++)
add_subdirectory(option_scanner)

add_executable(TwsStrategyCppServer main.cpp)

