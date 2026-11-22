
cmake_minimum_required(VERSION 3.20)

project(DeltaRescue
    VERSION 1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_executable(DeltaRescue
    src/main.cpp
    third_party/src/gl.c
)

target_include_directories(DeltaRescue PRIVATE
    include
    third_party/include
)

target_link_libraries(DeltaRescue PRIVATE
    glfw
    OpenGL::GL
)

if(MSVC)
    target_compile_options(DeltaRescue PRIVATE /W4)
else()
    target_compile_options(DeltaRescue PRIVATE -Wall -Wextra -pedantic)
endif()
