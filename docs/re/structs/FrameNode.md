# FrameNode (0xA0 bytes) - transform-hierarchy node used for bones

Element of the array at `Model+0x00` (`Model` = 0x11C object, see [ModelMesh](ModelMesh.md)); `Model+0x06` (`bone_count`) elements, allocated by `0x5CC7A0` as
`new[bone_count*0xA0 + 4]` with a leading element-count cookie, ctor `0x60EB90`, dtor `0x60EC40` (= unparent). **One FrameNode per bone**; `Model.node(i) = *Model + i*0xA0` (`0x5CF8F0`).
Also used by `Actor+0x8C`. Matrices are D3D row-major with **row-vector** convention (`world = local x parent_world`). Classes in the original: probably `en3d::frame`.

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x00 | 0x40 | D3DMATRIX | `local` | local (parent-relative) transform; written by the pose evaluator (`0x5B8910` builds it from an AnimKey) | read (0x60EE60 reads `param_1` as the local, 0x5B5E40) |
| 0x40 | 4 | Frame* | `parent` | 0 = root | verified (0x60EC70, 0x60EE60) |
| 0x44 | 4 | Frame* | `prev` | previous sibling | verified |
| 0x48 | 4 | Frame* | `next` | next sibling | verified |
| 0x4C | 4 | Frame* | `first_child` | | verified |
| 0x50 | 4 | Frame* | `last_child` | | verified |
| 0x54 | 4 | i32 | `dirty` | world matrix stale. Set by `0x60EE10` (after a local change), set on all children when a parent is recomputed | verified |
| 0x58 | 0x40 | D3DMATRIX | `world` | cached world matrix; `0x60EF80` returns `node+0x58` after forcing evaluation. This is the matrix array passed to `skin_deform` (stride 0xA0) | verified (0x5F4C10, 0x60EE60) |
| 0x98 | 4 | fn* | `on_change` | callback, called with `ctx` after the world matrix was recomputed and by 0x60EE10 | verified |
| 0x9C | 4 | void* | `ctx` | callback argument (for bones: the owning Model; callback = `0x5CC9E0`, which is an **empty function (`ret`)** - a no-op hook; set by 0x60EDD0 in the 0x5CC7A0 loop) | read |

Operations (all exported by the actor family):
* `0x60EC70 set_parent(new_parent)` - unlink from old parent's child list (fixing `prev/next/first/last`), append as **last child** of the new one; no-op if unchanged; `0` detaches.
* `0x60EE60 update_world()` - if parent: first `parent.update_world()`, and if the parent was recomputed mark self dirty. If dirty: `world = local` (root) or `world = local x parent.world` (`matrix_multiply(dst, a, b)` of v3d), call `on_change`, mark all children dirty, clear own flag; returns 1 if recomputed.
* `0x60EE10 touch()` - `dirty = 1`, call `on_change`. (Used by pose evaluator and by the static-instance flush after copying a matrix into `local`.)
* `0x60EDD0 set_callback(ctx, fn)`.
