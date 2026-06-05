cmake_minimum_required(VERSION 3.10)

project(flac_parser)
# Set the CXX standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# enable warnings
add_compile_options(-Wall)

add_executable(flac_parser libflac.cpp main.cpp)