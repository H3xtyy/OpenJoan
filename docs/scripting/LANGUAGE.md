# `.sce` scenario language — lexical level, grammar, execution model

Scope: lexical level, grammar, evaluator and coroutine model of the original parser/VM.
Where the binary and the census disagree, **the binary wins**; all statements below carry the `joan.exe` address they were read from
(function table: [re/functions/script_vm.md](../re/functions/script_vm.md); layouts: [SceToken](../re/structs/SceToken.md), [SceStatement](../re/structs/SceStatement.md),
[SceScript](../re/structs/SceScript.md), [SceDynArr](../re/structs/SceDynArr.md)). Builtin names/arities: [BUILTINS.md](BUILTINS.md),
[ScriptBuiltinTable](../re/structs/ScriptBuiltinTable.md). Evidence for the shipped corpus: all 42 files in `$JOAN_GAME_DIR/script/scenario` (CRLF, Windows-1250 in the
Polish install). Items marked `TODO(re)` need a dump the exports do not contain.

## 1. Overview

A file is one script: a quoted name, `{`, a list of **items**, `}`. The language has no loops, no user functions and no `and/or/not`; control flow is
`if`/`else`, `exit`, the blocking `WAIT { ... }` and the include `SCRIPT("file")`. All side effects go through builtin calls (`FUNC`, `ACTION`, `DIALOG`, `CAMERA`).
Scripts are *compiled once* into a tree of C++ objects (statements) and *interpreted every game update* (section 5). Variables live inside the tree.

## 2. Lexical level

### 2.1 Tokenizer algorithm (0x6840D0, then 0x6622D0 / 0x662330)

The lexer is a 4-state machine over the raw bytes (Windows-1250 in the original; the algorithm is byte-oriented, so the same rules apply to UTF-8 text).
`isspace` = space, `\t`, `\n`, `\v`, `\f`, `\r`.

| state | on a non-space byte | on a space byte |
|---|---|---|
| 1 *between lexemes* | store the byte as the first byte of a new lexeme; if it is `/` and the next byte is `/` go to 4 (comment); if it is `"` go to 3 (string); otherwise go to 2 (word). **The first byte is not tested for being a delimiter** | skip |
| 2 *word* | if the byte is one of `( ) , " { } / :` it ends the word: emit the pending word; then, if the byte is `"` start a string (state 3), else emit the one-byte lexeme and stay in state 2 with an empty word (a `//` pair here switches to state 4 **and drops the pending word**). Any other byte is appended | emit the pending word, go to 1 |
| 3 *string* | append (only while < 0x100 bytes are stored); a `"`, `\r` or `\n` terminates and emits the lexeme (quotes included) | (spaces are part of the string) |
| 4 *comment* | skip to the `\n`, then clear the word buffer and go to 1 | |

Consequences a reimplementation must reproduce (they are what the shipped scripts are written against):
* Operators are **not** delimiters. `a==b`, `x=5`, `x+1` are each one word and fail. Every operator, `=`, `-` and parenthesised sub-expression needs whitespace around it. Only `( ) , { } / :` and `"` split words.
* A lexeme that *starts* with a delimiter byte glues the following non-delimiter bytes to itself: `(joan_recno` is one lexeme; `( joan_recno` is fine. After a `"..."` string, `,x` is one lexeme but `, x` is fine. All shipped scripts put a space after a leading `(` `)` `,` `{` `}`.
* `//` starts a comment only at the beginning of a lexeme or after a delimiter; `word//c` loses `word`. No shipped file does this.
* Strings have no escapes and may not contain `"`; a newline inside a string makes the lexeme unterminated. Stored text is at most 0x100 bytes *including both quotes* (= 254 characters of content); a longer string is truncated, loses its closing quote and is rejected as `Bad token`.
* Words are not length-checked (the stack buffer is 260 bytes).
* The line counter starts at 1 and increments on every `\n`, wherever it occurs (also inside comments and strings).
* A file that ends inside a `//` comment emits a stray `/` lexeme (a division token) at the end; harmless because the parser stops at the script's closing `}`.

Classification of each lexeme (0x662330): (1) look it up in the 44-entry keyword/punctuation table (`bsearch`, comparison function `FUN_007C07B0`, table sorted by `qsort` on first use);
(2) otherwise if the first byte is a digit **or `-`**: a number — float if the text contains `.`, else integer; the value is `atoi`/`atof` of the text, so trailing junk is ignored (`2000*aliveCount` is the integer 2000, `0;` is 0, `-77` is one token,
`-45.375` is one token, `-x` is the integer 0); (3) first byte `"`: string, quotes removed; (4) first byte a letter or `_` and all bytes letters/digits/`_`: identifier; (5) anything else keeps kind 0x22 and is a parse error wherever it appears.
Because of rule (1) the bare lexeme `-` is the minus operator, while `-5` is a literal. There is no unary minus, no unary `!`, no `++`/`--`.

