# joan.exe and companions — verified facts (triage 2026-10-19, updated 2026-10-22)

> **Reference binary: the Steam release's `joan.exe`** (sha256 `ad1dd30f…`, 6,680,784 bytes, PE timestamp 0x400BA40A).
> It has no pre-entry stub and no CD check, and carries a CodeView record (`D:\ww\Joan_Release\joan.pdb`, VC7).
> **All addresses in `docs/re/functions/` and in code citations refer to this build unless a line says `retail:`.**
> A separate 2004 retail build (sha256 `236ddfa9…`) stays documented below for its stub and protection. Index of the
> retail build: `docs/re/functions/_index_retail.md`.

## Steam build — fixed points
| Steam | retail | symbol | note |
|---|---|---|---|
| 0x7B71E6 | 0x7B5F32 | `WinMainCRTStartup` | MSVC7 / MSVC6 CRT entry; in the Steam build it is the PE entry point |
| 0x420EA0 | 0x56BB30 | `WinMain` | same structure; Steam has **no CD check** (no `GetLogicalDrives`, no `USEUKEY.SYS`, no disc message) |
| 0x447150 | 0x56BEA0 | `v3d_swap_check` | copies the CPU-optimised `V3D_*.DLL` over `V3D_87.DLL`, asks for a restart |
| 0x4D6C50 | 0x4D5B50 | (engine init, called from WinMain with hInstance) | `TODO(re)` |
| 0x45AAF0 | 0x45A830 | (reads `CONFIG.DAT`) | `TODO(re)` settings |

Window title differs: retail `"Joanna d'Arc"`, Steam `"Wars & Warriors: Joan of Arc"` (so some strings are per
build/language in code, not only in data). The `.rdata` layouts differ by 0x78 bytes; `.text` is a different
compilation (VC7 vs VC6), so retail↔Steam address mapping must be re-derived per function (string xrefs).


