message(STATUS "Loading Linux specific Cmake configuration")

# Compute the project root (This file lives in projectConfig)
set(PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")


# Find system packages
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Path to vendored glad 9GLAD2 often generates src/gl.c include/gl.h, instead of glad.*)
set(GLAD_SRC "${PROJECT_ROOT}/third_party/glad/src/gl.c")
set(GLAD_INC "${PROJECT_ROOT}/third_party/glad/include/")

# Add GLAD library
add_library(glad STATIC "${GLAD_SRC}")
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

target_include_directories(glad PUBLIC "${GLAD_INC}")

# Link GLAD2 into Dexium
target_link_libraries(Dexium PUBLIC glad)

# Linux-specific system libs to link to Dexium
set (DEXIUM_PLATFORM_LIBS
    GL
    X11
    dl
    pthread
)

# Link glfw and glm
target_link_libraries(Dexium PUBLIC glfw glm ${DEXIUM_PLATFORM_LIBS})

message(STATUS "Linux platform configuration finished (glad from ${GLAD_SRC})")
