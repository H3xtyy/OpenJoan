// SPDX-License-Identifier: GPL-3.0-or-later
// en3d/genamt.cpp: functions of the original d:\ww\en3d\genamt.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (en3d/Genamt.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

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

// ---------------------------------------- from modules/game_b115_09.cpp (fix/uninit)
namespace {
namespace game_b115_09 {

JOAN_COUNTER(c_compare, "Archive::compare_47d290");

// ======================================================================= 0x0047d290
i32 JOAN_CDECL archive_compare(const char* name1, const char* /*name2: never used*/, char* outdir) {
    c_compare.hit();
    constexpr std::size_t kArc = sizeof(joan::orig::ResourceIdx) > 0x138 ? sizeof(joan::orig::ResourceIdx) : 0x138;  // 0x135 on x86, 0x149 on x64
    alignas(alignof(void*)) u8 arc1[kArc];  // [ebp-0x144]
    alignas(alignof(void*)) u8 arc2[kArc];  // [ebp-0x27c]
    joan::orig::as_res_idx(arc1)->construct();  // 0x004179f0
    joan::orig::as_res_idx(arc2)->construct();
    // fix/uninit: arc2 is never opened, so its entry count +0x121 (read by 0x0059be20 through
    // 0x0059c200) is uninitialised: it starts at 0 (empty archive -> every lookup "not found").
    joan::orig::as_res_idx_l(arc2)->rec_count = 0;
    joan::orig::as_res_idx_l(arc1)->init(joan::orig::arg::str(name1), static_cast<char>(i32{0}), static_cast<char>(i32{0}));  // 0x0059b920 open
    char drive[4];
    char dir[0x100];
    char path[0x104];
    if (outdir == nullptr) {
        joan::orig::libc::_splitpath(joan::orig::arg::mptr(name1), joan::orig::arg::mptr(drive), joan::orig::arg::mptr(dir), joan::orig::arg::mptr(static_cast<char*>(nullptr)), joan::orig::arg::mptr(static_cast<char*>(nullptr)));  // _splitpath
        joan::orig::libc::_makepath(joan::orig::arg::mptr(path), joan::orig::arg::mptr(static_cast<const char*>(drive)), joan::orig::arg::mptr(static_cast<const char*>(dir)), joan::orig::arg::mptr(str(d::kEmpty)), joan::orig::arg::mptr(static_cast<const char*>(nullptr)));  // _makepath
        outdir = path;
    }
    i32 size2 = 0;  // [ebp-0x5b0] (fix/uninit: 0 until the first successful lookup)
    for (i32 i = 1; i <= joan::orig::as_res_idx_l(arc1)->rec_count; ++i) {
        alignas(alignof(void*)) u8 buf1[sizeof(joan::orig::FileMem)];  // [ebp-0x5a8]; a FileMem: 0x11c on x86, 0x12c on x64
        alignas(alignof(void*)) u8 buf2[sizeof(joan::orig::FileMem)];  // [ebp-0x6cc]
        alignas(alignof(void*)) u8 diff[sizeof(joan::orig::AnimDef) > 0x18c ? sizeof(joan::orig::AnimDef) : 0x18c];  // [ebp-0x858]; an AnimDef: 0x18b on x86, 0x1d3 on x64
        i32 size1 = 0;              // [ebp-0x48c] (written by 0x0059c2a0)
        joan::orig::as_file_mem(buf1)->construct();                                  // 0x00587f80
        void* const data1 = thiscall<void*>(fn::get_file_rec_59c2a0, arc1, i, &size1);  // 0x0059c2a0
        joan::orig::as_file_mem(buf1)->open_file(joan::orig::arg::file(data1));                                  // 0x005885e0
        const char* const entry = joan::orig::arg::as<const char*>(joan::orig::as_res_idx_l(arc1)->get_index_rec(joan::orig::arg::i32(i)));  // 0x0059c3b0
        joan::orig::as_file_mem(buf2)->construct();                                     // 0x00587f80
        void* const data2 = thiscall<void*>(fn::get_file_name_59c200, arc2, entry, &size2);  // 0x0059c200
        joan::orig::as_file_mem(buf2)->open_file(joan::orig::arg::file(data2));                                     // 0x005885e0
        joan::orig::as_anim_def(diff)->construct();                                     // 0x005b2390
        joan::orig::as_anim_def(diff)->load_streamed(static_cast<void*>(buf1), joan::orig::arg::u32(size1), static_cast<void*>(buf2), joan::orig::arg::u32(size2), joan::orig::arg::str(outdir), joan::orig::arg::str(entry), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));                // 0x005b2aa0
        joan::orig::as_anim_def(diff)->release(joan::orig::arg::u32(i32{0}));  // 0x005b4170
        joan::orig::as_anim_def(diff)->destruct();          // 0x00418c30
        joan::orig::as_file_mem(buf2)->destruct();          // 0x00587fd0
        joan::orig::as_file_mem(buf1)->destruct();          // 0x00587fd0
    }
    joan::orig::as_res_idx_l(arc2)->deinit();  // 0x0059bb40 close
    joan::orig::as_res_idx_l(arc1)->deinit();
    joan::orig::as_archive(arc2)->dtor();  // 0x00417a30 dtor
    joan::orig::as_archive(arc1)->dtor();
    return 1;
}

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: second archive never opened -> its entry count starts at 0 (lookups "not found"); size 0.
FIX("gen_reslx_amt", 0x0047d290, &archive_compare);
#undef FIX

}  // namespace game_b115_09
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using i8 = std::int8_t;