### 2.2 Token classes

| kind | spelling | notes |
|---|---|---|
| identifier (0x21) | `[A-Za-z_][A-Za-z0-9_]*` | variable names; **any keyword (below) is not an identifier** |
| keyword | see 2.3 | table in the exe is upper case; lookup is case-insensitive (see 2.3) |
| integer (1) | `-?[0-9]...` | `atoi`: 32-bit, wraps on overflow |
| float (2) | `-?[0-9]+.[0-9]+` | 626 in the install, e.g. `15021.0`, `-45.375`; stored as `f32` |
| string (3) | `"` any bytes except `"` and newline `"` | no escapes; localised text and `[0001N]`-style dialog ids are plain text |
| punct | `{ } = , :` and `( ) + - * / ^ < > <= >= == != && \|\|` (`^` = power) | token kinds in [SceToken](../re/structs/SceToken.md); `:` is only used by the unused dialog grammar |
| comment | `//` to end of line | only at lexeme start; no block comments exist; 6902 in the install |
| end of file | | the parser never reads past the last token |

Positions: `line` is 1-based. The original reports only the line.

Punctuation found in the install (token counts from a census of the install): `,` 54419 · `(` 28003 · `)` 28003 · `{` 5785 · `}` 5785 · `=` 5326 · `==` 2348 · `>` 1200 · `&&` 975 · `-` 509 ·
`!=` 143 · `||` 136 · `+` 79 · `<` 77 · `>=` 13 · `<=` 9 · `*` 7 · `;` 2 (the two `;` are glued to the preceding number, `QG_one_recno = 0;` lexes as `0;` = 0; a `;` after `)` would be an unknown lexeme).
The exe has **no `!` and no `%` token** (an earlier census suggested the lexer "additionally accepts `!`, `/` and `%`"; it does not); the operator `^` at precedence 6 evaluates `pow`, see 6.2.

### 2.3 Keywords (case-insensitive, reserved)

Kinds 0x0B..0x20 (the first 12 are verified by their parse functions, the call keywords by the error text order, the type keywords by 0x6439D0):

| kind | keyword | role |
|---|---|---|
| 0x0B | `ID` | dialog grammar (unused) |
| 0x0C | `INIT` | run-once block |
| 0x0D | `RESET` | dialog grammar (unused) |
| 0x0E | `WAIT` | blocking block |
| 0x0F | `CHOICE` | dialog grammar (unused) |
| 0x10 / 0x11 | `IF` / `ELSE` | conditional |
| 0x12 / 0x13 / 0x14 | `SPEAKER` / `UNIT` / `OBJECT` | dialog grammar (unused) |
| 0x15 | `GOTO` | dialog grammar (unused) |
| 0x16 | `EXIT` | abort the current pass |
| 0x17 / 0x18 / 0x19 / 0x1A | `FUNC` / `DIALOG` / `CAMERA` / `ACTION` | builtin calls |
| 0x1B | `XYZ` | vector literal `XYZ(x, y, z)` |
| 0x1C | `SCRIPT` | include |
| 0x1D / 0x1E / 0x1F / 0x20 | `INT` / `FLOAT` / `STRING` / `VECTOR` | variable declaration |

The shipped scripts write `INT WAIT FUNC ACTION DIALOG SCRIPT` in upper case and `init exit if else` in lower case; because the table stores upper-case spellings the comparison function must ignore case
(`FUN_007C07B0` is the case-insensitive comparator, inferred by use). `EVENTSYSTEM` (kind 0x0A) is also in the table. `ID UNIT OBJECT RESET GOTO CHOICE SPEAKER CAMERA FLOAT STRING VECTOR XYZ EVENTSYSTEM` are therefore **reserved** and cannot be variable names
(no shipped script uses them as identifiers). All kinds above were confirmed by a dump of the table at 0xA214C8.

### 2.4 Keywords versus the census

`docs/scripting/BUILTINS.md` lists `for` ×345, `and` ×529, `not` ×170, `while`, `switch`, `case`, `break`, `continue`, `return`, `or`, `main` as keywords. They are not: the census regex matches words inside string literals.
Reproduced exactly by the lexer test (words inside strings not touching a quote or word character):

