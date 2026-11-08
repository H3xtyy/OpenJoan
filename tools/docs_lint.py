#!/usr/bin/env python3
"""docs_lint.py — structural documentation checks (skill oj-documentation §4).

- every `src/<module>/` directory that holds sources has a README.md
- every top-level page the contributor guide promises exists
- every `[[link]](path)` in docs/ that points into the repo resolves
- every `@see docs/....md` in a header names an existing page
Exit non-zero on any failure. Format/builtin completeness checks are added with Phase 1 loaders.
"""
from __future__ import annotations
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
# The layout of skill oj-documentation section 1 (pages that must exist).
REQUIRED = ["docs/README.md",
            "docs/ARCHITECTURE.md", "docs/RUNTIME.md", "docs/X64.md",
            "docs/CONTRIBUTING.md", "docs/GLOSSARY.md", "docs/Doxyfile",
            "src/data/README.md",
            "docs/re/BINARY.md", "docs/re/MODULES.md", "docs/re/TIMING.md", "docs/re/RENDER_STATES.md",
            "docs/formats/INVENTORY.md", "docs/render/D3D8_USAGE.md", "docs/render/EN3D_RENDER.md",
            "docs/scripting/LANGUAGE.md", "docs/scripting/BUILTINS.md"]
SRC_EXT = {".cpp", ".h", ".hpp"}


def main() -> int:
    bad = 0
    for rel in REQUIRED:
        if not (ROOT / rel).exists():
            print(f"missing required page: {rel}"); bad += 1
    modules = {p.parent for p in (ROOT / "src").rglob("*") if p.suffix in SRC_EXT}
    tops = set()
    for m in modules:
        parts = m.relative_to(ROOT / "src").parts
        if parts:
            tops.add(ROOT / "src" / parts[0])
    for m in sorted(tops):
        if not (m / "README.md").exists():
            print(f"module without README.md: {m.relative_to(ROOT)}"); bad += 1
    link_re = re.compile(r"\[[^\]]*\]\(([^)#\s]+)(?:#[^)]*)?\)")
    for md in (ROOT / "docs").rglob("*.md"):
        for m in link_re.finditer(md.read_text(encoding="utf-8", errors="replace")):
            target = m.group(1)
            if "://" in target or target.startswith("mailto:"):
                continue
            path = (md.parent / target).resolve()
            if not path.exists():
                print(f"{md.relative_to(ROOT)}: broken link {target}"); bad += 1
    see_re = re.compile(r"@see\s+(docs/[\w./-]+\.md)")
    for src in (ROOT / "src").rglob("*"):
        if src.suffix not in SRC_EXT:
            continue
        for m in see_re.finditer(src.read_text(encoding="utf-8", errors="replace")):
            if not (ROOT / m.group(1)).exists():
                print(f"{src.relative_to(ROOT)}: @see target missing {m.group(1)}"); bad += 1
    print(f"{'FAIL' if bad else 'OK'}: {bad} problem(s)")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
