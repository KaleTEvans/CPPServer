cmake_minimum_required(VERSION 3.10)
project(option_scanner_test)

set(LIBRARY_NAME isb_option_scanner)

set(OPTION_SCANNER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file (GLOB OPTION_SCANNER_SOURCES ${OPTION_SCANNER_SRC_DIR}/*.cpp)

include_directories(${OPTION_SCANNER_INCLUDE_DIR})

add_library(${LIBRARY_NAME} ${OPTION_SCANNER_INCLUDES} ${OPTION_SCANNER_SOURCES})

add_executable(option_scanner_test ${CMAKE_CURRENT_SOURCE_DIR}/option_scanner_test.cpp)
target_link_libraries(option_scanner_test PRIVATE ${LIBRARY_NAME})
target_compile_options(option_scanner_test PRIVATE -Wno-switch)