# resdb family — `ilib` File / FileMem / Db / ResDb / ResX / ResLX / GameSet (joan.exe)

Exported pseudocode: `$JOAN_WORK_DIR/decomp/binfmt/resdb/` (call tree in its `INDEX.md`). Specs: [res.md](../../formats/res.md),
[gameset.md](../../formats/gameset.md), [dbf.md](../../formats/dbf.md), [i_cursor.md](../../formats/i_cursor.md),
[pal_std.md](../../formats/pal_std.md). Structs: [File](../structs/File.md), [FileMem](../structs/FileMem.md), [Db](../structs/Db.md),
[DbField](../structs/DbField.md), [ResDb](../structs/ResDb.md), [ResX](../structs/ResX.md), [ResLX](../structs/ResLX.md),
[GameSet](../structs/GameSet.md).

Status: **verified** = decompiled and every field accounted for; **read** = read, role clear; **inferred** = from callers/shape.
All classes are MSVC6 `__thiscall` (ECX = this) unless noted. Ghidra often hides `this` — the "args" below list the *visible*
parameters after `this`. Names are proposals (`<module>_<verb>_<noun>`); source files are `d:\ww\ilib\{gameset,resdb,resx,reslx,db,filemem,filetxt}.cpp`.
The base class `File` has no assert string in the export; `ilib\file.cpp` is the presumed source (inferred).

## File (class `File`, 0x10E bytes) — the one place where Win32 file I/O happens

Layout in [File.md](../structs/File.md). Every `.res`, `.set`, `.dbf`, `.fin`, `.sce`... reader sits on these primitives.

| address | proposed name | role | status |
|---|---|---|---|
| 0x4179C0 | `File::File()` (`__fastcall`) | sets `handle = INVALID_HANDLE_VALUE (-1)` at +0x105; returns this | verified |
| 0x586DA0 | `File::open(const char* path, bool report_errors, bool packed)` | refuse (fatal `File : file name is too long.`) if `strlen(path) > 0x104`; close any open handle; copy `path` to +0 (the *unprefixed* path), store flags at +0x109 / +0x10D; `CreateFileA(root_dir + path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL)`; on failure calls the fatal-error sink 0x586CB0 (`Error opening file %s.`) so it never returns 0 in practice. `root_dir` is the string pointed to by the global at 0xA1579C (install directory) | verified |
| 0x586F00 | `File::create(path, report_errors, packed)` | `CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NORMAL)` (**no** root prefix); fatal `Error creating file %s` on failure; returns 1 | verified |
| 0x587260 | `File::close` (`__fastcall`) | `CloseHandle` if handle != -1, then handle = -1 | verified |
| 0x5872B0 | `File::~File` | calls `close` | verified |
| 0x5873A0 | `File::read(void* buf, uint32 n)` | `ReadFile(handle, buf, n)`. **Packed mode** (flag +0x10D != 0): first reads a u16 `len` from the file; if `len != 0` only `min(n, len)` bytes are read and the remaining `len - read` bytes are skipped with a relative seek (so records stored with a u16 length prefix can be read into a smaller/larger buffer). Always zero-fills `buf[read..n)`. If `ReadFile` fails and `report_errors` is set: message box `Error reading file %s, Retry ?` (0x5A8BE0); Yes seeks back to the start position and retries. Returns the `ReadFile` BOOL. Never used with packed=1 by any reader in this export | verified |
| 0x587C20, 0x587D20 | `File::read_u16` (two identical copies) | `ReadFile` 2 bytes, returns 0 on failure (+ fatal `Error reading file %s` when `report_errors`) | verified |
| 0x587E20 | `File::read_u32` | same for 4 bytes | verified |
| 0x587EA0 | `File::seek(int32 off, int whence)` | `SetFilePointer(handle, off, NULL, whence)`; **whence == -1 means 0 (FILE_BEGIN)**, 1 = current, 2 = end | verified |
| 0x587EF0 | `File::tell` | `SetFilePointer(handle, 0, NULL, FILE_CURRENT)` (the return value is the position) | verified |
| 0x587F30 | `File::size` | `GetFileSize(handle, NULL)` (low 32 bits only) | verified |
| 0x5872E0 | `File::write(buf, n)` | in packed mode first writes a u16 length prefix (`n` if `n < 0x10000`, else 0 = "unknown"); then `WriteFile`; fatal `Error writing file %s` on failure. Used only by the DBF writer 0x581DF0 | verified |
| 0x587CA0 | `File::write_u16(v)` (`__fastcall`, value on stack) | `WriteFile` 2 bytes | verified |
| 0x586CB0 | `fatal_error(handlers*, fmt, ...)` | re-entrancy guard (global 0xA64528); calls handler[0], `vsprintf` into a 100-byte buffer, handler[1], `OutputDebugStringA`, `MessageBoxA(title "Wars & Warriors: Joan of Arc", MB_ICONHAND)`, handler[2], then `exit(-2)` (0x7B47FC). All file errors with `report_errors` end here. Handler table at 0xA47D78 | read |
| 0x5A8BE0 | `retry_box(text)` | `MessageBox`-style Retry/Cancel dialog; nonzero = retry | read |

