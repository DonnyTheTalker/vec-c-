cmake_minimum_required(VERSION 3.27)
project(vec)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_FILES main.cpp)
add_executable(vec_run ${SOURCE_FILES})

include_directories(src)
add_subdirectory(src)

target_link_libraries(vec_run src)

add_subdirectory(test)