| word | census | keyword tokens in code | inside strings |
|---|---|---|---|
| INT | 4671 | 4671 | 0 |
| if | 3538 | 3456 | 82 |
| WAIT | 1797 | 1797 | 0 |
| else | 590 | 588 | 2 |
| exit | 40 | 38 | 2 |
| init | 42 | 42 | 0 |
| for 345, and 529, not 170, or 34, return 34, while 16, continue 9, break 7, switch 4, case 2, main 2 | | 0 | all |

Code-only keyword counts: `FUNC` 20304, `ACTION` 2450, `DIALOG` 1286, `SCRIPT` 19, `WAIT` 1797 (`WAIT {` forms: `DIALOG` 902, `FUNC` 840, `ACTION` 54; the single `WAIT { dialog_recno = DIALOG(...) }` is inside a comment).
`FLOAT`, `STRING`, `VECTOR`, `XYZ`, `CAMERA`, `GOTO`, `RESET`, `CHOICE`, `SPEAKER`, `UNIT`, `OBJECT`, `ID` never occur in code: only `INT` variables are exercised by the shipped scripts.

### 2.5 Corrections to the census reading

* A sign belongs to the number token (`-77`, `-45.375`, `-5`); the reading "a sign is a separate `-` token" is wrong. A lone `-` is subtraction.
* `if` without braces does not exist and would not parse: the `if` body must be `{ ... }` (the old example on this page was illustrative, not from a file).
* Keywords are case-insensitive and there are 22 of them, not 10.
* `!`, `%` do not exist; `/` is division, a pow operator exists at the highest precedence.

## 3. Grammar (EBNF, reconciled against the parse functions)

Terminals in quotes are keywords (any case) or punctuation; `STRING`, `INT_LIT`, `FLOAT_LIT`, `IDENT` are token classes.

```
script        = STRING "{" { item } "}" ;                       (* 0x649140; the string is the script's own name, not a path *)
item          = vardecl | statement ;                           (* 0x643550: variable keywords are routed to the scope's variable list *)
vardecl       = ( "INT" | "FLOAT" | "STRING" | "VECTOR" ) IDENT ;   (* 0x6439D0; no initialiser *)
statement     = init | wait | if | exit | assign | call | scriptcall | dialog_stmt ;
init          = "INIT" "{" { item } "}" ;                       (* 0x63FCA0 *)
wait          = "WAIT" "{" { call | assign } "}" ;              (* 0x6404E0 *)
if            = "IF" math "{" { item } "}" [ "ELSE" ( if | "{" { item } "}" ) ] ;   (* 0x645A40 *)
exit          = "EXIT" ;                                        (* 0x6409F0 *)
assign        = IDENT "=" operand ;                             (* 0x644680 *)
call          = ( "FUNC" | "DIALOG" | "CAMERA" | "ACTION" ) "(" STRING { "," operand } ")" ;   (* 0x644190, 0x643C00 *)
scriptcall    = "SCRIPT" "(" STRING ")" ;                       (* 0x645DE0 *)
operand       = INT_LIT | FLOAT_LIT | STRING | vector | call | scriptcall | IDENT | math ;   (* 0x644B10 *)
vector        = "XYZ" "(" number "," number "," number ")" ;    (* 0x6620D0; number = INT_LIT | FLOAT_LIT, ints are converted *)
math          = "(" mexpr ")" ;                                 (* 0x647C90 *)
mexpr         = mterm { binop mterm } ;
mterm         = INT_LIT | FLOAT_LIT | STRING | vector | functioncall | scriptcall | IDENT | "(" mexpr ")" ;
functioncall  = "FUNC" "(" STRING { "," operand } ")" ;        (* inside math only FUNC (kind 0x17) is accepted as a call *)
binop         = "+" | "-" | "*" | "/" | "^" | "<" | ">" | "<=" | ">=" | "==" | "!=" | "&&" | "||" ;
```

Notes:
* The name token of a call is a **string**: `FUNC("set_unit_pos", ...)`; it is resolved at compile time against the builtin table (case-insensitive, 407 entries); an unknown name is a fatal parse error (`Wrong function name (%s)!`). The keyword (`FUNC`/`ACTION`/`DIALOG`/`CAMERA`) only selects the call class, **not** the name set: the same table serves all four.
* A bare arithmetic expression is not an operand: `x = a + 1` does not parse; write `x = ( a + 1 )`. Inside `math` there is no unary operator: every position alternates operand / binary operator (checked at parse time, `Error: Illegal expression.`); parentheses must balance.
* The `if` condition is a `math` (so the parentheses are mandatory): `if ( FUNC("is_game_finish") )` and `if ( a == 1 && ( b > 2 ) )` are both valid.
* `else` is recognised only immediately after the `}` of the `if` body. `else if` nests a new `if` object inside the first one's child list (flat list, section 5.4).
* `WAIT` children are only `FUNC/DIALOG/CAMERA/ACTION` calls and assignments; anything else before `}` is the fatal error `Expecting '}' after Wait.`.
* Variables must be declared **before** use in the file: names are resolved while parsing (`Variable not found ! (%s)`, fatal). A declaration is visible in the enclosing block and everything nested in it; an included script cannot see the includer's variables.
* The script's closing `}` ends parsing; anything after it is ignored. Variables may be declared in any block (script, `init`, `if` ...); lookup is innermost-block-first, later declarations of the same name in the same block shadow earlier ones.

