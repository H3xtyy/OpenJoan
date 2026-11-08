# SceStatement — compiled `.sce` statement objects (value base, container base, 27 classes)

Reference build `joan.exe`. Compile-time creation: factory `0x640DB0` (class id -> `operator new` via 0x63E450 + constructor + `set_parent`), token-kind mapping
`0x6488D0`, list append `0x643550` / `0x63EA20`. Language semantics: [scripting/LANGUAGE.md](../../scripting/LANGUAGE.md). Script root object: [SceScript](SceScript.md).
Function table: [functions/script_vm.md](../functions/script_vm.md). Arrays: [SceDynArr](SceDynArr.md).

## 1. Value base class (vtable 0x9DA944, ctor 0x63E550, 0x14 bytes)

Every statement, expression operand, variable and constant derives from it. The compiled script is a tree of these objects; **run-time state (variable values, cursors,
`init` done flags) lives inside the objects**, there is no separate frame.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | vtable* | vtbl | see section 4 | ctors |
| 0x04 | 4 | SceStatement* | parent | enclosing container (0 for a top-level `Script`); set by slot 1 `set_parent` (0x63E630) at creation | 0x63E630, 0x640DB0 `(vt+4)(parent)` |
| 0x08 | 4 | i32 | class_id | statement class 1..0x1B (section 3); ctor default -1 | 0x63E550 (`param_1[2] = -1`), each ctor overwrites |
| 0x0C | 4 | i32 | value_type | 1 = int, 2 = float, 3 = string, 0x1B = vector, 0x22 = no value (ctor default) | 0x63E550 (`param_1[3] = 0x22`), 0x63E660, 0x63F140 |
| 0x10 | 4 | union | value | type 1: `i32` inline; type 2: `f32` inline; type 3: `char*` to a **401-byte buffer** (`operator new(0x191)`, capacity 400 chars + NUL, writes via `strncpy(...,400)` + `[400]=0`); type 0x1B: `float*` to 12 bytes (`operator new(0xC)`, not zeroed) | 0x63E660 frees the pointer for every type except 1 and 2; 0x644900, 0x643F80 |

`value_ptr()` (slot 7, 0x63F140) returns `this+0x10` for types 1/2 and the stored pointer otherwise.

## 2. Container base class (vtable 0x9DA980, ctor 0x63E6E0, 0x20 bytes; derives from the value base)

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x14 | 4 | SceDynArr* | stmts | child statements (pointers, 1-based, **cursor = program counter**). Created lazily (`operator new(0x1D)` + dynarr(4,1)) by 0x63EA20 / 0x643550 | 0x63ECD0 count, 0x63EA20 |
| 0x18 | 4 | SceDynArr* | vars | variable declarations (class 0x11 objects) of this scope | 0x63EF50, 0x63F100 |
| 0x1C | 4 | SceDynArr* | ids | `Id` marker objects (dialog grammar, only created on the root) | 0x63F8A0 |

Destructor chain: 0x63E780 -> 0x63E7F0 (deletes ids, statements, variables, then 0x63E660 frees the value).

## 3. The 27 classes

`Create` = constructor; `exec` = vtable slot 3 (offset +0x0C), `parse` = slot 4 (+0x10). "Size" is the `operator new` argument. Values were read from the constructors
(class id stored at `+8`) and from address adjacency of exec/parse pairs (exec directly precedes parse in the binary); vtable addresses are from the constructors.
The exec status codes are explained in section 5.

