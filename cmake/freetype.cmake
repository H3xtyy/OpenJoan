# FreeType (https://freetype.org, FreeType License - compatible with GPL-3.0-or-later; ADR-0004): rasterises the
# characters of double-byte editions (Chinese, Japanese, Korean) that the game's bitmap fonts do not have
# (src/en2d/cjk_glyphs.cpp). Always the pinned release built with the project as a static library, without any of its
# optional dependencies (zlib, bzip2, libpng, HarfBuzz, Brotli), so every preset (Linux x86-64, Linux i686, Windows)
# builds it the same way and no system package is needed. Offline: FETCHCONTENT_SOURCE_DIR_FREETYPE=<unpacked source>.
# Result: target freetype.
include_guard(GLOBAL)

set(JOAN_FREETYPE_VERSION "2-13-3" CACHE STRING "FreeType release tag (VER-x-y-z) built with the project")

include(FetchContent)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
set(_joan_shared_libs "${BUILD_SHARED_LIBS}")
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(freetype
    URL "https://github.com/freetype/freetype/archive/refs/tags/VER-${JOAN_FREETYPE_VERSION}.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(freetype)
set(BUILD_SHARED_LIBS "${_joan_shared_libs}")
include(${CMAKE_CURRENT_LIST_DIR}/quiet_targets.cmake)
joan_quiet_targets("${freetype_SOURCE_DIR}")
# FreeType's headers as system headers: our warning set does not judge them.
get_target_property(_joan_ft_inc freetype INTERFACE_INCLUDE_DIRECTORIES)
if(_joan_ft_inc)
    set_property(TARGET freetype APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_joan_ft_inc}")
endif()
set_target_properties(freetype PROPERTIES POSITION_INDEPENDENT_CODE ON)
message(STATUS "FreeType: ${JOAN_FREETYPE_VERSION} built from source (${freetype_SOURCE_DIR})")
