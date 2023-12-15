cmake_minimum_required(VERSION 3.27)
project(lox)

set(CMAKE_CXX_STANDARD 20)

include_directories(deps)

add_compile_definitions(FMT_HEADER_ONLY)
add_executable(lox src/main.cpp
        src/Scanner.cpp
        src/Scanner.h
        src/Token.cpp
        src/Token.h
        src/Lox.cpp
        src/Lox.h
)
