cmake_minimum_required(VERSION 3.10)
project(IntradayStrategyBuilder)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64 -O0 -fPIC")

# Set the output directory for the executables to one level up
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Executables)

set(TWS_API_CPP ${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs)

# TwsApiC++ Library
add_library(TwsApiL0 STATIC ${TWS_API_CPP}/TwsApiC++/Src/TwsApiL0.cpp)

# Include directories for the TwsApiC++ library
target_include_directories(TwsApiL0 PUBLIC
    ${TWS_API_CPP}/TwsApiC++/Api
    ${TWS_API_CPP}/TwsApiC++/Src
    ${TWS_API_CPP}/source/PosixClient/Shared
    ${TWS_API_CPP}/source/PosixClient/src
)

set(OPTION_SCANNER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/option_scanner/include)
file(GLOB OPTION_SCANNER_INCLUDES ${OPTION_SCANNER_INCLUDE_DIR}/*.h)

add_subdirectory(third_party_libs/TwsApiC++)
add_subdirectory(option_scanner)

add_executable(TwsStrategyCppServer main.cpp)
target_link_libraries(TwsStrategyCppServer PRIVATE TwsApiL0 pthread) # pthread for threading support

target_compile_options(TwsApiL0 PRIVATE -Wno-switch)