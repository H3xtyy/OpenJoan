# BoneTree / BoneNode (en3d::bonetree, `d:\ww\en3d\bonetree.cpp`)

BoneTree — 12 bytes (`operator_new(0xC)`, ctor 0x5B9800 = memset 0). Parser: see [am3.md](../../formats/am3.md).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0 | 4 | i32 | `count` | bones (max index + 1) | 0x5B9990 |
| 4 | 4 | BoneNode* | `storage` | `(count+1)*0x1C` bytes, slot 0 = virtual root (-1) | 0x5B9990 |
| 8 | 4 | BoneNode* | `nodes` | `storage + 0x1C`; bone *i* at `nodes + i*0x1C` | 0x5B9990, copy ctor 0x5B9830 |

BoneNode — 0x1C bytes, reset by 0x5B9790:

| offset | type | name | meaning |
|---|---|---|---|
| 0x00 | i32 | `index` | own index |
| 0x04 | i32 | `name_id` | id in the global bone-name registry (0 = unnamed) |
| 0x08 | i32 | `parent` | -999 (0xFFFFFC19) = none initially; accessor 0x5B9FE0; root test is `== -1` in 0x5B6A90 |
| 0x0C | i32 | `next_sibling` | -999 = none (verified 0x5B9DF0) |
| 0x10 | i32 | `prev_sibling` | -999 = none. `0x5BA0A0(this, c)` returns **this slot** (previous sibling) |
| 0x14 | i32 | `first_child` | -999 = none (set by 0x5B9DF0 when the parent had no children) |
| 0x18 | i32 | `last_child` | -999 = none. `0x5BA040(this, b)` returns **this slot** (the *last* child) |

`0x5B9DF0(this, child, new_parent)` re-parents: unlink from the old parent's list (patching prev/next, `first_child`/`last_child`), then append as **last child** of `new_parent` (`-999` = detach; root parent is stored as -1/-999). Because `0x5B6B30` walks `last_child` then `prev_sibling`, children are visited **last to first**.

## Added by the actor-family pass
* `ModelSet` owns its own BoneTree at `ModelSet+0x44` (`+0x48` = "frame hierarchy built" flag): copied from the AnimDef's tree (`0x5C5610`, copy ctor `0x5B9830`, requires `tree.count == Model.bone_count` i.e. `Model+6`) or parsed from the model's own `<name>.AM3` (`0x5C5400` file / `0x5C5560` memory -> `0x5B9AF0`).
* Compatibility test `0x5BA1D0(model_tree, anim_tree)`: returns 1 ("remap needed") if `model.count < anim.count`; else 0 when any of: all anim bones unnamed (`name_id==0`), every named anim bone has the same `name_id` as the model bone at the same index, all model bones unnamed; else 1.
* Remap build `0x5BA300(model_tree, int out[anim.count], anim_tree)`: if either tree is entirely unnamed -> `out[i] = i` (or -999 when `i >= model.count`); else for each anim bone `i`: `out[i] = index of the model bone with equal name_id` (linear scan `0x5BA160`), `-999` when unnamed or not found. Result is `ActorDef.bone_map`.
* `0x5C56D0` builds the frame hierarchy: for each bone `i`, `frame(i).set_parent(parent==-1||-999 ? null : frame(parent))` ([FrameNode](FrameNode.md)); `0x5B6A90` does the key conversion for animations. BoneNode slot 1 `name_id` is filled from the registry (`0x5BA8B0`) while `0x5B9AF0` parses: first pass reads `{i32 idx, char[50] name, i32 n_children, children idx...}` to find `max index + 1`, then allocates via `0x5B9990`, resolves names, then `0x5B9DF0(child,parent)` links (all set to parent `-1` first).

Name registry (0x5BA570 init, 0x5BA760 add): dynarray of 0x38-byte `{i32 id, char name[50], pad}` sorted by name; `add` copies at most 50 characters and returns
the id; the registry is global (`DAT_00A67698`).
