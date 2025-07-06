cmake_minimum_required(VERSION 3.10)
project(BlightestStone)

set(CMAKE_CXX_STANDARD 17)

# Check if we are on Windows or Linux
if(WIN32)
    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")
    endif()

    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)

elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_image REQUIRED SDL2_image)
endif()

# Gather source and header files
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.h" "${PROJECT_SOURCE_DIR}/include/*.hpp")

# Add executable with all sources and headers
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Debug macro for conditional compilation
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:_DEBUG>)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_image_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
)

# Link libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
		SDL2::SDL2
		SDL2::SDL2main
		SDL2_image::SDL2_image
		nlohmann_json::nlohmann_json)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES} ${SDL2_image_LIBRARIES})
endif()

# ---- VIRTUAL FOLDERS FOR VISUAL STUDIO ----

# Put .cpp files under "Source Files"
source_group("Source Files" FILES ${SOURCES})

# Put .h/.hpp files under "Header Files"
source_group("Header Files" FILES ${HEADERS})