| id | name (proposed) | size | ctor / vtable | exec | parse | extra fields | role |
|---|---|---|---|---|---|---|---|
| 1 | SceScript | 0x70 | 0x63F190 / 0x9DA9C8 | 0x649060 `run_step` | 0x649140 | see [SceScript](SceScript.md) | root of a file; also used standalone by `SCRIPT()` |
| 2 | SceId | 0x25 | 0x6424F0 / 0x9DAD34 | 0x63F790 (block run) | 0x63F8A0 | container; `+0x20 u8` (0), `+0x21 i32 id` (getter 0x642540, setter 0x642560) | **dialog grammar** `ID n { ... }` |
| 3 | SceInit | 0x71 | 0x642630 / 0x9DAD70 (derives from SceScript) | 0x63FB80 | 0x63FCA0 | `+0x70 u8 done` | `init { ... }` run-once block (**.sce**) |
| 4 | SceReset | 0x71 | 0x642700 / 0x9DADAC | 0x63FDE0 | 0x63FF00 | `+0x70 u8 done` | `reset { ... }`, same code as Init (dialog grammar) |
| 5 | SceSpeaker | 0x18 | 0x6427D0 / 0x9DADE8 | 0x640040 (returns 1) | 0x640060 | `+0x14 i32` = result of 0x5270B0(string) | dialog grammar `SPEAKER : "name"` |
| 6 | SceUnit | 0x18 | 0x642890 / 0x9DAE24 | 0x640150 (returns 1) | 0x640170 | `+0x14 i32` = 0x5270B0(string) | dialog grammar `UNIT : "name"` |
| 7 | SceObject | 0x18 | 0x642950 / 0x9DAE60 | 0x640260 (returns 1) | 0x640280 | `+0x14 i32` = 0x4A36C0(string) | dialog grammar `OBJECT : "name"` |
| 8 | SceWait | 0x21 | 0x642A10 / 0x9DAE9C | 0x640370 | 0x6404E0 | container; `+0x20 u8 started` | `WAIT { calls / assignments }` (**.sce**) |
| 9 | SceGoto | 0x18 | 0x642AE0 / 0x9DAED8 | 0x640690 | 0x6408F0 | `+0x14 i32` target id | dialog grammar `GOTO : n` |
| 10 | SceExit | 0x14 | 0x642BA0 / 0x9DAF14 | 0x6409D0 (returns -1) | 0x6409F0 | - | `exit` (**.sce**) |
| 11 | SceChoice | 0x18 | 0x640A50 / 0x9DAC7C | 0x640BB0 (returns 1) | 0x640BD0 | `+0x14 SceDynArr*` of 401-byte strings | dialog grammar `CHOICE { "a", "b" }`; dtor 0x640AF0 |
| 12 | SceIf | 0x24 | 0x648770 / 0x9DB69C | 0x6456F0 | 0x645A40 | container; `+0x20` pointer deleted by dtor 0x648810, never written by the parser | `if (...) {...} [else ...]` (**.sce**) |
| 13 | SceElse | 0x20 | 0x642C60 / 0x9DAF50 | 0x648560 (block run) | 0x648630 | container | marker child inside SceIf; a stray `else` is parsed as an unconditional block |
| 14 | SceCompare | 0x20 | 0x642D20 / 0x9DAF8C | 0x644D80 | 0x645210 | `+0x14 i32 op`, `+0x18/+0x1C` Operand* (dtor 0x642DB0 -> 0x6452F0) | **unreachable legacy** (`a < b`); only created by SceCondition |
| 15 | SceCondition | 0x20 | 0x642E20 / 0x9DAFC8 | 0x6453A0 | 0x6454E0 | `+0x14 i32 op` (0x22 none), `+0x18 left`, `+0x1C right` (dtor 0x642EB0 -> 0x644CD0) | **unreachable legacy** (`( a < b ) && c`); the parser never creates it |
| 16 | SceOperand | 0x1C | 0x6447A0 / 0x9DB3BC | 0x644900 | 0x644B10 | `+0x14` SceVar* (identifier operand), `+0x18` sub-statement (constant / call / script call / math); dtor 0x644850 | value wrapper: every call argument, assignment RHS, math operand |
| 17 | SceVar | 0x48 | 0x642F20 / 0x9DB004 | 0x6439B0 (returns 1) | 0x6439D0 | `+0x14 char name[0x34]` (copied by `strcpy`, no bound check) | variable declaration `INT/FLOAT/STRING/VECTOR name` (**.sce**) |
| 18 | SceAssign | 0x1C | 0x6443D0 / 0x9DB318 | 0x644530 | 0x644680 | `+0x14` SceVar* (target), `+0x18` SceOperand* (RHS); dtor 0x644480 | `name = operand` (**.sce**) |
| 19 | SceConst | 0x14 | 0x643300 / 0x9DB150 | 0x643800 (returns 1) | 0x643820 | value stored in the base | literal int / float / string / `XYZ(...)` |
| 20 | SceCall | 0x18 | 0x643E30 / 0x9DB25C | 0x643F80 | 0x644190 | `+0x14` inner call object (class 0x15..0x18), dtor 0x643ED0; slots 7 / 8 / 13 forward to the inner object (0x644330, 0x644360, 0x644390) | statement or operand `FUNC/DIALOG/CAMERA/ACTION(...)` (**.sce**) |
| 21 | SceFunc | 0x22 | 0x643000 / 0x9DB040 (container) | 0x643B60 | 0x643C00 | `+0x20 u16 builtin id`; children = argument Operands | `FUNC` call object |
| 22 | SceDialog | 0x22 | 0x6430C0 / 0x9DB084 (derives from SceFunc) | 0x643B60 (shared with FUNC) | 0x643C00 (shared) | same | `DIALOG` call object |
| 23 | SceCamera | 0x22 | 0x643180 / 0x9DB0C8 | shared (slot 3 = 0x643B60 per the same pattern) | 0x643C00 (shared) | same | `CAMERA` call object (no shipped script uses it) |
| 24 | SceAction | 0x24 | 0x643240 / 0x9DB10C | shared | 0x643D40 (calls 0x643C00, then resolves the 2nd argument of ids 0x143, 0x146, 0x147 at parse time) | `+0x20 u16 id`, `+0x22 u16 enum` (order / task enum from 0x4BBBC0 / 0x4E4610) | `ACTION` call object |
| 25 | SceVecLit | 0x20 | 0x6433C0 / 0x9DB18C | 0x6437B0 (returns 1) | 0x6437D0 (= 0x6620D0 into `+0x14`) | `+0x14 f32[3]` | never created by the parser (XYZ literals are SceConst of type 0x1B) |
| 26 | SceScriptCall | 0x18 | 0x643480 / 0x9DB1C8 | 0x645D40 | 0x645DE0 | `+0x14 i32` script-cache slot; dtor 0x645CA0 releases the slot | `SCRIPT("file")` include (**.sce**) |
| 27 | SceMath | 0x24 | 0x646290 / 0x9DB578 | 0x6463F0 | 0x647C90 | container (operands are its children); `+0x20 SceDynArr*` RPN list; dtor 0x646330 | parenthesised expression `( a + b ... )`, the `if` condition (**.sce**) |

