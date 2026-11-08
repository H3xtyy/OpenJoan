# Game file names and the install folder

Not a file format: how the game's own path spellings meet the user's install. Code: `src/platform/files.cpp`
(`to_path`, `native_path`; module overview in [../../src/platform/README.md](../../src/platform/README.md)). Every
reader of the game opens files through the ported KERNEL32 file calls (`CreateFileA`, `FindFirstFileA`, ...), so path
handling is the same everywhere. The original opens files through the case-insensitive Win32 API with backslash
paths taken from data files (`scenario1\S1L1w`, `Unit\`, `Terrain\`, see [../re/BINARY.md](../re/BINARY.md)); on Linux
the install may have any case.

## Name resolution (Linux, `to_path`)
- `\` becomes `/`; an empty name or a path that exists as written is used unchanged.
- Otherwise each path component that does not exist as written is matched against the directory entries ignoring
  ASCII case (`strcasecmp`), so `SCRIPT\scenario\s1l1.sce` finds `script/scenario/S1L1.sce`.
- From the first component with no match on, the rest is kept as written (a file being created).
- On Windows the name goes to the file system unchanged (system code page).
- `native_path(name)` gives the same result as a string, for code that opens files without the KERNEL32 calls
  (FFmpeg for the movies and music).

## Directory searches
`FindFirstFileA` / `FindNextFileA` follow Windows rules: `*` and `?` wildcards matched case-insensitively, `*.*`
equals `*`, `.` and `..` are listed as NTFS does, and entries come in NTFS order (upper-case ordinal comparison).

## Files in memory
`[Platform] FilesInMemory` (`src/openjoan.ini`) reads the item, particle and item-script folders into memory at
start-up; the game's read-only opens of those files are then served from memory (`start_memory_files`).

## Not done
There is no overlay of several directories: the install folder is the only root, and files added to a folder after
the game opened it are seen by the next search like on any file system.
