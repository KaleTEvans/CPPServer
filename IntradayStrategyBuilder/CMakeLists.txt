cmake_minimum_required(VERSION 3.10)
project(IntradayStrategyBuilder)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64 -O0 -fPIC")

# Root paths for third_party_libs
set(TWS_API_CPP ${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/TwsApiWrapper)
set(CPP_SERVER ${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/CppServer)

# Source paths for the CppServer library
# set(CPP_SERVER_HTTP_SRC_DIR ${CPP_SERVER}/source/server/http)

# Source files for the CppServer library
#file (GLOB CPP_SERVER_HTTP_SOURCES ${CPP_SERVER_HTTP_SRC_DIR}/*.cpp)

# Libraries
add_library(TwsApiL0 STATIC ${TWS_API_CPP}/TwsApiC++/Src/TwsApiL0.cpp)
#add_library(CppServerHttp STATIC ${CPP_SERVER_HTTP_SOURCES})

# Include directories for the TwsApiC++ library
target_include_directories(TwsApiL0 PUBLIC
    ${TWS_API_CPP}/TwsApiC++/Api
    ${TWS_API_CPP}/TwsApiC++/Src
    ${TWS_API_CPP}/source/PosixClient/Shared
    ${TWS_API_CPP}/source/PosixClient/src
)

# Include directories for CppServer
#target_include_directories(CppServerHttp PUBLIC ${CPP_SERVER}/include/server/http)

# Include paths for project files
set(OPTION_SCANNER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/option_scanner/include)
set(HTTP_SERVER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/http_server/include)

file(GLOB OPTION_SCANNER_INCLUDES ${OPTION_SCANNER_INCLUDE_DIR}/*.h)
file(GLOB HTTP_SERVER_INCLUDES ${HTTP_SERVER_INCLUDE_DIR}/*.h)

add_subdirectory(${CPP_SERVER})
add_subdirectory(${TWS_API_CPP}/TwsApiC++)
add_subdirectory(option_scanner)
add_subdirectory(http_server)

add_executable(TwsStrategyCppServer main.cpp)
target_link_libraries(TwsStrategyCppServer PRIVATE TwsApiL0 pthread) # pthread for threading support
target_link_libraries(TwsStrategyCppServer PRIVATE cppserver)

target_compile_options(TwsApiL0 PRIVATE -Wno-switch)

# Set the output directory for the executables to one level up
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Executables)

set_target_properties(
    TwsStrategyCppServer
    option_scanner_test
    http_server_test
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Executables
)