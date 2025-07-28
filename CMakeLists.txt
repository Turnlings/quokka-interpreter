cmake_minimum_required(VERSION 3.10)
project(QuokkaInterpreter C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(interpreter
    src/interpreter.c
    src/file_utils.c
    src/lexer.c
    src/parser.c
    src/token.c
    src/transpiler.c
    src/evaluator.c
)

# Add include directories (headers in include/)
target_include_directories(interpreter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)