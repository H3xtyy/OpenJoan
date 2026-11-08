# SDL3 (https://libsdl.org, zlib licence): the platform layer of joan_re.exe (src/platform/, phase C4): window,
# events, keyboard / mouse, timers. Same scheme as OpenAL Soft (cmake/openal_soft.cmake):
#   - JOAN_SDL3_SYSTEM=ON: an installed SDL3 (find_package(SDL3), e.g. libsdl3-dev on Linux or vcpkg) - the default
#     on Linux / macOS;
#   - otherwise (the default on Windows): CMake downloads the pinned release and builds it with the project (SDL3.dll,
#     copied next to joan_re.exe). Offline: FETCHCONTENT_SOURCE_DIR_SDL3=<unpacked source>.
# Result: target SDL3::SDL3; JOAN_SDL3_DLL_TARGET = the target whose file is the DLL to copy (empty for the system's).
include_guard(GLOBAL)

if(WIN32)
    set(_joan_sdl3_system_default OFF)
else()
    set(_joan_sdl3_system_default ON)
endif()
option(JOAN_SDL3_SYSTEM "Use an installed SDL3 (find_package) instead of building the pinned release"
       ${_joan_sdl3_system_default})
set(_joan_sdl3_default "3.4.20")
# A build folder configured with an earlier default keeps that value in its cache: move it to the current default
# (an explicitly chosen other release stays).
if(NOT DEFINED CACHE{JOAN_SDL3_VERSION} OR "$CACHE{JOAN_SDL3_VERSION}" STREQUAL "3.4.16")
    set(JOAN_SDL3_VERSION "${_joan_sdl3_default}" CACHE STRING "SDL3 release built when JOAN_SDL3_SYSTEM is OFF" FORCE)
endif()

set(JOAN_SDL3_DLL_TARGET "")
if(JOAN_SDL3_SYSTEM)
    find_package(SDL3 CONFIG REQUIRED)
    message(STATUS "SDL3: installed library (${SDL3_DIR})")
else()
    include(FetchContent)
    set(SDL_SHARED ON)
    set(SDL_STATIC OFF)
    set(SDL_TEST_LIBRARY OFF)
    set(SDL_TESTS OFF)
    set(SDL_EXAMPLES OFF)
    set(SDL_INSTALL OFF)
    set(SDL_DISABLE_INSTALL ON)
    set(SDL_DISABLE_INSTALL_DOCS ON)
    FetchContent_Declare(sdl3
        URL "https://github.com/libsdl-org/SDL/archive/refs/tags/release-${JOAN_SDL3_VERSION}.tar.gz"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(sdl3)
    include(${CMAKE_CURRENT_LIST_DIR}/quiet_targets.cmake)
    joan_quiet_targets("${sdl3_SOURCE_DIR}")
    # SDL's headers as system headers: our /W4 /WX does not judge them.
    foreach(_t IN ITEMS SDL3-shared SDL3_Headers)
        if(TARGET ${_t})
            get_target_property(_inc ${_t} INTERFACE_INCLUDE_DIRECTORIES)
            if(_inc)
                set_property(TARGET ${_t} APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_inc}")
            endif()
        endif()
    endforeach()
    set(JOAN_SDL3_DLL_TARGET SDL3-shared)
    message(STATUS "SDL3: ${JOAN_SDL3_VERSION} built from source (${sdl3_SOURCE_DIR})")
endif()
