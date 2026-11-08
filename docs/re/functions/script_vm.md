# `.sce` script VM — tokenizer, parser, statement classes, interpreter (joan.exe)

Pseudocode: `$JOAN_WORK_DIR/decomp/script/script_vm/` (1051 functions). Language reference (EBNF, semantics): [scripting/LANGUAGE.md](../../scripting/LANGUAGE.md).
Structs: [SceToken](../structs/SceToken.md), [SceDynArr](../structs/SceDynArr.md), [SceStatement](../structs/SceStatement.md), [SceScript](../structs/SceScript.md);
earlier partial notes: [video.md](video.md) section 8, [EsScriptSlot](../structs/EsScriptSlot.md). The builtin id switch 0x649920 and its ~400 handlers are
documented elsewhere ([ScriptBuiltinTable](../structs/ScriptBuiltinTable.md), BUILTINS.md); this page only records how a call statement reaches it.
Source files named by the binary: `d:\ww\uti3d\esstate2.cpp` (0x645DE0), `d:\ww\ilib\dynarr.cpp`, `d:\ww\stackt.h` (operator stack 0x648EE0..0x648FD0).
The module is the "ScriptStatementCompile" system (string in 0x6495CC). Confidence: V = verified in >= 2 places or by a data cross-check, R = read in one function, I = inferred.

## 1. Lexer and tokens

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x6840D0 | `sce_lex_run(tokenizer, text)` | 4-state character machine (1 between lexemes, 2 word, 3 string, 4 comment); pushes every lexeme via vtable slot 1 with its line number; shared with the particle-script parser 0x677390 | pseudocode; delimiters `( ) , " { } / :` | V |
| 0x684590 | `sce_pool_copy` | thunk to the chunked string pool copy 0x5A18D0 (stores the lexeme, returns the stored pointer) | | V |
| 0x6622D0 | `sce_token_push(text, line)` | drops empty lexemes; `sce_token_classify` then dynarr push (12-byte token) | | V |
| 0x662330 | `sce_token_classify(token*)` | kind from the keyword table (bsearch 44 x 8 bytes at 0xA214C8, qsort on first use), else number / string / identifier / 0x22 | LANGUAGE.md section 2 | V |
| 0x662590 / 0x6625C0 | `sce_kw_cmp_sort` / `sce_kw_cmp_search` | comparators wrapping `FUN_007C07B0` | | V |
| 0x661EA0 / 0x661F20 / 0x661F70 / 0x684010 | `sce_tokenstream_ctor / dtor_deleting / dtor / base_ctor` | token array (elem 0xC, grow 0x1E), vtable 0x9DBB54, base 0x9DCD98 (string pool) | | R |
| 0x642370 | `sce_tok_peek` | current token; skips kind-9 markers while counting lines | | V |
| 0x6423E0 | `sce_tok_next` | advance and peek; throws `"Unexpected end-of-file."` on the last token | | V |
| 0x642440 | `sce_tok_at_end` | `count <= cur` | | V |
| 0x63EE20 | `dynarr_advance` | `cur < count ? ++cur : fail` (token stream and statement lists) | | V |
| 0x6498E0 | `sce_tokenstream_rewind` | cursor = 1, line counter (`+0x29`) = 1 | | V |
| 0x661FF0 | `sce_expect_int(out)` | token kind must be 1 ("Expecting int." thrown), `atoi`, advance | | V |
| 0x662060 | `sce_expect_float(out)` | kind 2 ("Expecting float."), `atof` cast f32 | | V |
| 0x6620D0 | `sce_parse_xyz(out f32[3])` | `XYZ ( n , n , n )`, ints converted to float; errors "Expecting XYZ(...)!", "Expecting ( after XYZ!", "Vector components must be seperated by a comma.", "Expecting ) to close vector." (all thrown) | | V |

## 2. Compile driver and error reporting

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x649280 | `sce_compile(script, text, name)` | clear, token stream, lexer, `script.parse`, bool result; catch blocks 0x6493A0 / 0x6494C2 / 0x6495CC (message box via 0x586BC0, script cleared, false) | SceScript section 3 | V |
| 0x6496D1 | `sce_compile_epilogue` | shared tail: restore arena globals and name prefix, destroy stream, result byte | | R |
| 0x6498A0 | `sce_set_last_message(str)` | `strncpy(0xA6A118, str, 0xFF)` | | V |
| 0x63EB80 | `sce_report_error(this, fmt, line, ...)` | builds `<name><sep>line:%d <msg>` in 0xA69F38 and calls 0x586CB0 (**message box then `exit`-class call, fatal**) | | V |
| 0x586CB0 | `error_box_fatal` | `MessageBoxA(title "Wars & Warriors: Joan of Arc")`, then `FUN_007B47FC(-2)` | | R |
| 0x586BC0 | `error_box` | same box, non-fatal | | R |
| 0x649140 | `SceScript::parse` | `"name" { statements }` | | V |
| 0x649860 | `SceScript::set_name` | `strcpy(this+0x20, name)` | | V |