## Allocator (referenced everywhere; one module for all of `ilib`)

| address | proposed name | role | status |
|---|---|---|---|
| 0x58E7B0 | `mem_alloc(size, file, line)` (`__thiscall`, this = allocator list) | `operator new(size + 0x11)`; 16-byte tracking header (next/prev links, size at `ptr-8`, file/line); user block is **filled with 0xEE**; links into a doubly linked list of live blocks; returns `ptr + 16`. Out of memory -> 0x586AF0 (fatal) | read |
| 0x58EB30 | `mem_realloc(ptr, size, file, line)` | `size == 0` frees; `ptr == 0` allocs; unchanged size returns `ptr`; else allocates a new block, copies `min(old,new)` bytes, relinks, fills the old block with 0xFF and frees | read |
| 0x58ED70 | `mem_free(ptr, file, line)` | unlink from the list, release | read |

The `file`/`line` operands are the `d:\ww\...cpp` strings used to attribute every call site (see MODULES.md).
`DAT_00A500A4` is a global scratch buffer; "shared buffer" mode in ResDb/ResX/ResLX points the object's buffer at it instead of allocating.

## ilib\filemem.cpp — `FileMem`: a File-or-memory byte source

Layout in [FileMem.md](../structs/FileMem.md). A `FileMem` is either (a) an owned memory image of a file/range or (b) a thin forwarding wrapper
over a parent `File` or parent `FileMem` (when `+0x108 == 0` every method forwards).

| address | proposed name | role | status |
|---|---|---|---|
| 0x588000 | `FileMem::load_file(path, report_errors, packed)` | `release()`; copy `path` to +0; open a temporary `File(path, report, 0)`; `size = GetFileSize`; allocate; read the whole file; close; cursor = base; owns = 1; returns File::open's result | verified |
| 0x588280 | `FileMem::from_file(File* src, int32 len)` | `release()`; copy name + flags from `src`; `len == -1` or too large -> rest of file from the current position; allocate `len` bytes and read them through `src` (packed flag temporarily cleared); remember `src` in +0x114; seek `src` back to where it was. With `src == NULL` size = 0 | verified |
| 0x588430 | `FileMem::from_mem(FileMem* src, int32 len)` | same for a parent `FileMem` (parent in +0x118) | verified |
| 0x5886D0 | `FileMem::release` (`__fastcall`) | if a parent exists and a cursor is set, seek the parent to the cursor offset (so the parent continues where the child stopped); clear parents/cursor; free the buffer if owned | verified |
| 0x588830 | `FileMem::size` | memory size, or parent's size when forwarding | verified |
| 0x5888C0 | `FileMem::seek(off, whence)` | whence 0 / 1 / 2 (-1 -> 0) on `base`, `cursor`, `base+size`; returns the new offset; forwards when no buffer | verified |
| 0x5889F0 | `FileMem::tell` | `cursor - base` | verified |
| 0x588A80 | `FileMem::read(buf, n)` | exact twin of `File::read` (packed prefix, zero fill, retry loop) on top of 0x589490 | verified |
| 0x5892F0 | `FileMem::read_u16` | 2 bytes via 0x589490 (0 on short read) | verified |
| 0x589490 | `FileMem::raw_read(buf, n, &got)` | clamps `n` to the remaining bytes, `memcpy`, advances the cursor, `got = n` (or 0); always returns TRUE | verified |

## ilib\filetxt.cpp — `FileTxt` (text tokenizer over a memory range; only constructor is in this export)

