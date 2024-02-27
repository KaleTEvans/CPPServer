cmake_minimum_required(VERSION 3.10)
project(http_server_test)

set(LIBRARY_NAME isb_http_server)

set(CPP_SERVER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../third_party_libs/CppServer/include)
set(HTTP_SERVER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file (GLOB HTTP_SERVER_SOURCES ${HTTP_SERVER_SRC_DIR}/*.cpp)

include_directories(${HTTP_SERVER_INCLUDE_DIR})

add_library(${LIBRARY_NAME} ${HTTP_SERVER_INCLUDES} ${HTTP_SERVER_SOURCES})

add_executable(http_server_test ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_include_directories(http_server_test PRIVATE ${CPP_SERVER_INCLUDE})
target_link_libraries(http_server_test PRIVATE cppserver)
target_link_libraries(http_server_test PRIVATE ${LIBRARY_NAME})