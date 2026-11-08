#!/usr/bin/env python3
"""pe_inventory.py — triage a Win32/Win64 PE for reverse engineering.

Prints (or writes as JSON/Markdown) sections with entropy, imports grouped by DLL,
exports (demangled hint), embedded source paths, referenced data-file extensions,
and heuristics for packers/DRM. Pure Python; needs only `pefile` (pip install pefile).

Usage:
  pe_inventory.py FILE [FILE...] [--json out.json] [--md out.md] [--strings-min 6]
"""
from __future__ import annotations

import argparse
import collections
import json
import math
import re
import sys
from pathlib import Path

try:
    import pefile
except ImportError:  # pragma: no cover
    sys.exit("pip install pefile")

MACHINES = {0x14C: "x86 (32-bit)", 0x8664: "x86-64", 0xAA64: "ARM64"}
SRC_PATH_RE = re.compile(rb"[a-zA-Z]:\\[\w\\\-. ]+\.(?:cpp|c|h|hpp|inl|asm)", re.I)
DATA_EXT_RE = re.compile(
    rb"\.(am[123]|smo|sce|res|pro|set|cm|txr|fin|lgt|sca|scr|bm|obj|nvo|pso|dds|wma|wmv|mda|uni|sav|txt|x|bmp|tga|jpg|png|flc|wav)\b",
    re.I,
)
LEGACY_API_DLLS = {
    "d3d8.dll": "Direct3D 8", "d3d9.dll": "Direct3D 9", "d3dx8*.dll": "D3DX8", "dinput8.dll": "DirectInput 8",
    "dinput.dll": "DirectInput", "dsound.dll": "DirectSound", "wmvcore.dll": "Windows Media Format",
    "winmm.dll": "WinMM (timeGetTime/mixer/mci)", "openal32.dll": "Creative OpenAL", "eax.dll": "EAX",
    "ddraw.dll": "DirectDraw", "glide*.dll": "3dfx Glide", "quartz.dll": "DirectShow",
}
PACKER_HINTS = [
    (rb"UPX!", "UPX"), (rb"ASProtect", "ASProtect"), (rb"Armadillo", "Armadillo"), (rb"SecuROM", "SecuROM"),
    (rb"SafeDisc", "SafeDisc"), (rb"stxt371", "SafeDisc v2/3"), (rb"StarForce", "StarForce"), (rb"\.sforce", "StarForce"),
    (rb"Themida", "Themida"), (rb"VMProtect", "VMProtect"), (rb"EXECryptor", "EXECryptor"), (rb"\.cms_t", "SecuROM"),
    (rb"\.tages", "Tages"), (rb"PECompact", "PECompact"), (rb"ProtectDisc", "ProtectDISC"), (rb"LaserLock", "LaserLock"),
]


def entropy(data: bytes) -> float:
    if not data:
        return 0.0
    counts = collections.Counter(data)
    n = len(data)
    return -sum(c / n * math.log2(c / n) for c in counts.values())


