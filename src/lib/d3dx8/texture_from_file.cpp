// Original library sources (re/attribution.csv original_file, under lib/): d3dx8/d3dx8tex.cpp.
//
// Batch 113 / list 01: statically linked D3DX8 texture loaders (0x00765258..0x00765f8e) and
// the IJG libjpeg error manager that D3DX8 links in right after them.
//
// Range identification (D3DX8 d3dx8tex.lib, texture file entry points; D3DERR_INVALIDCALL =
// 0x8876086c):
//   0x00765258 D3DXCreateTextureFromFileInMemoryEx(dev, data, size, W, H, Mips, Usage, Fmt, Pool,
//              Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTex)   stdcall ret 0x3c
//              (already replaced by lib/d3dx8/texture_in_memory.cpp, module d3dx/texture)
//   0x00765669 D3DXCreateCubeTextureFromFileInMemoryEx(dev, data, size, Size, Mips, Usage, Fmt,
//              Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppCube)  ret 0x38 (left)
//   0x00765aab D3DXCreateTextureFromFileExA        ret 0x38  -> 0x00765258   (replaced here)
//   0x00765b0e D3DXCreateTextureFromFileExW        ret 0x38  (no caller in the game)
//   0x00765b71 D3DXCreateTextureFromResourceExA    ret 0x3c  (no caller)
//   0x00765bd7 D3DXCreateTextureFromResourceExW    ret 0x3c  (no caller)
//   0x00765c3d D3DXCreateTextureFromFileInMemory   ret 0x10  (not registered)
//   0x00765c66 D3DXCreateCubeTextureFromFileExA    ret 0x34  -> 0x00765669   (replaced here)
//   0x00765cc6 D3DXCreateCubeTextureFromFileExW    ret 0x34  (no caller)
//   0x00765d26 D3DXCreateCubeTextureFromFileInMemory ret 0x10 (not registered)
//   0x00765d4e D3DXCreateTextureFromFileA          ret 0xc   (not registered)
//   0x00765dea D3DXCreateCubeTextureFromFileA      ret 0xc   (not registered)
//   0x00765f43 jpeg_std_error (IJG jerror.c)       cdecl     (not registered)
//
// The "not registered" entries are trivial forwarders / constant writers left to the original
// (patch_entry would accept them now; the writer did not implement them).
//
// The *FromFileEx wrappers map the file with D3DX's CFileMapping helper (16 bytes on the stack:
// +0 HANDLE file = -1, +4 HANDLE mapping = -1, +8 void* view = 0, +0xc DWORD size = 0;
// ctor 0x007886c6, Open(name, wide) 0x007886d8 thiscall ret 8, dtor 0x00788832), then call
// the in-memory loader with (view, size) and return its HRESULT (or Open's failure HRESULT).
// The helpers are library code with no thunk; they are called at their real addresses.
//
// Modules: d3dx/b113_01.
#include "../../callconv.hpp"
#include <cstdint>

#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {

using u32 = std::uint32_t;
using i32 = std::int32_t;

struct Obj {};
using joan::call::thiscall;

// D3DX8 CFileMapping (stack object of the *FromFileEx wrappers; its methods: lib/imports/import_stubs.cpp, same
// struct). x64: pointer-sized handles and view.
struct FileMapping {
    std::uintptr_t file;     // +0x0  HANDLE, -1 when closed
    std::uintptr_t mapping;  // +0x4  HANDLE, -1 when closed
    void* view;              // +0x8  MapViewOfFile result
    u32 size;                // +0xc  GetFileSize result
};
static_assert(sizeof(void*) != 4 || sizeof(FileMapping) == 0x10, "CFileMapping is 16 bytes");

constexpr fn::Code kMappingCtor = fn::CFileMapping_CFileMapping;   // thiscall, ret
constexpr fn::Code kMappingOpen = fn::CFileMapping_Open;   // thiscall (const void* name, BOOL wide), ret 8
constexpr fn::Code kMappingDtor = fn::CFileMapping_dtor;   // thiscall, ret
constexpr fn::Code kTexFromMemEx = fn::fn_765258;  // D3DXCreateTextureFromFileInMemoryEx
constexpr fn::Code kCubeFromMemEx = fn::D3DXCreateCubeTextureFromFileInMemoryEx; // D3DXCreateCubeTextureFromFileInMemoryEx

using TexFromMemExFn = i32(JOAN_STDCALL*)(void* device, void* data, u32 size, u32 width, u32 height, u32 mips,
                                      u32 usage, u32 format, u32 pool, u32 filter, u32 mip_filter,
                                      u32 color_key, void* src_info, void* palette, void* out);
using CubeFromMemExFn = i32(JOAN_STDCALL*)(void* device, void* data, u32 size, u32 edge, u32 mips, u32 usage,
                                       u32 format, u32 pool, u32 filter, u32 mip_filter, u32 color_key,
                                       void* src_info, void* palette, void* out);

// 0x00765aab D3DXCreateTextureFromFileExA (stdcall, 14 args, ret 0x38).
static i32 JOAN_STDCALL create_texture_from_file_ex_a(void* device, const char* file, u32 width, u32 height,
                                                   u32 mips, u32 usage, u32 format, u32 pool, u32 filter,
                                                   u32 mip_filter, u32 color_key, void* src_info, void* palette,
                                                   void* out) {
    FileMapping map;
    thiscall<void*>(kMappingCtor, &map);
    i32 hr = thiscall<i32>(kMappingOpen, &map, static_cast<const void*>(file), 0);
    if (hr >= 0) {
        hr = reinterpret_cast<TexFromMemExFn>(fn::code_value(kTexFromMemEx))(device, map.view, map.size, width, height, mips,
                                                              usage, format, pool, filter, mip_filter, color_key,
                                                              src_info, palette, out);
    }
    thiscall(kMappingDtor, &map);
    return hr;
}

// 0x00765c66 D3DXCreateCubeTextureFromFileExA (stdcall, 13 args, ret 0x34).
static i32 JOAN_STDCALL create_cube_texture_from_file_ex_a(void* device, const char* file, u32 edge, u32 mips,
                                                        u32 usage, u32 format, u32 pool, u32 filter,
                                                        u32 mip_filter, u32 color_key, void* src_info,
                                                        void* palette, void* out) {
    FileMapping map;
    thiscall<void*>(kMappingCtor, &map);
    i32 hr = thiscall<i32>(kMappingOpen, &map, static_cast<const void*>(file), 0);
    if (hr >= 0) {
        hr = reinterpret_cast<CubeFromMemExFn>(fn::code_value(kCubeFromMemEx))(device, map.view, map.size, edge, mips, usage,
                                                                format, pool, filter, mip_filter, color_key,
                                                                src_info, palette, out);
    }
    thiscall(kMappingDtor, &map);
    return hr;
}

JOAN_REPLACE("d3dx/b113_01", "D3DXCreateTextureFromFileExA", 0x00765aab, &create_texture_from_file_ex_a);
JOAN_REPLACE("d3dx/b113_01", "D3DXCreateCubeTextureFromFileExA", 0x00765c66,
                    &create_cube_texture_from_file_ex_a);

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::d3dx::create_texture_from_file_ex_a(void* a0, const char* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, void* a11, void* a12, void* a13) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::create_texture_from_file_ex_a>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)); }  // 0x765aab
auto joan::orig::d3dx::create_cube_texture_from_file_ex_a(void* a0, const char* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, void* a10, void* a11, void* a12) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::create_cube_texture_from_file_ex_a>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)); }  // 0x765c66
