# FFmpeg (LGPL, linked dynamically) for the game's movies and music (src/media/, phase C3).
#
#   - JOAN_FFMPEG_ROOT=<folder> with include/, lib/*.lib (or lib*.so / lib*.dylib) and, on Windows, bin/*.dll;
#   - otherwise on 64-bit Windows: third_party/ffmpeg-win64 when it exists (the same minimal build for x64:
#     `third_party/ffmpeg-win64/build.sh <FFmpeg n7.1.1 checkout> <jobs>`), else vcpkg's FFmpeg
#     (`vcpkg install ffmpeg[core,avcodec,avformat,swscale,swresample]:x64-windows`; found in JOAN_VCPKG_ROOT,
#     VCPKG_ROOT, C:/vcpkg or the toolchain's vcpkg tree: <vcpkg>/installed/x64-windows has the same include/ lib/
#     bin/ layout);
#   - otherwise: the system's FFmpeg through pkg-config (libavformat-dev etc. on Linux).
# Result: target joan_ffmpeg (INTERFACE: headers as system includes + the five libraries) and JOAN_FFMPEG_DLLS (DLLs to
# copy next to joan_re.exe; empty when the system's libraries are used).
include_guard(GLOBAL)

set(JOAN_FFMPEG_ROOT "" CACHE PATH "FFmpeg folder (include/, lib/, bin/); empty = bundled (Windows x64) or pkg-config")
set(JOAN_FFMPEG_DLLS "")
set(_joan_ffmpeg_libs avformat avcodec swscale swresample avutil)

set(_joan_ffmpeg_root "${JOAN_FFMPEG_ROOT}")
if(NOT _joan_ffmpeg_root AND WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(EXISTS "${PROJECT_SOURCE_DIR}/third_party/ffmpeg-win64/include/libavformat/avformat.h")
        set(_joan_ffmpeg_root "${PROJECT_SOURCE_DIR}/third_party/ffmpeg-win64")
    else()
        set(_joan_ffmpeg_vcpkg_roots "${JOAN_VCPKG_ROOT}" "$ENV{VCPKG_ROOT}" "C:/vcpkg" "C:/src/vcpkg")
        if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
            list(PREPEND _joan_ffmpeg_vcpkg_roots "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
        endif()
        foreach(_root IN LISTS _joan_ffmpeg_vcpkg_roots)
            foreach(_tree "${_root}" "${_root}/installed/x64-windows")
                if(_root AND EXISTS "${_tree}/include/libavformat/avformat.h" AND EXISTS "${_tree}/lib/avformat.lib")
                    set(_joan_ffmpeg_root "${_tree}")
                    break()
                endif()
            endforeach()
            if(_joan_ffmpeg_root)
                break()
            endif()
        endforeach()
    endif()
    if(NOT _joan_ffmpeg_root)
        message(FATAL_ERROR "FFmpeg for x64 Windows not found. Build the minimal one into third_party/ffmpeg-win64 "
                            "(third_party/ffmpeg-win64/build.sh <FFmpeg n7.1.1 checkout> <jobs>, llvm-mingw), or "
                            "`vcpkg install ffmpeg[core,avcodec,avformat,swscale,swresample]:x64-windows` (pass "
                            "-DJOAN_VCPKG_ROOT=<vcpkg> if it is not in C:/vcpkg), or -DJOAN_FFMPEG_ROOT=<folder with "
                            "include/ lib/ bin/>.")
    endif()
endif()

add_library(joan_ffmpeg INTERFACE)
# libavutil/common.h wants the C99 integer constant macros in C++ (MSVC has them; GCC/Clang may need the define).
target_compile_definitions(joan_ffmpeg INTERFACE __STDC_CONSTANT_MACROS)
if(_joan_ffmpeg_root)
    target_include_directories(joan_ffmpeg SYSTEM INTERFACE "${_joan_ffmpeg_root}/include")
    foreach(_lib IN LISTS _joan_ffmpeg_libs)
        find_library(_joan_ffmpeg_${_lib} NAMES ${_lib} PATHS "${_joan_ffmpeg_root}/lib" NO_DEFAULT_PATH REQUIRED)
        target_link_libraries(joan_ffmpeg INTERFACE "${_joan_ffmpeg_${_lib}}")
    endforeach()
    if(WIN32)
        # Only FFmpeg's own five DLLs (a vcpkg bin/ folder holds every installed package's DLLs).
        foreach(_lib IN LISTS _joan_ffmpeg_libs)
            file(GLOB _joan_ffmpeg_dll "${_joan_ffmpeg_root}/bin/${_lib}-*.dll")
            list(APPEND JOAN_FFMPEG_DLLS ${_joan_ffmpeg_dll})
        endforeach()
    endif()
    message(STATUS "FFmpeg: ${_joan_ffmpeg_root}")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(JOAN_FFMPEG_PC REQUIRED IMPORTED_TARGET libavformat libavcodec libswscale libswresample libavutil)
    target_link_libraries(joan_ffmpeg INTERFACE PkgConfig::JOAN_FFMPEG_PC)
    message(STATUS "FFmpeg: system (pkg-config, libavformat ${JOAN_FFMPEG_PC_libavformat_VERSION})")
endif()
