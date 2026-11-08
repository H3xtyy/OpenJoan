// game/trackres.cpp: functions of the original d:\ww\trackres.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cstdarg>
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

void* const kHeap = obj(d::g_mem);

using joan::call::cdecl_call;

// 12-byte vector passed by value: floats, as the callers declare it (System V passes it in XMM registers).
struct V3 {
    float x, y, z;
};

}  // namespace

// ---------------------------------------- from modules/game_b101_01.cpp (game/b101_01)
//
//    - TrackRes::free_45cc20 0x0045cc20 (ret 0) -> al = 0.
namespace {
namespace game_b101_01 {

JOAN_COUNTER(c_track_free, "TrackRes::free_45cc20");

struct H {
    // ------------------------------------------------------------------ 0x0045cc20
    u8 track_free_45cc20() {
        c_track_free.hit();
        for (i32 i = 0; i < jo::self<jo::PathGraph>(this)->node_count; ++i) {
            joan::orig::as_track_node(&jo::self<jo::PathGraph>(this)->nodes[i])->release();  // 0x0043ab30
        }
        if (jo::self<jo::PathGraph>(this)->nodes != nullptr) {
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::PathGraph>(this)->nodes)), str(d::s_d_ww_trackres_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a053f4)) + 9));  // 0x0058ed70 "d:\\ww\\trackres.cpp"
            jo::self<jo::PathGraph>(this)->nodes = nullptr;
            jo::view<u32>(jo::self<jo::PathGraph>(this)->node_count) = 0;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_01", name, addr, fn)
B("TrackRes::free_45cc20", 0x0045cc20, &H::track_free_45cc20);
#undef B

}  // namespace game_b101_01
}  // namespace

// ---------------------------------------- from modules/game_b101_04.cpp (game/b101_04)
//
//    - Track::add_point 0x004a9ac0 (x, y, z), ret 0xc -> al = 1: grows the 0x54-byte node
//      array +0x4a (0x58eb30 "trackres.cpp"), links the new node to every node with 0 or 1
//      links (+0x14 count, +0x18 array, new(4) / realloc).
//
//  - 0x004a9ac0: the caller (0x4eb920) returns eax; the upper 24 bits are reproduced (0x4ee8f0's
//    eax on the first insert, the node array base otherwise; `this` if +0x46 was negative).
namespace {
namespace game_b101_04 {

using joan::orig::helpers::addr;

using joan::orig::helpers::op_new;
using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_track_add, "Track::add_point");

struct H {
    // ------------------------------------------------------------------ 0x004a9ac0
    std::uintptr_t track_add(u32 x, u32 y, u32 z) {  // eax: al = 1, upper bits from the last pointer in eax
        c_track_add.hit();
        const i32 line = global<i16>(d::g_a053f8);
        if (jo::self<jo::PathGraph>(this)->node_count == 0) {
            jo::self<jo::PathGraph>(this)->node_count = jo::self<jo::PathGraph>(this)->node_count + 1;
            jo::view<u8*>(jo::self<jo::PathGraph>(this)->nodes) = joan::orig::arg::as<u8*>(joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::PathGraph>(this)->nodes)), static_cast<u32>(sizeof(jo::TrackNode)), str(d::s_d_ww_trackres_cpp), joan::orig::arg::i32(line + 7)));  // 0x0058eb30
            crt_memset(jo::view<void*>(jo::self<jo::PathGraph>(this)->nodes), 0, sizeof(jo::TrackNode));
            jo::view<u32>(jo::self<jo::PathGraph>(this)->nodes->link_count) = 0;
            jo::TrackNode* const v = jo::self<jo::PathGraph>(this)->nodes;
            jo::view<u32>(v->pos[0]) = x;
            jo::view<u32>(v->pos[1]) = y;
            jo::view<u32>(v->pos[2]) = z;
            const u32 r = thiscall<u32>(fn::X_nop_ee8, jo::view<void*>(jo::self<jo::PathGraph>(this)->nodes));  // 0x004ee8f0
            return (r & 0xffffff00u) | 1u;
        }
        jo::self<jo::PathGraph>(this)->node_count = jo::self<jo::PathGraph>(this)->node_count + 1;
        jo::view<u8*>(jo::self<jo::PathGraph>(this)->nodes) = joan::orig::arg::as<u8*>(joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::PathGraph>(this)->nodes)), joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(jo::self<jo::PathGraph>(this)->node_count) * static_cast<u32>(sizeof(jo::TrackNode)))), str(d::s_d_ww_trackres_cpp), joan::orig::arg::i32(global<i16>(d::g_a053f8) + 0x11)));  // 0x0058eb30
        auto node = [this](i32 i) { return jo::view<u8*>(jo::self<jo::PathGraph>(this)->nodes) + static_cast<i32>(static_cast<u32>(i) * static_cast<u32>(sizeof(jo::TrackNode))); };
        jo::TrackNode* const nv = jo::as_track_node(node(jo::self<jo::PathGraph>(this)->node_count - 1));
        jo::view<u32>(nv->pos[0]) = x;
        jo::view<u32>(nv->pos[1]) = y;
        jo::view<u32>(nv->pos[2]) = z;
        jo::view<u32>((jo::as_track_node(node(jo::self<jo::PathGraph>(this)->node_count - 1)))->link_count) = 1;
        void* const links = op_new(sizeof(jo::TrackNodePtr));
        (jo::as_track_node(node(jo::self<jo::PathGraph>(this)->node_count - 1)))->links = static_cast<jo::TrackNodePtr*>(links);
        // eax at the end is the last node visited, or `this` (4a9c02) when the loop never runs.
        u8* e = reinterpret_cast<u8*>(this);
        for (i32 i = jo::self<jo::PathGraph>(this)->node_count - 1; i >= 0; --i) {
            e = node(i);
            if (jo::as_track_node(e)->link_count == 1) {
                jo::as_track_node(e)->link_count = jo::as_track_node(e)->link_count + 1;
                jo::as_track_node(e)->links = static_cast<jo::TrackNodePtr*>(joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(jo::as_track_node(e)->links), joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(jo::as_track_node(e)->link_count) * static_cast<u32>(sizeof(jo::TrackNodePtr)))), str(d::s_d_ww_trackres_cpp), joan::orig::arg::i32(global<i16>(d::g_a053f8) + 0x1e)));  // 0x0058eb30
                u8* const newest = node(jo::self<jo::PathGraph>(this)->node_count - 1);
                jo::as_track_node(e)->links[jo::as_track_node(e)->link_count - 1].p = jo::as_track_node(newest);
                (jo::as_track_node(node(jo::self<jo::PathGraph>(this)->node_count - 1)))->links[0].p = jo::as_track_node(e);
            }
            if (jo::as_track_node(e)->link_count == 0) {
                jo::as_track_node(e)->link_count = jo::as_track_node(e)->link_count + 1;
                void* const l = op_new(sizeof(jo::TrackNodePtr));
                jo::as_track_node(e)->links = static_cast<jo::TrackNodePtr*>(l);
                jo::as_track_node(e)->links[0].p = jo::as_track_node(node(jo::self<jo::PathGraph>(this)->node_count - 1));
                (jo::as_track_node(node(jo::self<jo::PathGraph>(this)->node_count - 1)))->links[0].p = jo::as_track_node(e);
            }
        }
        return (addr(e) & ~std::uintptr_t{0xff}) | 1u;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_04", name, addr, fn)
