cmake_minimum_required(VERSION 3.20)
project(Dexium VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS off)

# --- Global options ---
option(DEXIUM_USE_AUDIO "Enable Audio support" ON)
option(DEXIUM_USE_ImGui "Enable Dear ImGui integration" ON)

# --- Add main Dexium target ---
file(GLOB_RECURSE DEXIUM_SRC CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE DEXIUM_HEADERS CONFIGURE_DEPENDS include/*.hpp)

# --- CREATE DEXIUM LIBRARY ---
#for debug mode, optionally make it an OBJECT instead
#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    #add_library(Dexium OBJECT ${DEXIUM_SRC} ${DEXIUM_HEADERS})
#else()
    #add_library(Dexium STATIC ${DEXIUM_SRC} ${DEXIUM_HEADERS})
#endif()
add_library(Dexium STATIC ${DEXIUM_SRC} ${DEXIUM_HEADERS})

# THIRD-PARTY LIBRARIES
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
add_subdirectory(third_party/fmt)

# Compatibility layer for the stupid OpenAL-Soft library(Seriously the alsoft::fmt dependency is a joke and I've wasted 30hrs trying to get it to compile)
if (TARGET fmt_vendor AND NOT TARGET alsoft::fmt)
    add_library(alsoft::fmt ALIAS fmt_vendor)
elseif (TARGET fmt AND NOT TARGET alsoft::fmt)
    add_library(alsoft::fmt ALIAS fmt)
endif()

target_include_directories(Dexium
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/STB
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt
)

target_link_libraries(Dexium
PUBLIC glfw
PUBLIC glad::glad
PUBLIC glm::glm
PUBLIC fmt_vendor
)

# --- Link OpenAL (if enabled) ---
if (DEXIUM_USE_AUDIO)
    set(MINIAUDIO_DIR "${CMAKE_SOURCE_DIR}/third_party/miniaudio")

    # OpenAl-Soft has been removed as a audio dependency. It's a horrible library and if I were the maintainer's I'd curl up into a ball until the world vanishes

    target_include_directories(Dexium PUBLIC
            "${MINIAUDIO_DIR}"
    )

    # Correct library directory
    target_link_directories(Dexium PRIVATE
            "{MINIAUDIO_DIR}"
    )

    target_compile_definitions(Dexium PUBLIC DEXIUM_USING_AUDIO)
endif()

# --- Link Dear ImGui (if enabled) ---
if (DEXIUM_USE_ImGui)
    SET(IMGUI_DIR "${CMAKE_SOURCE_DIR}/third_party/imgui")

    target_include_directories(Dexium PUBLIC
        "${IMGUI_DIR}"
        "${IMGUI_DIR}/backends"
    )

    target_sources(Dexium PRIVATE
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    )

    # Allows to check for 'DEXIUM_USING_ImGui' in src code
    target_compile_definitions(Dexium PUBLIC DEXIUM_USING_ImGui)
endif()


# --- Add subdirectory for tests ---
add_subdirectory(Tests)

# --- IDE GROUPING ---
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source" FILES ${DEXIUM_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Include" FILES ${DEXIUM_HEADERS})