using joan::img::obj;

using joan::hook::global;

JOAN_COUNTER(c_main_args, "Tool::main_args");

// ---------------------------------------------------------------------------- cdecl
i32 JOAN_CDECL main_args(i32 argc, char** argv) {  // 0x0047d550
    c_main_args.hit();
    char* am2 = nullptr;  // [ebp-0x4]
    char* am3 = nullptr;  // [ebp-0x8]
    const i32 third = 0;  // [ebp-0xc], never written
    i32 sflag = 0;        // [ebp-0x10]
    i32 lflag = 0;        // [ebp-0x14]
    i32 names = 0;        // [ebp-0x18]
    for (i32 i = 1; i < argc; ++i) {
        if (static_cast<i8>(argv[i][0]) == 0x2d) {
            switch (static_cast<i8>(argv[i][1])) {
                case 'q': global<float>(d::g_9ff7b4) = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(argv[i] + 2)))); break;
                case 'd': global<float>(d::g_9ff7b8) = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(argv[i] + 2)))); break;
                case 't': global<float>(d::g_9ff7b0) = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(argv[i] + 2)))); break;
                case 's': sflag = 1; break;
                case 'L': lflag = 1; break;
                default: break;
            }
        } else {
            if (names == 0) am2 = argv[i];
            if (names == 1) am3 = argv[i];
            ++names;
        }
    }
    if (lflag != 0) {
        if (am2 == nullptr || am3 == nullptr) {
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_please_specify_am2_and_am3_r));  // 0x00586cb0
            return 1;
        }
        joan::orig::archive::compare_47d290(joan::orig::arg::str(am2), joan::orig::arg::str(am3), joan::orig::arg::str(third));  // 0x0047d290
        return 0;
    }
    if (am2 == nullptr) am2 = const_cast<char*>(str(d::s_AM2_9d36f0));  // "*.AM2"
    joan::orig::scan_dir(joan::orig::arg::str(am2), joan::orig::arg::i32(sflag));  // 0x0047cf60
    return 0;
}

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("gen_amt_main", 0x0047d550, &main_args);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::archive::compare_47d290(const char* a0, const char* a1, const char* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b115_09::archive_compare>(a0, a1, a2)); }  // 0x47d290
