# `shader/*.nvo` and `*.pso` - D3D8 shader bytecode

The `shader/` folder holds 7 `.nvo` and 2 `.pso` files next to NVIDIA's `nvlinker.dll`. They are what the original Direct3D 8 renderer
assembled at run time; OpenJoan links them with its own reimplementation (`src/render/nvlink.cpp`) and never runs the DLL.

| file kind | content | signature |
|---|---|---|
| `.nvo` | vertex-shader fragments produced by `nvasm -l` (NVIDIA `NVFO` container): a header, per-fragment names (`NVFN` records such as `transform_and_write_position`), the vs.1.x tokens and a symbol table (`c_*` constants, `r_*` temporaries) | first four bytes `NVFO`, then a u32 (0x14 in all seven shipped files) |
| `.pso` | a plain ps.1.x token stream, not linked | begins with the pixel-shader version token |

- Linker: `src/render/nvlink.{hpp,cpp}` is a token-for-token reimplementation of NVLink 2.0 (parsing, symbol ids starting at 125, constant
  instances, temporary allocation, the `mov` optimiser, the 128-instruction limit); `nvlink_com.cpp` is the `INVLink` COM object the game
  calls. The `[Fixes] Shaders` switch applies corrected fragment copies after loading.
- Consumers: hardware skinning (`hardware_skin.nvo`, see [am1.md](am1.md)), lighting, fog and clip planes.
- The linker emits vs.1.x tokens and `src/gl` translates them for OpenGL (see [../render/D3D8_USAGE.md](../render/D3D8_USAGE.md)); the shaders are not rewritten as GLSL.
- TODO(docs): the `.nvo` header fields after the magic (symbol table and fragment record layout); `nvlink.cpp` is the authority until then.
