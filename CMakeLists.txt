cmake_minimum_required(VERSION 3.10)
project(LexerProject C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(compiler
    src/compiler.c
    src/file_utils.c
    src/lexer.c
    src/parser.c
    src/token.c
    src/transpiler.c
    src/backend.c
)

target_link_libraries(compiler PRIVATE ${LLVM_LIBS})
llvm_map_components_to_libnames(LLVM_LIBS core support irreader)

# Add include directories (headers in include/)
target_include_directories(compiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)