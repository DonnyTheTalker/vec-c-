project(Vector)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_FILES
    vec.cpp
)

add_library(src STATIC ${SOURCE_FILES})
