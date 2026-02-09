cmake_minimum_required(VERSION 3.16)
project(fmt_vendor LANGUAGES CXX)

# Create a library target
file(GLOB FMT_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cc")
add_library(fmt_vendor ${FMT_SOURCES})

# Specify incldue dirs
target_include_directories(fmt_vendor PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

# Set CXX target to CXX17(Dexium uses this standard)
target_compile_features(fmt_vendor PUBLIC cxx_std_17)