### 3.1 Examples (from `S1L1.sce`, checked against the grammar)

```
"S1L1.sce"
{
	INT joan_recno
	init
	{
		if( FUNC("get_scenario_stage") == 0 )
		{
			joan_recno = FUNC("load_unit", "JOAN_OF_ARC", "FRENCH")
			FUNC("set_unit_pos", joan_recno, 15021.0, 12684.0, -45.375)
		}
	}
	if( FUNC("get_scenario_stage") == 1 ) { FUNC("set_scenario_stage", 2)  exit }
	SCRIPT("script\scenario\S1L1_tactic.sce")
	WAIT { DIALOG("add_dialog", joan_recno, "[0001M]Hey, Should we attack now?") }
}
```

## 4. Types, variables, scope

* Value types: int (32-bit signed), float (IEEE-754 binary32), string (at most 400 bytes + NUL, `strncpy` clamps), vector (3 x f32). Internal tags 1, 2, 3, 0x1B ([SceStatement](../re/structs/SceStatement.md) section 1).
* `INT x` creates `x = 0`; `FLOAT x` 0.0; `STRING x` the empty string; `VECTOR x` **uninitialised** (12 bytes from `operator new`) (0x6439D0). There is no initialiser syntax; `INT a = 5` leaves `= 5` which is a fatal parse error.
* Variable names are at most 51 characters (copied with an unbounded `strcpy` into a 52-byte field; longer names corrupt the heap in the original). Names are compared with `FUN_007C07B0` (probably case-insensitive).
* A variable's value is persistent state of the compiled object: it survives across ticks and across passes. Because `SCRIPT("x")` statements with the same file name share one compiled object, variables of an included script are shared between all of its includers.
* `SCRIPT(...)` is the only way scripts communicate besides builtin side effects (`set_scenario_bit_flag`, ...).
* **Typing is dynamic and unchecked.** `name = operand` copies *by the type of the right-hand side* (ints and floats are raw 32-bit copies, no conversion; strings `strncpy` 400; vectors 12 bytes) into the variable and does not change the variable's declared type (0x644530). Assigning a float to an `INT` variable stores the float's bit pattern. No shipped script mixes types.
* A call's result type comes from the builtin table (`1` int, `2` float, `3` string); its value is whatever the builtin writes.

## 5. Execution model

### 5.1 Tick and cursors

* The engine calls `run_step(0)` (vtable slot 3 of the root Script, 0x649060) **once per game update** from the battle update (`FUN_00471620`), skipping updates while `DAT_00A500C2 == 5` or `DAT_00A59298 == 2`; once from the scenario start (`FUN_0041C810`) and once for the one-shot `*_des` description script (`battle::fn_0041CC80`). The call rate is the rate of the world update in the main loop ([TIMING.md](../re/TIMING.md): the shipped loop is variable-step and effectively uncapped, nominal 30 Hz); the language has no timer of its own and no statement depends on the call rate except through pass counting (an `INT` incremented once per pass would be rate dependent; the shipped scripts increment counters only on events) and `pause_script` (real milliseconds).
* Every container (script, `init`, `if`, `WAIT`, math, call) keeps a **cursor** (1-based, in its statement list). `run_step` executes the statement under the cursor; status 1 advances, 0 suspends **without advancing**, so the next tick re-enters the same statement, which in turn resumes its own cursor. The whole call stack is therefore a tree of cursors, i.e. the script is a set of coroutines driven by repeated polling.
* When the last statement finishes the cursor is rewound and `run_step` returns 1: **the next tick starts again at the first statement.** Scripts therefore behave like `while (true) { body; yield }` unless something suspends mid-pass; `init` and the `if` gates keep the body idempotent.
* A pass that executes `exit` returns -1 (all cursors on the path rewound), see 5.5. The caller of the main script ignores the result in the update loop.
* There is no real concurrency, no threads, no preemption; a statement that suspends blocks everything after it in its own script (and in every script that includes it) until it completes.

