cmake_minimum_required(VERSION 3.10)
project(LexerProject C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add the executable target with your source files
add_executable(compiler
    compiler.c
    lexer.c
    transpiler.c
    file_utils.c
    token.c
)

# Add include directories
target_include_directories(compiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})