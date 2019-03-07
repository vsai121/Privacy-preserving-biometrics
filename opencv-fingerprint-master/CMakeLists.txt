cmake_minimum_required(VERSION 2.8.3)
project( OpenCVTest )
find_package( OpenCV REQUIRED )

#add_subdirectory(src)
#file(GLOB SourceFiles
#    "*.h"
#    "*.cpp"
#)

add_definitions( "-std=c++11" )

include_directories ("${PROJECT_SOURCE_DIR}/lib/thinning")
include_directories ("${PROJECT_SOURCE_DIR}/lib/binarisation")
include_directories ("${PROJECT_SOURCE_DIR}/lib/optimisation")
include_directories ("${PROJECT_SOURCE_DIR}/lib/gap-optimisation")
include_directories ("${PROJECT_SOURCE_DIR}/lib/extraction")

add_subdirectory(lib/thinning)
add_subdirectory(lib/binarisation)
add_subdirectory(lib/optimisation)
add_subdirectory(lib/gap-optimisation)
add_subdirectory(lib/extraction)

add_executable( fingerprint_process src/fingerprint_feature_extractor.cpp )

target_link_libraries( fingerprint_process ${OpenCV_LIBS} ExtractionAlgorithms ThinningAlgorithms BinarisationAlgorithms OptimisationAlgorithms )


