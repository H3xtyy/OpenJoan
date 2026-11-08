# `.scr` — scenario descriptor (`scenario/scenario_01..08.scr`)

Loader: `scenario.cpp` — joan.exe **0x4CCFB0** (`scenario::fn_004ccfb0`; builds `SCENARIO\scenario_%02d.scr`
from the scenario number, string `SCENARIO\` at 0x9D57D4; silently does nothing when the file is absent — it
checks existence with 0x590B80 first). Grammar: [keyvalue.md](keyvalue.md).

## Layout (positional in the original, RE: 0x4CCFB0)
```
code : SCENARIO_01          ; token pair skipped, value read as string  (stored by script/save code elsewhere)
StepNo : 1                  ; integer N = number of step groups that follow (the step-count field of the `Scenario::load_scr` object); 0x1b2 bytes allocated per step
-- N times, six (key, string) pairs in this order, each copied into a 0x41-byte field --
WorldFileName : scenario1\S1L1w
UnitFileName : scenario1\S1L1.uni
ScriptFileName : S1L1
DescriptionFileName : S1L1_des
ObjectiveFileName : S1L1_obj
ObjectiveMapFileName : S1L1_map
```
Correction to the earlier draft: `StepNo` is the **count** of step groups (all 8 shipped files have 1), not a
campaign order index; the reader loops `for i < StepNo`. A blank line between `StepNo` and the group is present in
every file (two key:value records) — the loader treats the file as one field sequence. The trailing per-step fields
of the 0x1b2-byte runtime struct (0x186..0x1aa) are zeroed / set to -1 by the reader and are not file data.

| key | meaning | resolved how (verified on all 8 files) |
|---|---|---|
| `code` | scenario code | `SCENARIO_01..08`; **not** the file number: `scenario_02.scr` holds `SCENARIO_03` and `scenario_03.scr` holds `SCENARIO_02` |
| `StepNo` | number of step groups | 1 in all files |
| `WorldFileName` | `map\` + name | `.wor`, `.day`, `.ter` exist for all (`.map` only for scenarios 1-5; absent for 6 and 7, so it is not required) |
| `UnitFileName` | `map\` + name, with `.uni` | exists as written for scenarios 1 and 2; for 3-8 only `<base>_1.uni`, `<base>_2.uni`… exist (multi-part levels) |
| `ScriptFileName` | `script\scenario\<name>.sce` | exists for all |
| `DescriptionFileName` | `script\scenario\<name>.sce` (the `_des` script) | exists for all (`<name>EX.res` is the string table next to it) |
| `ObjectiveFileName` | `script\scenario\<name>.obj` | exists for all |
| `ObjectiveMapFileName` | `script\scenario\<name>.bm` | exists for all |

Oddities in the data: `scenario_06.scr` reuses world/unit/map of scenario 5 (`S5L1w`, `S5L1.uni`, `S5L1_map`) but has its
own scripts (`S9L1`, `S9L1_des`, `S9L1_obj`); `scenario_08.scr` points at `scenario6\S6L1w`; `scenario_07` → scenario7.
Whitespace around `:` is a mix of spaces and tabs.

## Open questions
- `TODO(re) 0x4CCFB0`: the reader ignores the key text, so whether a mis-ordered file is rejected cannot be
  tested against the original.
- Which of `_finish.sce`, `_tactic.sce`, `<name>EX.res` are derived from `ScriptFileName` is decided by the script
  system, not by this file.
