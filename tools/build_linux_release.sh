#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# The Linux release build (docs/RELEASING.md), run inside the manylinux_2_28 container (AlmaLinux 8: glibc 2.28, GCC 14
# from gcc-toolset-14), so that the result starts on any x86-64 distribution with glibc 2.28 or newer (2018 on):
#   podman run --rm -v "$PWD":/src:Z -w /src quay.io/pypa/manylinux_2_28_x86_64 tools/build_linux_release.sh v1.2.3 [jobs]
# The C++ runtime (libstdc++, libgcc) is linked statically into the executable and into OpenAL Soft; SDL3, OpenAL Soft
# and a minimal FFmpeg are built here and shipped next to the executable. Everything else SDL loads at run time from the
# player's system when present (X11 or Wayland, OpenGL, ALSA / PulseAudio / PipeWire, udev). Nothing needs systemd.
# Output: dist/openjoan-<version>-linux-x64.tar.gz and dist/OpenJoan-<version>-x86_64.AppImage (tools/package_release.py).
set -euo pipefail
version=${1:?usage: build_linux_release.sh <version> [jobs]}
jobs=${2:-$(nproc)}
root=$(cd "$(dirname "$0")/.." && pwd)
work=${JOAN_RELEASE_WORK:-/tmp/openjoan-release}
mkdir -p "$work"

# Headers and pkg-config files of what SDL3 and OpenAL Soft can use (the libraries themselves come from the player's system).
dnf install -y -q git make pkgconfig file \
    libX11-devel libXext-devel libXrandr-devel libXcursor-devel libXi-devel libXfixes-devel libXScrnSaver-devel \
    libXtst-devel libxkbcommon-devel wayland-devel wayland-protocols-devel mesa-libEGL-devel mesa-libGL-devel \
    mesa-libgbm-devel libdrm-devel alsa-lib-devel pulseaudio-libs-devel systemd-devel dbus-devel
git config --global --add safe.directory '*'  # the checkout belongs to another user than the container's root
export PATH=/opt/python/cp312-cp312/bin:$PATH  # a current Python for the tools (the system one is 3.6)
python3 -m pip install -q ninja patchelf pillow

# libdecor (window decorations on Wayland desktops without server-side ones, GNOME): AlmaLinux 8 has no package. SDL3
# loads libdecor-0.so.0 at run time and only needs its header and pkg-config file to build that support.
libdecor=0.2.2
if [ ! -f "$work/libdecor/include/libdecor-0/libdecor.h" ]; then
    curl -fsSL "https://gitlab.freedesktop.org/libdecor/libdecor/-/archive/$libdecor/libdecor-$libdecor.tar.gz" | tar xz -C "$work"
    mkdir -p "$work/libdecor/include/libdecor-0" "$work/libdecor/lib/pkgconfig"
    cp "$work/libdecor-$libdecor/src/libdecor.h" "$work/libdecor/include/libdecor-0/"
    printf 'prefix=%s\nincludedir=${prefix}/include\n\nName: libdecor\nDescription: header only (loaded at run time)\nVersion: %s\nCflags: -I${includedir}/libdecor-0\nLibs: -ldecor-0\n' \
        "$work/libdecor" "$libdecor" > "$work/libdecor/lib/pkgconfig/libdecor-0.pc"
fi
export PKG_CONFIG_PATH="$work/libdecor/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"

# A minimal FFmpeg (the game's WMV / WMA movies and music only), shipped in the archive.
if [ ! -f "$work/ffmpeg/lib/libavformat.so" ]; then
    [ -d "$work/ffmpeg-src" ] || git clone -q --depth 1 --branch n7.1.1 https://github.com/FFmpeg/FFmpeg.git "$work/ffmpeg-src"
    "$root/tools/build_ffmpeg_linux.sh" "$work/ffmpeg-src" "$work/ffmpeg" "$jobs"
fi

build=${JOAN_RELEASE_BUILD:-build/linux-x64-portable}  # not the preset's folder: a host build of it stays as it is
static_runtime="-static-libstdc++ -static-libgcc"
cmake --preset linux-x64-release -B "$build" -DJOAN_SDL3_SYSTEM=OFF -DJOAN_OPENAL_SYSTEM=OFF -DJOAN_FFMPEG_ROOT="$work/ffmpeg" \
    -DCMAKE_EXE_LINKER_FLAGS="$static_runtime" -DCMAKE_SHARED_LINKER_FLAGS="$static_runtime"
cmake --build "$build" -j"$jobs"
python3 "$root/tools/package_release.py" "$build" "$version" --appimage --max-glibc 2.28
