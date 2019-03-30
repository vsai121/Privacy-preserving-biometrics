cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

## Use -std=c++11 as default.
set(CMAKE_CXX_STANDARD 11)
## Disable C++ extensions
set(CMAKE_CXX_EXTENSIONS OFF)
## Require full C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SecureHamming
LANGUAGES CXX)

find_package(helib 1.0.0 EXACT REQUIRED)

include_directories(include)

add_executable(keyGen keyGenerator.cpp)
add_executable(encrypt encrypt.cpp)
add_executable(decrypt decrypt.cpp)
add_executable(compute compute.cpp)

target_link_libraries(keyGen helib)
target_link_libraries(encrypt helib)
target_link_libraries(decrypt helib)
target_link_libraries(compute helib)
