cmake_minimum_required(VERSION 3.20)
project(Dexium VERSION 0.16.2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DEXIUM_USE_AUDIO "Enable Audio Support" ON)
option(DEXIUM_USE_ImGui "Enable Dear ImGui integration" ON)

# -------------------------------------------------------------
# Collect sources BEFORE creating the Dexium target
# -------------------------------------------------------------
file(GLOB_RECURSE DEXIUM_SRC CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE DEXIUM_HEADERS CONFIGURE_DEPENDS include/*.hpp)

# -------------------------------------------------------------
# Create Dexium library target ONCE
# -------------------------------------------------------------
add_library(Dexium STATIC
        ${DEXIUM_SRC}
        ${DEXIUM_HEADERS}
        Tests/AssetRedo.cpp
        Tests/AssetRedo.cpp
)

target_include_directories(Dexium PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/STB
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt/src
)

# -------------------------------------------------------------
# Detect platform and include configuration
# -------------------------------------------------------------
if (WIN32)
    message(STATUS "Configuring Dexium for Windows")
    include(${CMAKE_CURRENT_SOURCE_DIR}/config/Windows.cmake)
elseif (UNIX)
    message(STATUS "Configuring Dexium for Linux/Unix")
    include(${CMAKE_CURRENT_SOURCE_DIR}/config/Linux.cmake)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# -------------------------------------------------------------
# Third-party libs
# -------------------------------------------------------------
target_link_libraries(Dexium
        PUBLIC fmt_vendor
        PUBLIC ${DEXIUM_PLATFORM_LIBS}
)

# -------------------------------------------------------------
# Audio support
# -------------------------------------------------------------
if (DEXIUM_USE_AUDIO)
    set(MINIAUDIO_DIR "${CMAKE_SOURCE_DIR}/third_party/miniaudio")
    target_include_directories(Dexium PUBLIC "${MINIAUDIO_DIR}")
    target_compile_definitions(Dexium PUBLIC DEXIUM_USING_AUDIO)
endif()

# -------------------------------------------------------------
# Dear ImGui support
# -------------------------------------------------------------
if (DEXIUM_USE_ImGui)
    include(${CMAKE_CURRENT_SOURCE_DIR}/config/ImGui.cmake)
    target_link_libraries(Dexium PUBLIC imgui)
    target_compile_definitions(Dexium PUBLIC DEXIUM_USING_ImGui)
endif()

# -------------------------------------------------------------
# IDE grouping
# -------------------------------------------------------------
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source" FILES ${DEXIUM_SRC})