### 5.2 Exec status codes and what each statement class does with them

Return values of `exec`: **1** done, **0** not done (suspend), **-1** abort, **2** jump taken (GOTO only), **-2** internal error. Table of how a *container* reacts to the status of the child under its cursor:

| container | child 1 | child 0 | child -1 | child 2 | child -2 / other |
|---|---|---|---|---|---|
| Script `run_step` 0x649060 | advance; at the last one rewind, return 1 | return 0 | rewind, return -1 | re-run the statement now under the cursor (no advance) | re-run the same statement forever (spin) |
| `INIT` / `RESET` 0x63FB80 / 0x63FDE0 | advance; at the end rewind, set `done`, return 1 | return 0 | rewind, return -1 | return 2 (cursor unchanged) | spin |
| `ID` 0x63F790 / `ELSE` 0x648560 | advance; at the end rewind, return 1 | return 0 | rewind, return -1 | return 2 | spin (Id) / advance (Else) |
| `IF` 0x6456F0 | advance (at the Else marker or the end: rewind, return 1) | return 0 | rewind, return -1 | rewind, return 2 | advance (treated like 1) |
| `SCRIPT()` 0x645D40 | - (see 5.6) | | | | |
| `WAIT` 0x640370 | children are *polled*, not run: see 5.3 | | | | |

Statement-class exec results: `Const`, `Var`, `Speaker`, `Unit`, `Object`, `Choice`, `VecLit`: 1. `Exit`: -1. `Assign`: 1 (-1 if the RHS operand returns -1). `Call`: the inner call object's status (`FUNC`: always 1). `Math`: 1 (-1 if an operand returns -1 in the single-operand path; -2 on a type error). `Operand`: the sub-statement's result is ignored, 1.

### 5.3 WAIT

`WAIT { c1 c2 ... }` is a two-phase block (0x640370, class 8; a `started` byte at `+0x20`):
1. **Start phase (first entry):** every child is `exec`'d once, in order, ignoring their status (calls start their action: a dialog opens, a unit task is issued, a timer is armed; assignments store the call's return value). `started = 1`, cursor rewound.
2. **Poll phase (this and every later tick):** the children are visited in order from the first; for each child of class `Call` (0x14) the *poll* method (vtable slot 13, +0x34) of its inner call object is evaluated; for a child of class `Assign` (0x12) the same slot is invoked on the assignment's *target variable object* (0x644390 forwards through `+0x14`; the SceVar vtable 0x9DB004 body is not exported, presumably "true"). At the first child whose poll returns false the cursor is rewound and `WAIT` returns **0** (the whole script suspends; the other children are not polled this tick). When all polls are true: `started = 0`, cursor rewound, return **1**.
3. An empty `WAIT { }` returns 1 immediately.

What "finished" means per builtin (dialog closed, task done, `pause_script` time elapsed ...) is implemented in the poll methods of the four call classes (vtables 0x9DB040 / 0x9DB084 / 0x9DB0C8 / 0x9DB10C, slot 13), which are not in the export (`TODO(re)`). The only fact recovered is `FUNC("pause_script", n)` (id 0x14A): it stores `DAT_00A6A254 = now_ms + n` (0x65C4A0; `DAT_00A500BA` is the real elapsed-ms clock, TIMING.md) and returns 1; its poll presumably compares the clock with that variable. For a `FUNC` call outside `WAIT` the exec always returns 1 (synchronous). `DIALOG` / `ACTION` outside `WAIT` start the action and, as far as the call wrapper shows, do not wait (`TODO(re)` their exec).

### 5.4 INIT, IF/ELSE, EXIT

* `INIT { ... }`: runs its children until done (it may span several ticks if it contains a suspending statement), then sets `done` and from then on is a no-op returning 1 (0x63FB80). A script may contain several `INIT` blocks, each with its own flag. The flag lives in the compiled object, so an included script that is re-entered later (it is rewound but not re-created) does **not** re-run its `init`. `RESET` is parsed and executed with the same code as `INIT` (nothing in the engine resets the flag; unused).
* `IF cond { body } [ELSE ...]` (class 12, 0x6456F0): the condition (child 1, a Math object) is evaluated **only when the If's own cursor is on child 1** (i.e. on fresh entry, never when resuming after a suspension in the body). Truth value of the result by type: int `!= 0`; float `!= 0.0`; string `strlen > 0`; vector `!= <global vector DAT_00A69CD8>` (presumably the zero vector); any other type (or a failed math evaluation) makes the whole `exec` return **-2**. True: the body (children up to the `Else` marker, class 13) is executed; False: the children after the marker (the else part, or the nested `if`) are executed. The children list is *flat*: `[cond, body..., ElseMarker, else-body...]`; in `else if` the nested `if` object is one child after the marker. After the executed part the cursor is rewound and 1 is returned. A condition that yields true on entry is not re-evaluated while the body is suspended, so `if (x) { WAIT {...} ... }` completes even if `x` changes meanwhile.
* `EXIT` (0x6409D0): exec returns -1. Every container on the path rewinds its cursor and passes -1 up; `run_step` returns -1 to its caller (the update loop ignores it; the next tick starts a new pass). Inside an included script `SCRIPT()` converts the child's -1 to 1 for the parent (5.6), so `exit` leaves only the included script's current pass.

