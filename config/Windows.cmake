message(STATUS "Loading Windows-specific CMake configuration")

#
# ---- Dependencies via vcpkg ----
#
# IMPORTANT:
# vcpkg must be used by passing the toolchain file at configure time:
#
#   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
#

# glfw3, glm, imgui (if installed via vcpkg)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# If using imgui from vcpkg:
# find_package(imgui CONFIG REQUIRED)


#
# ---- glad (vendor, because vcpkg glad2 produces gl names) ----
#
add_library(glad STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/gl.c"
)

target_include_directories(glad PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include"
)


#
# ---- ImGui docking (vendored) ----
#
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")

file(GLOB IMGUI_SOURCES
        "${IMGUI_DIR}/*.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
        "${IMGUI_DIR}"
        "${IMGUI_DIR}/backends"
)

# Link backend dependencies
target_link_libraries(imgui PUBLIC glfw glad opengl32)


#
# ---- Platform libraries ----
#
set(DEXIUM_PLATFORM_LIBS
        glfw
        glm::glm
        glad
        imgui
        opengl32
)

message(STATUS "Windows platform configuration finished")
