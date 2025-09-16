cmake_minimum_required(VERSION 3.10)
project(QuokkaInterpreter C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Collect all .c files from src/ and src/utils/
file(GLOB SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/features/*.c"
)

add_executable(interpreter ${SRC_FILES})

# Add include directories (headers in include/)
target_include_directories(interpreter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)