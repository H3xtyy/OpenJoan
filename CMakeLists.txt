cmake_minimum_required(VERSION 3.21)
project(openjoan
    VERSION 0.1.0
    DESCRIPTION "Open-source reimplementation of Wars and Warriors: Joan of Arc (2004)"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(JOAN_BUILD_ENGINE "Build the standalone SDL3/OpenGL executable (needs SDL3)" ON)
option(JOAN_BUILD_TOOLS  "Build asset tools (jres, jdbf, jmodel; jview needs SDL3)" ON)
option(JOAN_BUILD_TESTS  "Build unit tests" ON)
option(JOAN_BUILD_DATA_TEST "Build tests/data_image_reference_test with the game executable (hook builds): the committed data image against the reference joan.exe in JOAN_GAME_DIR (skipped when absent)" ON)
option(JOAN_BUILD_HOOK   "Build openjoan.exe, the reimplementation of joan.exe (Windows x64, Linux x86-64; Linux x86 as the objcmp reference)" OFF)
option(JOAN_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
# The executable's static data is committed as typed source (src/data/README.md): src/data/image.cpp
# includes src/data/rdata.inc / data.inc. No build reads joan.exe; the game's data files are the only input at run time.

# ---------------------------------------------------------------------------
# Common warning flags
# ---------------------------------------------------------------------------
add_library(joan_warnings INTERFACE)
if(MSVC)
    # C / C++ only: the hook's openjoan.exe also compiles a resource script, and rc.exe rejects these options.
    target_compile_options(joan_warnings INTERFACE "$<$<COMPILE_LANGUAGE:C,CXX>:/W4;/permissive-;/utf-8>")
    # fopen/getenv/strcpy "unsafe" deprecation warnings: we use the portable standard functions.
    target_compile_definitions(joan_warnings INTERFACE _CRT_SECURE_NO_WARNINGS)
    # x64 only: C4366 "the result of the unary '&' operator may be unaligned" for members of the #pragma pack(1)
    # classes (src/original/gen: the original's byte layouts). x64 loads / stores tolerate unaligned addresses, the
    # members are scalars / arrays of float / int (no aligned SSE types), so every such pointer is valid there.
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        target_compile_options(joan_warnings INTERFACE "$<$<COMPILE_LANGUAGE:C,CXX>:/wd4366>")
    endif()
    if(JOAN_WARNINGS_AS_ERRORS)
        target_compile_options(joan_warnings INTERFACE "$<$<COMPILE_LANGUAGE:C,CXX>:/WX>")
    endif()
else()
    target_compile_options(joan_warnings INTERFACE -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wno-sign-conversion)
    if(JOAN_WARNINGS_AS_ERRORS)
        target_compile_options(joan_warnings INTERFACE -Werror)
    endif()
endif()

# ---------------------------------------------------------------------------
# Portable libraries (no third-party dependencies)
# ---------------------------------------------------------------------------
add_subdirectory(tools/lib/core)
add_subdirectory(tools/lib/ilib)
add_subdirectory(tools/lib/en3d)

if(JOAN_BUILD_ENGINE)
    # Packages installed with vcpkg in classic mode (`vcpkg install sdl3:x64-windows`)
    # are only visible to CMake through vcpkg's toolchain file. If no toolchain file
    # was given, look for a vcpkg installation ourselves and use its installed tree.
    if(NOT CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_joan_triplet x64-windows)
        else()
            set(_joan_triplet x86-windows)
        endif()
        if(NOT WIN32)
            string(REPLACE "windows" "${CMAKE_SYSTEM_NAME}" _joan_triplet "${_joan_triplet}")
            string(TOLOWER "${_joan_triplet}" _joan_triplet)
        endif()
        set(_joan_vcpkg_roots "${JOAN_VCPKG_ROOT}" "$ENV{VCPKG_ROOT}" "C:/vcpkg" "C:/src/vcpkg" "$ENV{USERPROFILE}/vcpkg" "$ENV{HOME}/vcpkg")
        foreach(_root IN LISTS _joan_vcpkg_roots)
            if(_root AND EXISTS "${_root}/installed/${_joan_triplet}/share/sdl3")
                message(STATUS "Using vcpkg packages from ${_root}/installed/${_joan_triplet}")
                list(APPEND CMAKE_PREFIX_PATH "${_root}/installed/${_joan_triplet}")
                break()
            endif()
        endforeach()
    endif()

    find_package(SDL3 CONFIG QUIET)
    if(SDL3_FOUND)
        add_subdirectory(tools/lib/platform)
        add_subdirectory(tools/lib/render)
        add_subdirectory(tools/lib/app)
    else()
        message(WARNING "SDL3 not found: the 'joan' executable will not be built (libraries, tools and tests still are). "
                        "Install it (vcpkg install sdl3:x64-windows / your package manager) and, if vcpkg is not in "
                        "C:/vcpkg, pass -DJOAN_VCPKG_ROOT=<vcpkg folder> or -DSDL3_DIR=<folder with SDL3Config.cmake>.")
    endif()
endif()

if(JOAN_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(JOAN_BUILD_TESTS OR (JOAN_BUILD_HOOK AND JOAN_BUILD_DATA_TEST))
    enable_testing()
endif()
if(JOAN_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(NOT JOAN_BUILD_HOOK)
    message(WARNING "This configuration does NOT build openjoan.exe (the game executable). Choose a preset that does: "
                    "linux-x64 / linux-x64-release, windows-hook-x64, vs2026-hook-x64 / vs2026-hook-x64-release, mingw-x64.")
endif()

if(JOAN_BUILD_HOOK)
    # 64-bit Windows or Linux. 32-bit x86 only on Linux and only as the objcmp reference build (tools/objcmp-x86.sh,
    # cmake/linux-i686.cmake): the presets for 32-bit builds were retired at the end of Phase 2.
    if(NOT ((WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8) OR
            (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND (CMAKE_SIZEOF_VOID_P EQUAL 8 OR CMAKE_SIZEOF_VOID_P EQUAL 4))))
        message(FATAL_ERROR "JOAN_BUILD_HOOK requires 64-bit Windows (Visual Studio -A x64, presets windows-hook-x64 / "
                            "vs2026-hook-x64 / mingw-x64) or Linux (presets linux-x64 / linux-x64-release; the 32-bit "
                            "objcmp reference build: tools/objcmp-x86.sh).")
    endif()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
        # x86-64 Linux: a position-independent executable (the distributions' default, ASLR for the image too). The
        # code, the data image and the brk heap then lie far above 4 GB: no address of ours fits 32 bits any more.
        include(CheckPIESupported)
        check_pie_supported()
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif()
    add_subdirectory(src)
endif()
