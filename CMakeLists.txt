cmake_minimum_required(VERSION 3.20)
project(Dexium
    VERSION 0.1.0
    LANGUAGES CXX
)

# Enable folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# vcpkg toolchain support
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

# Gather files
file(GLOB DEXIUM_SRC CONFIGURE_DEPENDS src/*.cpp)
file(GLOB DEXIUM_HEADERS CONFIGURE_DEPENDS include/*.hpp)

# STB_Image
file(GLOB DEXIUM_EXT CONFIGURE_DEPENDS external/STB/*.hpp)

# Ensure at least one main.cpp exists
if(NOT DEXIUM_SRC)
    message(FATAL_ERROR "No source files found in src/*.cpp — nothing to build.")
endif()

# Build as executable (for now)
add_executable(DexiumTest ${DEXIUM_SRC} ${DEXIUM_HEADERS})

# Include headers
target_include_directories(DexiumTest PUBLIC include)

# Link deps from vcpkg
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(DexiumTest
    PRIVATE glfw
    PRIVATE glad::glad
    PRIVATE glm::glm
)

# Filters in Visual Studio
source_group("Source Files" FILES ${DEXIUM_SRC})
source_group("Header Files" FILES ${DEXIUM_HEADERS})
source_group("External Files", FILES ${DEXIUM_EXT})

