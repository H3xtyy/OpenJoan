#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Pack a release-flavour build into dist/openjoan-<version>-<platform>.tar.gz or .zip (docs/RELEASING.md).

  tools/package_release.py <build dir> <version> [--out dist]

Only named files go in: the executable, its settings file, the runtime libraries it is shipped with, the licences and
the install page. Nothing else from the build directory can reach an archive, and no game file is ever in one.

On Windows the DLLs of WINDOWS_DLLS are shipped (SDL3, OpenAL Soft, FFmpeg); an llvm-mingw build also needs the C++
runtime DLLs of its toolchain. On Linux SDL3 and OpenAL Soft are
shipped next to the executable (RUNPATH $ORIGIN); FFmpeg stays the system's, because its libraries pull in the
distribution's codec packages, so the archive's REQUIREMENTS.txt names the FFmpeg libraries this build was linked with.
"""
import argparse
import glob
import os
import re
import shutil
import subprocess
import sys
import tarfile
import tempfile
import zipfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUNDLED_LINUX = ('libSDL3.so', 'libopenal.so')
WINDOWS_DLLS = re.compile(r'^(SDL3|soft_oal|OpenAL32|(avformat|avcodec|avutil|swscale|swresample)-\d+)\.dll$', re.I)
MINGW_RUNTIME = ('libc++.dll', 'libunwind.dll')


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
    ship = []
    for so in needed:
        if so.startswith(BUNDLED_LINUX):
            if so not in where or not os.path.exists(where[so]):
                sys.exit('%s not found by ldd' % so)
            ship.append((where[so], so))
    return ship, [so for so in needed if so.startswith(('libav', 'libsw'))]


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
            with open(os.path.join(d, 'REQUIREMENTS.txt'), 'w') as f:
                f.write('OpenGL 4.6 and these FFmpeg libraries from your distribution:\n')
                f.writelines('  %s\n' % so for so in ffmpeg)
        for path, base in files:
            shutil.copyfile(path, os.path.join(d, base))
            shutil.copymode(path, os.path.join(d, base))
        strip_executable(os.path.join(d, os.path.basename(exe)), windows)
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


if __name__ == '__main__':
    main()
