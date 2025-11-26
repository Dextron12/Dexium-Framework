# config/Linux.cmake - Linux/Unix configuration for Dexium-Framework
message(STATUS "Loading Linux-specific CMake configuration")

# Compute project root (this file lives in config/)
set(PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

# Find system packages
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)    # header-only but find_package is OK if available

# Path to vendored glad (GLAD 2 often generates src/gl.c and include/gl.h)
set(GLAD_SRC "${PROJECT_ROOT}/third_party/glad/src/gl.c")
set(GLAD_INCLUDE_DIR "${PROJECT_ROOT}/third_party/glad/include")

# Basic sanity checks so errors are clear
if (NOT EXISTS "${GLAD_SRC}")
    message(FATAL_ERROR "GLAD source not found: ${GLAD_SRC}\nMake sure you generated GLAD 2 and placed it in third_party/glad (expect src/gl.c).")
endif()

if (NOT EXISTS "${GLAD_INCLUDE_DIR}/gl.h" AND NOT EXISTS "${GLAD_INCLUDE_DIR}/glad/glad.h")
    message(FATAL_ERROR "GLAD headers not found in ${GLAD_INCLUDE_DIR}\nExpect either include/gl.h (GLAD2) or include/glad/glad.h (GLAD1).")
endif()

# Add glad library (use correct source file name for GLAD2: gl.c)
add_library(glad STATIC "${GLAD_SRC}")
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# Ensure include directory is correct for the generated layout.
# GLAD2 sometimes places gl.h at include/gl.h; GLAD1 places include/glad/glad.h.
if (EXISTS "${GLAD_INCLUDE_DIR}/gl.h")
    target_include_directories(glad PUBLIC "${GLAD_INCLUDE_DIR}")
else()
    target_include_directories(glad PUBLIC "${GLAD_INCLUDE_DIR}")
endif()

# If CMake still can't determine the language for the target (very rare),
# force the linker language to C to stop the "cannot determine linker language" error.
# This is safe because GLAD's source is C.
get_target_property(_glad_sources glad SOURCES)
if ("${_glad_sources}" STREQUAL "NOTFOUND" OR NOT _glad_sources)
    # defensive: force language
    set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)
endif()

# Link glad into Dexium (Dexium must exist before including this file — ensured in top-level CMakeLists)
target_link_libraries(Dexium PUBLIC glad)

# Linux-specific system libs to link with Dexium
set(DEXIUM_PLATFORM_LIBS
        GL
        X11
        dl
        pthread
)

# Also link glfw and glm to Dexium here (or let the top-level do it)
target_link_libraries(Dexium PUBLIC glfw glm ${DEXIUM_PLATFORM_LIBS})

message(STATUS "Linux platform configuration finished (glad from ${GLAD_SRC})")
