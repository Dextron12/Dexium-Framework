cmake_minimum_required(VERSION 3.20)
project(Dexium
    VERSION 0.1.0
    LANGUAGES CXX
)

# Enable folders in IDEs like Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use vcpkg toolchain if present
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Warnings
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -pedantic -Wno-unused-parameter)
endif()

# Collect files (flat structure)
file(GLOB DEXIUM_SRC CONFIGURE_DEPENDS src/*.cpp)
file(GLOB DEXIUM_HEADERS CONFIGURE_DEPENDS include/*.hpp)

# Define library
add_library(Dexium STATIC
    ${DEXIUM_SRC}
    ${DEXIUM_HEADERS}
)

# Include path
target_include_directories(Dexium PUBLIC include)

# Dependencies from vcpkg
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(Dexium
    PRIVATE glfw
    PRIVATE glad
    PRIVATE glm
)

# Visual Studio filters
source_group("Source Files" FILES ${DEXIUM_SRC})
source_group("Header Files" FILES ${DEXIUM_HEADERS})