| address | proposed name | role | status |
|---|---|---|---|
| 0x589780 | `FileTxt::init(const void* data, uint32 len)` | `File()` base ctor; store `len` at +0x196; allocate `len+1`, copy `data`, put **0x1A (Ctrl-Z) as terminator** at `[len]`; base +0x112 = cursor +0x10E = buffer. sizeof(FileTxt) = 0x19A (animscr allocates 0x19A) | verified |
| 0x589890 | `FileTxt::~FileTxt` | free the buffer | read |
| 0x589C70 | `FileTxt::next_token(flag)` | tokeniser (keys like `StartTime`, `Range` are matched by the caller with a case-insensitive compare 0x7C07B0) | read (other family: text_formats) |

## ilib\db.cpp — `Db`: dBase III table reader (format: [dbf.md](../../formats/dbf.md); layout: [Db.md](../structs/Db.md))

| address | proposed name | role | status |
|---|---|---|---|
| 0x580730 | `Db::Db(path, load_all_records, load_fields)` (`__thiscall`) | `File()`; zero +0x13A, +0x12E, +0x13E, +0x13F, +0x143; `cached_record = -1` (+0x136); if `path != NULL` -> `open` | verified |
| 0x580880 | `Db::open(path, load_all_records, load_fields)` | `release()`; `File::open(path, 1, 0)`; read the 32-byte header into +0x10E; if `load_fields`: read `header_len - 32` bytes (descriptors + 0x0D terminator + 1) into a new array (+0x13F) and count descriptors until a descriptor whose first byte is 0x0D (+0x143); else both = 0. If `load_all_records == 0`: allocate a `record_len` buffer (+0x12E) and keep the file open (rows are read on demand); else allocate `n_records * record_len`, seek to `header_len + 1`, read all, **close the file**, set owns flag (+0x13E). Finally current record = 1 | verified |
| 0x580AE0 | `Db::open_memory(const void* image, bool load_fields)` | same header/descriptor parse from a RAM image (`image` = start of a complete dBase file): copies 32 + (`header_len`-32) bytes; **records pointer = image + header_len + 1** (not owned); current record = 1. Used for every table stored inside a `.set`/`Resource` section | verified |
| 0x580C70 | `Db::get_record(int n)` | `n < 1` -> current record (+0x132); out of range (`n > n_records`) -> NULL. Memory mode: `records + record_len * (n-1)`. File mode: if `n == cached` return buffer, else `seek(header_len + 1 + record_len*(n-1))`, read `record_len` bytes into +0x12E. **Returns a pointer to the first field byte (the 0x20/0x2A delete flag byte is skipped and never examined)** | verified |
| 0x415280 | `Db::record_count` (`__fastcall`) | returns the u32 at +0x112 | verified |
| 0x580DA0 | `Db::release` (`__fastcall`) | free record buffer (+0x12E), field array (+0x13F), owned in-memory records (+0x13A when +0x13E) | verified |
| 0x580810 | `Db::~Db` | `release()` then `File::close` | verified |
| 0x581DF0 | `Db::write_with_added_field(FieldDesc desc /*32 bytes by value*/, uint32 width, File dest)` | **editor/tool helper, not used at run time**: rewrites the table into `dest` with one extra column: header with `header_len = (n_fields+1)*0x20 + 0x21`, `record_len += width`, old descriptors, the new 32-byte descriptor, 0x0D, then each record as `0x20` flag + old data + `width` spaces, finally 0x1A. For type `'N'` the width is `width & 0xFF`, otherwise `width & 0xFFFF` (so `C` fields carry a 16-bit length across descriptor bytes 16..17). Error `Cannot write DataBase` if descriptors/records are not loaded | read |
| 0x590210 | `copy_trim_field(dst, src, n)` | rtrim-length (0x58F390: counts trailing spaces) then `memcpy` + NUL | verified |
| 0x590270 | `atoi_field(src, n)` (`__thiscall`) | copy `min(n,0x78)` bytes into the object's scratch, NUL, `atoi` (0x7B15CB) | verified |
| 0x590170 | `copy_rtrim(dst, src, n)` | copy `min(n,0x78)` bytes, strip trailing spaces in place | verified |
| 0x58F390 | `len_without_trailing_spaces(s, off, n)` | helper of 0x590210 (`n == -1` -> strlen) | read |

## ilib\resdb.cpp — `ResDb`: single-blob file addressed by external offsets (layout: [ResDb.md](../structs/ResDb.md))

