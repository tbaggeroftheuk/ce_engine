cmake_minimum_required(VERSION 3.16)
include(FetchContent)

project(ce_engine)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(DATA_FILE_NAME "data.tcf")

set(SRC_DIR source)
set(INCLUDE_DIR include)


set(GRAPHICS_BACKEND "X11" CACHE STRING "Graphics backend")

if(WIN32)
    set(HOST_OS "Windows")
elseif(APPLE)
    set(HOST_OS "macOS")
elseif(UNIX)
    set(HOST_OS "Linux")
else()
    set(HOST_OS "Unknown OS")
endif()

file(GLOB_RECURSE CPP_SRC "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE C_SRC "${SRC_DIR}/*.c")

add_executable(ce_engine
    ${CPP_SRC}
    ${C_SRC}
)


target_include_directories(ce_engine PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/third_party
    ${INCLUDE_DIR}/third_party/imgui
    ${INCLUDE_DIR}/third_party/lua
)


target_compile_options(ce_engine PRIVATE
    -Wall
    -Wextra
)


target_compile_definitions(ce_engine PRIVATE
    ENGINE_BUILT_ON_OS="${HOST_OS}"
    CE_DATA_FILE_NAME="${DATA_FILE_NAME}"
)

if(GRAPHICS_BACKEND STREQUAL "X11")
    target_compile_definitions(ce_engine PRIVATE GRAPHICS_BACKEND_X11)
endif()

if(GRAPHICS_BACKEND STREQUAL "WAYLAND")
    target_compile_definitions(ce_engine PRIVATE GRAPHICS_BACKEND_WAYLAND)
endif()

find_package(raylib QUIET)

if(NOT raylib_FOUND)

    message(STATUS "raylib not found, downloading...")

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
    )

    FetchContent_MakeAvailable(raylib)

endif()

target_link_libraries(ce_engine raylib)

if(WIN32)

target_link_libraries(ce_engine
    ole32
    uuid
    comdlg32
    imm32
    oleaut32
)

else()

target_link_libraries(ce_engine
    m
    GL
    X11
    pthread
    dl
    rt
    Xi
)

endif()

# =========================
# Asset packing
# =========================

add_custom_command(
    TARGET ce_engine PRE_BUILD
    COMMAND python ${CMAKE_SOURCE_DIR}/scripts/tcf.py
            pack
            ${CMAKE_SOURCE_DIR}/assets
            ${CMAKE_BINARY_DIR}/data.tcf
)