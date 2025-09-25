cmake_minimum_required(VERSION 3.20)

# Add other optional 3rd party libs here
option(DEXIUM_HAS_JSON "Build with nlohmann/json" ON)

project(Dexium VERSION 0.0.11 LANGUAGES CXX)

# Enable folders in DIEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set CXX Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use vcpkg toolchain - Dont like it... suck it!
if (DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")
endif()

# Gather source/header files
file(GLOB DEXIUM_SRC CONFIGURE_DEPENDS src/*.cpp)
file(GLOB DEXIUM_HEADERS CONFIGURE_DEPENDS include/*.hpp)
file(GLOB DEXIUM_EXT CONFIGURE_DEPENDS external/STB/*.hpp)

# Ensure at least one main.cpp exists
if (NOT DEXIUM_SRC)
    message(FATAL_ERROR "No source files found in src/*.cpp - nothing to build")
endif()

# Create executable
add_executable(DexiumTest ${DEXIUM_SRC} ${DEXIUM_HEADERS}
        include/helpers.h
        src/helpers.cpp)

# Incldue directories
target_include_directories(DexiumTest PUBLIC include)
target_include_directories(DexiumTest PUBLIC ${CMAKE_SOURCE_DIR}/external/STB)

#Optional 3rd party libs
if (DEXIUM_USE_JSON)
    #First try find system instaleld nlohmann/json
    find_package(nlohmann_json CONFIG REQUIRED)

    if (nlohmann_json_FOUND)
        message(STATUS "Using system-installed nlohmann_json")
        target_link_libraries(Dexium PRIVATE nlohmann_json::nlohmann_json)
    else()
        message(STATUS "System nlohmann_json not found, using vendored copy")
        add_subdirectory(third_party/nlohmann_json)
        target_link_libraries(Dexium PRIVATE nlohmann_json)
    endif ()

    target_compile_definitions(Dexium PRIVATE DEXIUM_HAS_JSON)
endif ()

# Find packages via VCPKG
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Link libraries
target_link_libraries(DexiumTest
PRIVATE glfw
PRIVATE glad::glad
PRIVATE glm::glm
)

# Group files in an IDE CLion/Visual Studio
source_group("Source Files" FILES ${DEXIUM_SRC})
source_group("Header Files" FILES ${DEXIUM_HEADERS})
source_group("External Files" Files ${DEXIUM_EXT})