| address | proposed name | role | status |
|---|---|---|---|
| 0x59B6D0 | `ResDb::open(path, load_all, shared_buf)` | `close()`; zero +0x10F, +0x113; +0x123 = `load_all`; `File::open(path,1,0)`. If `!load_all`: +0x117 = `shared_buf`, buffer (+0x11B) = shared ? scratch 0xA500A4 : NULL. If `load_all`: size = file size, allocate, read the entire file, close the handle, +0x117 = 0. +0x10E = 1 | verified |
| 0x59B440 | `ResDb::close` (`__fastcall`) | if open: free the buffer unless it is the shared scratch, close the handle unless load-all, clear flag | verified |
| 0x59B830 | `ResDb::get_blob(uint32 offset)` | blob layout = `u32 size` then `size` bytes. Streaming: `seek(offset)`, `size = read_u32`, (re)allocate the buffer unless shared, `read(buf, size)`, return buf. Load-all: returns `image + offset + 4`. The returned pointer is the byte after the size field | verified |
| 0x59B4F0 | `ResDb::get_blob_for_record(int rec)` | looks up a row (0x580C70), takes the 4 bytes at `row + this[+0x113]`; if they equal the 4-byte pattern at 0x9D8E78 returns 0 (no blob), else treats them as a u32 offset and does the streaming `get_blob` sequence. **Suspicious**: it calls the Db row accessor with the ResDb `this`; the real caller probably passes a combined Db+ResDb object (callers not in the export) | inferred |

## ilib\resx.cpp — `ResX`: section container, **u16 count, 9-byte names** (layout: [ResX.md](../structs/ResX.md); format: [gameset.md](../../formats/gameset.md))

| address | proposed name | role | status |
|---|---|---|---|
| 0x59C550 | `ResX::open(path, load_all, shared_buf)` | close any previous; `File::open(path,1,0)`; flags +0x11F/+0x120; clear buffer/cache (+0x112, +0x116, +0x125 = -1, +0x129, +0x12D, +0x131 = 0); `count = read_u16` (sign-extended) into +0x121; allocate `(count+1)*13` bytes, read the whole index (including the sentinel); build the sorted copy (0x59D050). If `load_all`: size = `off[count] - off[0]`, allocate, read, close the file; else (`shared_buf` -> buffer = scratch 0xA500A4). Sets open flag +0x11E | verified |
| 0x59C780 | `ResX::close` (`__fastcall`) | free sorted copy, index, buffer (unless shared scratch), close file unless load-all; clear flag | verified |
| 0x59D050 | `ResX::build_sorted_index` | copy the `count` real entries, **overwrite each entry's offset field with its original index (0-based)**, `qsort(count, stride 13, FUN_0059D160)` (comparator not exported; presumably `strcmp` on the 9-byte name) | verified (comparator inferred) |
| 0x59CA60 | `ResX::find(const char* name)` | binary search of the sorted copy with `strcmp`; returns `original_index + 1` (1-based) or 0; exact, case-sensitive | verified |
| 0x59CBB0 | `ResX::get_section(int idx)` (1-based) | **Load-all**: `buffer + off[idx-1] - off[0]`. **Streaming**: size = `off[idx] - off[idx-1]`; if not shared and not streaming-buffered and `idx-1 == cached` return the cache; else `seek(off[idx-1])`, grow the cache buffer (+0x112, capacity +0x116) when needed, `read`, remember `idx-1`; with shared scratch: no caching. Optional "stream buffer" mode (+0x129 non-NULL, capacity +0x12D, skip +0x131): skips `+0x131` leading bytes and fails (returns 0) when the section is larger than the capacity | verified |
| 0x59C950 | `ResX::get_section_by_name(name)` | `find` then `get_section`; 0 if absent | verified |

## ilib\reslx.cpp — `ResLX`: same container, **u32 count, 34-byte names** (layout: [ResLX.md](../structs/ResLX.md); format: [res.md](../../formats/res.md))

