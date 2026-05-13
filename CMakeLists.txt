cmake_minimum_required(VERSION 3.16)

project(sorting_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sorting_lab
    src/main.cpp
)

target_include_directories(sorting_lab PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(sorting_lab PRIVATE /W4)
else()
    target_compile_options(sorting_lab PRIVATE -Wall -Wextra -pedantic)
endif()