def analyse(path: Path, strings_min: int) -> dict:
    pe = pefile.PE(str(path), fast_load=True)
    pe.parse_data_directories()
    raw = path.read_bytes()
    info: dict = {
        "file": str(path),
        "size": len(raw),
        "machine": MACHINES.get(pe.FILE_HEADER.Machine, hex(pe.FILE_HEADER.Machine)),
        "linker": f"{pe.OPTIONAL_HEADER.MajorLinkerVersion}.{pe.OPTIONAL_HEADER.MinorLinkerVersion}",
        "timestamp": pe.FILE_HEADER.TimeDateStamp,
        "image_base": hex(pe.OPTIONAL_HEADER.ImageBase),
        "entry_point": hex(pe.OPTIONAL_HEADER.ImageBase + pe.OPTIONAL_HEADER.AddressOfEntryPoint),
        "subsystem": pe.OPTIONAL_HEADER.Subsystem,
        "sections": [],
        "imports": {},
        "exports": [],
        "source_paths": [],
        "data_extensions": {},
        "flags": [],
    }
    ep_rva = pe.OPTIONAL_HEADER.AddressOfEntryPoint
    names = collections.Counter()
    for s in pe.sections:
        name = s.Name.rstrip(b"\0").decode(errors="replace")
        names[name] += 1
        data = s.get_data()
        sec = {
            "name": name,
            "va": hex(pe.OPTIONAL_HEADER.ImageBase + s.VirtualAddress),
            "vsize": s.Misc_VirtualSize,
            "rawsize": s.SizeOfRawData,
            "entropy": round(entropy(data), 2),
            "exec": bool(s.Characteristics & 0x20000000),
            "write": bool(s.Characteristics & 0x80000000),
            "contains_entry": s.VirtualAddress <= ep_rva < s.VirtualAddress + max(s.Misc_VirtualSize, s.SizeOfRawData),
        }
        info["sections"].append(sec)
        if sec["exec"] and sec["entropy"] > 7.0:
            info["flags"].append(f"section {name}: executable with entropy {sec['entropy']} (packed/encrypted code?)")
        if not re.fullmatch(r"\.(text|rdata|data|idata|edata|rsrc|reloc|bss|tls|pdata|CRT|textbss|didat|00cfg|gfids|voltbl)", name):
            info["flags"].append(f"section {name}: non-standard name")
    for n, c in names.items():
        if c > 1:
            info["flags"].append(f"duplicate section name {n} x{c} (wrapper/protector appended its own?)")
    if hasattr(pe, "DIRECTORY_ENTRY_IMPORT"):
        for e in pe.DIRECTORY_ENTRY_IMPORT:
            dll = e.dll.decode(errors="replace")
            info["imports"][dll] = [i.name.decode() if i.name else f"#{i.ordinal}" for i in e.imports]
            for pat, desc in LEGACY_API_DLLS.items():
                if re.fullmatch(pat.replace(".", r"\.").replace("*", ".*"), dll, re.I):
                    info["flags"].append(f"imports {dll}: {desc} — must be replaced in the port")
    if hasattr(pe, "DIRECTORY_ENTRY_EXPORT"):
        info["exports"] = [s.name.decode() for s in pe.DIRECTORY_ENTRY_EXPORT.symbols if s.name]
    info["source_paths"] = sorted({m.decode() for m in SRC_PATH_RE.findall(raw)})
    info["data_extensions"] = dict(collections.Counter(m.lower().decode() for m in DATA_EXT_RE.findall(raw)))
    for pat, name in PACKER_HINTS:
        if re.search(pat, raw):
            info["flags"].append(f"packer/DRM string hint: {name}")
    strs = re.findall(rb"[\x20-\x7e]{%d,}" % strings_min, raw)
    info["printable_strings"] = len(strs)
    info["interesting_strings"] = sorted({
        s.decode() for s in strs
        if re.search(rb"(?i)shader|vs\.1|ps\.1|D3DX|Direct|OpenAL|wmv|version|error|loading|expecting|\.sys|\.dll", s)
    })[:400]
    return info


def to_markdown(info: dict) -> str:
    out = [f"# {Path(info['file']).name}", "",
           f"- machine: **{info['machine']}**, linker {info['linker']}, entry {info['entry_point']}, base {info['image_base']}, size {info['size']}",
           "", "## Sections", "", "| name | VA | vsize | rawsize | entropy | X | W | EP |", "|---|---|---|---|---|---|---|---|"]
    for s in info["sections"]:
        out.append(f"| `{s['name']}` | {s['va']} | {s['vsize']:#x} | {s['rawsize']:#x} | {s['entropy']} | {'x' if s['exec'] else ''} | {'w' if s['write'] else ''} | {'EP' if s['contains_entry'] else ''} |")
    out += ["", "## Flags", ""] + [f"- {f}" for f in info["flags"]] or ["- none"]
    out += ["", "## Imports", ""]
    for dll, fns in info["imports"].items():
        out.append(f"- **{dll}** ({len(fns)}): {', '.join(fns[:40])}{' …' if len(fns) > 40 else ''}")
    if info["exports"]:
        out += ["", "## Exports", ""] + [f"- `{e}`" for e in info["exports"]]
    if info["source_paths"]:
        out += ["", f"## Embedded source paths ({len(info['source_paths'])})", ""] + [f"- `{p}`" for p in info["source_paths"]]
    if info["data_extensions"]:
        out += ["", "## Data-file extensions referenced", "", ", ".join(f"`.{k}`×{v}" for k, v in sorted(info["data_extensions"].items()))]
    return "\n".join(out) + "\n"


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="+", type=Path)
    ap.add_argument("--json", type=Path)
    ap.add_argument("--md", type=Path)
    ap.add_argument("--strings-min", type=int, default=6)
    a = ap.parse_args()
    results = [analyse(f, a.strings_min) for f in a.files]
    if a.json:
        a.json.write_text(json.dumps(results, indent=1))
    md = "\n".join(to_markdown(r) for r in results)
    if a.md:
        a.md.write_text(md, encoding="utf-8")
    if not a.json and not a.md:
        print(md)
    else:
        for r in results:
            print(f"{r['file']}: {r['machine']}, {len(r['sections'])} sections, {len(r['imports'])} import DLLs, {len(r['flags'])} flags")


if __name__ == "__main__":
    main()
