#!/usr/bin/env python3
"""sce_builtins.py — catalogue the .sce scripting language as used by the shipped scripts.

Walks a directory of `.sce` files and reports: call families (FUNC/ACTION/DIALOG/SCRIPT),
every builtin name with call count and the distinct arity/argument-type signatures observed,
ACTION order/task names, WAIT{} forms, declared variable types, and control-flow keywords.
Output is Markdown suitable for docs/scripting/BUILTINS.md.

Second mode (`--gen-table`): read the finished docs/scripting/BUILTINS.md and emit the C++ X-macro
file src/script/builtins/table_data.inc (one `OPENJOAN_BUILTIN(id, name, result_tag, family,
arg_min, arg_max, wait_rule)` row per table id, 408 rows = sentinel + ids 1..407), so the engine's
builtin table is generated from the documentation and can never drift from it. The WAIT-rule
classification (`WAIT_RULES` below) is cross-checked against the ids that §5.1-§5.3 list as
waiting; an optional `--tsv` (the Ghidra dump `builtin_table_0xa1a214.tsv`) cross-checks ids,
names and result tags against the binary. `--check` verifies the committed file is current (CI).

Usage:
  sce_builtins.py SCRIPT_DIR [--md out.md] [--encoding cp1250]
  sce_builtins.py --gen-table docs/scripting/BUILTINS.md --out src/script/builtins/table_data.inc
                  [--tsv $JOA_WORK_DIR/decomp/script/builtin_table_0xa1a214.tsv] [--check]
"""
from __future__ import annotations

import argparse
import collections
import dataclasses
import re
import sys
from pathlib import Path

# ---------------------------------------------------------------------------------------------
# --gen-table: BUILTINS.md -> table_data.inc
# ---------------------------------------------------------------------------------------------

BUILTIN_COUNT = 408  # sentinel + ids 1..407 (docs/re/structs/ScriptBuiltinTable.md §1)
VARIADIC = 255  # arg_max value meaning "no upper bound known"

# WAIT completion rule per id, classified from BUILTINS.md §5.1 (FUNC poll 0x6509E0), §5.2 (ACTION
# poll 0x6612E0) and §5.3 (DIALOG poll 0x660E60). Every id absent here polls "done immediately".
# The generator refuses to run when this map disagrees with the ids the three sections list.
WAIT_RULES: dict[int, str] = {
    330: "pause_timer",  # pause_script: deadline < now_ms
    316: "dialog_cut_scene",  # add_dialog: waits only inside a cut scene, until lines finish
    319: "choice_click",  # add_choice_dialog: waits for the player's click on a choice
    323: "order_pending",  # give_order: order neither current nor pending
    304: "ui_closed",  # start_buy_interface: shop UI closed
    329: "ui_closed",  # display_title_screen: title finished
    385: "ui_closed",  # show_scenario_brief: brief closed
    398: "ui_closed",  # show_letter_interface: letter UI closed
    296: "cut_scene_state",  # start_cut_scene: cut-scene state machine (0x65AF50)
    297: "cut_scene_state",  # start_cut_scene_2
    302: "cut_scene_state",  # change_camera_in_cut_scene
    303: "cut_scene_state",  # is_cut_scene_status_black_screen: black screen reached
    390: "cut_scene_state",  # is_cut_scene_finished: no cut scene running
    301: "custom",  # start_cut_scene_camera: world entity scan (0x653A90)
}
WAIT_RULE_NAMES = ("done_immediately", "pause_timer", "dialog_cut_scene", "choice_click",
                   "order_pending", "ui_closed", "cut_scene_state", "custom")


@dataclasses.dataclass
class Row:
    """One builtin-table row as emitted into table_data.inc."""
    id: int
    name: str
    result_tag: int
    family: str  # func | dialog | action (the call class whose dispatcher has a case for the id)
    arg_min: int
    arg_max: int
    wait_rule: str = "done_immediately"


def _cells(line: str) -> list[str]:
    return [c.strip() for c in line.strip().strip("|").split("|")]


def _ints(text: str) -> list[int]:
    return [int(x) for x in re.findall(r"\d+", text)]


