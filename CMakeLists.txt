cmake_minimum_required(VERSION 3.14)
project(GeneticSimEngine LANGUAGES CXX)

# ─────────────────────────────────────────────────────────────────────────────
#  C++ Standard & Portability
# ─────────────────────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)   # Standard C++17 only

# ─────────────────────────────────────────────────────────────────────────────
#  Compiler Flags
# ─────────────────────────────────────────────────────────────────────────────
if(MSVC)
    add_compile_options(
        /O2          # Full optimization for simulation speed[cite: 22]
        /W4          # High warning level[cite: 22]
        /WX          # Treat warnings as errors to keep code clean[cite: 22]
        /permissive- # Standards-conformant mode[cite: 22]
    )
else()
    add_compile_options(
        -O2
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -ffast-math   # Faster floating-point math (useful for NN calculations)[cite: 22]
    )
endif()

# ─────────────────────────────────────────────────────────────────────────────
#  Executable Target
# ─────────────────────────────────────────────────────────────────────────────
# IMPORTANT: Matrix.cpp is removed because Matrix<T> is now a template in Matrix.h
add_executable(genetic_sim
    src/main.cpp
    src/NeuralNetwork.cpp
    src/Agent.cpp
    src/Environment.cpp
)

# Tells CMake to look in 'src/' for headers like Agent.h or Matrix.h[cite: 22]
target_include_directories(genetic_sim PRIVATE src)

# ─────────────────────────────────────────────────────────────────────────────
#  Output configuration
# ─────────────────────────────────────────────────────────────────────────────
# Keeps your build folder tidy by putting the executable in a 'bin' subfolder[cite: 22]
set_target_properties(genetic_sim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ─────────────────────────────────────────────────────────────────────────────
#  Build Information
# ─────────────────────────────────────────────────────────────────────────────
message(STATUS "Project:     ${PROJECT_NAME}")
message(STATUS "Build type:  ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler:    ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")