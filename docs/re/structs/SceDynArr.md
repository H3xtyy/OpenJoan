# SceDynArr — `ilib::dynarr`, the cursor array used by the script engine

Source `d:\ww\ilib\dynarr.cpp`. Reference build `joan.exe`. Used for the token array, statement lists, variable lists, RPN lists, Choice strings
and the script cache ([SceToken](SceToken.md), [SceStatement](SceStatement.md), [SceScript](SceScript.md)). Size on the heap: `operator_new(0x1D)` (29 bytes).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | capacity | allocated elements; initial = `grow` (4th ctor arg 0) | ctor 0x5843D0 |
| 0x04 | 4 | i32 | grow | growth step (3rd ctor arg) | 0x5843D0, 0x584510 `grow(capacity + grow)` |
| 0x08 | 4 | i32 | cur | **cursor**, 1-based, 0 = empty; `push` sets `cur = count` | 0x584620, 0x4B9D40, 0x585660 |
| 0x0C | 4 | i32 | count | number of elements | 0x417AD0, 0x584620 |
| 0x10 | 4 | i32 | stride | element size in bytes (2nd ctor arg) | 0x5843D0 |
| 0x14 | 4 | i32 | (unknown) | initialised to -1 | 0x5843D0 |
| 0x19 | 4 | void* | data | element storage `malloc(stride * capacity)` (tracked allocator 0x58E7B0, file `dynarr.cpp`); **unaligned** | 0x5843D0 |

Constructor `0x5843D0(this, elem_size, grow, capacity=0)`. Instances in the script engine:

| use | elem size | grow |
|---|---|---|
| token array | 0x0C | 0x1E |
| statement list (`container+0x14`), variable list (`+0x18`), id markers (`+0x1C`), RPN list (`Math+0x20`), operand-index scratch | 4 (pointer / int) | 1 (RPN and token scratch: 0x14) |
| Choice strings (`Choice+0x14`) | 0x191 (401, a script string) | 1 |
| script cache table (`dynarrb`, 0x648AA0) | 0x10C | 0x1E |

Operations (all 1-based):

| function | behaviour |
|---|---|
| `push(&elem)` 0x584620 | `count++; cur = count;` grows if `capacity < count`; copies `stride` bytes (zero-fills when the source is NULL) |
| `at(i)` 0x41B3F0 | element value (first 4 bytes) of index i, 0 if `i < 1 || i > count` |
| `at_ptr(i)` 0x417200 | address of element i, 0 if out of range |
| `current()` 0x585F20 / `current_ptr()` 0x4B9C30 | element value / address at `cur`; 0 if `cur == 0` |
| `count()` 0x417AD0 / `cursor()` 0x417520 | `+0x0C` / `+0x08` |
| `rewind()` 0x4B9D40 | `cur = (count < 1) ? count : 1` |
| `set_cursor(i)` 0x585660 | `cur = i` if `1 <= i <= count` |
| `advance()` 0x63EE20 | `if (cur < count) { cur++; return true; } return false` (the cursor never moves past the last element) |
| `remove(i)` 0x584A10 | delete element i (callers also destroy the object first) |

The cursor is the **program counter** of the script engine: every container keeps its own cursor in its statement list, which is how suspended `WAIT`s resume.