## 3. Container / script object

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x63E550 | `SceValue::ctor` | vtbl 0x9DA944, `+8 = -1`, `+0xC = 0x22`, rest 0 | V |
| 0x63E600 / 0x63E5B0 | `SceValue::dtor / deleting dtor` | frees value via 0x63E660 | R |
| 0x63E660 | `SceValue::free_value` | no free for types 1 and 2, `free(+0x10)` otherwise | V |
| 0x63E630 | `set_parent` | `this+4 = p` (slot 1) | V |
| 0x63E6E0 | `SceContainer::ctor` | vtbl 0x9DA980, zero `+0x14..+0x1F` | V |
| 0x63E780 / 0x63E730 | `SceContainer::dtor / deleting dtor` | via 0x63E7F0 | R |
| 0x63E7F0 | `SceContainer::free_children` | delete `ids`, `stmts`, `vars` (virtual dtor + dynarr remove), then value | R |
| 0x63EA20 | `SceContainer::add_child(class_id)` | lazily create `stmts`, factory 0x640DB0, push, return index | V |
| 0x63EF50 | `SceContainer::add_variable` | lazily create `vars`, new SceVar (0x48), `set_parent(this)`, push | V |
| 0x63E9F0 / 0x63EF20 | `child_at(i)` / `var_at(i)` | 1-based element, 0 out of range | V |
| 0x63ECD0 / 0x63F100 | `child_count` / `var_count` | 0 if the array does not exist | V |
| 0x63EB00 / 0x63F080 | `delete_child(i)` / `delete_var(i)` | virtual dtor(1) + dynarr remove | R |
| 0x63EDD0 / 0x6436B0 | `delegate_find_by_name / find_by_id` | forward to parent (slots 6 / 5), 0 without parent | V |
| 0x63EE70 | `SceContainer::find_by_name` | reverse scan of `vars` (strcmp-class on `var+0x14`), then parent | V |
| 0x643700 | `SceScript::find_by_id` | reverse scan of `stmts` for class 2 with matching `+0x21`, then parent | V |
| 0x63F140 | `value_ptr` | inline for types 1/2 else stored pointer | V |
| 0x642470 / 0x6424D0 / 0x642590 / 0x6424B0 / 0x642490 | `value_type`, `self` (x2), `null` (x2) | trivial slots | V |
| 0x63E450 / 0x63E4F0 | `sce_stmt_alloc(size)` / `sce_stmt_free` | arena allocation (4-byte aligned, `arena_count++/--`) or plain `new/delete` | V |
| 0x63F190 | `SceScript::ctor` | vtbl 0x9DA9C8, `+8 = 1`, `+0x64/+0x68/+0x6C = 0` | V |
| 0x63F200 / 0x63F250 | `SceScript::dtor_deleting / dtor` | clear + base dtor | V |
| 0x63F2C0 | `SceScript::clear` | slot 2 | V |
| 0x63F4E0 / 0x63F530 / 0x643520 | free `dlgres` / free arena / `dlgres` dtor | | R |
| 0x63F580 | `SceScript::attach_ex_res(path)` | loads `<path>EX.res` (`dlgres`, 0x637EC0) and appends the sound table (0x639560, base name up to the first '.') | R |
| 0x641A40..0x642310 | `*::memory_size` family | diagnostic sizes, per class | R |

## 4. Statement creation

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x643550 | `sce_make_statement(container, kind, line)` | kinds 0x1D..0x20 -> `add_variable`, returns the SceVar; others: `stmts` push of `0x6488D0(kind)`; returns the pointer (NULL for unsupported kinds, a later null call crashes) | V |
| 0x6488D0 | `sce_kind_to_class` | token kind -> class id (SceStatement section 3) | V |
| 0x640DB0 | `sce_new_statement(class_id)` | switch 1..0x1B: `sce_stmt_alloc(size)` + ctor, then `set_parent(container)` | V |
| class ctors | 0x63F190, 0x6424F0, 0x642630, 0x642700, 0x6427D0, 0x642890, 0x642950, 0x642A10, 0x642AE0, 0x642BA0, 0x640A50, 0x648770, 0x642C60, 0x642D20, 0x642E20, 0x6447A0, 0x642F20, 0x6443D0, 0x643300, 0x643E30, 0x643000, 0x6430C0, 0x643180, 0x643240, 0x6433C0, 0x643480, 0x646290 | class ids 1..0x1B in order (SceStatement section 3); each has a wrapper `0x6425B0`-style deleting dtor and a dtor | V |