### 5.5 Call statements and arguments (interface to the dispatcher)

* **Argument evaluation order is last-to-first** (0x643B60): for `FUNC("f", A, B, C)` the Operands of `C`, `B`, `A` are executed in that order, then slot 12 (`call_impl`, the builtin switch 0x649920 for `FUNC`) is invoked with the call object; it reads argument `i` as `child(i)` (1-based, the name string is not an argument) and the value fields at `operand+0xC` (type tag) / `+0x10`, and writes the result into the call object's `+0x10`. The exec returns 1.
* An Operand copies the value of its source at the moment it executes (variables are read then, call results are captured, `SCRIPT()` results become the int status). Nested calls therefore execute depth-first, right-to-left across sibling arguments.
* **No arity or type checking** exists. The dispatcher reads exactly as many arguments as its `case` needs; a missing argument yields a NULL child and a crash; surplus arguments are evaluated and ignored; the type tags of arguments are not validated or converted. An id without a `case` (no `default`) leaves the call's value untouched (0 for ints).
* The call wrapper (class 0x14) copies the inner object's type and value into its own slot after executing it; as a statement the value is discarded, as an operand it flows to the consumer.
* `ACTION` (0x643D40): when the builtin is `give_order` (0x143), `give_task` (0x146) or `give_team_task` (0x147), the **second argument is evaluated once at compile time** and its string translated to an enumerator (order names from 0x4BBBC0, task names from 0x4E4610, stored in the call at `+0x22`); the argument therefore has to be a string literal.
* Strings returned by builtins are copied into 400-byte buffers; concatenation and assignment clamp to 400 bytes.

### 5.6 SCRIPT("file") — include / sub-coroutine

Compile time (0x645DE0): look up `"file"` (as written, relative to the install directory: `script\scenario\S1L1_tactic.sce`) in the global include cache (0x648C50); on a hit increment its refcount, on a miss open the file, read it, compile it as an independent Script (`set_parent(0)`, own variable scope) and register it. Then `<file up to the first '.'>EX.res` (dialog text table) is attached to the new script. Missing file: fatal.
Cache consequences: all `SCRIPT("x")` with the same name share **one** compiled object (state included); the object is destroyed when the last referencing statement is destroyed.
Run time (0x645D40): the statement runs the included script's `run_step(ctx)` (the same loop as a top-level script) and stores the child's status as its own int value; status 0 (child suspended) -> the statement returns 0 and the parent suspends too; any other status (1 finished, -1 exit/error) -> the statement returns 1. A finished child has rewound itself, so reaching the statement again on a later tick runs the child **from its first statement again** (its `init` blocks stay done). An included script that never finishes a pass (a long `WAIT`) holds the includer at that statement.

### 5.7 Errors

| situation | behaviour |
|---|---|
| lexical: unterminated / over-long string | C++ exception -> message box `Bad token ...Line number`, compile returns false, script cleared (non-fatal) |
| premature end of file, "Expecting int/float/XYZ" in literals | exception -> message box with `Line number`, `Error token string`, `Error token type`; compile false, script cleared |
| every other parse error (`Expecting ...`, `Variable not found`, `Wrong function name`, `Token type does not correspond to a statement!`, `Cannot open script`, `Illegal expression`, unbalanced parentheses) | `MessageBoxA` "Wars & Warriors: Joan of Arc" with `<script name>` + `line:<n> <message>` and **process exit**. A port should treat these as load failures with the same text |
| compile returns false for the main scenario script | the Script object stays empty (`stmts` NULL) and `run_step` returns 1 immediately every tick |
| run time: expression type error | `Math::exec` returns -2 and leaves the previous value; the consumer carries on with stale data (usually a no-op) |
| run time: unknown builtin id | no effect |
| run time: `-2` reaching a script/init/id loop | infinite loop (spin) — do not reproduce; report as script error |
| run time: argument shortage / wrong tags | undefined (NULL dereference) |

## 6. Expressions

### 6.1 Parse and evaluation scheme

