cmake_minimum_required(VERSION 3.20)

#Helper function for optional dependencies
function(dexium_add_optional_library TARGET OPTION_NAME SYSTEM_NAME VENDOR_PATH INTERFACE_TARGET)
    if (${OPTION_NAME})
        # First try system package
        find_package(${SYSTEM_NAME} CONFIG QUIET)

        if (TARGET ${INTERFACE_TARGET})
            message(STATUS "Using system-installed ${SYSTEM_NAME}")
        else()
            # Fall back to vendor copy relative to project root
            set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/${VENDOR_PATH}")
            if (EXISTS "${VENDOR_DIR}/CMakeLists.txt")
                message(STATUS "System ${SYSTEM_NAME} not found, using vendored copy at ${VENDOR_DIR}")
                add_subdirectory(${VENDOR_DIR})
            else()
                message(FATAL_ERROR "Optional library ${SYSTEM_NAME} not found (system or vendor). Looked in ${VENDOR_DIR}")
            endif()
        endif()

        target_link_libraries(${TARGET} PRIVATE ${INTERFACE_TARGET})

        string(TOUPPER "${SYSTEM_NAME}" SYS_UPPER)
        string(REPLACE "-" "_" SYS_UPPER "${SYS_UPPER}")
        target_compile_definitions(${TARGET} PRIVATE DEXIUM_HAS_${SYS_UPPER}=1)
    endif()
endfunction()



# Use vcpkg toolchain - Dont like it... suck it!
if (DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file" FORCE)
endif()



# Add other optional 3rd party libs here
option(DEXIUM_USE_JSON "Build with nlohmann/json" ON)

project(Dexium VERSION 0.0.11 LANGUAGES CXX)

# Enable folders in DIEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set CXX Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Gather source/header files
file(GLOB DEXIUM_SRC CONFIGURE_DEPENDS src/*.cpp)
file(GLOB DEXIUM_SRC_CORE CONFIGURE_DEPENDS src/core/*.cpp)
file(GLOB DEXIUM_HEADERS CONFIGURE_DEPENDS include/*.hpp)
file(GLOB DEXIUM_EXT CONFIGURE_DEPENDS third_party/STB/*.hpp)

add_executable(Dexium-Core ${DEXIUM_SRC} ${DEXIUM_SRC_CORE} ${DEXIUM_HEADERS} ${DEXIUM_EXT})

# Ensure at least one main.cpp exists
if (NOT DEXIUM_SRC)
    message(FATAL_ERROR "No source files found in src/*.cpp - nothing to build")
endif()

# Incldue directories
target_include_directories(Dexium-Core PUBLIC include)
target_include_directories(Dexium-Core PUBLIC ${CMAKE_SOURCE_DIR}/third_party/STB)

#Optional 3rd party libs
dexium_add_optional_library(
        Dexium-Core
        DEXIUM_USE_JSON
        nlohmann_json
        third_party/nlohmann_json
        nlohmann_json::nlohmann_json
)


# Find packages via VCPKG
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Link libraries
target_link_libraries(Dexium-Core
PRIVATE glfw
PRIVATE glad::glad
PRIVATE glm::glm
)

# Group files in an IDE CLion/Visual Studio
source_group("Source Files" FILES ${DEXIUM_SRC})
source_group("Header Files" FILES ${DEXIUM_HEADERS})
source_group("External Files" Files ${DEXIUM_EXT})

