cmake_minimum_required(VERSION 3.14)
project(GeneticSimEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)   


if(MSVC)
    add_compile_options(
        /O2          
        /W4         
        /WX         
        /permissive- 
    )
else()
    add_compile_options(
        -O2
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -ffast-math   
    )
endif()


add_executable(genetic_sim
    src/main.cpp
    src/NeuralNetwork.cpp
    src/Agent.cpp
    src/Environment.cpp
)


target_include_directories(genetic_sim PRIVATE src)


set_target_properties(genetic_sim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "Project:     ${PROJECT_NAME}")
message(STATUS "Build type:  ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler:    ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
