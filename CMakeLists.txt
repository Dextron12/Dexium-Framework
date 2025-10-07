cmake_minimum_required(VERSION 3.20)
project(Dexium VERSION 0.1.0 LANGUAGES CXX)

# ===== BASIC SETTINGS ======
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS off)

# ===== GLOB FILES ========
file(GLOB_RECURSE DEXIUM_SRC CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE DEXIUM_HEADERS CONFIGURE_DEPENDS include/*.hpp)

message(STATUS "Dexium Source Files: ${DEXIUM_SRC}")

# ===== CREATE LIBRARY =======
# for debug mode, optionally make it an OBJECT library instead
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_library(Dexium OBJECT ${DEXIUM_SRC} ${DEXIUM_HEADERS})
else()
    add_library(Dexium STATIC ${DEXIUM_SRC} ${DEXIUM_HEADERS})
endif()

# ===== INCLUDE DIRECTORIES =====
target_include_directories(Dexium PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt
${CMAKE_CURRENT_SOURCE_DIR}/third_party/STB
)

# ===== THIRD-PARTY LIBS =======
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
add_subdirectory(third_party/fmt)

target_link_libraries(Dexium
PUBLIC glfw
PUBLIC glad::glad
PUBLIC glm::glm
PUBLIC fmt::fmt
)

# ====== DEFINE MACROS FOR DEBUG/RELEASE =======
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Dexium PUBLIC DEXIUM_DEBUG=1)
else()
    target_compile_definitions(Dexium PUBLIC DEXIUM_RELEASE=1)
endif()

# ===== OPTIONAL DEPENDENCIES =========
option(DEXIUM_USE_JSON "Build with nlohmann/json" ON)
if (DEXIUM_USE_JSON)
    find_package(nlohmann_json CONFIG REQUIRED)
    target_link_libraries(Dexium PUBLIC nlohmann_json::nlohmann_json)
endif()

# ======= IDE GROUPING ========
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source" FILES ${DEXIUM_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Include" FILES ${DEXIUM_HEADERS})


# aDD SUBdIRECTORY FOR TESTING
add_subdirectory(Tests)