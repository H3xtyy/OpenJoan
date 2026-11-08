# OpenAL Soft (https://openal-soft.org, LGPL-2.1-or-later): the open-source OpenAL that answers the game's OpenAL 1.0
# calls instead of Creative's OpenAL32.dll (phase C2, claude/NEXT.md). Linked as a shared library (LGPL: users can
# swap it), so joan_re.exe ships with soft_oal.dll on Windows / uses the system's libopenal.so.1 on Linux.
#
# Where it comes from (the same scheme will serve SDL3 later):
#   - JOAN_OPENAL_SYSTEM=ON: an installed OpenAL Soft (find_package: its OpenALConfig.cmake, e.g. a Linux distribution's
#     libopenal-dev / openal-soft-devel, or vcpkg's openal-soft). The default on Linux / macOS.
#   - otherwise (the default on Windows): CMake downloads the pinned release and builds it with the project (no
#     install step, no vcpkg). An offline build can point FETCHCONTENT_SOURCE_DIR_OPENAL_SOFT at an unpacked copy.
# On Windows the DLL is named soft_oal.dll (OpenAL Soft's own name for its implementation DLL): the game folder holds
# Creative's OpenAL32.dll, and a DLL of that name next to joan_re.exe would be taken for ours.
#
# Result: target OpenAL::OpenAL; JOAN_OPENAL_DLL_TARGET = the target whose file is the DLL to copy next to joan_re.exe
# (empty when the system's library is used).
include_guard(GLOBAL)

if(WIN32)
    set(_joan_openal_system_default OFF)
else()
    set(_joan_openal_system_default ON)
endif()
option(JOAN_OPENAL_SYSTEM "Use an installed OpenAL Soft (find_package) instead of building the pinned release"
       ${_joan_openal_system_default})
set(_joan_openal_soft_default "1.25.2")
# A build folder configured with an earlier default keeps that value in its cache: move it to the current default
# (an explicitly chosen other release stays).
if(NOT DEFINED CACHE{JOAN_OPENAL_SOFT_VERSION} OR "$CACHE{JOAN_OPENAL_SOFT_VERSION}" STREQUAL "1.24.3")
    set(JOAN_OPENAL_SOFT_VERSION "${_joan_openal_soft_default}" CACHE STRING
        "OpenAL Soft release built when JOAN_OPENAL_SYSTEM is OFF" FORCE)
endif()

set(JOAN_OPENAL_DLL_TARGET "")
if(JOAN_OPENAL_SYSTEM)
    find_package(OpenAL CONFIG QUIET)
    if(NOT TARGET OpenAL::OpenAL)
        find_package(OpenAL MODULE REQUIRED)  # CMake's FindOpenAL (OpenAL::OpenAL since CMake 3.25)
    endif()
    message(STATUS "OpenAL: installed library (${OpenAL_DIR}${OPENAL_LIBRARY})")
else()
    include(FetchContent)
    # OpenAL Soft's own options (its CMakeLists uses policy CMP0077, so plain variables set them): only the library.
    set(ALSOFT_UTILS OFF)
    set(ALSOFT_NO_CONFIG_UTIL ON)
    set(ALSOFT_EXAMPLES OFF)
    set(ALSOFT_TESTS OFF)
    set(ALSOFT_INSTALL OFF)
    set(ALSOFT_INSTALL_CONFIG OFF)
    set(ALSOFT_INSTALL_HRTF_DATA OFF)
    set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF)
    set(ALSOFT_INSTALL_EXAMPLES OFF)
    set(ALSOFT_INSTALL_UTILS OFF)
    set(ALSOFT_UPDATE_BUILD_VERSION OFF)  # no git needed
    # 1.25 can build its sources as C++20 modules; a plain build is more robust across generators / compilers (with
    # modules on, llvm-mingw's clang fails in al/auxeffectslot.cpp; without them 1.25.2 builds for i686 and Linux).
    set(ALSOFT_ENABLE_MODULES OFF)
    set(LIBTYPE SHARED)
    set(CMAKE_POLICY_DEFAULT_CMP0092 NEW)  # no default /W3 in its compiler flags (1.24 asks for CMake 3.13 policies)
    FetchContent_Declare(openal_soft
        URL "https://github.com/kcat/openal-soft/archive/refs/tags/${JOAN_OPENAL_SOFT_VERSION}.tar.gz"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(openal_soft)
    if(WIN32)
        set_target_properties(OpenAL PROPERTIES OUTPUT_NAME soft_oal)
    endif()
    # Third-party code: its warnings are not ours to fix (and would break the zero-warning build log), so OpenAL
    # Soft's targets (and its bundled fmt) compile with warnings off. With MSVC, 1.24.3 gave C4293 in
    # common/ringbuffer.cpp (a 64-bit-only shift in a discarded if-constexpr branch) and C4161.
    include(${CMAKE_CURRENT_LIST_DIR}/quiet_targets.cmake)
    joan_quiet_targets("${openal_soft_SOURCE_DIR}")
    set(JOAN_OPENAL_DLL_TARGET OpenAL)
    message(STATUS "OpenAL: OpenAL Soft ${JOAN_OPENAL_SOFT_VERSION} built from source (${openal_soft_SOURCE_DIR})")
endif()
