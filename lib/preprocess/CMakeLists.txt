file(GLOB SourceFiles "*.h" "*.cpp")
add_library(
    preProcessAlgorithms
    fpenhancement.cpp
    checksize.cpp
    normalizer.cpp
    ridgefilter.cpp
    ridgeorient.cpp	
)
