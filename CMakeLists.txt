cmake_minimum_required(VERSION 3.16)
include(FetchContent)

project(ce_engine)

# Set language standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# Data file
set(DATA_FILE_NAME "data.tcf")

# Source and include directories
set(SRC_DIR source)
set(INCLUDE_DIR include)

if(WIN32)
    # Force x64 architecture
    set(CMAKE_GENERATOR_PLATFORM x64 CACHE STRING "" FORCE)
endif()

# Graphics backend
set(GRAPHICS_BACKEND "X11" CACHE STRING "Graphics backend")

# Detect host OS
if(WIN32)
    set(HOST_OS "Windows")
elseif(APPLE)
    set(HOST_OS "macOS")
elseif(UNIX)
    set(HOST_OS "Linux")
else()
    set(HOST_OS "Unknown OS")
endif()

# Gather source files
file(GLOB_RECURSE CPP_SRC "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE C_SRC "${SRC_DIR}/*.c")

# Create executable
add_executable(ce_engine
    ${CPP_SRC}
    ${C_SRC}
)

# Include directories
target_include_directories(ce_engine PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/third_party
    ${INCLUDE_DIR}/third_party/imgui
    ${INCLUDE_DIR}/third_party/lua
)

# Compiler warning flags (cross-platform)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ce_engine PRIVATE -Wall -Wextra)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(ce_engine PRIVATE /W3)
endif()

# Compile-time definitions
target_compile_definitions(ce_engine PRIVATE
    ENGINE_BUILT_ON_OS="${HOST_OS}"
    CE_DATA_FILE_NAME="${DATA_FILE_NAME}"
)

if(GRAPHICS_BACKEND STREQUAL "X11")
    target_compile_definitions(ce_engine PRIVATE GRAPHICS_BACKEND_X11)
elseif(GRAPHICS_BACKEND STREQUAL "WAYLAND")
    target_compile_definitions(ce_engine PRIVATE GRAPHICS_BACKEND_WAYLAND)
endif()

# Find or download raylib
find_package(raylib QUIET)

if(NOT raylib_FOUND)
    message(STATUS "raylib not found, downloading...")

    # Disable extra warnings on MSVC
    set(RAYLIB_NO_WARNINGS ON CACHE BOOL "Disable extra warning flags for MSVC")

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
    )
    FetchContent_MakeAvailable(raylib)
endif()

target_link_libraries(ce_engine raylib)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(ce_engine
        ole32
        uuid
        comdlg32
        imm32
        oleaut32
    )
else()
    # Ensure OpenGL, X11, pthread, etc. are linked on Linux
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    target_link_libraries(ce_engine
        m
        OpenGL::GL
        ${X11_LIBRARIES}
        pthread
        dl
        rt
        Xi
    )
    target_include_directories(ce_engine PRIVATE ${X11_INCLUDE_DIR})
endif()

# Pack assets if folder exists
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(
        TARGET ce_engine PRE_BUILD
        COMMAND python "${CMAKE_SOURCE_DIR}/scripts/tcf.py"
                pack
                "${CMAKE_SOURCE_DIR}/assets"
                "${CMAKE_BINARY_DIR}/data.tcf"
    )
else()
    message(WARNING "Assets folder not found: skipping tcf.py pack")
endif()