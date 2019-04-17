cmake_minimum_required(VERSION 2.8.3)
project(SecureBiometrics
LANGUAGES CXX)
find_package( OpenCV REQUIRED )
find_package(helib 1.0.0 EXACT REQUIRED)
find_package(OpenMP)

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

## Use -std=c++11 as default.
set(CMAKE_CXX_STANDARD 11)
## Disable C++ extensions
set(CMAKE_CXX_EXTENSIONS OFF)
## Require full C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(keyGen src/keyGenerator.cpp)
add_executable(encrypt src/encrypt.cpp)
add_executable(decrypt src/decrypt.cpp)
add_executable(compute src/compute.cpp)
add_executable(enroll src/enroll.cpp)

target_link_libraries(keyGen helib)
target_link_libraries(encrypt helib)
target_link_libraries(decrypt helib)
target_link_libraries(compute helib OpenMP::OpenMP_CXX)
target_link_libraries(enroll helib)

include_directories ("${PROJECT_SOURCE_DIR}/lib/thinning")
include_directories ("${PROJECT_SOURCE_DIR}/lib/binarisation")
include_directories ("${PROJECT_SOURCE_DIR}/lib/optimisation")
include_directories ("${PROJECT_SOURCE_DIR}/lib/gap-optimisation")
include_directories ("${PROJECT_SOURCE_DIR}/lib/extraction")
#include_directories ("${PROJECT_SOURCE_DIR}/lib/preprocess")


add_subdirectory(lib/thinning)
add_subdirectory(lib/binarisation)
add_subdirectory(lib/optimisation)
add_subdirectory(lib/gap-optimisation)
add_subdirectory(lib/extraction)
#add_subdirectory(lib/preprocess)

add_executable( fingerprint_process src/fingerprint_feature_extractor.cpp )

target_link_libraries( fingerprint_process ${OpenCV_LIBS} ExtractionAlgorithms ThinningAlgorithms BinarisationAlgorithms OptimisationAlgorithms )


