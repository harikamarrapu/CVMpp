cmake_minimum_required(VERSION 3.14)
project(CVM++ VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

set(SOURCES
    src/lexer.cpp
    src/parser.cpp
    src/compiler.cpp
    src/vm.cpp
    src/main.cpp
)

add_executable(cvm ${SOURCES})
target_include_directories(cvm PRIVATE src)

file(COPY scripts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