B("Track::add_point", 0x004a9ac0, &H::track_add);
#undef B

}  // namespace game_b101_04
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
//
//    - TrackRes::free 0x004eba00 / CamRes::free 0x00428d50 (if loaded: release, free +0 with
//      0x0058ed70, clear), Plasma::free 0x004c5960, TrackRes::load 0x004eba90 /
//      CamRes::load 0x00428de0 (id + 12-byte value by value, then loaded = 1).
namespace {
namespace game_b82_18 {

// Memory::free(ptr, file, line) on the allocator 0x00a48838.
void mem_free(const void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

struct Vec12 {
    float a, b, c;
};
static_assert(sizeof(Vec12) == 12);

struct H {
    // 0x004eba00 / 0x00428d50: free a loaded resource.
    void res_free(std::uintptr_t release, img::Addr line_global, img::Addr file) {
        if (static_cast<std::int8_t>(jo::self<jo::TrackRes>(this)->loaded) == 1) {
            thiscall(release, this);
            if (jo::self<jo::TrackRes>(this)->paths != nullptr) {
                mem_free(jo::self<jo::TrackRes>(this)->paths, str(file),
                         static_cast<std::int32_t>(global<std::int16_t>(line_global)) + 7);
                jo::self<jo::TrackRes>(this)->paths = nullptr;
                jo::view<std::uint32_t>(jo::self<jo::TrackRes>(this)->path_count) = 0;
            }
            jo::self<jo::TrackRes>(this)->loaded = 0;
        }
    }
    void track_free() { res_free(fn::code_value(fn::Track_reset_all_4ebae0), d::g_a053e4, d::s_d_ww_trackres_cpp); }  // 0x004ebae0, trackres.cpp

    void track_load(const char* id, Vec12 v) {  // 0x004eba90
        joan::orig::as_track_res(this)->load_file(id, v);         // 0x004ebb40
        jo::self<jo::TrackRes>(this)->loaded = 1;
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("TrackRes::free", 0x004eba00, &H::track_free);
B82("TrackRes::load", 0x004eba90, &H::track_load);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
//
//    - Track::reset_all_4ebae0 0x004ebae0 (ret 0): short i = 1..[+4]: 0x004ec3d0 on
//      0x0049ab40(i).
namespace {
namespace game_b102_05 {

struct H {
    // ------------------------------------------------------------------ 0x004ebae0
    void reset_all_4ebae0() {
        for (i16 i = 1; static_cast<i32>(i) <= jo::self<jo::TrackRes>(this)->path_count; i = static_cast<i16>(i + 1)) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_table52(this)->slot(joan::orig::arg::i16(static_cast<i32>(i))));  // 0x0049ab40
            joan::orig::as_path_graph(e)->free();                                              // 0x004ec3d0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("Track::reset_all_4ebae0", 0x004ebae0, &H::reset_all_4ebae0);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
//
//    - Tracks::load 0x004ebb40 (path, V3) ret 0x10 (trackres.cpp).
//
//  * SEH frames (0x0041baf0, 0x00428e90, 0x004ebb40) only guard operator new + ctor and the
//    stack text reader: normal path reproduced (0x00589890 on every exit).
namespace {
namespace game_b83_07 {

static_assert(sizeof(V3) == 12, "V3");
struct alignas(4) Path {  // 0x005a1170 string, no destructor
    std::uint8_t b[0x194];
};
struct alignas(8) Script {  // 0x00589550 / 0x00589890 text reader
    std::uint8_t b[((sizeof(jo::FileTxt) > 0x1a0 ? sizeof(jo::FileTxt) : 0x1a0) + 8 - 1) / 8 * 8];  // 0x1a0 on x86; FileTxt is 0x1a6 on x64
};

JOAN_COUNTER(c_tracks_load, "Tracks::load");

using joan::orig::helpers::stricmp_;
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90

struct H {
    // ------------------------------------------------------------------ 0x004ebb40
    void tracks_load(const char* path, V3 v) {
        c_tracks_load.hit();
        Path p;
        joan::orig::as_string(&p)->construct();
        joan::orig::as_string(&p)->assign(joan::orig::arg::str(path));
        Script r;
        joan::orig::as_file_txt(&r)->construct(joan::orig::arg::str(c_str(p)));
        std::int32_t n = 0;
        for (;;) {
            const char* const tok = joan::orig::arg::as<const char*>(joan::orig::as_file_txt(&r)->get_token(1));
            if (tok == nullptr) break;
            if (stricmp_(tok, str(d::s_CreateTrackArray)) == 0) {  // "CreateTrackArray"
                joan::orig::as_track_res(this)->create_array(joan::orig::arg::mptr(static_cast<void*>(&r)));  // 0x004ec240
                if (jo::view<std::uint32_t>(jo::self<jo::TrackRes>(this)->path_count) == 0) {
                    joan::orig::as_file_txt(&r)->destruct();
                    return;
                }
            }
            if (stricmp_(tok, str(d::s_CreateTrack)) != 0) continue;  // "CreateTrack"
            if (jo::view<std::uint32_t>(jo::self<jo::TrackRes>(this)->path_count) == 0) {
                joan::orig::as_file_txt(&r)->destruct();
                return;
            }
            ++n;
            std::uint8_t* const e = reinterpret_cast<std::uint8_t*>(&jo::self<jo::TrackRes>(this)->paths[n - 1]);
            jo::as_path_graph(e)->id = n;
            joan::orig::as_track_res(this)->parse_track(joan::orig::arg::mptr(static_cast<void*>(&r)), joan::orig::arg::mptr(static_cast<void*>(e)), v);  // 0x004ebd50
        }
        joan::orig::as_file_txt(&r)->destruct();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Tracks::load", 0x004ebb40, &H::tracks_load);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
//
//    - Tracks::parse_track 0x004ebd50 (reader, entry, V3 offset) ret 0x14 (trackres.cpp).
//
//  * 0x00429110 / 0x004ebd50: the original line buffer ([ebp-0x2c]) is 0x24 bytes but read_line
//    (0x00589f10) is called with max 0x40 (writes up to 0x41 bytes); a 0x24..0x2b char line
//    only clobbers dead locals (token, this), a longer one overwrites the saved ebp / return
//    address (original crashes). Here the buffer is 0x41 bytes (+ a leading pad byte for the
//    buf[strlen - 1] = 0 write of an empty line, which in the original hits the dead high byte
//    of the loop counter [ebp-0x30]).
namespace {
namespace game_b84_03 {

using joan::hook::x87::Fpu;

static_assert(sizeof(V3) == 12, "V3");
// Line buffer for read_line (0x00589f10, max 0x40 + NUL) with a pad byte in front (buf[-1]).
struct alignas(4) Line {
    char raw[4 + 0x44];
    char* buf() { return raw + 4; }
};

JOAN_COUNTER(c_track_parse, "Tracks::parse_track");

constexpr img::Addr kMemory = d::g_mem;

using joan::orig::helpers::game_new;  // operator new
using joan::orig::helpers::stricmp_;
std::int32_t atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
double atof_(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
void* memset_(void* d, std::int32_t c, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(c), joan::orig::arg::u32(n)); }
char* strcpy_(void* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
using joan::orig::helpers::line_base;
void* mem_realloc(void* p, std::int32_t n, img::Addr file, std::int32_t line) {
    return joan::orig::as_mem(obj(kMemory))->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(n), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058eb30
}

// Text script reader (0x00589550 object).
struct Reader {
    void* r;
    const char* tok() { return joan::orig::arg::as<const char*>(joan::orig::as_file_txt(r)->get_token(1)); }  // 0x00589c70
    void line(char* buf) { joan::orig::as_file_txt(r)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), 0x40); }          // 0x00589f10
    // tok(); atoi(tok())
    std::int32_t skip_int() {
        tok();
        return atoi_(tok());
    }
    double skip_num() {
        tok();
        return atof_(tok());
    }
};

// atof() + float at PC24, stored as float.
float add_f(double a, float b) { return static_cast<float>(Fpu{true}.add(a, static_cast<double>(b))); }

struct H {
    // ------------------------------------------------------------------ 0x004ebd50
    const char* track_parse(void* reader, std::uint8_t* e, V3 v) {
        c_track_parse.hit();
        Reader rd{reader};
        const float vx = v.x, vy = v.y;
        Line ln;
        char* const buf = ln.buf();
        rd.tok();
        rd.tok();
        rd.line(buf);
        strcpy_(jo::as_path_graph(e)->name, buf);   // +0x4
        rd.tok();
        rd.line(buf);
        strcpy_(jo::as_path_graph(e)->label, buf);  // +0x25
        jo::as_path_graph(e)->node_count = rd.skip_int();
        if (jo::view<std::uint32_t>(jo::as_path_graph(e)->node_count) == 0) return rd.tok();
        jo::view<void*>(jo::as_path_graph(e)->nodes) = mem_realloc(jo::view<void*>(jo::as_path_graph(e)->nodes), jo::as_path_graph(e)->node_count * static_cast<std::int32_t>(sizeof(jo::TrackNode)), d::s_d_ww_trackres_cpp,
                                         line_base(d::g_a053e8) + 0x1f);
        memset_(jo::view<void*>(jo::as_path_graph(e)->nodes), 0, static_cast<std::uint32_t>(jo::as_path_graph(e)->node_count * static_cast<std::int32_t>(sizeof(jo::TrackNode))));
        for (std::int32_t i = 0; i < jo::as_path_graph(e)->node_count; ++i) {
            const std::int32_t id = rd.skip_int();
            std::uint8_t* const p = reinterpret_cast<std::uint8_t*>(&jo::as_path_graph(e)->nodes[id - 1]);
            jo::as_track_node(p)->id = id;
            jo::view<void*>(jo::as_track_node(p)->owner) = e;
            jo::as_track_node(p)->i32_4 = rd.skip_int();
            jo::as_track_node(p)->pos[0] = add_f(rd.skip_num(), vx);
            jo::as_track_node(p)->pos[1] = add_f(rd.skip_num(), vy);
            jo::as_track_node(p)->pos[2] = static_cast<float>(rd.skip_num());
            jo::as_track_node(p)->link_count = rd.skip_int();
            if (jo::view<std::uint32_t>(jo::as_track_node(p)->link_count) != 0) {
                jo::as_track_node(p)->links = static_cast<jo::TrackNodePtr*>(game_new(static_cast<std::uint32_t>(jo::as_track_node(p)->link_count * static_cast<std::int32_t>(sizeof(jo::TrackNodePtr)))));
                memset_(jo::as_track_node(p)->links, 0, static_cast<std::uint32_t>(jo::as_track_node(p)->link_count * static_cast<std::int32_t>(sizeof(jo::TrackNodePtr))));
            }
        }
        for (;;) {
            const char* const t = rd.tok();
            if (stricmp_(t, str(d::s_edge_start)) == 0) {  // "edge_start"
                const std::int32_t a = atoi_(rd.tok());
                const std::int32_t b = rd.skip_int();
                std::uint8_t* const pa = reinterpret_cast<std::uint8_t*>(&jo::as_path_graph(e)->nodes[a - 1]);
                std::uint8_t* const pb = reinterpret_cast<std::uint8_t*>(&jo::as_path_graph(e)->nodes[b - 1]);
                for (std::int32_t k = 0; k < jo::as_track_node(pa)->link_count; ++k) {
                    if (jo::as_track_node(pa)->links[k].p != nullptr) continue;
                    jo::as_track_node(pa)->links[k].p = jo::as_track_node(pb);
                    break;
                }
                for (std::int32_t k = 0; k < jo::as_track_node(pb)->link_count; ++k) {
                    if (jo::as_track_node(pb)->links[k].p != nullptr) continue;
                    jo::as_track_node(pb)->links[k].p = jo::as_track_node(pa);
                    break;
                }
            } else if (stricmp_(t, str(d::s_rbrace)) == 0) {  // "}"
                return nullptr;
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("Tracks::parse_track", 0x004ebd50, &H::track_parse);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
//
//      0x004ec320 NameList::find       (thiscall(name), ret 4) - records of 0x52 bytes.
namespace {
namespace game_b92_04 {

JOAN_COUNTER(c_namelist_find, "NameList::find");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x004ec320
    u32 namelist_find(const char* name) {
        c_namelist_find.hit();
        for (i32 i = 0; i < jo::self<jo::TrackRes>(this)->path_count; ++i) {
            if (stricmp_(jo::self<jo::TrackRes>(this)->paths[i].label, name) == 0) return jo::view<u32>(jo::self<jo::TrackRes>(this)->paths[i].id);
        }
        return 0;
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("NameList::find", 0x004ec320, &H::namelist_find);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b103_05.cpp (game/b103_05)
//
//    - 0x004ec3a0 Rec52::zero: memset(this, 0, 0x52), returns memset's result.     (dead code)
namespace {
namespace game_b103_05 {

struct H {
    // ------------------------------------------------------------------------ 0x004ec3a0
    void* zero_52() { return std::memset(static_cast<void*>(this), 0, sizeof(jo::PathGraph)); }                        // memset
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_05", name, addr, fn)
B("Rec52::zero", 0x004ec3a0, &H::zero_52);
#undef B

}  // namespace game_b103_05
}  // namespace

// ---------------------------------------- from modules/game_b101_07.cpp (game/b101_07)
//
//    - TrackRes::free 0x004ec3d0 (ret 0): 0x43ab30 on the +0x46 entries (0x54 bytes) at +0x4a,
//      heap free of +0x4a (0x58ed70, "d:\\ww\\trackres.cpp", [0xa053f0] + 9), +0x4a = +0x46 = 0.
//
//  - Return values: callers of 0x4ec3d0, 0x50aca0, 0x50c680 (0x43ab80 overwrites ax and
//    returns a short), 0x50c8a0, 0x518890, 0x524dc0 ignore eax: void here. 0x4ea9c0,
//    0x510120, 0x5101c0 return the callee's eax as the originals do.
namespace {
namespace game_b101_07 {

JOAN_COUNTER(c_track_free, "TrackRes::free");

struct H {
    // ------------------------------------------------------------------ 0x004ec3d0
    void track_free() {
        c_track_free.hit();
        for (i32 i = 0; i < jo::self<jo::PathGraph>(this)->node_count; ++i) {
            joan::orig::as_track_node(&jo::self<jo::PathGraph>(this)->nodes[i])->release();  // 0x0043ab30
        }
        if (jo::self<jo::PathGraph>(this)->nodes != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a053f0)) + 9;
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::PathGraph>(this)->nodes)), str(d::s_d_ww_trackres_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::PathGraph>(this)->nodes = nullptr;
            jo::view<u32>(jo::self<jo::PathGraph>(this)->node_count) = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_07", name, addr, fn)
B("TrackRes::free", 0x004ec3d0, &H::track_free);
#undef B

}  // namespace game_b101_07
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
//
//    - CamZones::CamZones 0x00425dc0 (memset 0x4c), CamZones::init 0x00425e20,
//      CamZones::load 0x00426330 (path), Tracks::Tracks 0x004eb4f0 (memset 0x10),
//      Tracks::init 0x004eb550 (empty), Tracks::load 0x004eb640 (path).
namespace {
namespace game_leaves82b {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::orig::helpers::at;

struct Vec12 {  // 12-byte struct passed by value (floats)
    float a, b, c;
};
static_assert(sizeof(Vec12) == 12, "Vec12");

JOAN_COUNTER(c_tracks_load, "Tracks::load");

using joan::orig::helpers::game_new;      // operator new

struct H {
    void* tracks_ctor() {  // 0x004eb4f0
        std::memset(static_cast<void*>(this), 0, sizeof(jo::Tracks));                        // memset
        return this;
    }
    void empty() {}  // 0x004eb550, 0x004d69f0, 0x0047b7a0 ...
    // 0x004eb640 Tracks::load(path): as 0x00426330 with +0xc and the origin +0.
    std::uint32_t tracks_load(const char* path) {
        c_tracks_load.hit();
        if (jo::view<void*>(jo::self<jo::Tracks>(this)->res) != nullptr) {
            joan::orig::as_track_res(jo::view<void*>(jo::self<jo::Tracks>(this)->res))->free();  // 0x004eba00
            void* const old = jo::view<void*>(jo::self<jo::Tracks>(this)->res);
            if (old != nullptr) joan::orig::as_track_res(old)->destroy(1u);  // 0x004eb5f0 delete
        }
        void* const mem = game_new(sizeof(jo::TrackRes));  // 9 on x86
        void* const p = mem != nullptr ? joan::orig::as_track_res(mem)->construct() : nullptr;  // 0x004eb9a0
        jo::view<void*>(jo::self<jo::Tracks>(this)->res) = p;
        Vec12 origin;
        std::memcpy(&origin, jo::self<jo::Tracks>(this)->origin, 12);
        joan::orig::as_track_res(jo::view<void*>(jo::self<jo::Tracks>(this)->res))->load(path, origin);  // 0x004eba90
        return 0;
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Tracks::Tracks", 0x004eb4f0, &H::tracks_ctor);
L82("Tracks::init", 0x004eb550, &H::empty);
L82("Tracks::load", 0x004eb640, &H::tracks_load);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
//
//    - destroy helpers 0x004866b0 / 0x004eb570 / 0x00425ef0, scalar deleting destructors
//      0x00416640 / 0x00417250.
namespace {
namespace game_b83_07_2 {

using joan::call::thiscall;
using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x004eb570
    std::uintptr_t destroy_c() {
        if (jo::self<jo::Tracks>(this)->res == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        joan::orig::as_track_res(jo::view<void*>(jo::self<jo::Tracks>(this)->res))->free();  // 0x004eba00
        void* const p = jo::view<void*>(jo::self<jo::Tracks>(this)->res);
        std::uintptr_t r = 0;
        if (p != nullptr) r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_track_res(p)->destroy(1u));  // 0x004eb5f0
        jo::self<jo::Tracks>(this)->res = nullptr;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Obj::destroy_c", 0x004eb570, &H::destroy_c);
#undef B

}  // namespace game_b83_07_2
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
//
//      0x004eb9d0 X::fwd_4eba00           (thiscall, ret 0)
namespace {
namespace game_b95_04 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    u32 fwd_4eba00() {  // 0x004eb9d0 -> 0x004eba00
        joan::orig::as_track_res(this)->free();  // 0x004eba00
        return 0;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("X::fwd_4eba00", 0x004eb9d0, &H::fwd_4eba00);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
//
//    - Tracks::create_array 0x004ec240 (reader) ret 4 / CameraTracks::create_array 0x00429980
//      (reader) ret 4.
namespace {
namespace game_b84_03_2 {

using joan::hook::global;

using joan::call::cdecl_call;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_create_array, "Tracks::create_array");

constexpr img::Addr kMemory = d::g_mem;

std::int32_t atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
double atof_(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
void* memset_(void* d, std::int32_t c, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(c), joan::orig::arg::u32(n)); }
using joan::orig::helpers::line_base;
void* mem_realloc(void* p, std::int32_t n, img::Addr file, std::int32_t line) {
    return joan::orig::as_mem(obj(kMemory))->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(n), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058eb30
}

// Text script reader (0x00589550 object).
struct Reader {
    void* r;
    const char* tok() { return joan::orig::arg::as<const char*>(joan::orig::as_file_txt(r)->get_token(1)); }  // 0x00589c70
    void line(char* buf) { joan::orig::as_file_txt(r)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), 0x40); }          // 0x00589f10
    // tok(); atoi(tok())
    std::int32_t skip_int() {
        tok();
        return atoi_(tok());
    }
    double skip_num() {
        tok();
        return atof_(tok());
    }
};

struct H {
    // ------------------------------------------------------------------ 0x004ec240 / 0x00429980
    const void* create_array(void* reader, std::int32_t stride, img::Addr file, img::Addr line_global) {
        c_create_array.hit();
        Reader rd{reader};
        rd.tok();
        rd.tok();
        jo::self<jo::TrackRes>(this)->path_count = atoi_(rd.tok());
        const char* const t = rd.tok();
        if (jo::view<std::uint32_t>(jo::self<jo::TrackRes>(this)->path_count) == 0) return t;
        jo::view<void*>(jo::self<jo::TrackRes>(this)->paths) = mem_realloc(jo::view<void*>(jo::self<jo::TrackRes>(this)->paths), jo::self<jo::TrackRes>(this)->path_count * stride, file, line_base(line_global) + 0x13);
        return memset_(jo::view<void*>(jo::self<jo::TrackRes>(this)->paths), 0, static_cast<std::uint32_t>(jo::self<jo::TrackRes>(this)->path_count * stride));
    }
    const void* track_array(void* reader) { return create_array(reader, static_cast<std::int32_t>(sizeof(jo::PathGraph)), d::s_d_ww_trackres_cpp, d::g_a053ec); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("Tracks::create_array", 0x004ec240, &H::track_array);
#undef B

}  // namespace game_b84_03_2
}  // namespace

// ---------------------------------------- from modules/game_b82_18_2.cpp (game/b82_18)
namespace {
namespace game_b82_18_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;  // operator delete

struct H {
    // scalar deleting destructors
    void* sdd(std::uintptr_t dtor, std::uint32_t flags) {
        thiscall(dtor, this);
        if (flags & 1u) game_delete(this);
        return this;
    }
    void* sdd_4eb5f0(std::uint32_t flags) { return sdd(fn::code_value(fn::X_fwd_4eba00), flags); }  // 0x004eb9d0

    void* ctor_memset9() {  // 0x004eb9a0 / 0x00428cf0
        std::memset(static_cast<void*>(this), 0, sizeof(jo::TrackRes));      // memset (9 bytes on x86)
        return this;
    }

    std::uint32_t fwd_4eb520() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_tracks(this)->release()); }  // 0x004eb570
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("fwd_004eb520", 0x004eb520, &H::fwd_4eb520);
B82("sdd_004eb5f0", 0x004eb5f0, &H::sdd_4eb5f0);
B82("TrackRes::TrackRes", 0x004eb9a0, &H::ctor_memset9);
#undef B82

}  // namespace game_b82_18_2
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x004eb760
    char test_segment_4eb760(void* a, i32 index, Vec3 p, Vec3 q, u32 b) {
        u8* const list = jo::view<u8*>(jo::self<jo::Tracks>(this)->res);
        if (index == 0 || index > jo::as_track_res(list)->path_count) return 0;
        u8* const e = jo::view<u8*>(jo::as_track_res(list)->paths) + (index - 1) * static_cast<i32>(sizeof(jo::PathGraph));  // 0x52 on x86
        if (e == nullptr) return 0;
        return joan::orig::arg::as<char>(joan::orig::as_path_graph(e)->route(joan::orig::arg::bits(p.x), joan::orig::arg::bits(p.y), joan::orig::arg::bits(p.z), joan::orig::arg::bits(q.x), joan::orig::arg::bits(q.y), joan::orig::arg::bits(q.z), a, b));  // 0x004ec5f0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Path::test_segment_4eb760", 0x004eb760, &H::test_segment_4eb760);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct Vec3 {
    float x, y, z;
};

struct H {
    // 0x004eb810
    u32 slot_call(void* a, i32 idx, Vec3 va, Vec3 vb, u32 c) {
        if (idx == 0) return 0;
        u8* const tab = jo::view<u8*>(jo::self<jo::Tracks>(this)->res);
        if (idx > jo::as_track_res(tab)->path_count) return 0;
        const std::uintptr_t e = reinterpret_cast<std::uintptr_t>(jo::as_track_res(tab)->paths) + static_cast<std::uintptr_t>(static_cast<std::intptr_t>(idx - 1) * static_cast<std::intptr_t>(sizeof(jo::PathGraph)));
        if (e == 0) return 0;
        return joan::orig::arg::as<u32>(joan::orig::as_path_graph(reinterpret_cast<void*>(static_cast<std::uintptr_t>(e)))->find_path(va, vb, a, c));  // 0x004d3790
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("Slots::call_4d3790", 0x004eb810, &H::slot_call);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b103_05_2.cpp (game/b103_05)
namespace {
namespace game_b103_05_2 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------------ 0x004eb8c0
    std::uintptr_t table52_test(i32 index) {  // eax: al = the result
        if (index == 0) return 0;  // original: caller's eax with al = 0 (no references)
        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(jo::self<jo::Tracks>(this)->res->paths);
        const std::uintptr_t e = base + static_cast<std::uintptr_t>(static_cast<std::intptr_t>(index - 1) * static_cast<std::intptr_t>(sizeof(jo::PathGraph)));
        if (e == 0) return base & ~std::uintptr_t{0xff};
        return thiscall<u32>(fn::TrackRes_free_45cc20, reinterpret_cast<void*>(e));  // 0x0045cc20
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_05", name, addr, fn)
B("Table52::test", 0x004eb8c0, &H::table52_test);
#undef B

}  // namespace game_b103_05_2
}  // namespace

// ---------------------------------------- from modules/game_b102_05_2.cpp (game/b102_05)
namespace {
namespace game_b102_05_2 {

using joan::call::thiscall;
using joan::img::obj;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "by-value 12-byte argument");

struct H {
    // ------------------------------------------------------------------ 0x004eb920
    u32 call_4eb920(i32 index, Vec3 v) {
        if (index == 0) return 0;
        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(jo::self<jo::Tracks>(this)->res->paths);
        const std::uintptr_t elem = base + static_cast<std::uintptr_t>(static_cast<std::intptr_t>(index - 1) * static_cast<std::intptr_t>(sizeof(jo::PathGraph)));
        if (elem == 0) return 0;
        return joan::orig::as_track(obj(elem))->add_point(joan::orig::arg::bits(v.x), joan::orig::arg::bits(v.y), joan::orig::arg::bits(v.z));  // 0x004a9ac0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("Track::call_4eb920", 0x004eb920, &H::call_4eb920);
#undef B

}  // namespace game_b102_05_2
}  // namespace

// ---------------------------------------- from modules/game_b92_04_2.cpp (game/b92_04)
namespace {
namespace game_b92_04_2 {

using joan::call::import_call;
using joan::orig::helpers::at;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_nearest_two, "Points::nearest_two");

struct H {
    // ------------------------------------------------------------------------ 0x004ec480
    u32* nearest_two(u32* out1, u32* out2, V3 a, V3 b) {
        c_nearest_two.hit();
        float best1 = 999999.0f, best2 = 999999.0f;
        u32 id1 = 0, id2 = 0;
        for (i32 i = 0; i < jo::self<jo::PathGraph>(this)->node_count; ++i) {
            u8* const e = reinterpret_cast<u8*>(&jo::self<jo::PathGraph>(this)->nodes[i]);
            V3 tmp;
            V3 d = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(jo::as_track_node(e)->pos), &a));  // 0x0060cc90
            float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&d)));  // st0 (double) stored as a float
            if (!(len >= best1)) {  // C0: less or unordered
                best1 = len;
                id1 = jo::view<u32>(jo::as_track_node(e)->id);
            }
            V3 tmp2;
            V3 d2 = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp2, static_cast<void*>(jo::as_track_node(e)->pos), &b));  // 0x0060cc90
            len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&d2)));
            if (!(len >= best2)) {
                best2 = len;
                id2 = jo::view<u32>(jo::as_track_node(e)->id);
            }
        }
        *out1 = id1;
        *out2 = id2;
        return out2;
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Points::nearest_two", 0x004ec480, &H::nearest_two);
#undef B92

}  // namespace game_b92_04_2
}  // namespace

// ---------------------------------------- from modules/game_b109_15.cpp (game/b109_15)
namespace {
namespace game_b109_15 {

using joan::call::import_call;
using joan::orig::helpers::at;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;
using i8 = std::int8_t;

using joan::call::thiscall;
using joan::orig::helpers::wadd;

float fl(u32 bits) {
    float f;
    std::memcpy(&f, &bits, 4);
    return f;
}

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");

double v3d_length(const Vec3* v) { return import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(v)); }
// 0x0060cc90 (thunk 0x0040a010): out = a - b (float components), cdecl; returns out.
Vec3 vsub(const void* a, const void* b) {
    Vec3 tmp{0.0f, 0.0f, 0.0f};
    const Vec3* r = joan::orig::arg::as<const Vec3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
    return *r;
}

JOAN_COUNTER(c_route, "PathGraph::route_4ec5f0");

struct H {
    // ------------------------------------------------------------------ 0x004ec5f0
    u8* node(u32 index) { return reinterpret_cast<u8*>(jo::self<jo::PathGraph>(this)->nodes) + static_cast<std::intptr_t>(static_cast<i32>(index - 1u)) * static_cast<std::intptr_t>(sizeof(jo::TrackNode)); }  // 1-based
    u32 next_stamp() {
        if (jo::self<jo::PathGraph>(this)->u32_4e == 0) {
            for (i32 i = 0; i < jo::self<jo::PathGraph>(this)->node_count; ++i) jo::self<jo::PathGraph>(this)->nodes[i].search_stamp = 0;
            jo::self<jo::PathGraph>(this)->u32_4e = 1;
        }
        const u32 v = jo::self<jo::PathGraph>(this)->u32_4e;
        jo::self<jo::PathGraph>(this)->u32_4e = v + 1;
        return v;
    }
    static i32 path_count(void* path) { return joan::orig::arg::as<i32>(joan::orig::as_list(path)->count_66e8a0()); }       // 0x0066e8a0
    static const Vec3* path_get(void* path, i32 i) { return joan::orig::arg::as<const Vec3*>(joan::orig::as_list(path)->from_end(joan::orig::arg::i32(i))); }  // 0x0066e8d0 (a waypoint)
    static void path_clear(void* path) { joan::orig::as_path(path)->clear(); }                 // 0x004eeca0
    // Sum of the segment lengths from waypoint `first` - 1 to the end (the original's v74).
    static i32 rest_length(void* path, i32 first) {
        i32 v74 = 0;
        for (i32 k = first; k < path_count(path); ++k) {
            const Vec3* const pa = path_get(path, k - 1);
            const Vec3* const pb = path_get(path, k);
            const Vec3 a = *pa;
            const Vec3 b = *pb;
            const Vec3 d = vsub(&a, &b);
            const float acc = static_cast<float>(v74);
            const double len = v3d_length(&d);
            v74 = ftol(Fpu{true}.add(len, static_cast<double>(acc)));
        }
        return v74;
    }

    u8 route(u32 x1, u32 y1, u32 z1, u32 x2, u32 y2, u32 z2, void* path, u32 a24) {
        c_route.hit();
        path_clear(path);
        const i32 i8_ = ftol(static_cast<double>(fl(x1)));
        const i32 ic = ftol(static_cast<double>(fl(y1)));
        const i32 i10 = ftol(static_cast<double>(fl(z1)));
        const i32 i14 = ftol(static_cast<double>(fl(x2)));
        const i32 i18 = ftol(static_cast<double>(fl(y2)));
        const i32 i1c = ftol(static_cast<double>(fl(z2)));
        if (jo::view<u32>(jo::self<jo::PathGraph>(this)->node_count) == 0 || jo::self<jo::PathGraph>(this)->nodes == nullptr) return 0;
        u32 n20 = 0, n24 = 0;  // always written by 0x004ec480
        joan::orig::as_path_graph(this)->nearest_two(&n20, &n24, Vec3{fl(x1), fl(y1), fl(z1)}, Vec3{fl(x2), fl(y2), fl(z2)});  // 0x004ec480
        if (n20 == 0 || n24 == 0) return 0;
        u8* n28 = node(n20);
        u8* const n2c = node(n24);
        u32 v38 = next_stamp();
        const Vec3 v44{fl(x1), fl(y1), fl(z1)};
        float v48;
        {
            const Vec3 d = vsub(n28 + 8, &v44);
            v48 = static_cast<float>(v3d_length(&d));
        }
        {
            joan::orig::as_path_box(path)->set(joan::orig::arg::i32(i8_), joan::orig::arg::i32(ic), joan::orig::arg::i32(i10), joan::orig::arg::i32(i14), joan::orig::arg::i32(i18), joan::orig::arg::i32(i1c));  // 0x0066e640
            if (joan::orig::arg::as<i8>(joan::orig::as_path(this)->find_4ed040(joan::orig::arg::mptr(n28), joan::orig::arg::mptr(n2c), joan::orig::arg::u32(v38), joan::orig::arg::mptr(path), joan::orig::arg::i32(a24))) == 0) {  // 0x004ed040
                path_clear(path);
                return 0;
            }
            const Vec3* const v4c = path_get(path, 1);
            const Vec3* v50 = path_get(path, 2);
            if (v50 != nullptr) {
                i32 v54 = 2;
                const Vec3 v60 = *v4c;
                Vec3 v6c = *v50;
                i32 v70;
                {
                    const Vec3 d = vsub(&v60, &v6c);
                    v70 = ftol(v3d_length(&d));
                }
                {
                    const Vec3 d = vsub(&v6c, &v44);
                    v48 = static_cast<float>(v3d_length(&d));
                }
                i32 v74 = rest_length(path, 3);
                for (;;) {
                    if (v4c == nullptr || v50 == nullptr || v70 <= 0) break;
                    if (v70 >= 10 && !(v48 < 10.0f || v48 != v48)) break;  // 0x9cfc20 = 10.0f; NaN continues
                    const double a = static_cast<double>(wadd(v74, v70));
                    const double b = Fpu{true}.add(static_cast<double>(v74), static_cast<double>(v48));
                    if (!(b > a)) break;
                    ++v54;
                    v50 = path_get(path, v54);
                    if (v50 == nullptr) break;
                    v6c = *v50;
                    {
                        const Vec3 d = vsub(&v60, &v6c);
                        v70 = ftol(v3d_length(&d));
                    }
                    {
                        const Vec3 d = vsub(&v6c, &v44);
                        v48 = static_cast<float>(v3d_length(&d));
                    }
                    v74 = rest_length(path, v54 + 1);
                }
                if (v50 == nullptr) {
                    path_clear(path);
                    return 0;
                }
                const Vec3 from = *v50;
                joan::orig::as_path_graph(this)->nearest_two(&n20, &n24, from, Vec3{fl(x2), fl(y2), fl(z2)});  // 0x004ec480
                n28 = node(n20);
                v38 = next_stamp();
            }
            path_clear(path);
        }
        joan::orig::as_path_box(path)->set(joan::orig::arg::i32(ftol(static_cast<double>(jo::as_track_node(n28)->pos[0]))), joan::orig::arg::i32(// 0x0066e640
                      ftol(static_cast<double>(jo::as_track_node(n28)->pos[1]))), joan::orig::arg::i32(ftol(static_cast<double>(jo::as_track_node(n28)->pos[2]))), joan::orig::arg::i32(ftol(static_cast<double>(jo::as_track_node(n2c)->pos[0]))), joan::orig::arg::i32(ftol(static_cast<double>(jo::as_track_node(n2c)->pos[1]))), joan::orig::arg::i32(ftol(static_cast<double>(jo::as_track_node(n2c)->pos[2]))));
        if (joan::orig::arg::as<i8>(joan::orig::as_path(this)->find_4ed040(joan::orig::arg::mptr(n28), joan::orig::arg::mptr(n2c), joan::orig::arg::u32(v38), joan::orig::arg::mptr(path), joan::orig::arg::i32(a24))) != 0) {  // 0x004ed040
            joan::orig::as_path_box(path)->set_end(joan::orig::arg::i32(i14), joan::orig::arg::i32(i18), joan::orig::arg::i32(i1c));                     // 0x0066e720
            return 1;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x00751750
    struct SlideFrame {
        char text[0x20];  // [ebp-0x34]
        i32 i;            // [ebp-0x14]
        i32 count;        // [ebp-0x10]
        i32 j;            // [ebp-0xc]
        i32 dy;           // [ebp-0x8]
        void* self;       // [ebp-0x4]
    };
#if UINTPTR_MAX == 0xffffffffu  // the original (x86) frame
    static_assert(sizeof(SlideFrame) == 0x34, "slide frame");
#endif
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_15", name, addr, fn)
B("PathGraph::route_4ec5f0", 0x004ec5f0, &H::route);
#undef B

}  // namespace game_b109_15
}  // namespace

// ---------------------------------------- from modules/game_b109_02.cpp (game/b109_02)
namespace {
namespace game_b109_02 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::x87::ftol;
using i8 = std::int8_t;

using joan::call::thiscall;
using joan::orig::helpers::wadd;

using i16 = std::int16_t;

JOAN_COUNTER(c_path, "Path::find_4ed040");

struct H {
    // ---- 0x004ed040 helpers (this = path finder)
    static void emit_step(void* out, void* q) {
        joan::orig::as_path(out)->add_segment(joan::orig::arg::i32(jo::as_track_node(q)->grid_x), joan::orig::arg::i32(jo::as_track_node(q)->grid_y), static_cast<i32>(jo::as_track_node(q)->i16_34), joan::orig::arg::i32(jo::as_track_node(q)->grid_x), joan::orig::arg::i32(jo::as_track_node(q)->grid_y), static_cast<i32>(jo::as_track_node(q)->i16_34));  // 0x0066e790
    }
    static i32 before(void* a, void* b) { return joan::orig::arg::as<i32>(joan::orig::as_track_node(a)->sorts_before(joan::orig::arg::mptr(b))); }  // 0x0045d0f0

    // 0x004ed040
    u8 find_path(void* start, void* goal, u32 ctx, void* out, i32 steps) {
        c_path.hit();
        if (start == goal) return 1;
        i32 pt[2] = {0, 0};
        joan::orig::as_point(pt)->set(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x004eeb80
        pt[0] = joan::hook::x87::ftol(jo::as_track_node(goal)->pos[0]);
        pt[1] = joan::hook::x87::ftol(jo::as_track_node(goal)->pos[1]);
        joan::orig::as_anim(start)->start(joan::orig::arg::u32(ctx), nullptr);  // 0x00489820
        jo::as_track_node(start)->cost_so_far = 0;
        jo::as_track_node(start)->cost_heuristic =
            joan::orig::arg::as<i32>(joan::orig::as_path(this)->distance_4eda90(joan::orig::arg::u32(pt[0]), joan::orig::arg::u32(pt[1]), joan::orig::arg::u32(jo::as_track_node(start)->grid_x), joan::orig::arg::u32(jo::as_track_node(start)->grid_y)));  // 0x004eda90
        jo::as_track_node(start)->cost_total = wadd(jo::as_track_node(start)->cost_so_far, jo::as_track_node(start)->cost_heuristic);
        jo::as_track_node(start)->weight_sum = jo::as_track_node(start)->weight;
        const i32 w48 = jo::as_track_node(start)->weight < 0 ? 0 : jo::as_track_node(start)->weight;
        jo::as_track_node(start)->weight_max = static_cast<i16>(w48);
        void* open = start;
        void* closed = nullptr;
        for (; open != nullptr && steps > 0; --steps) {
            void* const cur = open;
            open = jo::as_track_node(cur)->next_open;
            jo::as_track_node(cur)->next_open = nullptr;
            if (cur == goal) {
                for (void* p = cur; jo::as_track_node(p)->parent != nullptr; p = jo::as_track_node(p)->parent) {
                    for (i32 i = 0; i < jo::as_track_node(p)->link_count; ++i) {
                        if (jo::as_track_node(p)->links[i].p == jo::as_track_node(p)->parent) emit_step(out, jo::as_track_node(p)->parent);
                    }
                }
                return 1;
            }
            for (i32 i = 0; i < jo::as_track_node(cur)->link_count; ++i) {
                void* const nb = jo::as_track_node(cur)->links[i].p;
                if (nb == nullptr) continue;
                if (nb == jo::as_track_node(cur)->parent) continue;
                if (joan::orig::arg::as<i32>(joan::orig::as_track_node(nb)->has_stamp(joan::orig::arg::u32(ctx))) == 0) {  // 0x004eed10
                    joan::orig::as_anim(nb)->start(joan::orig::arg::u32(ctx), cur);  // 0x00489820
                    const i32 step = joan::orig::arg::as<i32>(joan::orig::as_path(this)->random_range(joan::orig::arg::u32(jo::as_track_node(nb)->grid_x), joan::orig::arg::u32(jo::as_track_node(nb)->grid_y), joan::orig::arg::u32(jo::as_track_node(cur)->grid_x), joan::orig::arg::u32(jo::as_track_node(cur)->grid_y)));  // 0x004ee2e0
                    jo::as_track_node(nb)->cost_so_far = wadd(jo::as_track_node(cur)->cost_so_far, step);
                    jo::as_track_node(nb)->cost_heuristic = joan::orig::arg::as<i32>(joan::orig::as_path(this)->distance_4eda90(joan::orig::arg::u32(pt[0]), joan::orig::arg::u32(pt[1]), joan::orig::arg::u32(jo::as_track_node(nb)->grid_x), joan::orig::arg::u32(jo::as_track_node(nb)->grid_y)));
                    jo::as_track_node(nb)->cost_total = wadd(jo::as_track_node(nb)->cost_so_far, jo::as_track_node(nb)->cost_heuristic);
                    jo::as_track_node(nb)->weight_sum = wadd(jo::as_track_node(cur)->weight_sum, jo::as_track_node(nb)->weight);
                    jo::as_track_node(nb)->weight_max = jo::as_track_node(cur)->weight_max > jo::as_track_node(nb)->weight ? jo::as_track_node(cur)->weight_max : jo::as_track_node(nb)->weight;
                    if (open == nullptr || before(nb, open) != 0) {
                        jo::as_track_node(nb)->next_open = jo::as_track_node(open);
                        open = nb;
                    } else {
                        void* p = open;
                        while (jo::as_track_node(p)->next_open != nullptr && before(nb, jo::as_track_node(p)->next_open) == 0) p = jo::as_track_node(p)->next_open;
                        jo::as_track_node(nb)->next_open = jo::as_track_node(p)->next_open;
                        jo::as_track_node(p)->next_open = jo::as_track_node(nb);
                    }
                } else {
                    const i32 step = joan::orig::arg::as<i32>(joan::orig::as_path(this)->random_range(joan::orig::arg::u32(jo::as_track_node(nb)->grid_x), joan::orig::arg::u32(jo::as_track_node(nb)->grid_y), joan::orig::arg::u32(jo::as_track_node(cur)->grid_x), joan::orig::arg::u32(jo::as_track_node(cur)->grid_y)));
                    const i32 g = wadd(jo::as_track_node(cur)->cost_so_far, step);
                    const i32 hh = jo::as_track_node(nb)->cost_heuristic;
                    const i32 fsum = wadd(g, hh);
                    const i32 wsum = wadd(jo::as_track_node(cur)->weight_sum, jo::as_track_node(nb)->weight_sum);
                    const i32 wmax = jo::as_track_node(cur)->weight_max > jo::as_track_node(nb)->weight ? jo::as_track_node(cur)->weight_max : jo::as_track_node(nb)->weight;
                    const i32 cmp = joan::orig::arg::as<i32>(joan::orig::as_path(this)->pick_diff(joan::orig::arg::i32(fsum), joan::orig::arg::i32(wsum), joan::orig::arg::i32(wmax), joan::orig::arg::i32(jo::as_track_node(nb)->cost_total), joan::orig::arg::i32(jo::as_track_node(nb)->weight_sum), static_cast<i32>(jo::as_track_node(nb)->weight_max)));  // 0x004ee320
                    if (cmp >= 0) continue;
                    joan::orig::as_member(nb)->set_group_48be30(joan::orig::arg::mptr(cur));  // 0x0048be30
                    jo::as_track_node(nb)->cost_so_far = g;
                    jo::as_track_node(nb)->cost_heuristic = hh;
                    jo::as_track_node(nb)->cost_total = fsum;
                    jo::as_track_node(nb)->weight_sum = wsum;
                    jo::as_track_node(nb)->weight_max = static_cast<i16>(wmax);
                    if (jo::as_track_node(nb)->closed == 0) {
                        if (open == nb) {
                            open = jo::as_track_node(nb)->next_open;
                            jo::as_track_node(nb)->next_open = nullptr;
                        } else {
                            void* p = open;
                            while (jo::as_track_node(p)->next_open != nullptr && jo::as_track_node(p)->next_open != nb) p = jo::as_track_node(p)->next_open;
                            jo::as_track_node(p)->next_open = jo::as_track_node(nb)->next_open;
                            jo::as_track_node(nb)->next_open = nullptr;
                        }
                        if (open == nullptr || !(jo::as_track_node(nb)->cost_total > jo::as_track_node(open)->cost_total)) {
                            jo::as_track_node(nb)->next_open = jo::as_track_node(open);
                            open = nb;
                        } else {
                            void* p = open;
                            while (jo::as_track_node(p)->next_open != nullptr && before(nb, jo::as_track_node(p)->next_open) == 0) p = jo::as_track_node(p)->next_open;
                            jo::as_track_node(nb)->next_open = jo::as_track_node(p)->next_open;
                            jo::as_track_node(p)->next_open = jo::as_track_node(nb);
                        }
                    }
                    if (jo::as_track_node(nb)->child_count > 0) joan::orig::as_path(this)->propagate_costs(joan::orig::arg::mptr(nb));  // 0x004892d0
                }
            }
            jo::as_track_node(cur)->next_open = jo::as_track_node(closed);
            closed = cur;
            jo::view<u8>(jo::as_track_node(cur)->closed) = 1;
        }
        void* best = nullptr;
        i32 best_v = 0;
        for (void* p = closed; p != nullptr; p = jo::as_track_node(p)->next_open) {
            const i32 v = wadd(jo::as_track_node(p)->cost_so_far / 16, jo::as_track_node(p)->cost_heuristic);
            if (best == nullptr || v < best_v) {
                best = p;
                best_v = v;
            }
        }
        for (void* p = open; p != nullptr; p = jo::as_track_node(p)->next_open) {
            const i32 v = wadd(jo::as_track_node(p)->cost_so_far / 16, jo::as_track_node(p)->cost_heuristic);
            if (best == nullptr || v < best_v) {
                best = p;
                best_v = v;
            }
        }
        if (best != nullptr) {
            joan::orig::as_path_box(out)->set_end(joan::orig::arg::i32(jo::as_track_node(best)->grid_x), joan::orig::arg::i32(jo::as_track_node(best)->grid_y), static_cast<i32>(jo::as_track_node(best)->i16_34));  // 0x0066e720
            for (void* p = best; jo::as_track_node(p)->parent != nullptr; p = jo::as_track_node(p)->parent) {
                for (i32 i = 0; i < jo::as_track_node(p)->link_count; ++i) {
                    if (jo::as_track_node(p)->links[i].p == jo::as_track_node(p)->parent) {
                        emit_step(out, jo::as_track_node(p)->parent);
                        break;
                    }
                }
            }
        }
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_02", name, addr, fn)
B("Path::find_4ed040", 0x004ed040, &H::find_path);
#undef B

}  // namespace game_b109_02
}  // namespace

// ---------------------------------------- from modules/game_b106_10.cpp (game/b106_10)
namespace {
namespace game_b106_10 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::img::obj;

constexpr img::Addr kRandom = d::g_misc;

struct H {
    // 0x004eda90
    u32 distance_4eda90(u32 a, u32 b, u32 c, u32 d) {
        return joan::orig::arg::as<u32>(joan::orig::as_misc(obj(kRandom))->points_distance_590410(joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(a), joan::orig::arg::i32(b)));  // 0x00590410
    }

  private:
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_10", name, addr, fn)
B("Obj::distance_4eda90", 0x004eda90, &H::distance_4eda90);
#undef B

}  // namespace game_b106_10
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::img::obj;

void* const kRandom = obj(d::g_misc);

struct H {
    // ------------------------------------------------------------------------ 0x004ee2e0
    i32 random_range(u32 a, u32 b, u32 c, u32 d) {
        return joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->points_distance_590410(joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(a), joan::orig::arg::i32(b)));  // 0x00590410
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("X::random_range", 0x004ee2e0, &H::random_range);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

using i32 = std::int32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004ee320
    i32 pick_diff(i32 a1, i32 a2, i32 a3, i32 a4, i32 a5, i32 a6) {
        (void)a2;
        (void)a5;
        if (a3 != a6) return a3 - a6;
        return a1 - a4;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("pick_diff", 0x004ee320, &H::pick_diff);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* reset_34() {  // 0x004ee410
        joan::orig::as_point(&jo::self<jo::PathSearchState>(this)->pos)->construct();  // 0x004eea60
        memset_(this, 0, static_cast<u32>(sizeof(jo::PathSearchState)));  // 0x34 on x86
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("Struct34::reset", 0x004ee410, &H::reset_34);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

using joan::orig::helpers::at;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

struct H {
    // ------------------------------------------------------------------------ 0x004ee530
    i32 free_8() {
        if (jo::self<jo::PathSearchState>(this)->buffer == nullptr) return static_cast<i32>(reinterpret_cast<std::uintptr_t>(this));
        joan::orig::operator_(jo::self<jo::PathSearchState>(this)->buffer);  // operator delete (eax garbage: the only caller, 0x0043ab30, and its callers ignore it)
        jo::self<jo::PathSearchState>(this)->buffer = nullptr;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Obj::free_8", 0x004ee530, &H::free_8);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b101_07.cpp (game/b101_07)
namespace {
namespace game_b101_07_2 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ 0x004ee5e0
    void* clear_4ee5e0() {
        joan::orig::as_path_search_state(&jo::self<jo::TrackNode>(this)->search_stamp)->reset();  // 0x004ee410
        crt_memset(this, 0, sizeof(jo::TrackNode));
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_07", name, addr, fn)
B("Obj::clear_4ee5e0", 0x004ee5e0, &H::clear_4ee5e0);
#undef B

}  // namespace game_b101_07_2
}  // namespace

// ---------------------------------------- from modules/game_b100_07.cpp (game/b100_07)
namespace {
namespace game_b100_07 {

struct H {
    void nop_ee8() {}  // 0x004ee8f0
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_07", name, addr, fn)
B100("X::nop_ee8", 0x004ee8f0, &H::nop_ee8);
#undef B100

}  // namespace game_b100_07
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004eea00
    i32 key_diff(i32 a, i32, i32 c, i32 d, i32, i32 g) {
        if (c != g) return static_cast<i32>(static_cast<u32>(c) - static_cast<u32>(g));
        return static_cast<i32>(static_cast<u32>(a) - static_cast<u32>(d));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("X::key_diff", 0x004eea00, &H::key_diff);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

struct H {
    void* self_4eea60() { return this; }                                         // 0x004eea60
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj4eea60::self", 0x004eea60, &H::self_4eea60);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b82_18_3.cpp (game/b82_18)
namespace {
namespace game_b82_18_3 {

using joan::orig::helpers::at;

struct H {
    // 0x004eeb80 stores two dwords; every caller (Point::set) builds an integer point (x, y) in an 8-byte local, so
    // the stores are 4 bytes wide on x64 too (the Pair facade's pointer members would write 16 bytes).
    void* pair_set(std::uint32_t a, std::uint32_t b) {  // 0x004eeb80
        jo::view<std::uint32_t>(jo::self<jo::Point>(this)->x_) = a;
        jo::view<std::uint32_t>(jo::self<jo::Point>(this)->y_) = b;
        return this;
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("Pair::set", 0x004eeb80, &H::pair_set);
#undef B82

}  // namespace game_b82_18_3
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

using joan::call::thiscall;

struct H {
    // 0x004eeca0
    std::int32_t reset_4eeca0() {
        joan::orig::as_dynarray(this)->zap_584580(0);  // 0x00584580
        return 0;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Obj4ee::reset", 0x004eeca0, &H::reset_4eeca0);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

using joan::orig::helpers::at;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004eed10
    u32 id_is(u32 v) { return jo::self<jo::TrackNode>(this)->search_stamp == v ? 1u : 0u; }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("X::id_is", 0x004eed10, &H::id_is);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Track::free_points() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b101_01::H::track_free_45cc20>(this)); }  // 0x45cc20
auto joan::orig::Track::add_point(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_04::H::track_add>(this, a0, a1, a2)); }  // 0x4a9ac0
auto joan::orig::Tracks::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b::H::tracks_ctor>(this)); }  // 0x4eb4f0
auto joan::orig::Tracks::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_18_2::H::fwd_4eb520>(this)); }  // 0x4eb520
auto joan::orig::Tracks::init() -> void { joan::bind::member<&::game_leaves82b::H::empty>(this); }  // 0x4eb550
auto joan::orig::Tracks::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b83_07_2::H::destroy_c>(this)); }  // 0x4eb570
auto joan::orig::TrackRes::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18_2::H::sdd_4eb5f0>(this, a0)); }  // 0x4eb5f0
auto joan::orig::Tracks::load_4eb640(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves82b::H::tracks_load>(this, a0)); }  // 0x4eb640
auto joan::orig::TrackRes::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18_2::H::ctor_memset9>(this)); }  // 0x4eb9a0
auto joan::orig::TrackRes::free() -> void { joan::bind::member<&::game_b82_18::H::track_free>(this); }  // 0x4eba00
auto joan::orig::TrackRes::create_array(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_03_2::H::track_array>(this, a0)); }  // 0x4ec240
auto joan::orig::NameList::find_4ec320(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_04::H::namelist_find>(this, a0)); }  // 0x4ec320
auto joan::orig::PathGraph::free() -> void { joan::bind::member<&::game_b101_07::H::track_free>(this); }  // 0x4ec3d0
auto joan::orig::PathGraph::route(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, void* a6, std::uint32_t a7) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b109_15::H::route>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x4ec5f0
auto joan::orig::Path::find_4ed040(void* a0, void* a1, std::uint32_t a2, void* a3, std::int32_t a4) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b109_02::H::find_path>(this, a0, a1, a2, a3, a4)); }  // 0x4ed040
auto joan::orig::Path::distance_4eda90(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_10::H::distance_4eda90>(this, a0, a1, a2, a3)); }  // 0x4eda90
auto joan::orig::Path::random_range(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_02::H::random_range>(this, a0, a1, a2, a3)); }  // 0x4ee2e0
auto joan::orig::Path::pick_diff(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_04::H::pick_diff>(this, a0, a1, a2, a3, a4, a5)); }  // 0x4ee320
auto joan::orig::PathSearchState::reset() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_04::H::reset_34>(this)); }  // 0x4ee410
auto joan::orig::PathSearchState::free_buffer() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_05::H::free_8>(this)); }  // 0x4ee530
auto joan::orig::TrackNode::nop_4ee8f0() -> void { joan::bind::member<&::game_b100_07::H::nop_ee8>(this); }  // 0x4ee8f0
auto joan::orig::TrackNode::compare_keys(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_04::H::key_diff>(this, a0, a1, a2, a3, a4, a5)); }  // 0x4eea00
auto joan::orig::Point::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::self_4eea60>(this)); }  // 0x4eea60
auto joan::orig::Point::set(std::uint32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18_3::H::pair_set>(this, a0, a1)); }  // 0x4eeb80
auto joan::orig::Path::clear() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b88_06::H::reset_4eeca0>(this)); }  // 0x4eeca0
auto joan::orig::TrackNode::has_stamp(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_04::H::id_is>(this, a0)); }  // 0x4eed10
