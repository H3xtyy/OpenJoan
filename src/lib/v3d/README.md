# src/lib/v3d

`v3d87.cpp`: the 23 vector-maths exports of `v3d_87.dll` (the skinning math) that joan.exe imports, ported bit-exactly.
There is no static-link source for them; the DLL is never loaded (the imports are answered by the override table,
`src/runtime/loader/import_overrides.hpp`). Float arithmetic follows the x87 code through [`src/fp`](../../fp/README.md);
the render path has a native twin in [`src/render/v3d.*`](../../render/README.md). No registrations.

Verification: the replay (skinned animation feeds game state), the smoke checklist.

See also: [`src/lib/README.md`](../README.md).
