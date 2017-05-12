cmake_minimum_required(VERSION 2.8)

project(build-disp)

set(CMAKE_BUILD_TYPE "Debug")

add_definitions(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.63 REQUIRED COMPONENTS log program_options system filesystem thread)

include_directories(${Boost_INCLUDE_DIRS})

file(GLOB SRC_LIST "*.cpp" "*.hpp")

add_executable(${PROJECT_NAME} "main.cpp" ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread dl)
