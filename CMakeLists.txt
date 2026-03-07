cmake_minimum_required(VERSION 3.20)
project(Dexium VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Configureable options ---
option(DEXIUM_USE_ImGui "Enable Dear ImGui(docking) integration" OFF)

# Specify the filename inside Tests to build as the test application
set(DEXIUM_LIVE_TEST "textureComm.cpp" CACHE STRING "Filename inside Tests/ to compile as the live test")

# --- Collect Project sources ---
file(GLOB_RECURSE DEXIUM_SRC CONFIGURE_DEPENDS src/*.cpp)
file (GLOB_RECURSE DEXIUM_HEADERS CONFIGURE_DEPENDS include/*.hpp)

# --- FMT Inclusion (For CXX20 Print + Colours) --> Core Module
add_subdirectory(third_party/fmt)

# --- STB_Image Inclusion (Used for loading images into Dexium)
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE third_party/STB/) # Header only

# Validate the live test
set(LIVE_TEST_SOURCES "")

# Configure LiveTest as a seperate target with its own executable
if (DEXIUM_LIVE_TEST)
    set(LIVE_TEST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Tests/${DEXIUM_LIVE_TEST}")

    if (NOT EXISTS "${LIVE_TEST_FILE}")
        message(FATAL_ERROR "DEXIUM_LIVE_TEST set to '${DEXIUM_LIVE_TEST}, but file not found!")
    endif()

    # Automatically name the exec after the filename
    get_filename_component(LIVE_TEST_NAME ${DEXIUM_LIVE_TEST} NAME_WE)

    message(STATUS "Creating live test executable: ${DEXIUM_LIVE_TEST}")

    add_executable(${LIVE_TEST_NAME} ${LIVE_TEST_FILE})

    # Link agaisnt Dexium lib
    target_link_libraries(${LIVE_TEST_NAME} PRIVATE Dexium)

    # Include Dexium headers
    target_include_directories(${LIVE_TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

else()
    message(STATUS "No live test selected")
endif()


# --- Create Dexium Library target ---
add_library(Dexium STATIC
    ${DEXIUM_SRC}
    ${DEXIUM_HEADERS}
    #Tests/Live/*.cpp # builds for the program stored in Tests/Live, only one program should be there at a time. This will eventually be updated to take a more robust path to the application

)

target_include_directories(Dexium PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt/src
)

# --- Detect platform and include configuration ---
if (WIN32)
    message(STATUS "Configuring Dexium for Windows")
    include(${CMAKE_CURRENT_SOURCE_DIR}/projectConfig/Windows.cmake)
elseif (UNIX)
    message(STATUS, "Configuring Dexium for Linux/Unix")
    include(${CMAKE_CURRENT_SOURCE_DIR}/projectConfig/Linux.cmake)
else()
    message(FATAL_ERROR "Unsupported platform")
    # MacOS/ios is explicitly not supported as there are currently no plans to build a OpenGL renderer for MAcOS(OpenGL on MAcOS is deprecated) and Vulkan on MacOS is Moltken(Vulkan -> Metal API), why should I adopt silly API's that one corperation uses?
endif()


# --- Third-party platform non-specific libs ---
target_link_libraries(Dexium
    PUBLIC fmt_vendor
    PUBLIC ${DEXIUM_PLATFORM_LIBS} # defined in platform specific CMakeLists(projectConfig) and brings in paltform specific libs
    PUBLIC stb_image
)


# --- OPTIONAL: Dear ImGui support ---
if (DEXIUM_USE_ImGui)
    include(${CMAKE_CURRENT_SOURCE_DIR}/projectConfig/ImGui.cmake)
    target_link_libraries(Dexium PUBLIC imgui)
    target_compile_definitions(Dexium PUBLIC DEXIUM_USING_ImGui)
endif()

# --- IDE grouping ---
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source" FILES ${DEXIUM_SRC})