| address | proposed name | role | status |
|---|---|---|---|
| 0x59B920 | `ResLX::open(path, load_all, shared_buf)` | identical to `ResX::open` except `count = read_u32` and entry stride 0x26 (38), offset at entry+0x22 | verified |
| 0x59BB40 | `ResLX::close` | as ResX | verified |
| 0x59C440 | `ResLX::build_sorted_index` | as ResX, stride 0x26, offset field at +0x22 replaced by original index, comparator at 0x4070FE (thunk, not exported) | verified (comparator inferred) |
| 0x59BF70 | `ResLX::get_payload(int idx)` (1-based) | as `ResX::get_section` with stride 0x26 | verified |
| 0x59BDD0 | `ResLX::payload_size(int idx)` | `off[idx] - off[idx-1]` | verified |
| 0x59C3B0 | `ResLX::entry(int idx)` | `idx` in `[1,count]` -> pointer to the 38-byte entry (the 34-byte name is at its start); else 0 | verified |
| 0x4179F0 | `ResLX::ResLX()` | base `File` ctor + flag init (object is 0x135 bytes: animscr allocates `new(0x135)`) | read |
| 0x417A30 | `ResLX::~ResLX` | `close()`, `File::~File` | read |

No ResLX name-lookup routine is in the export (the sorted copy exists but its binary-search caller is elsewhere).
TODO(re): find the ResLX `find(name)` and its comparator (0x4070FE) — needed to know whether lookups are case-insensitive.

## ilib\gameset.cpp — `GameSet`: the list of installed `Resource\*.SET` files + the currently open set (layout: [GameSet.md](../structs/GameSet.md))

