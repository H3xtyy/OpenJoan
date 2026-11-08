# Functions: `dds` family (texture loading, texture cache, texture directory index, JPEG wrapper)

Reference build joan.exe. Sources: `d:\ww\en3d\texturea.cpp`, `d:\ww\en3d\texdir.cpp`, `d:\ww\elib\jpeg.cpp`.
Pseudocode: $JOAN_WORK_DIR/decomp/binfmt/dds/. Format spec: docs/formats/dds.md; structs: docs/re/structs/Texture.md.
Status: verified / read / inferred.

## DDS header probe and search
| address | proposed name | role | status |
|---|---|---|---|
| 0x60B450 | dds_probe_has_alpha | `(path)->0/1`; last 3 chars = ext (stricmp vs two consts: tga? / dds); for dds reads 0x7C header bytes after the magic and returns "has alpha" from FourCC / 5 constant RGB masks; no magic or size validation | read |
| 0x609780 | dds_find_file_recursive | `(name, startdir, outpath)`: tries `dir\name.ext` for each ext in NULL-terminated table `PTR_DAT_00A18948`, `FindFirstFile` existence, then recurses into sub-dirs depth-first | read |
| 0x590B80 | file_exists | `FindFirstFile` success test | verified |
| 0x590C00 | dir_exists | strips trailing `\`, `FindFirstFile`, attribute 0x10 test | verified |
| 0x586DA0 | file_open | File-class open; errors "Error opening file %s." / "File : file name is too long." | read |
| 0x5873A0 | file_read | File-class read with "Error reading file %s, Retry ?" dialog | read |
| 0x587EA0 | file_seek | skip (used with `4`) | inferred |
| 0x587260 / 0x5872B0 | file_close / file_close_thunk | | inferred |
| 0x5A1440 / 0x5A1550 | path_copy / path_cat | `strncpy`/`strncat` into 400-char (+NUL at [400]) path buffers | verified |
| 0x5A1170 | str_clear | `buf[0]=0` | verified |

## Path builder / start-up of the texture index
| address | proposed name | role | status |
|---|---|---|---|
| 0x608E10 | texdir_init_search_paths | after `texturea_reset`: sets texture-manager `+0x30=0,+0x34=1,+0x38=1`; registers 3 extensions (masks 1,2,4); creates group named `DAT_009D9BF0`; adds `INT8X6\` and `INT8X6\ITEM\`; `SAVE\` (never scanned); then per texture-quality order (`DAT_00A5804A`) `TEXTUREL\`/`TEXTURE\`/`TEXTUREH\` each with sub-dirs `ITEM\ UNIT\ TERRAIN\ PARTICLE\ MISC\` then itself recursive; finally calls 0x604850 (scan all) | read |
| 0x604560 | texdir_add_directory | `(this, path, recurse, group_name, noscan_flag)`; strips trailing `\` (when len>1), resolves group by name (`0x6054C0`, default group 0) and calls 0x604650 | read |
| 0x604650 | texdir_group_add_dir | find-or-append directory record (stride 0x10C; case-insens compare); sets recurse byte +0x105, clears scan states +0x106/+0x107, noscan flag +0x108; if it already exists ORs the recurse flag; returns index | read |
| 0x604440 | texdir_add_group | find-or-append group (stride 0x3D) by name via 0x6054C0 | read |
| 0x6054C0 | texdir_find_group | linear search of group names (+4) with stricmp; -1 if missing | read |
| 0x604310 | texdir_register_extension | adds `{char name[6]; u16 mask}` (8 B) to the extension table if not present; mask = `1 << index` | read |
| 0x604850 | texdir_rescan_all | resets scan states of all dirs, zeroes group entry count (+0x25), then calls 0x604960 for every dir of every group | read |
| 0x604960 | texdir_scan_directory | lazy scan of dir `idx` in group: (1) if not noscan, enumerate `*.*`; for files: `_splitpath`, ext -> mask via table, base name must be <= 32 chars; counts matches, grows entry array (cap +4, stride 0x28); inserts entries into a sorted array (0x5911A0 bsearch-with-insert-pos, 0x5910F0 array insert) keyed by name; same name & same dir -> OR masks, different dir -> ignored (first wins); sets state +0x106 = 2; (2) if recurse flag: for each sub-directory (not `.`/`..`) registers it as a new dir via 0x604650 and scans it recursively; state +0x107 = 2 | read |
| 0x604150 | texdir_destroy | frees ext table, per-group entry arrays (+0x2D) and dir arrays (+0x39), the group array | read |

## Texture manager / cache (`en3d/texturea.cpp`)
| address | proposed name | role | status |
|---|---|---|---|
| 0x609260 | texturea_reset | flush cache (0x60A870(0)), clears flag +0x34, releases all textures in three pointer arrays (+0x00/+0x0C/+0x24 groups) and frees them, frees scratch buffer +0x20 | read |
| 0x60A550 | texture_release | refcount-- with cache parking: if refcount==1 and manager caching flag (+0x34) set -> re-ref (0x60A500, stamp = manager+0x38), insert into cache list (0x60B1B0); on refcount<1 remove from list by kind byte (+0x18: 0 -> 0x60B140, else 0x60B0D0, plus 0x60B3E0) and call vtable0 [1] (destructor) | read |
| 0x60A500 | texture_addref | refcount++ and stamp := manager+0x38; returns new count | verified |
| 0x60A870 | texturea_purge_cache | `(age)`: walks cache list (+0x18 count, +0x20 ptr); `age==0` or `age < tex.stamp` -> release; else tex.stamp++ and keep; compacts array; temporarily clears +0x34 | read |
| 0x60AC20 | texturea_register_a | sorted insert (cap +0x40, element 4 B) into list at manager +0x00/+0x04/+0x08 using comparator 0x60AD10; returns index | read |
| 0x60AD40 | texturea_register_b | same pattern, second list (+0x0C) | read |
| 0x60B1B0 | texturea_cache_insert | sorted insert into cache list (+0x18/+0x1C/+0x20, comparator 0x60B2B0), returns ptr or 0 if already present | read |
| 0x60B2E0 | texturea_register_c | same pattern, third list (+0x24) | read |
| 0x60B0D0 / 0x60B140 / 0x60B3E0 | texturea_list_remove_{a,b,c} | find (0x60AEC0 / 0x60B000 / 0x60B050) + delete from sorted list, count-- | read |
| 0x591070 / 0x5910F0 / 0x5911A0 | array_remove / array_insert / array_bsearch | generic stride-N sorted-array helpers (`bsearch` with insertion point out-param) | inferred |

## JPEG (`elib/jpeg.cpp`)
| address | proposed name | role | status |
|---|---|---|---|
| 0x5A8D80 | jpeg_decoder_ctor | allocs 0x15B-byte cinfo + 0x84-byte manager, installs 3 callbacks (0x5A8EA0 / 0x5A8F70 / 0x5A8F10), `jpeg_CreateDecompress(cinfo, 0x3E, 0x15B)` | read |
| 0x5A8FA0 | jpeg_decoder_dtor | `jpeg_destroy*` (0x788919 or 0x7888F7 depending on mode +8) and frees both blocks | read |
| 0x7A9A60 / 0x765F43 | jpeg_CreateDecompress / jpeg_std_error | libjpeg internals | inferred |

## Open questions
* TODO(re): 0x9D9AD8 / 0x9D9ADC / 0x9D9C00 / 0x9D9BF8 / 0x9D353C / 0x9D9BF0 strings; `PTR_DAT_00A18948` extension list;
  the five pixel-format tuples at 0x9D9AE0..0x9D9B7F (need a memory read; no shell was available).
* TODO(re): the name-lookup function that bsearches the texdir entry arrays and builds the final `dir\name.ext`
  path (callers of 0x5911A0 on group+0x2D) and the call site that creates the D3D texture (D3DX call).
* TODO(re): meaning of `DAT_00A48858` (loop 2..0 in 0x608E10) and `DAT_00A5804A` (texture quality, assumed 0..2).
