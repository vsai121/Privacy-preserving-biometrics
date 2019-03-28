cmake_minimum_required(VERSION 2.8.9)
project(SecureHamming)

set (PROJECT_LINK_LIBS libgmp.so libhelib.a libntl.so)
link_directories(lib)

include_directories(include)

add_executable(keygen key-gen.cpp)
add_executable(hamming hamming.cpp)
add_executable(main main.cpp ppb_lib.cpp)

target_link_libraries(keygen ${PROJECT_LINK_LIBS})
target_link_libraries(hamming ${PROJECT_LINK_LIBS})
target_link_libraries(main ${PROJECT_LINK_LIBS})
