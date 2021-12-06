cmake_minimum_required(VERSION 3.16)

project(thread_pool VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

include_directories("${CMAKE_SOURCE_DIR}/include")

add_executable(thread_pool ${PROJECT_SOURCE_DIR}/main.cc)