Full machine-generated detail: `docs/re/BINARY_pe_inventory.md` (from `tools/pe_inventory.py`).
Source: the user's installed copy, `C:\Program Files (x86)\Joanna d'Arc` (Polish release, 2004).

## Files
| file | size | what | notes |
|---|---|---|---|
| `joan.exe` | 6,418,432 | the game | x86-32, MSVC 6.0 linker, PE timestamp 1074504845 (2004-01-19), base 0x400000 |
| `JoanofArc.exe` | 266,279 | launcher | imports only KERNEL32/USER32/WINMM(timeGetTime); probably configures and spawns `joan.exe` (check what it does); file is read-only on disk |
| `VSetting.exe` | 1,495,093 | video settings tool | registry read, ShellExecute; writes the settings `joan.exe` reads via ADVAPI32 |
| `v3d_87.dll`, `v3d_x87.dll`, `v3d_sse.dll`, `v3d_k3d.dll` | ~270 KB each | vector math + skinning, CPU-dispatched (`vertexf_check_cpu`) | exports are MSVC-mangled C++ free functions and `SkinVertex` methods (see inventory) |
| `shader/nvlinker.dll` | 319,567 | NVIDIA NVLink vertex-shader fragment linker (2001) | export `CreateLinker`; consumes `shader/*.nvo` |
| `OpenAL32.dll`, `eax.dll`, `EaxAc3.dll`, `ALut.dll` | | Creative OpenAL 1.0 + EAX | replaced by OpenAL Soft |
| `util/wmfdist.exe` | 4 MB | Windows Media Format 9 runtime redistributable | confirms WMV/WMA via `wmvcore.dll` |
| `Unwise.exe`, `INSTALL.LOG` | | Wise installer | ignore |

## joan.exe sections
| name | VA | vsize | entropy | flags |
|---|---|---|---|---|
| `.text` | 0x401000 | 0x3E5000 | 5.80 | X — plain code; 9,779 printable strings; 109 source paths; import table intact |
| `.rdata` | 0x7E6000 | 0x24C0E | 4.87 | |
| `.data` | 0x80B000 | 0x78D44 | 1.26 | W |
| `.idata` | 0x884000 | 0x2541 | 4.02 | W — normal imports |
| **`.ekvam`** | 0x887000 | 0xC4000 | **7.43** | X, **contains the entry point 0x88EB83** |
| **`.znks`** | 0x94B000 | 0xE1E62 | 0.00 | W — zero-filled (runtime workspace of the stub) |
| **`.idata`** (2nd) | 0xA2D000 | 0x235E | 0.00 | W — second import directory, empty on disk |
| `.rsrc` | 0xA30000 | 0x13D0 | 3.86 | |
| `.reloc` | 0xA32000 | 0x2582C | 6.77 | |

**Verdict (2026-10-22, verified)**: the `.ekvam`/`.znks`/second-`.idata` trio is a thin, obfuscated
**pre-entry stub that does not modify the game image**. Evidence:

- `tools/re/wine_dump_image.py` dumped the running process (stopped inside the game's own `WinMain` at the CD
  prompt, i.e. long after the stub finished). `.text`, `.rdata`, `.ekvam`, `.rsrc`, `.reloc` and the second
  `.idata` are **byte-identical** to the file (0 differing bytes). `.idata` differs only where the loader filled
  the IAT; `.data` differs only in runtime globals; `.znks` differs in 43 bytes (stub state). No IAT slot points
  into the stub sections, so nothing is hooked either.
- Static analysis of the file is therefore sufficient; **no runtime dump or import-table rebuild is needed**.
- Stub entry 0x88EB83: `pusha`, then a call-over-string sequence resolving by name `CreateThread`,
  `GetCurrentThreadId`, `SetEvent`, `ResetEvent`, `OpenEventA`, `CloseHandle`, `RegCreateKeyExA`,
  `RegDeleteKeyA`, `RegCloseKey`, `PostMessageA`, `wsprintfA` (plain code 0x887000–0x88FB00; the rest of
  `.ekvam` is high-entropy data). Its parameter table sits at the start of `.znks` **on disk** (the section is
  otherwise zero): `0x94B000 = 0x88E960` (stub routine), `0x94B004 = 0x7B5F32` (**original entry point**), then
  14 pairs (call-site in `.text`, IAT slot of `GetProcessHeap`/`IsBadCodePtr`/`GetLogicalDrives`), e.g.
  (0x4D5B8A, 0x884950), (0x76BBFA, 0x884948). `TODO(re): 0x88E960` — what the stub does with those call sites
  (watchdog thread / self-check) is not needed for the port and was not pursued.
- Detect It Easy 3.21 (deep scan) recognises no packer/protector; no known signature strings (UPX, SafeDisc
  `BoG_`, SecuROM, StarForce, Themida…) exist in the file.
- **Original entry point (OEP) = 0x7B5F32** `WinMainCRTStartup` (MSVC6 CRT: `GetVersion`, `_ioinit` 0x7BFA96,
  `GetCommandLineA`, `GetStartupInfoA`, `GetModuleHandleA(NULL)` → `WinMain`).
- **`WinMain` = 0x56BB30**. Order: `FUN_0056BEA0()` (TODO(re): single-instance / env check), `vertexf_check_cpu()`
  (fails → MessageBox "V3d_87 invalid"), read `CONFIG.DAT` (0x45A830), then **a CD-presence check that belongs to
  Enlight's own code, not to the stub**: gated by config byte `0x8670CB` (`'\0'` or `' '` → check runs), it calls
  0x585AC0 with the cp1250 message at 0x7EF41C ("Proszę włożyć dysk z grą Joanna d'Arc do napędu CD i kliknąć na
  przycisk OK") and 0x58FA80 with `"USEUKEY.SYS"`; a failed check returns 0 before any window exists. The
  OpenJoan never implements this check.
- The copy-protection question is thus: **the retail exe has a CD check in WinMain** (the owner of the copy reported none —
  wrong for this build) **plus an unidentified pre-entry stub of no consequence for analysis**.

### A second, unprotected build exists
`$JOAN_WORK_DIR/game/` contained a different `joan.exe` before the reference copy was made (now preserved as
`$JOAN_WORK_DIR/dumps/joan.exe.alt-build-ts1074504714-sha3ef3047`, triage in `docs/re/pe/joan.exe.alt-build.md`):

| | retail (`JOAN_GAME_DIR`) | alternate build |
|---|---|---|
| size / sha256 | 6,418,432 / `236ddfa9…` | 6,672,428 / `3ef30477…` |
| PE timestamp | 1074504845 (2004-01-19 10:14:05 UTC) | 1074504714 (131 s earlier) |
| linker / compiler (DIE) | MS linker 6.0 (MSVC 6) | MS linker 6.20.8755, compiler 13.00.8830 (VC7 / VS.NET 2002), CodeView `D:\ww\Joan_Release\joan.pdb` age 15 |
| sections | `.text .rdata .data .idata .ekvam .znks .idata .rsrc .reloc`, EP in `.ekvam` | `.text .rdata .data .idata .rsrc .reloc`, EP 0x7B71E6 in `.text`, 44-byte overlay |
| `.text` | 0x3E5000 | 0x5CD700 (only ~13 % of 256-byte chunks shared — a different compilation, not a dump) |
| extra imports | — | `comdlg32!GetOpenFileNameA`, `MulDiv`, `GlobalMemoryStatus`, `CopyFileA`, `FreeLibrary` |
| CD check | yes (string + `GetLogicalDrives`) | **none** (no string, no `GetLogicalDrives`/`OpenEventA` import) |
| under Wine (DXVK) | stops at the CD dialog | **reaches the main menu in < 20 s** (`traces/shot-alt.png`), loads `wmvcore.dll` for the intro |

Its provenance was unknown when it was found in the work copy (2026-10-10); it was later identified as the Steam release. It is the better *dynamic*
analysis target (runs, no stub); the retail build remains the reference for *static* work until a decision is made.
Both share `.rdata` layout almost exactly (0x24C0E vs 0x24C86 bytes).

### Launching under Wine
- `joan.exe` runs standalone; `JoanofArc.exe` is **not** a required launcher. It is Enlight's `swapdll` tool
  (`d:\swapdll\*.cpp`, `vertexf.h`): calls `vertexf_check_cpu`, copies the matching `V3D_{X87,K3D,SSE}.DLL`
  over `V3D_87.DLL` (backup `V3D_OLD.DLL`) and `WinExec("JOAN.EXE")`. In this install `v3d_87.dll` is already
  byte-identical to `v3d_sse.dll`. `joan.exe` re-validates with `vertexf_check_cpu` itself.
- Wine 11.17 (Staging), WoW64 prefix `$JOAN_WORK_DIR/wine32` with DXVK d3d8: both builds load `d3d8.dll` (DXVK),
  `OpenAL32.dll` (Creative, from the game dir), `v3d_87.dll` + `V3D_SSE.DLL`. Logs in `$JOAN_WORK_DIR/traces/`.
- Files the game writes next to itself: `CONFIG.DAT` (2,821 B) and `Save/Player.dat` (114 B) — reason enough to
  never run it from `JOAN_GAME_DIR`.

### Ghidra
- Persistent project: `$JOAN_WORK_DIR/ghidra/openjoan.gpr` (headless import, x86:LE:32:default, cspec windows), with
  `tools/ghidra/NameByModule.java` applied (module namespaces from the `d:\ww\…` strings, fixed names
  `WinMainCRTStartup`/`WinMain`/`stub_entry`). Function index: `docs/re/functions/_index.md`; name map for
  re-applying in other tools: `$JOAN_WORK_DIR/ghidra/joan_names.json`.
- `rea` (MCP) sessions are ephemeral; re-apply names from the JSON with `set_addresses_names` when needed. rea
  analysis snapshots of the Steam and retail builds (one file each in `$JOAN_WORK_DIR/ghidra/`, named `rea-*.snapshot`)
  (`open_binary(path, snapshot_path=…)` restores it without re-analysis).

## Imports (joan.exe)
- **KERNEL32** (125) — files, threads, events, critical sections, QPC, `LoadLibraryA`/`GetProcAddress` (dynamic loads: `wmvcore.dll`, `SHADER\nvlinker.dll`, `V3D_SSE.DLL` etc., `eax.dll`).
- **USER32** (34) — classic Win32 window + message loop (`RegisterClassA`, `CreateWindowExA`, `PeekMessageA`, `MsgWaitForMultipleObjectsEx`), dialogs (`DialogBoxParamA`), `DrawTextA/W`.
- **GDI32** (18) — `CreateFontA/Indirect`, `ExtTextOutA`, `GetTextExtentPoint32A`, `CreateDIBSection`, `GetDIBits` → runtime TrueType rasterisation into textures.
- **ADVAPI32** (4) — registry settings. **ole32** (3) — COM init (for WMF). **WINMM** (10) — `timeGetTime`, mixer*, mci*.
- **d3d8** `Direct3DCreate8`; **DINPUT8** `DirectInput8Create`; **DSOUND** ordinal 1; **OpenAL32** 32 functions (al*/alc*).
- **v3d_87.dll** 23 mangled functions (`?skin_deform@@...`, `?matrix_inverse@@...`, `vertexf_check_cpu`).

