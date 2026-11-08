// game/unknown_am2check.cpp: functions whose original source file is unknown (formerly game/_unplaced/am2check.cpp;
// re/attribution.csv: segment edge ww/gameplay.cpp|ww/en3d/genamt.cpp; class/call graph -> ww/en3d/gen). They move to
// their original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::img::str;

using i32 = std::int32_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_scan_dir, "scan_dir");

// ---- 0x0047cf60 scan_dir -----------------------------------------------------------------
i32 JOAN_CDECL scan_dir(const char* path, i32 recurse) {
    c_scan_dir.hit();
    alignas(4) u8 frame[0xac4];  // [ebp-0xac4, ebp)
    auto L = [&frame](u32 off) { return reinterpret_cast<char*>(frame + sizeof(frame) - off); };
    struct FindData {  // struct _finddata_t (MSVC 6), 0x118 bytes
        u32 attrib;
        i32 time_create;
        i32 time_access;
        i32 time_write;
        u32 size;
        char name[260];
    };
    static_assert(sizeof(FindData) == 0x118, "finddata size");
    char* const find = L(0x124);  // the _finddata_t
    char* const name = reinterpret_cast<FindData*>(find)->name;
    i32& handle = *reinterpret_cast<i32*>(L(0x128));
    auto findfirst = [](const char* spec, char* data) { return joan::orig::arg::as<i32>(joan::orig::libc::_findfirst(joan::orig::arg::str(spec), joan::orig::arg::mptr(data))); };
    auto findnext = [](i32 h, char* data) { return joan::orig::arg::as<i32>(joan::orig::libc::_findnext(joan::orig::arg::i32(h), joan::orig::arg::mptr(data))); };
    auto splitpath = [](const char* p, char* drive, char* dir, char* fname, char* ext) {
        joan::orig::libc::_splitpath(joan::orig::arg::mptr(p), joan::orig::arg::mptr(drive), joan::orig::arg::mptr(dir), joan::orig::arg::mptr(fname), joan::orig::arg::mptr(ext));
    };
    auto makepath = [](char* out, const char* drive, const char* dir, const char* fname, const char* ext) {
        joan::orig::libc::_makepath(joan::orig::arg::mptr(out), joan::orig::arg::mptr(drive), joan::orig::arg::mptr(dir), joan::orig::arg::mptr(fname), joan::orig::arg::mptr(ext));
    };
    char* const none = nullptr;

    handle = findfirst(path, find);
    if (handle != -1) {
        splitpath(path, L(0x12c), L(0x22c), none, none);
        makepath(L(0x330), L(0x12c), L(0x22c), str(d::kEmpty), none);
        do {
            if ((reinterpret_cast<const FindData*>(find)->attrib & 0x10) == 0) {
                splitpath(name, none, none, L(0x430), none);
#if UINTPTR_MAX == 0xffffffffu
                void* const loader = L(0x5bc);
#else
                // x64: an AnimDef is 0x1d3 bytes (0x18b on x86) and would reach the names at ebp-0x430: kept outside.
                alignas(alignof(void*)) u8 loader_x64[sizeof(joan::orig::AnimDef)];
                void* const loader = loader_x64;
#endif
                joan::orig::as_anim_def(loader)->construct();                // 0x005b2390 ctor
                joan::orig::as_anim_def(loader)->load(joan::orig::arg::str(L(0x330)), joan::orig::arg::str(L(0x430)), 0u);  // 0x005b23d0
                joan::orig::as_anim_def(loader)->release(0u);                // 0x005b4170
                joan::orig::as_anim_def(loader)->destruct();                   // 0x00418c30 dtor
            }
        } while (findnext(handle, find) == 0);
        joan::orig::libc::_findclose(joan::orig::arg::i32(handle));  // _findclose
    }
    if (recurse != 0) {
        splitpath(path, L(0x5c0), L(0x6c0), L(0x7c0), L(0x8c0));
        makepath(L(0x9c4), L(0x5c0), L(0x6c0), str(d::s_star), none);  // "*"
        handle = findfirst(L(0x9c4), find);
        if (handle != -1) {
            do {
                if ((reinterpret_cast<const FindData*>(find)->attrib & 0x10) != 0 && std::strcmp(joan::orig::arg::str(name), str(d::s_dot_9d36e8)) != 0 &&
                    std::strcmp(joan::orig::arg::str(name), str(d::s_dot_dot)) != 0) {  // "." / ".."
                    makepath(L(0xac4), none, L(0x6c0), name, none);
                    makepath(L(0x9c4), L(0x5c0), L(0xac4), L(0x7c0), L(0x8c0));
                    joan::orig::scan_dir(joan::orig::arg::str(static_cast<const char*>(L(0x9c4))), joan::orig::arg::i32(recurse));  // 0x0047cf60
                }
            } while (findnext(handle, find) == 0);
            joan::orig::libc::_findclose(joan::orig::arg::i32(handle));  // _findclose
        }
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("scan_dir", 0x0047cf60, &scan_dir);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::scan_dir(const char* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b108_18::scan_dir>(a0, a1)); }  // 0x47cf60
