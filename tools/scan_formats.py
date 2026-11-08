#!/usr/bin/env python3
"""scan_formats.py — census of a game install's data files for format reverse engineering.

For every extension: count, total size, min/max size, text-vs-binary verdict, the most common
first 8 bytes (magic), which directories it lives in, and a sample path. Optionally dumps the
first N bytes of one sample per binary extension as hex.

Usage:
  scan_formats.py GAME_DIR [--md out.md] [--json out.json] [--hex 64]
"""
from __future__ import annotations

import argparse
import collections
import json
import os
from pathlib import Path

TEXT_BYTES = set(range(0x20, 0x7F)) | {0x09, 0x0A, 0x0D}


def is_text(sample: bytes) -> bool:
    if not sample:
        return True
    if b"\0" in sample[: len(sample) // 2]:
        return False
    # allow high-bit chars (cp1250 Polish text) but require mostly printable
    ok = sum(1 for b in sample if b in TEXT_BYTES or b >= 0x80)
    return ok / len(sample) > 0.95


def scan(root: Path, hex_bytes: int) -> dict:
    exts: dict[str, dict] = {}
    for dirpath, _dirs, files in os.walk(root):
        for fn in files:
            p = Path(dirpath) / fn
            ext = p.suffix.lower().lstrip(".") or "(none)"
            e = exts.setdefault(ext, {
                "count": 0, "bytes": 0, "min": None, "max": 0, "dirs": collections.Counter(),
                "magics": collections.Counter(), "text_votes": 0, "sample": None, "sample_hex": None,
            })
            try:
                size = p.stat().st_size
                with open(p, "rb") as fh:
                    head = fh.read(max(512, hex_bytes))
            except OSError:
                continue
            e["count"] += 1
            e["bytes"] += size
            e["min"] = size if e["min"] is None else min(e["min"], size)
            e["max"] = max(e["max"], size)
            e["dirs"][str(p.parent.relative_to(root))] += 1
            e["magics"][head[:8].hex(" ")] += 1
            e["text_votes"] += 1 if is_text(head[:512]) else 0
            if e["sample"] is None or size < Path(root, e["sample"]).stat().st_size:
                e["sample"] = str(p.relative_to(root))
                e["sample_hex"] = head[:hex_bytes].hex(" ")
    for e in exts.values():
        e["kind"] = "TEXT" if e["text_votes"] == e["count"] else ("MIXED" if e["text_votes"] else "BINARY")
        e["dirs"] = dict(e["dirs"].most_common(6))
        e["magics"] = dict(e["magics"].most_common(3))
    return dict(sorted(exts.items(), key=lambda kv: -kv[1]["count"]))


def to_md(root: Path, exts: dict, hex_bytes: int) -> str:
    out = [f"# Format inventory — `{root}`", "", "| ext | kind | count | total | min | max | dirs | common magic (8 bytes) |", "|---|---|---|---|---|---|---|---|"]
    for ext, e in exts.items():
        dirs = ", ".join(f"`{d}`×{n}" for d, n in e["dirs"].items())
        magic = "; ".join(f"`{m}`×{n}" for m, n in e["magics"].items())
        out.append(f"| `.{ext}` | {e['kind']} | {e['count']} | {e['bytes']:,} | {e['min']:,} | {e['max']:,} | {dirs} | {magic} |")
    out += ["", f"## Smallest sample per extension (first {hex_bytes} bytes)", ""]
    for ext, e in exts.items():
        out += [f"### `.{ext}` — `{e['sample']}`", "", "```", e["sample_hex"] or "", "```", ""]
    return "\n".join(out)


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("root", type=Path)
    ap.add_argument("--md", type=Path)
    ap.add_argument("--json", type=Path)
    ap.add_argument("--hex", type=int, default=64)
    a = ap.parse_args()
    exts = scan(a.root, a.hex)
    if a.json:
        a.json.write_text(json.dumps(exts, indent=1))
    md = to_md(a.root, exts, a.hex)
    if a.md:
        a.md.write_text(md, encoding="utf-8")
    if not a.md and not a.json:
        print(md)


if __name__ == "__main__":
    main()