Token kind -> class (0x6488D0): `0x0B->2, 0x0C->3, 0x0D->4, 0x0E->8, 0x0F->0xB, 0x10->0xC, 0x11->0xD, 0x12->5, 0x13->6, 0x14->7, 0x15->9, 0x16->0xA, 0x17..0x1A->0x14,
0x1C->0x1A, 0x21->0x12`; any other kind prints `"Token type does not correspond to a statement!"` (fatal, see LANGUAGE.md) and the caller then dereferences a NULL pointer.
Kinds 0x1D..0x20 never reach the factory: `0x643550` creates a SceVar (class 0x11) in the container's variable list via 0x63EF50 instead.

## 4. Virtual table layout (shared by all classes)

Slots are 4 bytes. Vtable spacing in the binary: value base 0x9DA944 -> container base 0x9DA980 = 0x3C (15 slots); 0x9DA980 -> script 0x9DA9C8 = 0x48; most
statement classes 0x3C apart; the call classes 0x9DB084 / 0x9DB0C8 / 0x9DB10C / 0x9DB150 0x44 apart (17 slots). Slots above 13 are unidentified.

| slot | offset | name | notes |
|---|---|---|---|
| 0 | 0x00 | `dtor(flags)` | deleting destructor |
| 1 | 0x04 | `set_parent(p)` | 0x63E630 |
| 2 | 0x08 | `clear()` | 0x63F2C0 for SceScript |
| 3 | 0x0C | `exec(ctx) -> status` | the interpreter; status codes in section 5 |
| 4 | 0x10 | `parse(stream)` | compile-time; `stream` is the token stream (passed as the implicit register argument, `param_2` in the pseudocode) |
| 5 | 0x14 | `find_by_id(id)` | SceScript 0x643700 (scan statements backwards for class 2 with `id`), otherwise delegate to parent (0x6436B0) |
| 6 | 0x18 | `find_by_name(name)` | container 0x63EE70 scans `vars` backwards with `FUN_007C07B0(name, var+0x14)`, then asks the parent; non-containers delegate (0x63EDD0) |
| 7 | 0x1C | `value_ptr()` | 0x63F140 |
| 8 | 0x20 | `value_type()` | returns `+0x0C` (0x642470) |
| 9 | 0x24 | `self()` | returns `this` |
| 10 | 0x28 | `null()` | returns 0 |
| 11 | 0x2C | `memory_size()` | diagnostic only (0x641A60 family) |
| 12 | 0x30 | `call_impl()` | call classes only: the builtin switch (FUNC = 0x649920); writes the result into `this+0x10` |
| 13 | 0x34 | `done(ctx) -> bool` | call classes: "is the started action finished?" used by `WAIT` via the non-virtual wrapper 0x644390. Bodies (a vtable dump): FUNC 0x9DB040 slot12 0x649920 / slot13 0x6509E0; DIALOG 0x9DB084 slot3 0x643B60 (inherited), slot12 0x65FD70 / slot13 0x660E60; CAMERA 0x9DB0C8 slot12 0x661020 / slot13 0x661040; ACTION 0x9DB10C slot12 0x661060 / slot13 0x6612E0. Semantics belong to the builtins pages. Vtables of SceVar (0x9DB004) and SceCall (0x9DB25C) have only 12 slots, so a `WAIT` child of class Assign reads slot 13 past the end of the SceVar vtable (undefined behaviour in the original; only occurs in commented-out code) |

## 5. Exec status codes

| value | meaning |
|---|---|
| 1 | finished, caller advances to the next statement |
| 0 | not finished, do **not** advance; the caller returns 0 up the chain and the same statement is re-entered on the next `run_step` (coroutine suspend) |
| -1 | abort: caller rewinds its cursor to 1 and returns -1 up the chain (`exit` produces it) |
| 2 | jump taken (only `GOTO`); the new cursor is already set |
| -2 (0xFFFFFFFE) | internal error (type mismatch in an expression, unknown goto target); handled only by `SceIf` (treated as 1); every other loop ignores it and spins on the same statement |

Per-class handling of a child's status is tabulated in [LANGUAGE.md](../../scripting/LANGUAGE.md) section 5.
