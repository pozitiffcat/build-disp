cmake_minimum_required(VERSION 2.8)

project(cmd-command)

set(CMAKE_BUILD_TYPE "Debug")

add_definitions(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.63 REQUIRED COMPONENTS log program_options system filesystem)

include_directories(${Boost_INCLUDE_DIRS} "../../")

add_library(${PROJECT_NAME} SHARED "cmd_command.cpp")

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread)
