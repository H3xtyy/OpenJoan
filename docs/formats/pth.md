# `.pth` — path line lists (`map/scenarioN/*.pth`, 15 files)

Reader: TODO(re) (no `CreatePathLineList` literal in the
binary — the reader is positional and never compares the block name; find it from the `.pth` extension builder
near 0x4F2F80). Grammar: [block.md](block.md) **without semicolons** (the value ends at the end of the line):
```
CreatePathLineList { PathType:<int>  PathPointNum:K  K x (X Y Z AssistFromLine AssistToLine TerrainNodeOnly) }
```
All values are integers. The field order is fixed (the field count is exactly `2 + 6K` in every file). `AssistFromLine`/`AssistToLine` are -1 when unused, `TerrainNodeOnly` is 0|1.
Observed `PathType` values: {0, 1}. Used by the AI/flocking path system (`flocksys.cpp`).
Exactly one file is empty (0 bytes, `scenario6/S6L1w.pth`) and holds no lists.
