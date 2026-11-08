# SceScript — the `.sce` Script object, compile driver and include cache

Reference build `joan.exe`. Slot table of the vtable 0x9DA9C8 was first listed in [functions/video.md](../functions/video.md) section 8 and
[EsScriptSlot.md](EsScriptSlot.md) (names there: `script_*` / `EsScript`); this page supersedes the field notes there. Base classes: [SceStatement](SceStatement.md).
Tokens: [SceToken](SceToken.md). Language: [scripting/LANGUAGE.md](../../scripting/LANGUAGE.md).

## 1. Object layout (class id 1, `operator new(0x70)`, ctor 0x63F190, vtable 0x9DA9C8)

Derives from the container base (0x20 bytes, see SceStatement section 2).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00..0x1F | | | container base | vtbl, parent, class_id = 1, value type 0x22 / value 0, `stmts` +0x14, `vars` +0x18, `ids` +0x1C | 0x63E550, 0x63E6E0, 0x63F190 |
| 0x20 | 0x44 | char[] | name | the quoted script name read from the file's first token (e.g. `S1L1.sce`); `strcpy`, **no bound check** (field ends at 0x63) | 0x649860 (`strcpy(this+0x20, ...)`), 0x63F580 reads it |
| 0x64 | 4 | dlgres* | dlg | companion `<path-without-extension>EX.res` text table, created by `script_attach_ex_res` 0x63F580 when the file exists (`0x590B80`), else 0 | 0x63F580, freed 0x63F4E0 |
| 0x68 | 4 | pool* | arena | compile-time allocation arena (8-byte object from 0x5A1730), created on first compile | 0x649280 |
| 0x6C | 4 | i32 | arena_count | live allocation counter of that arena (incremented by 0x63E450, decremented by 0x63E4F0) | 0x63E450 / 0x63E4F0 |

Init and Reset blocks (classes 3, 4) derive from this class and add `+0x70 u8 done`.

Parse-time arena: while a script is compiled, globals `DAT_00A6A0BC` (arena) / `DAT_00A6A0C0` (counter pointer) redirect `operator new` for statements (0x63E450) into the
arena; they are set from `this+0x68/+0x6C` by 0x649280, zeroed around a nested `SCRIPT()` object allocation (0x645DE0) and restored afterwards. A port does not need an arena.

## 2. Methods (vtable 0x9DA9C8)

| slot | address | name | behaviour |
|---|---|---|---|
| 0 | 0x63F200 | dtor | clear (0x63F250 -> 0x63F2C0), delete self if `flags & 1` |
| 1 | 0x63E630 | set_parent | `this+4 = p` |
| 2 | 0x63F2C0 | clear | swap in the arena globals, destroy `vars` / `stmts` / `ids`, `dlg` (0x63F4E0), then the arena (0x63F530), restore the globals |
| 3 | 0x649060 | run_step(ctx) | see section 3 |
| 4 | 0x649140 | parse(tokens) | see section 4 |
| 5 | 0x643700 | find_by_id | reverse scan of `stmts` for class 2 with id (0x642540), else parent |
| 6 | 0x63EE70 | find_by_name | reverse scan of `vars` by name (0x7C07B0), else parent |
| 7..11 | 0x63F140, 0x642470, 0x6424D0, 0x6424B0, 0x641C30 | value_ptr, value_type, self, null, memory_size | |

### 2.1 `run_step(ctx)` 0x649060 (the interpreter entry)

```
if stmts.count == 0: return 1
loop:
    s = stmts.current()                 // statement under the cursor
    r = s->exec(ctx)
    if r == -1: stmts.rewind(); return -1
    if r ==  0: return 0                // suspended, cursor stays on s
    if r == 1 and !stmts.advance():     // advance() fails on the last statement
        stmts.rewind(); return 1        // pass complete, next call starts at statement 1
    // r == 1 and advanced -> next statement;  r == 2 (goto) or any other value -> re-run the statement now under the cursor
```
Observations: a pass over all top-level statements is one `run_step` unless a statement suspends; after a completed pass the cursor is rewound, so the **next call
re-executes the whole script from the top** (this is how the tactic/finish scripts poll their conditions every tick). `exit` ends the pass with -1 (also rewound).

## 3. Compile driver `0x649280 sce_compile(this = Script, text, name_for_messages)` -> bool (returned in the low byte)

1. `this->clear()` (slot 2); store `"0 error(s), you're good to go!"` in the last-message buffer `DAT_00A6A118` (0xFF chars, 0x6498A0).
2. Build the token stream (0x661EA0); set `DAT_00A6A218 = name` (prefix of every parse-error message); create the arena on first use; redirect the allocation globals.
3. Run the lexer `0x6840D0(text)` (state machine, LANGUAGE.md section 2) which pushes each lexeme through `on_token`; rewind the stream, line counter = 1 (0x6498E0).
4. `this->parse(stream)` (slot 4, 0x649140).
5. Restore globals, destroy the token stream, return true.