`( mexpr )` is compiled by 0x647C90: operand tokens (INT, FLOAT, STRING, `FUNC` call, `XYZ`, `SCRIPT`, IDENT) each become an Operand child of the Math object and are represented in the expression list by their **positive child index**;
operators are represented by their negative token kind and parentheses are kept; the list is validated (alternation, no unary operators, balanced parentheses) and converted to **reverse Polish** by a shunting-yard pass: an incoming operator pops operators whose precedence is **>=** its own (left associative), `(` / `)` as usual. The RPN list is stored in the Math object.

Precedence (0x6484B0; larger binds tighter): `||` 1 · `&&` 2 · `< > <= >= == !=` 3 · `+ -` 4 · `* /` 5 · `^` 6.

Evaluation (0x6463F0): scan the RPN list with an operand stack of at most ~15 entries (stack-allocated 401-byte string temporaries; deeper expressions overflow). An operand index is pushed *unevaluated*; when an operator is reached the two top entries are fetched: **the left operand is executed first**, then the right one — except for `&&`: the right operand is executed only if the left value is a non-zero int or a non-zero float (a short-circuit); `||` always executes both. A single-operand expression `( x )` executes the operand immediately and pushes its value. After the scan the single remaining entry is the result: the Math object takes its type and value (strings copied, clamped to 400).

### 6.2 Operator semantics by operand types

Let `a` be the left and `b` the right operand.

| types | `+ - *` | `/` | pow | `< > <= >= == !=` | `&&` `||` |
|---|---|---|---|---|---|
| int, int | 32-bit wrapping int | **float** `(f32)a / (f32)b` (type becomes float!) | `(int)pow((double)a,(double)b)` truncated by `__ftol` | int 0/1 | int 0/1 (`a != 0`, `b != 0`) |
| float, float / int, float / float, int | f32 (int operand converted), result float | f32 divide | f32 `pow` | int 0/1 (f32 compare, exact) | int 0/1 against 0.0 |
| string, string | `+` = concatenation (clamped to 400), `-` `*` -> error | error | error | `strcmp(a,b)` ordering (`<`: `<0`, `>`: `>0`, `<=`: `<=0`, `>=`: `>=0`, `==`, `!=`), case-sensitive, byte-wise | error |
| vector, vector | `+` and `-` component-wise (0x60CDC0 / 0x60CE20), `*` error | error | error | `==` exact equality of the 3 floats (0x60CED0), `!=` its negation; `<` etc. error | error |
| anything else | `&&` -> int 0; every other operator -> evaluator returns -2 | | | | |

Division and modulo: **there is no `idiv`**. Integer `/` produces a float32 (`7 / 2 = 3.5`, divide by zero gives +-inf/NaN, no trap); storing it with `x = ( a / b )` into an `INT` variable stores the float bit pattern (garbage as an int). There is no `%` operator; the highest-precedence operator calls the C `pow` (x87 `FUN_007B2719`, also used by the gamma table in the light code) and truncates toward zero for ints (`__ftol`). No shipped script uses `/` or pow; `*` appears 7 times (`2000*aliveCount` lexes as the single integer 2000, see 2.1).
Int arithmetic wraps like x86 `add/sub/imul`. Float arithmetic is single precision per operation.
Comparisons of mixed int/float convert the int; comparisons in the old `Compare` class (unreachable) use raw bits and are not described.

### 6.3 Truthiness

Only two places test a truth value: the `if` condition (section 5.4: int `!= 0`, float `!= 0.0`, string non-empty, vector non-zero) and the operands of `&&` / `||` in `Math` (int/float only; strings and vectors there are an error, `&&` of a string yields 0). The result of a comparison is always an int 0 or 1.

### 6.4 String comparison

`strcmp` semantics on the raw bytes (code-page encoded text in the original): ordinal, case-sensitive, terminating NUL. Ordering of non-ASCII text follows the byte order of the code page, which is irrelevant for the shipped scripts (they compare strings only with `==`/`!=` if at all).

## 7. Constants and limits