def _parse_arity(spec: str) -> tuple[int, int]:
    """Turns the `nA=...` / `N args` text of an args cell into (arg_min, arg_max)."""
    spec = spec.replace("**", "")
    m = re.search(r"nA=([^|]*)", spec)
    if m:
        body = m.group(1).strip()
        if body.startswith("?"):
            return 0, VARIADIC
        head = re.split(r"\s+\(", body, maxsplit=1)[0]  # cut at the first "(kinds)" group
        head = head.split(" w", 1)[0].split(" ptr", 1)[0].split(" f32", 1)[0].split(" u16", 1)[0]
        if "+ pairs" in body:
            return int(_ints(head)[0]), VARIADIC
        if re.match(r"^\d+\.\.\s*($|[^0-9])", head):
            return int(_ints(head)[0]), VARIADIC
        nums = _ints(head)
        if "→" in body:  # "5 → 0x65B030, else 7 → 0x65B070"
            nums = [int(x) for x in re.findall(r"(?:^|[=\s])(\d+)\s*→", body)]
            nums += [int(x) for x in re.findall(r"else\s+(\d+)", body)]
        if not nums:
            raise ValueError(f"cannot parse arity from {spec!r}")
        return min(nums), max(nums)
    m = re.search(r"(\d+)[- ]args?\b", spec)
    if m:
        n = int(m.group(1))
        return n, n
    raise ValueError(f"cannot parse arity from {spec!r}")


def _parse_arity_sec4(spec: str) -> tuple[int, int]:
    """§4 `nA / widths` cell: `1, w32`, `2 or 1 (arity overload)`, `2..8 (switch on argc), ...`."""
    spec = spec.replace("**", "")
    spec = re.sub(r"\([^)]*\)", "", spec)  # drop parenthesised remarks
    head = spec.split(",", 1)[0].strip()
    head = re.sub(r"\s+per dispatcher.*", "", head)
    if ".." in head:
        a, b = _ints(head)[:2]
        return a, b
    nums = _ints(head)
    if not nums:
        raise ValueError(f"cannot parse §4 arity from {spec!r}")
    return min(nums), max(nums)


def _result_tag_from_ret(ret: str) -> int:
    r = ret.lower()
    if "float" in r:
        return 2
    if "str" in r:
        return 3
    return 1


def parse_builtins_md(md: str) -> tuple[list[Row], set[int]]:
    """Parses §2 (ids 1..353), §4 (354..407) and the §5 WAIT tables of BUILTINS.md.

    Returns the rows (without the sentinel) and the set of ids whose poll is not "done immediately".
    """
    sections: dict[str, list[str]] = collections.defaultdict(list)
    current = ""
    for line in md.splitlines():
        m = re.match(r"^(#{2,3}) (\d+(?:\.\d+)?[a-z]?)\.? ", line)
        if m:
            current = m.group(2)
        sections[current].append(line)

    rows: list[Row] = []
    for line in sections["2"]:
        if not line.startswith("| ") or line.startswith("| id ") or line.startswith("|---"):
            continue
        c = _cells(line)
        if len(c) < 9 or not c[0].isdigit():
            continue
        bid, name, ty, args, ret = int(c[0]), c[1].strip("`"), c[2], c[3], c[4]
        tag_digits = _ints(ty.replace("**", ""))
        if not tag_digits:
            raise ValueError(f"id {bid}: no result tag in {ty!r}")
        family = "func"
        if "DIALOG class" in args:
            family = "dialog"
        elif "ACTION class" in args:
            family = "action"
        amin, amax = _parse_arity(args)
        rows.append(Row(bid, name, tag_digits[0], family, amin, amax))

    for line in sections["4"]:
        if not line.startswith("| ") or line.startswith("| id ") or line.startswith("|---"):
            continue
        c = _cells(line)
        if len(c) < 7:
            continue
        ids = _ints(c[0].split("(")[0])
        if not ids:
            continue
        bid, name, na, ret = ids[0], c[1].strip("`"), c[3], c[4]
        amin, amax = _parse_arity_sec4(na)
        rows.append(Row(bid, name, _result_tag_from_ret(ret), "func", amin, amax))

    waiting: set[int] = set()
    for line in sections["5.1"]:
        if line.startswith("| ") and not line.startswith("| id ") and "any other id" not in line:
            c = _cells(line)
            if c and c[0] and c[0] != "---":
                for grp in re.findall(r"\(([0-9, ]+)\)", c[0]):
                    waiting.update(_ints(grp))
    for line in sections["5.2"]:
        for name in re.findall(r"Only `([a-z_]+)` \(id 0x[0-9A-F]+\) waits", line):
            waiting.update(r.id for r in rows if r.name == name)
    for line in sections["5.3"]:
        if line.startswith("| ") and not line.startswith("| id "):
            c = _cells(line)
            if len(c) >= 3 and "not done" in c[2]:
                for grp in re.findall(r"\(([0-9, ]+)\)", c[0]):
                    waiting.update(_ints(grp))
    return rows, waiting