Exceptions thrown by token-level code are C++ exceptions and are caught in 0x649280 (all three call `script_clear`, restore the globals, show the text in a message box with
0x586BC0 — non-fatal — and return false):
* `Catch@6493A0` — a **token pointer** (thrown by 0x662330 when a string token does not end with `"`): text `Bad token\n<first 18 chars>...\nLine number : <line>`;
* `Catch@6494C2` — a `const char*` ("Unexpected end-of-file.", "Expecting int.", "Expecting float.", "Expecting XYZ(...)!", "Vector components must be seperated by a comma." ...): that text
  plus `\nLine number: <stream line counter>\nError token string: <current token>\nError token type: <kind>`;
* `Catch@6495CC` — anything else: "Unhandled exception in ScriptStatementCompile::compile." plus the same suffix.
The failure text is also copied into `DAT_00A6A118`.

Parse-level errors (the "Expecting ..." family) do **not** throw: they call `sce_report_error` 0x63EB80, which formats `"<name>" + separator(0x9D05F0) + "line:%d " + message` into
`DAT_00A69F38` and calls `0x586CB0`: `OutputDebugString`, `MessageBoxA(..., "Wars & Warriors: Joan of Arc", MB_ICONERROR)`, then `FUN_007B47FC(0xFFFFFFFE)`
(almost certainly `exit(-2)`): **the process terminates**. A re-entrancy guard (`DAT_00A64528`) makes later errors silent.

## 4. Script file parse (0x649140)

`>= 2 tokens` else error "This script is too small to be valid."; first token must be a string (kind 3) -> `name` ("Must name script."); next must be `{` (kind 4)
("Missing opening brace for Script block."); then until a `}` (kind 5) is the current token: `stmt = make_statement(kind, line)` (0x643550) and `stmt->parse(stream)`.
After the loop the statement cursor is rewound; the closing `}` and anything after it are never read.

## 5. Include cache for `SCRIPT("file")` (see also [EsScriptSlot.md](EsScriptSlot.md))

Global table of slots (dynarrb, element 0x10C = 268): `{ i32 refcount; char name[260]; SceScript* script; }` (script at +0x108). Free slot = `refcount == 0` or script == NULL (0x648E40).

| function | behaviour |
|---|---|
| 0x648C50 `slot_find(name)` | reverse scan for a non-free slot whose name compares equal (0x7C07B0); returns index or 0 |
| 0x648BD0 `slot_add(name, script)` | appends (0x5857A0), copies the name, stores the script, `refcount = 1`, returns the index |
| 0x648D20 `slot_addref(i)` | `InterlockedIncrement(refcount)` |
| 0x648D60 `slot_release(i)` | `InterlockedDecrement`; at 0 deletes the Script object (arena globals zeroed during the delete) and removes the slot |
| 0x648CE0 `slot_script(i)` | slot+0x108 |

`SceScriptCall::parse` 0x645DE0: `SCRIPT ( "name" )`; if `slot_find(name)` hits it just `slot_addref`s (**all `SCRIPT("x")` statements with the same name share one Script object**,
including its variables, cursor and `init` flags); otherwise it opens the file through the engine file helper (0x586DA0, relative to the install/base path), reads it into a heap
buffer, allocates a fresh Script (outside the arena), `set_parent(0)` (an included script cannot see the includer's variables), registers the slot, compiles it with 0x649280 (`name` = the quoted path)
and then loads `<path up to the first '.'>EX.res` (0x63F580). The statement's previous slot (if the node is re-parsed) is released. A missing file raises "Cannot open script" (fatal, via 0x63EB80).

## 6. Who drives it (other pages)

| caller | what it does |
|---|---|
| `battle::fn_0041CC80` | per scenario record (stride 0x1B2): compiles `SCRIPT\SCENARIO\<name+0x82>.sce` into the Script stored at `record+0x186`, attaches `EX.res`; compiles `<name+0xC3>.sce` (the `*_des` description script), calls its `run_step(0)` **once** and deletes it |
| `FUN_0041C810` (scenario start) | slot 9 `self()` (no-op), cursor rewind (0x63ED40), then one `run_step` before the first frame |
| `FUN_00471620` (per game update) | unless `DAT_00A500C2 == 5` or `DAT_00A59298 == 2`, calls `run_step(0)` on the current scenario Script (`record+0x186`, or a second Script object held by the caller when its `+0x17` field is 0); the return value is ignored |

The `ctx` argument is 0 everywhere found; it is only forwarded to `exec` calls.

## 7. Globals

| address | meaning |
|---|---|
| 0xA6A0BC / 0xA6A0C0 | current allocation arena / counter pointer |
| 0xA6A218 | script name prefix for parse-error messages |
| 0xA6A118 | last compile message (256 bytes) |
| 0xA69F38 | formatted parse-error text buffer |
| 0xA64528 | error-box re-entrancy flag |
| 0xA6A268 | keyword table already sorted |
| 0xA6A114 | counter incremented by every `SCRIPT()` parse |
| 0xA6A254 | `FUNC("pause_script", n)` resume time: `now + n` (0x65C4A0); poll consumer not exported |
