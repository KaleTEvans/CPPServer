
set(LIBRARY_NAME isb_option_scanner)

set(OPTION_SCANNER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file (GLOB OPTION_SCANNER_SOURCES ${OPTION_SCANNER_SRC_DIR}/*.cpp)

include_directories(${OPTION_SCANNER_INCLUDE_DIR} ${OPTION_SCANNER_SRC_DIR})

add_library(${LIBRARY_NAME} ${OPTION_SCANNER_INCLUDES} ${OPTION_SCANNER_SOURCES})

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../third_party_libs/TwsApiC++/Api)
target_link_libraries(${LIBRARY_NAME} PUBLIC TWSAPI)

add_executable(option_scanner_test ${CMAKE_CURRENT_SOURCE_DIR}/option_scanner_test.cpp)