| item | value | source |
|---|---|---|
| keyword/punctuation table | 44 entries of 8 bytes at 0xA214C8 | 0x662330 |
| string literal storage | 0x100 bytes including both quotes | 0x6840D0 |
| script string capacity | 400 chars + NUL (401-byte buffers) | 0x5A1440, 0x5A14F0, 0x6439D0 |
| variable name field | 52 bytes (unchecked) | 0x642F20 (size 0x48) |
| script name field | 0x44 bytes at `Script+0x20` (unchecked) | 0x649860 |
| builtin table | 407 entries x 0x3C bytes at 0xA1A250 | 0x661E10 |
| math operand stack | ~15 entries | 0x6463F0 frame |
| include cache slot | 0x10C bytes | 0x648BD0 |
| operator kinds | `+` -0x32 · `-` -0x31 · `*` -0x30 · `/` -0x2F · `^` -0x2E · `<` -0x2D · `>` -0x2C · `<=` -0x2B · `>=` -0x2A · `==` -0x29 · `!=` -0x28 · `&&` -0x27 · `\|\|` -0x26 · `(` -0x25 · `)` -0x24 | 0x6463F0, 0x647C90 |
| error caption | `Wars & Warriors: Joan of Arc` | 0x586CB0 |

## 8. Second dialog-script grammar (unused)

The same parser and factory also implement statements of a dialog-oriented script language that **no shipped `.sce` uses** (the unit/animation scripts in `unitAI.res` / `animscr.res` are a different language, see the `script_res` pages). The words `INIT`, `WAIT`, `EXIT` of this family *are* used by the scenario scripts (sections 3 and 5); the rest is dead weight:

```
dialog_stmt = id | reset | speaker | unit | object | goto | choice ;
id       = "ID" INT_LIT "{" { item } "}" ;      (* 0x63F8A0: labelled section; registers a marker in the root script's id list; exec 0x63F790 *)
reset    = "RESET" "{" { item } "}" ;           (* 0x63FF00; run-once block with the same semantics as INIT *)
speaker  = "SPEAKER" ":" STRING ;               (* 0x640060; stores 0x5270B0(string): resolves a speaker name; exec is a no-op returning 1 *)
unit     = "UNIT"    ":" STRING ;               (* 0x640170; 0x5270B0(string) *)
object   = "OBJECT"  ":" STRING ;               (* 0x640280; 0x4A36C0(string) *)
goto     = "GOTO"    ":" INT_LIT ;              (* 0x6408F0; exec 0x640690 searches the root script for the Id with that number, repositions every cursor on the path and returns 2; -2 if absent *)
choice   = "CHOICE" "{" STRING { [","] STRING } "}" ;   (* 0x640BD0; collects the strings (commas optional); exec returns 1 *)
```
The error strings (`Expecting Id!`, `Expecting '{' after ID.`, `Expecting Speaker!`, `Expecting ':' after Unit.` ...) are the messages of these classes. `ID`/`RESET`/`SPEAKER`/`UNIT`/`OBJECT`/`GOTO`/`CHOICE` are reserved words even though no scenario script uses them. `Compare` and `Condition` statement classes (0x644D80 / 0x6453A0) are also unreachable: the parser never instantiates them.
A re-implementation may omit this family entirely; if kept, `GOTO` is the only construct that returns status 2.

## 9. Quirks a faithful re-implementation must keep (and those it should reject)

Keep: delimiter-only word splitting; `atoi` prefix parsing of numeric lexemes; sign inside the literal; right-to-left argument evaluation; left-to-right operand evaluation; `&&` short-circuit but `||` not; integer `/` yielding float; `if` condition evaluated only on entry; run-once `init`; the shared include cache; a pass restarts from the top each tick; `exit` aborting only the current pass.
Reject (report as script errors instead of reproducing): the spin loops on status -2, NULL dereferences on missing arguments, unchecked name copies, `VECTOR` variables with uninitialised memory.

## 10. Open questions

* Resolved: keyword table dump (all kinds, `^` = power, `EVENTSYSTEM` = 0x0A); `FUN_007C07B0` treated as `_stricmp`.
* Poll bodies of the call classes (slot 13: FUNC 0x6509E0, DIALOG 0x660E60, CAMERA 0x661040, ACTION 0x6612E0; exec slot 12: 0x649920, 0x65FD70, 0x661020, 0x661060) are specified on the builtins pages; the SceVar vtable has no slot 13 (polling an `Assign` child of `WAIT` is undefined in the original).
* `TODO(re)` `FUN_007B47FC` = `exit`?

## 11. Where the engine lives in OpenJoan

The script lexer, parser and VM are the original functions, ported one by one and registered in the code table; the
builtin dispatcher 0x649920 (`ScriptNode::exec_cmd`) is in `src/game/unknown_escomp.cpp` (`re/attribution.csv` maps every
address to its file), and the typed `Script*` layouts are in `src/original/gen/`. Statements on this page describe that code.

Open: `DAT_00A69CD8` assumed to be the zero vector; the separator `0x9D05F0` of the error box; row 407 of the builtin table
(`set_prisoner_can_be_attacked_flag`) comes from BUILTINS.md, not from the exported name table (which stops at id 406).
