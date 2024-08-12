cmake_minimum_required(VERSION 3.10)

project(isb_database_handler)

# Include files
include_directories(include)

# Source files
file (GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE isb_option_scanner Threads::Threads)