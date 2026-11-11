#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Pack a release-flavour build into dist/openjoan-<version>-<platform>.tar.gz or .zip (docs/RELEASING.md).

  tools/package_release.py <build dir> <version> [--out dist]

Only named files go in: the executable, its settings file, the runtime libraries it is shipped with, the licences and
the install page. Nothing else from the build directory can reach an archive, and no game file is ever in one.

On Windows the DLLs of WINDOWS_DLLS are shipped (SDL3, OpenAL Soft, FFmpeg); an llvm-mingw build also needs the C++
runtime DLLs of its toolchain. On Linux SDL3 and OpenAL Soft are
shipped next to the executable (RUNPATH $ORIGIN), and FFmpeg too when the build used a minimal one of its own
(JOAN_FFMPEG_ROOT, tools/build_ffmpeg_linux.sh); a build against the system's FFmpeg instead lists the libraries it was
linked with in the archive's REQUIREMENTS.txt. Every shipped Linux file gets RUNPATH $ORIGIN only (no build paths), and
--max-glibc refuses a build that needs a newer glibc than the one named (the portable build, tools/build_linux_release.sh,
passes 2.28).
--appimage (Linux) also writes dist/OpenJoan-<version>-x86_64.AppImage: the same files in an AppDir, packed by appimagetool
with the static AppImage runtime (no libfuse2 needed on the player's system). Both tools are pinned by version and
checksum and downloaded when JOAN_APPIMAGETOOL / JOAN_APPIMAGE_RUNTIME do not name local copies.
"""
import argparse
import hashlib
import glob
import os
import re
import shutil
import subprocess
import sys
import tarfile
import tempfile
import urllib.request
import zipfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPOSITORY = 'https://github.com/H3xtyy/OpenJoan'
FFMPEG_SOURCE = 'https://ffmpeg.org/releases/ffmpeg-7.1.1.tar.xz'
BUNDLED_LINUX = ('libSDL3.so', 'libopenal.so', 'libatomic.so')  # libatomic: OpenAL Soft's, a GCC runtime library
WINDOWS_DLLS = re.compile(r'^(SDL3|soft_oal|OpenAL32|(avformat|avcodec|avutil|swscale|swresample)-\d+)\.dll$', re.I)
MINGW_RUNTIME = ('libc++.dll', 'libunwind.dll')
APPIMAGETOOL = ('https://github.com/AppImage/appimagetool/releases/download/1.9.1/appimagetool-x86_64.AppImage',
                'ed4ce84f0d9caff66f50bcca6ff6f35aae54ce8135408b3fa33abfc3cb384eb0')
APPIMAGE_RUNTIME = ('https://github.com/AppImage/type2-runtime/releases/download/20251108/runtime-x86_64',
                    '2fca8b443c92510f1483a883f60061ad09b46b978b2631c807cd873a47ec260d')
APPRUN = '''#!/bin/sh
# OpenJoan's AppImage: the game folder is the folder of the .AppImage file (the executable finds it through $APPIMAGE).
here=$(dirname "$(readlink -f "$0")")
if [ -n "$APPIMAGE" ]; then
    game=$(dirname "$APPIMAGE")
    if [ ! -e "$game/openjoan.ini" ] && [ ! -e "$game/joan_re.ini" ] && [ -w "$game" ]; then
        cp "$here/usr/bin/openjoan.ini" "$game/openjoan.ini"
    fi
fi
exec "$here/usr/bin/openjoan" "$@"
'''
DESKTOP = '''[Desktop Entry]
Type=Application
Name=OpenJoan
Comment=Wars and Warriors: Joan of Arc, reimplemented (needs the game's data files)
Exec=openjoan
Icon=openjoan
Categories=Game;
Terminal=false
'''


def cache_value(build, key):
    for ln in open(os.path.join(build, 'CMakeCache.txt'), errors='ignore'):
        m = re.match(r'%s:\w+=(.*)$' % re.escape(key), ln)
        if m:
            return m.group(1).strip()
    return ''


def linux_libraries(exe):
    """(paths to ship, FFmpeg sonames the build needs)"""
    needed = re.findall(r'\(NEEDED\)\s+Shared library: \[([^\]]+)\]',
                        subprocess.run(['readelf', '-d', exe], capture_output=True, text=True, check=True).stdout)
    where = dict(re.findall(r'^\s*(\S+) => (\S+)', subprocess.run(['ldd', exe], capture_output=True, text=True).stdout, re.M))
    needed += [so for so in where if so.startswith('libatomic.so') and so not in needed]  # needed by a shipped library
    ship = []
    for so in needed:
        if so.startswith(BUNDLED_LINUX):
            if so not in where or not os.path.exists(where[so]):
                sys.exit('%s not found by ldd' % so)
            ship.append((where[so], so))
    return ship, [so for so in needed if so.startswith(('libav', 'libsw'))]


def glibc_needed(path):
    """the newest GLIBC_x.y version `path` needs, as a tuple (0,) when none"""
    out = subprocess.run(['objdump', '-T', path], capture_output=True, text=True, check=True).stdout
    versions = [tuple(int(n) for n in v.split('.')) for v in re.findall(r'\(GLIBC_([0-9.]+)\)', out)]
    return max(versions, default=(0,))


def set_runpath(path):
    """RUNPATH $ORIGIN only: the shipped libraries, and none of the build machine's folders"""
    if not shutil.which('patchelf'):
        sys.exit('patchelf is needed to set the RUNPATH of %s (pip install patchelf)' % path)
    subprocess.run(['patchelf', '--set-rpath', '$ORIGIN', path], check=True)


def pinned_tool(env, url_sum, folder):
    """a local copy named by `env`, else the pinned download checked against its SHA-256"""
    if os.environ.get(env):
        return os.environ[env]
    url, digest = url_sum
    path = os.path.join(folder, os.path.basename(url))
    with urllib.request.urlopen(url) as r, open(path, 'wb') as f:
        shutil.copyfileobj(r, f)
    if hashlib.sha256(open(path, 'rb').read()).hexdigest() != digest:
        sys.exit('%s: checksum mismatch' % url)
    os.chmod(path, 0o755)
    return path


def make_appimage(files_dir, exe_name, version, out_dir, t):
    """dist/OpenJoan-<version>-x86_64.AppImage from the archive's files"""
    app = os.path.join(t, 'OpenJoan.AppDir')
    bin_dir, doc_dir = os.path.join(app, 'usr', 'bin'), os.path.join(app, 'usr', 'share', 'doc', 'openjoan')
    os.makedirs(bin_dir)
    os.makedirs(doc_dir)
    for f in sorted(os.listdir(files_dir)):
        target = bin_dir if f == exe_name or f == 'openjoan.ini' or '.so' in f else doc_dir
        shutil.copy2(os.path.join(files_dir, f), os.path.join(target, f))
    with open(os.path.join(app, 'AppRun'), 'w') as f:
        f.write(APPRUN)
    os.chmod(os.path.join(app, 'AppRun'), 0o755)
    with open(os.path.join(app, 'openjoan.desktop'), 'w') as f:
        f.write(DESKTOP)
    sys.path.insert(0, os.path.join(ROOT, 'tools'))
    import make_icon  # our own artwork (ADR-0002)
    make_icon.draw(256).save(os.path.join(app, 'openjoan.png'))
    tools = os.path.join(t, 'tools')
    os.makedirs(tools)
    tool = pinned_tool('JOAN_APPIMAGETOOL', APPIMAGETOOL, tools)
    runtime = pinned_tool('JOAN_APPIMAGE_RUNTIME', APPIMAGE_RUNTIME, tools)
    out = os.path.join(out_dir, 'OpenJoan-%s-x86_64.AppImage' % version)
    env = dict(os.environ, ARCH='x86_64', APPIMAGE_EXTRACT_AND_RUN='1')  # no FUSE needed where it is built
    subprocess.run([tool, '--no-appstream', '--runtime-file', runtime, app, out], check=True, env=env)
    return out


def strip_executable(path, windows):
    """Removes the debug information: a release archive ships no symbols (an MSVC .exe has none in it to begin with)"""
    tool = shutil.which('x86_64-w64-mingw32-strip', path=os.path.join(os.environ.get('JOAN_MINGW', ''), 'bin')) if windows \
        else shutil.which('strip')
    if windows and not os.environ.get('JOAN_MINGW'):
        return  # MSVC: the debug information is in the .pdb, which is not shipped
    if not tool:
        sys.exit('no strip tool found: the archive would ship debug information')
    subprocess.run([tool, path], check=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('build')
    ap.add_argument('version')
    ap.add_argument('--out', default=os.path.join(ROOT, 'dist'))
    ap.add_argument('--appimage', action='store_true', help='Linux: also write an AppImage')
    ap.add_argument('--max-glibc', default='', help='Linux: refuse files that need a newer glibc than this (2.28)')
    a = ap.parse_args()
    if cache_value(a.build, 'JOAN_RELEASE') != 'ON':
        sys.exit('%s is not a release-flavour build (JOAN_RELEASE=ON)' % a.build)
    src = os.path.join(a.build, 'src')
    windows = os.path.exists(os.path.join(src, 'openjoan.exe'))
    platform = 'windows-x64' if windows else 'linux-x64'
    name = 'openjoan-%s-%s' % (a.version, platform)
    with tempfile.TemporaryDirectory() as t:
        d = os.path.join(t, name)
        os.makedirs(d)
        exe = os.path.join(src, 'openjoan.exe' if windows else 'openjoan')
        files = [(exe, os.path.basename(exe)), (os.path.join(src, 'openjoan.ini'), 'openjoan.ini'),
                 (os.path.join(ROOT, 'LICENSE'), 'LICENSE.txt'), (os.path.join(ROOT, 'docs', 'INSTALL.md'), 'INSTALL.md')]
        files.append((os.path.join(ROOT, 'third_party', 'licenses', 'OpenAL-Soft-COPYING.txt'), 'OPENAL-COPYING.txt'))
        if windows:
            dlls = [p for p in sorted(glob.glob(os.path.join(src, '*.dll'))) if WINDOWS_DLLS.match(os.path.basename(p))]
            files += [(p, os.path.basename(p)) for p in dlls]
            if 'mingw' in cache_value(a.build, 'CMAKE_TOOLCHAIN_FILE'):
                if not os.environ.get('JOAN_MINGW'):
                    sys.exit('set JOAN_MINGW: a mingw build ships its C++ runtime DLLs')
                bin_dir = os.path.join(os.environ['JOAN_MINGW'], 'x86_64-w64-mingw32', 'bin')
                files += [(os.path.join(bin_dir, n), n) for n in MINGW_RUNTIME]
                files.append((os.path.join(ROOT, 'third_party', 'licenses', 'LLVM-LICENSE.txt'), 'LLVM-LICENSE.txt'))
            files.append((os.path.join(ROOT, 'third_party', 'ffmpeg-win64', 'COPYING.LGPLv2.1'), 'FFMPEG-COPYING.LGPLv2.1.txt'))
        else:
            ship, ffmpeg = linux_libraries(exe)
            files += ship
            own_ffmpeg = cache_value(a.build, 'JOAN_FFMPEG_ROOT')
            if own_ffmpeg:
                files += [(os.path.join(own_ffmpeg, 'lib', so), so) for so in ffmpeg]
                files.append((os.path.join(ROOT, 'third_party', 'ffmpeg-win64', 'COPYING.LGPLv2.1'), 'FFMPEG-COPYING.LGPLv2.1.txt'))
                ffmpeg = []
            with open(os.path.join(d, 'REQUIREMENTS.txt'), 'w') as f:
                f.write('OpenGL 4.6%s\n' % (' and these FFmpeg libraries from your distribution:' if ffmpeg else ''))
                f.writelines('  %s\n' % so for so in ffmpeg)
        with open(os.path.join(d, 'SOURCE.txt'), 'w') as f:
            f.write('OpenJoan %s\nSource code: %s\nFFmpeg 7.1.1 source code: %s\n' % (a.version, REPOSITORY, FFMPEG_SOURCE))
        for path, base in files:
            shutil.copyfile(path, os.path.join(d, base))
            shutil.copymode(path, os.path.join(d, base))
        strip_executable(os.path.join(d, os.path.basename(exe)), windows)
        if not windows:
            shipped = [os.path.basename(exe)] + [base for _, base in files if '.so.' in base]
            for base in shipped[1:]:
                strip_executable(os.path.join(d, base), False)
            newest = (0,)
            for base in shipped:
                set_runpath(os.path.join(d, base))
                newest = max(newest, glibc_needed(os.path.join(d, base)))
            print('needs glibc %s or newer' % '.'.join(map(str, newest)))
            if a.max_glibc and newest > tuple(int(n) for n in a.max_glibc.split('.')):
                sys.exit('the build needs glibc %s, newer than --max-glibc %s' % ('.'.join(map(str, newest)), a.max_glibc))
        os.makedirs(a.out, exist_ok=True)
        if windows:
            out = os.path.join(a.out, name + '.zip')
            with zipfile.ZipFile(out, 'w', zipfile.ZIP_DEFLATED) as z:
                for f in sorted(os.listdir(d)):
                    z.write(os.path.join(d, f), name + '/' + f)
        else:
            out = os.path.join(a.out, name + '.tar.gz')
            with tarfile.open(out, 'w:gz') as z:
                z.add(d, name)
        print('%s (%d bytes)' % (out, os.path.getsize(out)))
        for f in sorted(os.listdir(d)):
            print('  ' + f)
        if a.appimage and not windows:
            image = make_appimage(d, os.path.basename(exe), a.version, a.out, t)
            print('%s (%d bytes)' % (image, os.path.getsize(image)))


if __name__ == '__main__':
    main()