## 5. Statement behaviour (exec = slot 3, parse = slot 4)

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x649060 | `SceScript::run_step` | interpreter loop (SceScript section 2.1) | V |
| 0x63F790 | `SceId::exec` | block run without done flag (-1 rewinds, 2 propagates, 0 suspends) | V |
| 0x63F8A0 | `SceId::parse` | `ID n { ... }`; also registers a class-2 marker in the root's `ids` | V |
| 0x63FB80 | `SceInit::exec` | run children once to completion, `done = 1` (+0x70) | V |
| 0x63FCA0 | `SceInit::parse` | `INIT { items }` | V |
| 0x63FDE0 / 0x63FF00 | `SceReset::exec / parse` | identical to Init with the word `RESET` | V |
| 0x640040 / 0x640060 | `SceSpeaker::exec / parse` | `SPEAKER : "string"` -> `+0x14 = 0x5270B0(str)` | R |
| 0x640150 / 0x640170 | `SceUnit::exec / parse` | `UNIT : "string"` -> 0x5270B0 | R |
| 0x640260 / 0x640280 | `SceObject::exec / parse` | `OBJECT : "string"` -> 0x4A36C0 | R |
| 0x640370 | `SceWait::exec` | two-phase WAIT (LANGUAGE.md section 5.3) | V |
| 0x6404E0 | `SceWait::parse` | `WAIT { (call | assignment)* }` | V |
| 0x640690 / 0x6408F0 | `SceGoto::exec / parse` | `GOTO : n`; exec searches the root for the matching Id, repositions cursors, returns 2, or -2 if absent | R |
| 0x6409D0 / 0x6409F0 | `SceExit::exec / parse` | exec returns -1 | V |
| 0x640BB0 / 0x640BD0 / 0x640AF0 | `SceChoice::exec / parse / dtor` | `CHOICE { "a" , "b" }`; strings collected into a dynarr | R |
| 0x6456F0 | `SceIf::exec` | cond (child 1) once, run until the Else marker or skip to after it | V |
| 0x645A40 | `SceIf::parse` | `IF math { items } [ ELSE IF... | ELSE { items } ]` | V |
| 0x648560 / 0x648630 | `SceElse::exec / parse` | marker class; stray `else { }` is an unconditional block | R |
| 0x644D80 / 0x645210 | `SceCompare::exec / parse` | legacy, unreachable | R |
| 0x6453A0 / 0x6454E0 | `SceCondition::exec / parse` | legacy, unreachable | R |
| 0x6484B0 | `sce_op_precedence(kind)` | `+ -`=4, `* /`=5, pow=6, six comparisons=3, `&&`=2, `||`=1, else 0 | V |
| 0x644900 | `SceOperand::exec` | exec sub (`+0x18`, else the variable `+0x14`), copy type and value into own slot (strings strncpy 400, vectors 12 bytes) | V |
| 0x644B10 | `SceOperand::parse` | picks the sub class from the current token: `1,2,3,0x1B`->Const, `0x17..0x1A`->Call, `0x1C`->ScriptCall, `0x21`->variable lookup (no sub), `(`->Math; else "Expecting script/function/variable/constant/math expression!" | V |
| 0x6439B0 / 0x6439D0 | `SceVar::exec / parse` | exec returns 1; parse sets type (INT 1, FLOAT 2, STRING 3 + 401-byte buffer, VECTOR 0x1B + 12 bytes), copies the name token | V |
| 0x644530 | `SceAssign::exec` | exec RHS; if it returned 0 nothing is stored and 1 is returned; -1 propagates; copies by the **RHS** type (1, 2 raw 32 bits; 3 string; 0x1B vector) into the target variable | V |
| 0x644680 | `SceAssign::parse` | `IDENT = operand` (variable lookup through slot 6; "Variable not found ! (%s)") | V |
| 0x643800 / 0x643820 | `SceConst::exec / parse` | exec 1; parse reads the literal token | V |
| 0x6437B0 / 0x6437D0 | `SceVecLit::exec / parse` | unused class | R |
| 0x643F80 | `SceCall::exec` | run inner (slot 3), copy inner type / value into own slot, return the inner status | V |
| 0x644190 | `SceCall::parse` | `FUNC|DIALOG|CAMERA|ACTION ( ... )`: creates the inner class 0x15..0x18 and parses it, then requires `)` | V |
| 0x644330 / 0x644360 / 0x644390 | `SceCall::value_ptr / value_type / poll` | forward to the inner object (slots 7, 8, 13) | V |
| 0x643B60 | `SceFunc::exec` | evaluate all argument Operands from the **last to the first**, then slot 12 (builtin switch), return 1 | V |
| 0x643C00 | `SceFunc::parse` | name token (kind 3) -> `sce_builtin_lookup`, then `( , operand)*`; errors "Expecting a function name!", "Wrong function name (%s)!" | V |
| 0x643D40 | `SceAction::parse` | base parse, then for ids 0x143 / 0x146 / 0x147 evaluates argument 2 now and stores the enum (0x4BBBC0 order names, 0x4E4610 task names) in `+0x22` | V |
| 0x661E10 | `sce_builtin_lookup(name, &type)` | linear search of 407 entries `{u32 id; char name[52]; u32 type}` from 0xA1A250, stride 0x3C (name comparison `FUN_007C07B0`); returns the id (u16) or 0 and writes the result type tag (1 int, 2 float, 3 string) to `call+0xC` | V |
| 0x649920 | `SceFunc::call_impl` (slot 12) | `switch (call.id)`, reads arguments with `child_at(i)->value`, writes the result to `call+0x10`; **no default**: an unknown id leaves the result untouched. Out of scope here | V |
| 0x6463F0 | `SceMath::exec` | postfix evaluator (LANGUAGE.md section 6) | V |
| 0x647C90 | `SceMath::parse` | collects operand/operator tokens, validates the sequence, converts to RPN (shunting-yard with `stackt` 0x648EA0 ctor, 0x648F60 push, 0x648FD0 pop, 0x649040 top, 0x648F40 count, 0x648EE0 dtor) | V |
| 0x646330 / 0x6462E0 | `SceMath::dtor` / wrapper | frees the RPN array | R |
| 0x645D40 | `SceScriptCall::exec` | run included script; 0 while it is suspended | V |
| 0x645DE0 | `SceScriptCall::parse` | `SCRIPT ( "file" )` with cache (SceScript section 5) | V |
| 0x645CA0 | `SceScriptCall::dtor` | releases the cache slot | V |
| 0x648AA0 / 0x648AE0 / 0x648B80 | cache table init / destroy / clear | | R |
| 0x648BD0 / 0x648C50 / 0x648CE0 / 0x648D20 / 0x648D60 / 0x648E40 | `slot_add / find / script / addref / release / is_free` | include cache | V |

