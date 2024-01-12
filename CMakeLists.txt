cmake_minimum_required(VERSION 3.27)
project(lox)

set(CMAKE_CXX_STANDARD 20)

include_directories(deps)

add_custom_command(
        OUTPUT src/Ast.h
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_ast.py ${CMAKE_CURRENT_SOURCE_DIR}/src/Ast.h
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_ast.py
        VERBATIM
)

add_compile_definitions(FMT_HEADER_ONLY)
add_executable(lox src/main.cpp
        src/Scanner.cpp
        src/Scanner.h
        src/Token.h
        src/Lox.cpp
        src/Lox.h
        src/Expr.h
        src/common.h
        src/Ast.h
        src/AstPrinter.h
        src/Parser.cpp
        src/Parser.h
        src/AstInterpreter.cpp
        src/AstInterpreter.h
)