def build_table(md_path: Path, tsv_path: Path | None) -> list[Row]:
    rows, waiting = parse_builtins_md(md_path.read_text(encoding="utf-8"))
    by_id = {r.id: r for r in rows}
    if len(rows) != len(by_id):
        dup = [i for i, n in collections.Counter(r.id for r in rows).items() if n > 1]
        raise SystemExit(f"duplicate ids in BUILTINS.md: {dup}")
    missing = sorted(set(range(1, BUILTIN_COUNT)) - set(by_id))
    if missing:
        raise SystemExit(f"BUILTINS.md lacks rows for ids {missing}")
    names = collections.Counter(r.name for r in rows)
    dup_names = [n for n, k in names.items() if k > 1]
    if dup_names:
        raise SystemExit(f"duplicate names in BUILTINS.md: {dup_names}")
    if waiting != set(WAIT_RULES):
        raise SystemExit(
            f"WAIT_RULES disagrees with BUILTINS.md §5: documented-waiting {sorted(waiting)} vs "
            f"classified {sorted(WAIT_RULES)}")
    for bid, rule in WAIT_RULES.items():
        by_id[bid].wait_rule = rule
    if tsv_path is not None:
        for line in tsv_path.read_text(encoding="utf-8").splitlines()[1:]:
            _, tid, tname, ttag = line.split("\t")
            tid, ttag = int(tid), int(ttag)
            if tid == 0:
                continue
            r = by_id.get(tid)
            if r is None or r.name != tname or r.result_tag != ttag:
                raise SystemExit(
                    f"id {tid}: BUILTINS.md ({r and (r.name, r.result_tag)}) differs from the "
                    f"binary dump ({tname}, {ttag})")
    table = [Row(0, "", 0, "func", 0, 0)] + [by_id[i] for i in range(1, BUILTIN_COUNT)]
    return table


def render_table(table: list[Row], md_path: Path) -> str:
    out = [
        "// Generated by tools/sce_builtins.py --gen-table from " + md_path.as_posix() + " — do not edit.",
        "// One row per builtin-table entry (docs/re/structs/ScriptBuiltinTable.md): sentinel + ids 1..407.",
        "// OPENJOAN_BUILTIN(id, name, result_tag, family, arg_min, arg_max, wait_rule)",
        "//   result_tag 0 sentinel, 1 int, 2 float, 3 string; family = call class whose dispatcher",
        "//   handles the id (func 0x649920, dialog 0x65FD70, action 0x661060); arg_min/arg_max from",
        "//   the dispatcher's argument fetches (255 = open-ended); wait_rule per BUILTINS.md §5.",
    ]
    for r in table:
        out.append(
            f'OPENJOAN_BUILTIN({r.id}, "{r.name}", {r.result_tag}, {r.family}, {r.arg_min}, '
            f"{r.arg_max}, {r.wait_rule})")
    out.append(f"// rows: {len(table)}")
    return "\n".join(out) + "\n"


def gen_table(md_path: Path, out_path: Path | None, tsv_path: Path | None, check: bool) -> int:
    table = build_table(md_path, tsv_path)
    assert len(table) == BUILTIN_COUNT
    text = render_table(table, md_path)
    if out_path is None:
        sys.stdout.write(text)
        return 0
    if check:
        if not out_path.exists() or out_path.read_text(encoding="utf-8") != text:
            print(f"{out_path} is stale: run tools/sce_builtins.py --gen-table", file=sys.stderr)
            return 1
        print(f"{out_path} is up to date ({len(table)} rows)")
        return 0
    out_path.write_text(text, encoding="utf-8")
    print(f"wrote {out_path}: {len(table)} rows, {sum(1 for r in table if r.wait_rule != 'done_immediately')} waiting ids")
    return 0

CALL_RE = re.compile(r'\b(FUNC|ACTION|DIALOG|SCRIPT)\s*\(\s*"([A-Za-z_0-9\\./ ]+)"\s*((?:,[^()]*?(?:\([^()]*\)[^()]*?)*)*)\)', re.S)
KEYWORDS = ["INT", "STRING", "FLOAT", "init", "exit", "main", "if", "else", "while", "for", "switch", "case", "break", "continue", "return", "WAIT", "ID", "and", "or", "not", "NULL"]