| address | proposed name | role | status |
|---|---|---|---|
| 0x58AE90 | `GameSet::scan()` | build the file list `RESOURCE\*.SET` (FindFirstFile, **sorted**, 0x582B00); `set_count` (+3) = list size; allocate `set_count * 0x46`; for each file `i`: open `RESOURCE\<file>` as ResX, take table `HEADER` (0x59C950 -> `Db::open_memory`), read record 1 and copy field 1 (8 chars, rtrim) to `sets[i].name[9]` and field 2 (0x3C chars, rtrim) to `sets[i].title[0x3D]` | verified |
| 0x58AE30 | `GameSet::free` (`__fastcall`) | if initialised: close the open set (0x58B140), free `sets`, clear the flag | verified |
| 0x58B140 | `GameSet::close_set` | `ResX::close`; clear the "set open" byte +0x285 | verified |
| 0x58B080 | `GameSet::select(idx)` | current index (+1, u16) = idx; path = `RESOURCE\` + `sets[idx-1].name` + literal at 0x9D8AC8 (presumably `.SET`); `ResX::open(path, 0, 0)`; +0x285 = 1 (from the `fin` export, same bytes in `binfmt_loaders`) | read |
| 0x58B180 | `GameSet::open_table(name)` (`__thiscall`) | `get_section_by_name(name)`; absent and `name != "RELEASE"` -> fatal `Error Loading %s`; `Db::open_memory(ptr, 0)` into the embedded Db at +0x13E; returns that Db. Used as `open_table("SDATE")`, `("ANIMATE")`, `("MODEL")`... | verified |
| 0x58B250 | `GameSet::find(name)` | linear search of the 0x46-byte names, 1-based (from fin.md) | read (other family) |

## Table readers that sit on top (all fixed-offset: DBF rows are never looked up by field name)

| address | proposed name | role | status |
|---|---|---|---|
| 0x4CE0F0 | `SDateRes::load` (sdateres.cpp) | `open_table("SDATE")`; `count = record_count` (u16 at +4); array of `count * 0x33`; per row: name 0x20 chars (rtrim) -> +0, int(4 chars @row+0x20) -> +0x21, int(2 @+0x24) -> +0x25, int(2 @+0x26) -> +0x29, int(1 @+0x28) -> +0x2D, `u16 recno = i+1` at +0x31. Fields DATENAME/YEAR/MONTH/DAY/WEEKDAY | verified |
| 0x4CA480 | `RaceRes::load` (raceres.cpp) | table whose name literal is at 0x9D5718 (`RACE`); `count` (i32 at +4), array `count * 0x34`; per row: `u8 recno` at +0, 8 chars @row+0 -> +1, 0x14 chars @row+8 -> +0x0A, 0x14 chars @row+0x1C -> +0x1F (CODE/DES/STD) | verified |
| 0x4172A0 | `AnimRes::load` (animres.cpp) | open `RESOURCE\ANIM_AM2.RES` and `RESOURCE\ANIM_AM3.RES` as ResLX (both **0 bytes in the retail install**: count reads as 0); `open_table("ANIMATE")`; array `count * 0x3A`; per row: `u16 recno` at +0, 0x20 chars @row+0 (CODE) -> +2, 8 chars @row+0x20 (DBFNAME) -> +0x23; then for each row rtrim DBFNAME and register an animation group (0x414970 -> animgrp.cpp 0x414A30 opens the section of that name, e.g. `GENERAL1`, `AMAZON1`) | verified |
| 0x418F30 | `AnimScr::load` (animscr.cpp) | open `RESOURCE\animscr.res` as ResLX (streaming); `count` from +0x121; array `count * 0x21B`; for each payload `i`: size = 0x59BDD0, data = 0x59BF70, wrap in a `FileTxt` (0x589780), parse by 0x419320 into the 0x21B record (`u16 recno`, name 0x20 chars from the first key, then `StartTime`/`EndTime`/`Range`/`StartForceTime`/`StartVerticalAngle`/`EndVerticalAngle`/`StartHorizontalAngle`/`EndHorizontalAngle`/`GravityLevel`/`AttackForce`/`VerticalAttackForce`/`EmitterFlag`/`AttachModel`/`SoundScriptName` ... defaults at +0x48 = 3.0f, +0x4C = 3000, +0x50 = 500). Format of those payloads belongs to the animscr family; TODO(re) | read |
| 0x5462B0 | `UnitAiRes::load` (uscrres.cpp) | open `SCRIPT\UNIT\unitAI.res` as ResLX (streaming); `count` from +0x121; array `count * 0x28`; record: `u16 recno` (1-based) at +0, `strncpy(name, entry name, 0x20)` at +2, `u16 ai_class` at +0x26 = 0x546520(name). The payloads are *not* read here | verified |
| 0x546520 | `unit_ai_class_from_name(name)` | case-insensitive (0x7C07B0) name -> class id: `human_light_infantry`=1, `human_light_general`=2, `special_general`=3, `human_heavy_infantry`=4, `human_heavy_general`=5, `human_archer`/`human_archer_ex`=6, `archer_gen`/`archer_ex_gen`=7, `human_cannon`=8, `autobow`=9, `batterram`=10, `camp_horse`=11, literal@0x9D7E94=12 (presumably `human_horse`), `citizen`=13, `human_trebuchet`=14, literal@0x9D7E70=15 (presumably `cart`), anything else (e.g. `PIG`) = 0 | verified (two literals TODO(re)) |
| 0x4915F0 | `MouseCursors::load` (mousecr.cpp) | `Db(RESOURCE\CURSOR.RES, load_all_records=1, load_fields=0)`; `count` (i16) at this+0xD2; array `count * 8` at +0xD4; per row `i`: 4 bytes @row+0x0E -> offset -> `ResDb::get_blob(offset)` -> pointer at +4 of the 8-byte entry; `int(3 chars @row+8)` -> i16 `hotspot_x`, `int(3 @row+0xB)` -> i16 `hotspot_y`. Row layout: HFILENAME C(8) @0, HHOTSPOT_X N(3) @8, HHOTSPOT_Y N(3) @0xB, HBITMAPPTR C(4) @0xE (4 raw bytes: **binary u32 offset into I_cursor.res**) | verified |
| 0x4941B0 | `MouseCursors::shutdown` | restore `ShowCursor` counter (+0x200), free bitmaps (+0x40, +0x1FC palette buffer), free the entry array, `ResDb::close`, per-slot cleanup of `0x16`-byte slots | read |
| 0x493EB0 | `MouseCursors::init(n_slots)` (from `binfmt_loaders`) | `ResDb::open(RESOURCE\I_CURSOR.RES, 1, 0)`; `File::open(RESOURCE\PAL_STD.RES)`, `seek(8)`, read 0x300 bytes into a 256x3 table, build a palette object (`vgabuf` 0x612600); `MouseCursors::load`; create a 32x32 cursor surface | read |

## Cross-reference of consumers (not in this export)

`item/*.res`, `unit/unit.res`, `unit/recruit.res`, `map/scenarioN/scenarioN.res`, `Resource/texteff.res`, `Resource/Wave.res`,
`Resource/A_wave?.res`, `Resource/S?L1_*.RES` are ResLX files by structure (visual check of name column + sentinel + payload), but their
openers are in other modules (itemres, unitres, world loaders, sound). They must call 0x59B920 or an inlined copy; TODO(re): confirm by xref on 0x59B920.