## 6. Not part of the script language (same address range)

0x6626A0..0x662830, 0x6628A0, 0x662CB0..0x662DB0 belong to `uti3d/lflare.cpp` (lens flares, see [lights.md](lights.md)); 0x661E10..0x662060 sit next to them only by address.
Callers of the VM (battle setup 0x41CC80 / 0x41EA60 / 0x41DFE0 / 0x41BF40 / 0x41D400, per-update 0x471620) are described in [SceScript](../structs/SceScript.md) section 6.

## 7. Open questions

* RESOLVED: keyword table 0xA214C8 dumped (kinds 0x17..0x1A = FUNC/DIALOG/CAMERA/ACTION, 0x1D..0x20 = INT/FLOAT/STRING/VECTOR, -0x2E = `^`, kind 9 = `"\n"`, kind 0xA = `EVENTSYSTEM`).
* `FUN_007C07B0` is treated as `_stricmp` (inferred by use elsewhere).
* Call-class bodies: FUNC slot12/13 = 0x649920 / 0x6509E0; DIALOG 0x65FD70 / 0x660E60; CAMERA 0x661020 / 0x661040; ACTION 0x661060 / 0x6612E0 (exports `script_vm2/`, `script_vm3/`; semantics owned by the builtins pages). SceVar/SceCall vtables have only 12 slots.
* TODO(re) `FUN_007B47FC`: confirm it is `exit` (the fatal parse-error path).
* TODO(re) `SceIf+0x20` pointer: dtor frees it, no writer found.
* TODO(re) 0x5270B0 / 0x4A36C0 (speaker/unit/object name resolvers of the unused dialog grammar).