def arg_kind(tok: str) -> str:
    t = tok.strip()
    if not t:
        return "?"
    if re.fullmatch(r"-?\d+", t):
        return "int"
    if re.fullmatch(r"-?\d*\.\d+f?", t):
        return "float"
    if t.startswith('"'):
        return "str"
    if "(" in t:
        return "expr"
    return "var"


def split_args(s: str) -> list[str]:
    out, depth, cur = [], 0, ""
    for ch in s:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur); cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur)
    return out


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("root", type=Path, nargs="?")
    ap.add_argument("--md", type=Path)
    ap.add_argument("--encoding", default="cp1250")
    ap.add_argument("--gen-table", type=Path, metavar="BUILTINS.md",
                    help="emit the C++ X-macro table from the builtin reference")
    ap.add_argument("--out", type=Path, help="--gen-table output (default: stdout)")
    ap.add_argument("--tsv", type=Path, help="--gen-table cross-check against the binary dump")
    ap.add_argument("--check", action="store_true", help="--gen-table: fail if --out is stale")
    a = ap.parse_args()
    if a.gen_table is not None:
        raise SystemExit(gen_table(a.gen_table, a.out, a.tsv, a.check))
    if a.root is None:
        ap.error("SCRIPT_DIR is required unless --gen-table is given")

    families: dict[str, collections.Counter] = collections.defaultdict(collections.Counter)
    sigs: dict[tuple[str, str], collections.Counter] = collections.defaultdict(collections.Counter)
    where: dict[tuple[str, str], set] = collections.defaultdict(set)
    orders, tasks = collections.Counter(), collections.Counter()
    waits, kw = collections.Counter(), collections.Counter()
    files = sorted(a.root.rglob("*.sce"))
    for f in files:
        text = f.read_text(encoding=a.encoding, errors="replace")
        code = re.sub(r"//[^\n]*", "", text)  # strip line comments
        for m in CALL_RE.finditer(code):
            fam, name, rest = m.group(1), m.group(2), m.group(3)
            args = split_args(rest.lstrip(",")) if rest.strip(", ") else []
            families[fam][name] += 1
            sigs[(fam, name)][",".join(arg_kind(x) for x in args)] += 1
            where[(fam, name)].add(f.name)
            if fam == "ACTION" and name == "give_order" and len(args) >= 2:
                orders[args[1].strip()] += 1
            if fam == "ACTION" and name in ("give_task", "give_team_task") and len(args) >= 2:
                tasks[args[1].strip()] += 1
        for m in re.finditer(r"WAIT\s*\{\s*([A-Za-z_]+)", code):
            waits[m.group(1)] += 1
        for k in KEYWORDS:
            kw[k] += len(re.findall(rf"(?<![\w\"]){re.escape(k)}(?![\w\"])", code))

    out = [f"# .sce builtin catalogue — {len(files)} scripts under `{a.root}`", ""]
    out += ["## Keyword frequency", "", ", ".join(f"`{k}`×{v}" for k, v in kw.most_common() if v), ""]
    out += ["## WAIT { … } forms", "", ", ".join(f"`WAIT {{ {k} }}`×{v}" for k, v in waits.most_common()), ""]
    for fam in ("FUNC", "ACTION", "DIALOG", "SCRIPT"):
        names = families[fam]
        out += [f"## {fam} — {len(names)} distinct, {sum(names.values())} calls", "", "| name | calls | observed signatures (arg kinds × count) | status | RE address | semantics |", "|---|---|---|---|---|---|"]
        for name, n in sorted(names.items(), key=lambda kv: (-kv[1], kv[0])):
            sig = "; ".join(f"`({s})`×{c}" for s, c in sigs[(fam, name)].most_common(3))
            out.append(f"| `{name}` | {n} | {sig} | TODO | | |")
        out.append("")
    out += ["## ACTION give_order — order names", "", ", ".join(f"{k}×{v}" for k, v in orders.most_common()), ""]
    out += ["## ACTION give_task / give_team_task — task names", "", ", ".join(f"{k}×{v}" for k, v in tasks.most_common()), ""]
    md = "\n".join(out)
    if a.md:
        a.md.write_text(md, encoding="utf-8")
        print(f"wrote {a.md}: {sum(len(v) for v in families.values())} builtins")
    else:
        print(md)


if __name__ == "__main__":
    main()