## Embedded libraries / strings of note
- "D3DX8 Shader Assembler Version 0.91", `ValidateVertexShader`, `ValidatePixelShader`, `D3DXERR_*` → runtime
  assembly of `vs.1.1`/`ps.1.x` text; shader fragments also from `.nvo` (NVFO) files.
- DirectX `.x` file templates (`XSkinMeshHeader`, `SkinWeights`, `VertexDuplicationIndices`, `PatchMesh`) → D3DX
  mesh/skin helpers were linked; `.smo` may be derived from `.x` content.
- libpng 1.0.5 (1999), zlib, IJG libjpeg (Adobe APP14 marker string).
- Script parser errors: "Expecting script/function/variable/constant/math expression!", "Expecting ) for the
  ending script!", "Expecting script name", "Cannot open script", "Error loading Speech Script : %1s".
- Versioning: "Version number mismatched.  The program currently supports version ", "Must have version number."
- Paths: `SCRIPT\SCENARIO\`, `SCRIPT\PSCRIPT\*.txt`, `SCRIPT\UNIT\`, `TERRAIN\`, `SAVE\%s%04d.%s`, `_AUTO%03d.SAV`,
  `*.SAV`, `MOVIE\ENLIGHT.WMV`, `MOVIE\TREVOR.WMV`, `MOVIE\INTRO.WMV`, `SHADER\hardware_skin.nvo`.
- Registry: `Software\Microsoft\DirectX` (version check).

## Open questions
- `TODO(re): 0x88E960` stub routine purpose (watchdog over 14 `.text` call sites). Not needed for the port.
- `TODO(re): 0x56BEA0` first call in `WinMain` (single-instance mutex? env check?).
- Which `v3d_*.dll` the launcher picks on a modern CPU: SSE (`v3d_87.dll == v3d_sse.dll` here).
- Text code page per language build: this install cp1250 (verified on the CD-check string and `*EX.res`).
- ~~Provenance of the alternate build~~ — Steam release (user, 2026-10-22); now the reference.
