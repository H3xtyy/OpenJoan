// game/world_s.cpp: functions of the original d:\ww\world_s.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>
#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../runtime/hitch.hpp"

#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../render/water.hpp"
#include "stream_cache_api.hpp"
#include "../en3d/scene_lists.hpp"
#include "item_preload.hpp"


#include "../runtime/replay/replay.hpp"


namespace jo = joan::orig;

// The tile order of Cell::build_ground while recording / replaying (compare_24, joan::replay::stable_ground_order).
namespace joan::ground_order {
const void* g_cell = nullptr;  // the cell whose tiles are sorted, or null: the original's address order
// 1-based index of `owner` in the cell's sector list (as Cell::build_ground looks it up), 0 if absent.
std::int32_t sector_index(const void* cell, const void* owner) {
    for (std::int32_t i = jo::as_cell(cell)->sector_count; i >= 1; --i)
        if (owner == jo::as_cell(cell)->sectors[i - 1].item) return i;
    return 0;
}
}  // namespace joan::ground_order

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::call::cdecl_call;

using joan::orig::helpers::stricmp_;

using joan::hook::x87::Fpu;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

// The packed colour of a VertexF1 / VertexF2 (attr[3], bytes +0x18 .. +0x1b).
std::uint8_t* vcol1(void* v) { return reinterpret_cast<std::uint8_t*>(&jo::as_vertex_f1(v)->attr[3]); }
std::uint8_t* vcol2(void* v) { return reinterpret_cast<std::uint8_t*>(&jo::as_vertex_f2(v)->attr[3]); }
#pragma pack(push, 1)
// A .pln directory entry (file format, 0x28 bytes): written by Cell::build_ground, read by World::load_pln.
struct PlnEntry {
    char name[0x22];
    std::int16_t sector;   // +0x22 sector number tried first
    std::int32_t offset;   // +0x24 file offset of the mesh
};
// GridCell::detail entries (levels 1 / 2): colour and height.
struct DetailEntry {
    std::uint8_t rgb[3];
    std::int16_t height;
};
#pragma pack(pop)
static_assert(sizeof(PlnEntry) == 0x28 && sizeof(DetailEntry) == 5);
// The cell's name (+0x20).
std::uint8_t* cell_name(const void* cell) { return reinterpret_cast<std::uint8_t*>(jo::as_cell(cell)->name); }

struct Obj {};
using joan::call::import_call;
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

constexpr Fpu kFpu{true};

struct Vec3 {
    float x, y, z;
};

// CRT.
std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
void strcpy_(char* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
double atof_(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
std::int32_t atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }

// Reader.
char* token(void* reader) { return joan::orig::as_file_txt(reader)->get_token(1); }  // 0x00589c70
void read_str(void* reader, char* buf, std::int32_t max) {
    joan::orig::as_file_txt(reader)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(max));  // 0x00589f10
}
// buf[strlen(buf) - 1] = 0 (writes buf[-1] for an empty string, inside the frame).
void chop(char* buf) { *(buf + static_cast<std::int32_t>(strlen_(buf)) - 1) = 0; }

using i16 = std::int16_t;

// A cell of the terrain grid built from the .GRI file: Cell::load_files 0x004f2f80 allocates sector_rows + 1 rows
// of sector_cols + 1 cells (and the array of row pointers), World::build_grid 0x004f6eb0 fills them, Cell::
// build_ground 0x0055b720 meshes them, load_files frees them. Bytes 0..0xf come from the file as 16 RLE planes
// (plane k = byte k of every cell, row by row).
#pragma pack(push, 1)
struct GridCell {
    std::uint8_t sector;          // 0x00 nonzero: the cell gets a sector ("newGrid<i><j>")
    std::uint8_t rgb[3];          // 0x01 vertex colour
    std::uint8_t shade;           // 0x04 255 - vertex alpha; also the second height grid
    std::uint8_t unused_5[2];     // 0x05
    std::int16_t height;          // 0x07
    std::int16_t sector_name;     // 0x09 1-based index into the file's sector names (0: "Default scene sector")
    std::int16_t texture[2];      // 0x0b 1-based indices into the file's texture names (0: none)
    std::uint8_t level;           // 0x0f subdivision: 0 flat quad, 1 (9 entries), 2 (25 entries)
    char texture_name[2][0x20];   // 0x10 the names of texture[0] / texture[1]
    std::uint8_t* detail;         // 0x50 levels 1 / 2: per entry r, g, b and an int16 height (5 bytes)
    std::uint8_t* planes() { return reinterpret_cast<std::uint8_t*>(this); }  // bytes 0..0xf, as read
};
#pragma pack(pop)
#if UINTPTR_MAX == 0xffffffffu  // the original's 0x54-byte record
static_assert(sizeof(GridCell) == 0x54 && offsetof(GridCell, level) == 0xf && offsetof(GridCell, detail) == 0x50, "GridCell");
#endif

void read_line(void* s, char* buf, std::int32_t max) { joan::orig::as_file_txt(s)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(max)); }  // 0x00589f10

struct Box {
    float v[6];
};

using u16 = std::uint16_t;

using joan::hook::x87::ftol;

}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
//
//    - WorldS::add_entry 0x0042b270 (no args) (world_s.cpp).
namespace {
namespace game_b83_07 {


JOAN_COUNTER(c_worlds_add, "WorldS::add_entry");

constexpr img::Addr kMemory = d::g_mem;

struct H {

    // ------------------------------------------------------------------ 0x0042b270
    std::int32_t worlds_add() {
        c_worlds_add.hit();
        if (jo::self<jo::Cell>(this)->sector_count >= jo::view<std::int32_t>(jo::self<jo::Cell>(this)->u32_68)) {
            jo::view<std::int32_t>(jo::self<jo::Cell>(this)->u32_68) += 0x100;
            jo::view<void*>(jo::self<jo::Cell>(this)->sectors) = joan::orig::as_mem(obj(kMemory))->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Cell>(this)->sectors)), joan::orig::arg::u32(jo::view<std::int32_t>(jo::self<jo::Cell>(this)->u32_68) * static_cast<std::int32_t>(sizeof(jo::WorldSectorPtrArray))), str(d::s_d_ww_world_s_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a0b78c)) + 5));  // 0x0058eb30
            jo::view<void*>(jo::self<jo::Cell>(this)->sector_names) = joan::orig::as_mem(obj(kMemory))->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Cell>(this)->sector_names)), joan::orig::arg::u32(jo::view<std::int32_t>(jo::self<jo::Cell>(this)->u32_68) * 0x21), str(d::s_d_ww_world_s_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a0b78c)) + 6));
        }
        const std::int32_t n = jo::self<jo::Cell>(this)->sector_count + 1;
        jo::self<jo::Cell>(this)->sector_count = jo::self<jo::Cell>(this)->sector_count + 1;
        jo::SceneS* const v = joan::orig::arg::as<jo::SceneS*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Cell>(this)->world->scene))->add_node());  // 0x005ea460
        jo::self<jo::Cell>(this)->sectors[n - 1].item = v;
        jo::self<jo::Cell>(this)->sector_names[(n - 1) * 0x21] = 0;  // 0x21-byte names
        return n;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("WorldS::add_entry", 0x0042b270, &H::worlds_add);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b91_01.cpp (game/b91_01)
//
//      0x0043d340 World::release()                                   thiscall
//
//  - 0x0043d340 returns this in eax on every path.
namespace {
namespace game_b91_01 {


// ---------------------------------------------------------------------------------------------
JOAN_COUNTER(c_world_release, "World::release");

void game_free(void* p, std::int32_t line) {  // 0x0058ed70 on the allocator 0x00a48838
    joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(p), str(d::s_d_ww_world_s_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a0b788)) + line));
}

struct H {

    // 0x0043d340
    void* world_release() {
        c_world_release.hit();
        joan::orig::as_cell(this)->unload();  // 0x0042ac40
        joan::orig::as_cell(this)->unload_files();  // 0x00449780
        for (void* p = jo::view<void*>(jo::self<jo::Cell>(this)->items_head); p != nullptr;) {
            void* const next = jo::as_world_item(p)->cell_next;
            joan::orig::as_world_item(p)->destroy(1u);  // 0x00416ca0 deleting dtor
            p = next;
        }
        if (jo::self<jo::Cell>(this)->sectors != nullptr) {
            game_free(jo::view<void*>(jo::self<jo::Cell>(this)->sectors), 0x1d);
            jo::self<jo::Cell>(this)->sectors = nullptr;
        }
        if (jo::self<jo::Cell>(this)->sector_names != nullptr) {
            game_free(jo::view<void*>(jo::self<jo::Cell>(this)->sector_names), 0x22);
            jo::self<jo::Cell>(this)->sector_names = nullptr;
        }
        jo::self<jo::Cell>(this)->u32_68 = 0;
        if (jo::self<jo::Cell>(this)->occluders != nullptr) {
            game_free(jo::view<void*>(jo::self<jo::Cell>(this)->occluders), 0x29);
            jo::self<jo::Cell>(this)->occluders = nullptr;
        }
        jo::view<std::uint32_t>(jo::self<jo::Cell>(this)->occluder_count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Cell>(this)->occluder_cap) = 0;
        if (jo::self<jo::Cell>(this)->p_84 != nullptr) {
            game_free(jo::self<jo::Cell>(this)->p_84, 0x31);
            jo::self<jo::Cell>(this)->p_84 = nullptr;
        }
        if (jo::self<jo::Cell>(this)->rows != nullptr) {
            for (std::int32_t i = 0; i <= static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_rows); ++i) {
                game_free(jo::view<std::int16_t**>(jo::self<jo::Cell>(this)->rows)[i], 0x39);
                jo::view<std::int16_t**>(jo::self<jo::Cell>(this)->rows)[i] = nullptr;
            }
            game_free(jo::view<void*>(jo::self<jo::Cell>(this)->rows), 0x3c);
            jo::self<jo::Cell>(this)->rows = nullptr;
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_01", name, addr, fn)
B("World::release", 0x0043d340, &H::world_release);
#undef B

}  // namespace game_b91_01
}  // namespace

// ---------------------------------------- from modules/game_b82_16.cpp (game/b82_16)
//
//    - Scene::load_script 0x0044e5f0 (reader, flag): CreateLight / CreateNode / CreateObject /
//      CreateWater / CreateTrigger / CreateProjectiveShadow / CreateParticle.
namespace {
namespace game_b82_16 {


// Stack dynamic array (ilib/dynarr.cpp, 0x1d bytes used).
struct alignas(4) DynArr {
    std::uint8_t b[((sizeof(jo::DynArray) > 0x20 ? sizeof(jo::DynArray) : 0x20) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger DynArray
};

JOAN_COUNTER(c_script, "Scene::load_script");

using joan::orig::helpers::pump;

struct H {

    // ------------------------------------------------------------------ 0x0044e5f0
    std::int32_t load_script(void* reader, std::int32_t flag) {
        c_script.hit();
        auto token = [&]() { return joan::orig::as_file_txt(reader)->get_token(1); };  // 0x00589c70
        char buf[0x100];
        if (flag != 0) {
            token();
            token();
            token();
            joan::orig::as_file_txt(reader)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), 0xff);  // 0x00589f10
            jo::self<jo::Cell>(this)->sector_rows = static_cast<std::int16_t>(joan::crt::atoi(joan::orig::arg::str(buf)));  // atoi
            token();
            joan::orig::as_file_txt(reader)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), 0xff);
            jo::self<jo::Cell>(this)->sector_cols = static_cast<std::int16_t>(joan::crt::atoi(joan::orig::arg::str(buf)));
            token();
        }
        DynArr list;
        joan::orig::dynarray(&list)->construct(4, 0x80, 0);  // 0x005843d0
        for (;;) {
            char* const t = token();
            if (t == nullptr) break;
            if (stricmp_(t, str(d::s_CreateLight)) == 0) {         // CreateLight
                joan::orig::as_scene(this)->create_light_49b970(joan::orig::arg::mptr(reader), joan::orig::arg::i32(flag));      // 0x0049b970
            } else if (stricmp_(t, str(d::s_CreateNode)) == 0) {  // CreateNode
            } else if (stricmp_(t, str(d::s_CreateObject)) == 0) {  // CreateObject
                joan::orig::as_cell(this)->create_object(joan::orig::arg::mptr(reader), joan::orig::arg::i32(flag), &list);  // 0x004f3df0
            } else if (stricmp_(t, str(d::s_CreateWater)) == 0) {  // CreateWater
                joan::orig::as_cell(this)->create_water(joan::orig::arg::mptr(reader), joan::orig::arg::i32(flag));      // 0x004f5210
            } else if (stricmp_(t, str(d::s_CreateTrigger)) == 0) {  // CreateTrigger
                joan::orig::as_scene(this)->create_trigger(joan::orig::arg::mptr(reader), joan::orig::arg::i32(flag));      // 0x00450ea0
            } else if (stricmp_(t, str(d::s_CreateProjectiveShadow)) == 0) {  // CreateProjectiveShadow
                joan::orig::as_scene(this)->create_projective_shadow(joan::orig::arg::mptr(reader), joan::orig::arg::i32(flag));      // 0x004749c0
            } else if (stricmp_(t, str(d::s_CreateParticle)) == 0) {  // CreateParticle
                joan::orig::as_world(this)->read_scene_object(joan::orig::arg::mptr(reader), joan::orig::arg::i32(flag));      // 0x00551930
            }
            pump();
        }
        for (std::int32_t i = joan::orig::as_dynarray(&list)->size(); i > 0; --i) {  // 0x00417ad0
            const std::int16_t id = *joan::orig::arg::as<std::int16_t*>(joan::orig::as_dynarray(&list)->get(joan::orig::arg::i32(i)));      // 0x00417200
            void* const rec = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(id)));           // 0x00425ce0
            joan::orig::as_res(rec)->release_slot(0, 0);                                             // 0x004a3e30
        }
        joan::orig::dynarray(&list)->destruct();  // 0x005844b0
        return 1;
    }
};

#define B16(name, addr, fn) JOAN_REPLACE("game/b82_16", name, addr, fn)
B16("Scene::load_script", 0x0044e5f0, &H::load_script);
#undef B16

}  // namespace game_b82_16
}  // namespace

// ---------------------------------------- from modules/game_b111_10.cpp (fix/uninit)
//
//      0x0049b970 Scene::create_light(reader, flag) (thiscall, ret 8)
namespace {
namespace game_b111_10 {

using joan::orig::helpers::at;

u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
using joan::orig::helpers::crt_atoi;
double crt_atof(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

JOAN_COUNTER(c_create_light, "Scene::create_light");

struct H {

    // ------------------------------------------------------------------ 0x0049b970
    // fix/uninit: the group id [ebp-0x164] is only written by a "Group" key; without one the
    // original compares the uninitialised local with 0 and, when it is >= 0, looks up / creates a
    // light group with that garbage id. The code treats a negative id as "no group", so it starts
    // at -1 here. The light pointer [ebp-0x150] is written on every flag != 0 path and only read
    // under flag != 0.
    struct LightLines {  // original: [ebp-0x150] light (high byte = buf1[-1]) | buf1 0x40 | buf2 0x100
        alignas(4) char pad[4];
        char b1[0x40];
        char b2[0x100];
    };
    u32 create_light(void* reader, i32 flag) {
        c_create_light.hit();
        LightLines l;
        auto token = [&]() { return joan::orig::as_file_txt(reader)->get_token(i32{1}); };  // 0x00589c70
        auto read_line = [&](char* buf, i32 n) { joan::orig::as_file_txt(reader)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(n)); };  // 0x00589f10
        auto strip_last = [&](char* buf) { buf[static_cast<i32>(crt_strlen(buf)) - 1] = 0; };
        u8* light = nullptr;
        token();
        token();
        read_line(l.b1, 0x40);
        strip_last(l.b1);
        if (flag != 0) {
            if (crt_stricmp(l.b1, str(d::s_Main_scene)) == 0) {  // "Main scene"
                light = joan::orig::arg::as<u8*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Cell>(this)->world->scene))->add_light());  // 0x005eb5d0
            } else {
                void* const scene = joan::orig::arg::as<void*>(joan::orig::as_world(this)->find_sector(joan::orig::arg::str(static_cast<char*>(l.b1))));  // 0x0054e6a0
                light = joan::orig::arg::as<u8*>(joan::orig::as_scene(scene)->new_child());                                        // 0x005f0040
            }
        }
        read_line(l.b2, 0xff);
        token();
        char* tok = token();
        if (flag != 0) {
            const i32 kind = crt_atoi(tok);
            if (static_cast<u32>(kind) <= 3) joan::orig::as_camera(light)->init(joan::orig::arg::u32(kind));  // 0x005c58a0 (cases 0..3)
        }
        for (i32 k = 0; k < 4; ++k) {
            token();
            tok = token();
            if (flag != 0) reinterpret_cast<u8*>(&jo::as_light(light)->colour)[k] = static_cast<u8>(crt_atoi(tok));  // Light::colour (+0x34 on x86)
        }
        const Fpu fpu{true};
        float deg = 0.0f;  // 0x3c8efa35 (pi / 180)
        {
            const u32 bits = 0x3c8efa35u;
            std::memcpy(&deg, &bits, 4);
        }
        float scale = 1.0f;
        tok = token();
        if (crt_stricmp(tok, str(d::s_HD)) == 0) scale = deg;  // "HD"
        tok = token();
        if (flag != 0) jo::as_light(light)->orbit_a = static_cast<float>(fpu.mul(crt_atof(tok), scale));
        scale = 1.0f;
        tok = token();
        if (crt_stricmp(tok, str(d::s_VD)) == 0) scale = deg;  // "VD"
        tok = token();
        if (flag != 0) jo::as_light(light)->orbit_b = static_cast<float>(fpu.mul(crt_atof(tok), scale));
        token();
        tok = token();
        if (flag != 0) jo::as_light(light)->position[0] = static_cast<float>(fpu.add(crt_atof(tok), jo::self<jo::Cell>(this)->origin_x));
        token();
        tok = token();
        if (flag != 0) jo::as_light(light)->position[1] = static_cast<float>(fpu.add(crt_atof(tok), jo::self<jo::Cell>(this)->origin_y));
        float* const plain[5] = {&jo::as_light(light)->position[2], &jo::as_light(light)->attenuation0, &jo::as_light(light)->attenuation1,
                                 &jo::as_light(light)->attenuation2, &jo::as_light(light)->range};
        for (i32 k = 0; k < 5; ++k) {
            token();
            tok = token();
            if (flag != 0) *plain[k] = static_cast<float>(crt_atof(tok));
        }
        if (flag != 0) joan::orig::as_light(light)->invalidate();  // 0x005c5c00
        token();
        read_line(l.b2, 0x40);
        strip_last(l.b2);  // empty line: writes b1[0x3f], as the original ([ebp-0x10d])
        const i32 type = joan::orig::arg::as<i32>(joan::orig::sound::attenuation_of_name(joan::orig::arg::str(static_cast<char*>(l.b2))));  // 0x004b03e0
        u32 rate = 0;                                                              // float bits
        float range = 0.0f;
        i32 group = -1;  // fix/uninit (see above)
        tok = token();
        while (crt_stricmp(tok, str(d::s_rbrace)) != 0) {  // "}"
            if (crt_stricmp(tok, str(d::s_Group)) == 0) {  // "Group"
                group = crt_atoi(token());
            } else if (crt_stricmp(tok, str(d::s_ModifyRate)) == 0) {  // "ModifyRate"
                const float r = static_cast<float>(crt_atof(token()));
                std::memcpy(&rate, &r, 4);
            } else if (crt_stricmp(tok, str(d::s_ModifyRange)) == 0) {  // "ModifyRange"
                range = static_cast<float>(crt_atof(token()));
            }
            tok = token();
        }
        if (flag != 0 && crt_stricmp(l.b1, str(d::s_Main_scene)) == 0)
            joan::orig::as_scene(jo::view<void*>(jo::self<jo::Cell>(this)->world->scene))->remove_light(joan::orig::arg::mptr(light));  // 0x005eb700
        if (flag != 0 && group >= 0) {
            u8* g = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_lights))->find_by_4(joan::orig::arg::u32(group)));  // 0x004b0810
            if (g == nullptr) {
                const u32 n = joan::orig::arg::as<u32>(joan::orig::as_light_faders(obj(d::g_lights))->add_group());  // 0x004b0640
                g = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_lights))->get_at_4(joan::orig::arg::i32(n)));         // 0x00484710
            }
            joan::orig::as_children(g)->set_type(joan::orig::arg::u32(type));   // 0x004afcf0
            joan::orig::as_children(g)->add_light(light);  // 0x004b0250
            jo::view<i32>(jo::as_children(g)->u32_4) = group;
            jo::view<u8*>(jo::as_light(light)->children) = g;
            if (type == 1) {
                const float m = jo::as_light(light)->attenuation1;
                const float hi = range * m + m;
                const float lo = m - range * m;
                joan::orig::as_children(g)->set_range(joan::orig::arg::bits(lo), joan::orig::arg::bits(hi), joan::orig::arg::u32(rate));  // 0x004afd20
            }
        }
        return 1;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// [ebp-0x164] group id without a "Group" key -> -1 (no group).
FIX("Scene::create_light_49b970", 0x0049b970, &H::create_light);
#undef FIX

}  // namespace game_b111_10
}  // namespace

// ---------------------------------------- from modules/game_world5.cpp (game/world5)
//
//    - Cell::load_files 0x004f2f80 (world_s.cpp; called by Cell::load 0x004329e0): the
//      cell's MAP\<dir>\<dir>.res archive, the .PFD / m.PFD path-finding data (+0x54 / +0x58,
//      registered with 0xa570a8), the .trk (+0x5c) and .cm (+0x60) files, _STG from the
//      archive, the grid either from .PLN (0x0055f320 / 0x00558140) or built from .GRI
//      (0x004f6eb0 / 0x0055b720), .STG, the 0xa3ce38 temporary array cleanup and .POR.
//      Its two stack arguments are unused; returns 1 (ret 8).
namespace {
namespace game_world5 {


constexpr img::Addr kFiles(d::g_misc);
constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kSounds(d::g_physic_sys);
constexpr img::Addr kWorldSCpp(d::s_d_ww_world_s_cpp);  // "d:\ww\world_s.cpp"
constexpr img::Addr kMap(d::s_MAP_9cf818);        // "MAP\"
constexpr img::Addr kPfd(d::s_PFD);        // ".PFD"

JOAN_COUNTER(c_load_files, "Cell::load_files");

// Path string object (0x194 bytes, no destructor).
void str_init(void* s) { joan::orig::as_string(s)->construct(); }                       // 0x005a1170
void str_assign(void* s, const void* text) { joan::orig::as_string(s)->assign(joan::orig::arg::str(text)); }  // 0x005a1440
void str_append(void* s, const void* text) { joan::orig::as_string(s)->append(joan::orig::arg::str(text)); }  // 0x005a1550
const char* str_c(void* s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(s)->c_str()); }          // 0x00417d90
bool file_exists(const char* path) { return joan::orig::as_misc(img::obj(kFiles))->is_file_exist(joan::orig::arg::str(path)) != 0; }  // 0x00590b80
void pump() {
    joan::orig::as_sound_stream(img::obj(d::g_snd))->update();  // 0x0059b1c0
    joan::orig::as_ambient_streams(img::obj(d::g_mus))->update();  // 0x005866d0
}
std::int16_t line_base() { return global<std::int16_t>(d::g_line_base_a0b794); }
// new(0x38) + 0x00664fd0 (path-finding data).
void* new_pfd() {
    void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::PathSpace)));  // 0x38 on x86; operator new
    return mem != nullptr ? joan::orig::as_path_space(mem)->construct() : nullptr;  // 0x00664fd0
}

struct H {

    // 0x004f2f80 Cell::load_files(unused, unused) -> 1.
    std::int32_t load_files(void*, std::int32_t) {
        c_load_files.hit();
        alignas(8) char dir_slash[0x104];  // ebp-0x114
        alignas(8) char dir[0x104];        // ebp-0x218
        const char* const name = joan::orig::arg::as<const char*>(joan::orig::libc::strrchr(joan::orig::arg::str(jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6), 0x5c));  // strrchr
        if (name != nullptr) {
            std::strncpy(static_cast<char*>(static_cast<void*>(dir_slash)), joan::orig::arg::str(jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6), joan::orig::arg::u32(static_cast<std::int32_t>(name - (jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6)) + 1));  // memcpy
            std::strncpy(static_cast<char*>(static_cast<void*>(dir)), joan::orig::arg::str(jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6), joan::orig::arg::u32(static_cast<std::int32_t>(name - (jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6)) + 1));
            dir_slash[static_cast<std::int32_t>(name - (jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6)) + 1] = 0;
            dir[static_cast<std::int32_t>(name - (jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6))] = 0;
        } else {
            dir_slash[0] = 0;
            dir[0] = 0;
        }
        alignas(8) std::uint8_t res_path[0x198];  // ebp-0x3b0
        str_init(res_path);
        str_assign(res_path, img::str(kMap));
        str_append(res_path, dir_slash);
        str_append(res_path, dir);
        str_append(res_path, &img::global<const char>(d::s_res));  // ".res"
        alignas(8) std::uint8_t archive[((sizeof(jo::ResourceIdxL) > 0x138 ? sizeof(jo::ResourceIdxL) : 0x138) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // ebp-0x4e8 (x64: the larger ResourceIdxL)
        joan::orig::as_archive(static_cast<void*>(archive))->construct(str_c(res_path), 0u, 0u);  // 0x0047cdf0

        jo::view<void*>(jo::self<jo::Cell>(this)->path_space) = new_pfd();
        alignas(8) std::uint8_t path[0x194];  // ebp-0x67c
        str_init(path);
        str_assign(path, img::str(kMap));
        str_append(path, dir_slash);
        str_append(path, cell_name(this));
        str_append(path, img::str(kPfd));
        if (file_exists(str_c(path))) {
            const char* const p = str_c(path);
            joan::orig::as_path_space(jo::view<void*>(jo::self<jo::Cell>(this)->path_space))->load(joan::orig::arg::str(p));  // 0x00670980
            jo::self<jo::Cell>(this)->path_space->map_id = 1;
        }
        pump();
        str_assign(path, img::str(kMap));
        str_append(path, dir_slash);
        str_append(path, cell_name(this));
        str_append(path, &img::global<const char>(d::s_m));  // "m"
        str_append(path, img::str(kPfd));
        if (file_exists(str_c(path))) {
            jo::view<void*>(jo::self<jo::Cell>(this)->path_space_b) = new_pfd();
            const char* const p = str_c(path);
            joan::orig::as_path_space(jo::view<void*>(jo::self<jo::Cell>(this)->path_space_b))->load(joan::orig::arg::str(p));  // 0x00670980
            jo::self<jo::Cell>(this)->path_space_b->map_id = 1;
        } else if (jo::view<void*>(jo::self<jo::Cell>(this)->path_space) != nullptr) {
            jo::view<void*>(jo::self<jo::Cell>(this)->path_space_b) = jo::view<void*>(jo::self<jo::Cell>(this)->path_space);
        }
        if (jo::view<void*>(jo::self<jo::Cell>(this)->path_space) != nullptr)
            joan::orig::as_players(img::obj(kSounds))->add_cell(jo::self<jo::Cell>(this)->path_space, jo::self<jo::Cell>(this)->path_space_b, joan::orig::arg::i32(jo::self<jo::Cell>(this)->origin_x), joan::orig::arg::i32(jo::self<jo::Cell>(this)->origin_y), joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::Cell>(this)->box)));  // 0x006ad270
        pump();

        {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Tracks)));  // 0x10 on x86
            jo::view<void*>(jo::self<jo::Cell>(this)->tracks) = mem != nullptr ? joan::orig::as_tracks(mem)->construct() : nullptr;  // 0x004eb4f0
        }
        joan::orig::as_tracks(jo::view<void*>(jo::self<jo::Cell>(this)->tracks))->init();  // 0x004eb550
        str_assign(path, img::str(kMap));
        str_append(path, dir_slash);
        str_append(path, cell_name(this));
        str_append(path, &img::global<const char>(d::s_trk));  // ".trk"
        if (file_exists(str_c(path))) {
            jo::self<jo::Cell>(this)->tracks->origin[0] = static_cast<float>(jo::self<jo::Cell>(this)->origin_x);
            jo::self<jo::Cell>(this)->tracks->origin[1] = static_cast<float>(jo::self<jo::Cell>(this)->origin_y);
            jo::view<std::uint32_t>(jo::self<jo::Cell>(this)->tracks->origin[2]) = 0;
            const char* const p = str_c(path);
            joan::orig::as_tracks(jo::view<void*>(jo::self<jo::Cell>(this)->tracks))->load_4eb640(p);  // 0x004eb640
        }
        pump();

        {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::CamZones)));  // 0x4c on x86
            jo::view<void*>(jo::self<jo::Cell>(this)->camera_zones) = mem != nullptr ? joan::orig::as_camera_zones(mem)->construct() : nullptr;  // 0x00425dc0
        }
        joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::Cell>(this)->camera_zones))->init();  // 0x00425e20
        str_assign(path, img::str(kMap));
        str_append(path, dir_slash);
        str_append(path, cell_name(this));
        str_append(path, &img::global<const char>(d::s_cm));  // ".cm"
        if (file_exists(str_c(path))) {
            jo::self<jo::Cell>(this)->camera_zones->origin[0] = static_cast<float>(jo::self<jo::Cell>(this)->origin_x);
            jo::self<jo::Cell>(this)->camera_zones->origin[1] = static_cast<float>(jo::self<jo::Cell>(this)->origin_y);
            jo::view<std::uint32_t>(jo::self<jo::Cell>(this)->camera_zones->origin[2]) = 0;
            const char* const p = str_c(path);
            joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::Cell>(this)->camera_zones))->load(p);  // 0x00426330
        }
        pump();

        str_assign(path, cell_name(this));
        str_append(path, &img::global<const char>(d::s_STG_9d8188));  // "_STG"
        alignas(8) std::uint8_t stg[((sizeof(jo::FileTxt) > 0x1a0 ? sizeof(jo::FileTxt) : 0x1a0) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // ebp-0x818 (x64: the larger FileTxt)
        {
            const std::uint32_t size = joan::orig::arg::as<std::uint32_t>(joan::orig::as_res_idx_l(static_cast<void*>(archive))->get_data_size_by_name(joan::orig::arg::str(str_c(path))));  // 0x0059bd60
            const std::uint32_t index = joan::orig::arg::as<std::uint32_t>(joan::orig::as_res_idx_l(static_cast<void*>(archive))->get_index(joan::orig::arg::str(str_c(path))));  // 0x0059be20
            const char* const data = joan::orig::arg::as<const char*>(joan::orig::as_res_idx_l(static_cast<void*>(archive))->read_rec(joan::orig::arg::i32(index)));  // 0x0059bf70
            joan::orig::as_file_txt(static_cast<void*>(stg))->construct_mem(joan::orig::arg::str(data), joan::orig::arg::i32(size));  // 0x00589780
        }
        joan::orig::as_world(this)->read_sectors(joan::orig::arg::mptr(static_cast<void*>(stg)));  // 0x0042c6b0
        pump();

        str_assign(path, img::str(kMap));
        str_append(path, dir_slash);
        str_append(path, cell_name(this));
        str_append(path, &img::global<const char>(d::s_GRI));  // ".GRI"
        alignas(8) std::uint8_t pln[0x194];  // ebp-0x9ac
        str_init(pln);
        str_assign(pln, img::str(kMap));
        str_append(pln, dir_slash);
        str_append(pln, cell_name(this));
        str_append(pln, &img::global<const char>(d::s_PLN));  // ".PLN"
        alignas(8) std::uint8_t grid_a[0x14];  // ebp-0x9c0
        alignas(8) std::uint8_t grid_b[0x14];  // ebp-0x9d4
        joan::orig::as_height_grid(static_cast<void*>(grid_a))->construct();  // 0x004c56b0
        joan::orig::as_height_grid(static_cast<void*>(grid_b))->construct();
        GridCell** rows = nullptr;  // ebp-0x9d8
        pump();

        bool from_pln = false;
        if (global<std::uint32_t>(d::g_a0b6ac) != 0 && file_exists(str_c(pln))) {
            const char* const gri = str_c(path);
            const char* const p = str_c(pln);
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(img::obj(kFiles))->compare_file_time_590e20(joan::orig::arg::str(p), joan::orig::arg::str(gri))) > 0) {  // 0x00590e20
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_world(this)->load_pln(joan::orig::arg::str(str_c(pln)))) != 0) from_pln = true;  // 0x0055f320
            }
        }
        if (from_pln) {
            alignas(8) std::uint8_t reader[((sizeof(jo::FileMem) > 0x120 ? sizeof(jo::FileMem) : 0x120) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // ebp-0xaf8 (x64: the larger FileMem)
            joan::orig::as_file_mem(static_cast<void*>(reader))->construct();  // 0x00587f80
            if (file_exists(str_c(path))) {
                joan::orig::as_file_mem(static_cast<void*>(reader))->open_588000(joan::orig::arg::str(str_c(path)), 1, static_cast<char>(joan::orig::arg::i32(0)));  // 0x00588000
                joan::orig::as_world_cell(this)->load_grid(joan::orig::arg::mptr(static_cast<void*>(reader)));            // 0x00558140
                joan::orig::as_file_mem(static_cast<void*>(reader))->close();                                // 0x005886d0
            }
            joan::orig::as_file_mem(static_cast<void*>(reader))->destruct();  // 0x00587fd0
        } else {
            joan::orig::as_height_grid(static_cast<void*>(grid_a))->create(joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_rows)), joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_cols)), joan::orig::arg::i16(0));  // 0x004c58a0
            joan::orig::as_height_grid(static_cast<void*>(grid_b))->create(joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_rows)), joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_cols)), joan::orig::arg::i16(0));
            constexpr std::int32_t kPtr = static_cast<std::int32_t>(sizeof(GridCell*));     // 4 in the original
            constexpr std::int32_t kCell = static_cast<std::int32_t>(sizeof(GridCell));     // 0x54 in the original
            rows = joan::orig::arg::as<GridCell**>(joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_rows) * kPtr + kPtr)), joan::orig::arg::str(img::str(kWorldSCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(line_base()) + 0xff)));  // 0x0058e7b0 alloc
            for (std::int32_t i = 0; i <= static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_rows); ++i) {
                rows[i] = joan::orig::arg::as<GridCell*>(joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(static_cast<std::uint32_t>((static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_cols) + 1) * kCell)), joan::orig::arg::str(img::str(kWorldSCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(line_base()) + 0x103)));
                std::memset(static_cast<void*>(rows[i]), 0, joan::orig::arg::u32(static_cast<std::uint32_t>((static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_cols) + 1) * kCell)));  // memset
            }
            alignas(8) std::uint8_t reader[((sizeof(jo::FileMem) > 0x120 ? sizeof(jo::FileMem) : 0x120) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // ebp-0xc18 (x64: the larger FileMem)
            joan::orig::as_file_mem(static_cast<void*>(reader))->construct();  // 0x00587f80
            if (file_exists(str_c(path))) {
                joan::orig::as_file_mem(static_cast<void*>(reader))->open_588000(joan::orig::arg::str(str_c(path)), 1, static_cast<char>(joan::orig::arg::i32(0)));  // 0x00588000
                joan::orig::as_world(this)->build_grid(joan::orig::arg::mptr(static_cast<void*>(reader)), joan::orig::arg::mptr(static_cast<void*>(grid_a)), joan::orig::arg::mptr(static_cast<void*>(grid_b)), joan::orig::arg::mptr(static_cast<void*>(rows)));  // 0x004f6eb0
                joan::orig::as_file_mem(static_cast<void*>(reader))->close();  // 0x005886d0
            }
            const char* const p = str_c(pln);
            joan::orig::as_world_cell(this)->build_ground(joan::orig::arg::mptr(static_cast<void*>(grid_a)), joan::orig::arg::mptr(static_cast<void*>(grid_b)), joan::orig::arg::mptr(static_cast<void*>(rows)), joan::orig::arg::str(p));  // 0x0055b720
            joan::orig::as_file_mem(static_cast<void*>(reader))->destruct();  // 0x00587fd0
        }
        pump();

        str_assign(path, img::str(kMap));
        str_append(path, dir_slash);
        str_append(path, cell_name(this));
        str_append(path, &img::global<const char>(d::s_STG_9d81b8));  // ".STG" (built, unused)
        joan::orig::as_file_txt(static_cast<void*>(stg))->rewind();                                    // 0x0058a290
        joan::orig::as_scene(this)->load_script(joan::orig::arg::mptr(static_cast<void*>(stg)), 1);             // 0x0044e5f0
        joan::orig::as_file(static_cast<void*>(stg))->file_close();                                // 0x00587260
        for (std::int32_t i = 0; i < global<std::int32_t>(d::g_a3ce30); ++i) {
            void* const p = global<void**>(d::g_a3ce38)[i];
            if (p == nullptr) continue;
            joan::orig::as_sector(p)->destroy(1u);  // 0x0041d3a0 delete
        }
        if (global<void*>(d::g_a3ce38) != nullptr) {
            joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(global<void*>(d::g_a3ce38)), joan::orig::arg::str(img::str(kWorldSCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(line_base()) + 0x157));  // 0x0058ed70 free
            global<void*>(d::g_a3ce38) = nullptr;
        }
        global<std::uint32_t>(d::g_a3ce30) = 0;
        global<std::uint32_t>(d::g_a3ce34) = 0;
        if (rows != nullptr) {
            for (std::int32_t i = 0; i <= static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_rows); ++i) {
                for (std::int32_t j = 0; j <= static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_cols); ++j) {
                    if (rows[i][j].detail != nullptr)
                        joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(rows[i][j].detail), joan::orig::arg::str(img::str(kWorldSCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(line_base()) + 0x167));
                }
                joan::orig::as_mem(img::obj(kMemory))->del(static_cast<void*>(rows[i]), joan::orig::arg::str(img::str(kWorldSCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(line_base()) + 0x16a));
                rows[i] = nullptr;
            }
            joan::orig::as_mem(img::obj(kMemory))->del(static_cast<void*>(rows), joan::orig::arg::str(img::str(kWorldSCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(line_base()) + 0x16d));
        }
        pump();

        str_assign(path, img::str(kMap));
        str_append(path, dir_slash);
        str_append(path, cell_name(this));
        str_append(path, &img::global<const char>(d::s_POR));  // ".POR"
        alignas(8) std::uint8_t por[((sizeof(jo::FileTxt) > 0x1a0 ? sizeof(jo::FileTxt) : 0x1a0) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // ebp-0xdbc (x64: the larger FileTxt)
        joan::orig::as_file_txt(static_cast<void*>(por))->construct(joan::orig::arg::str(str_c(path)));  // 0x00589550
        joan::orig::as_world(this)->read_portals(joan::orig::arg::mptr(static_cast<void*>(por)));  // 0x005593e0
        pump();
        joan::orig::as_file_txt(static_cast<void*>(por))->destruct();       // 0x00589890
        joan::orig::as_height_grid(static_cast<void*>(grid_b))->free();    // 0x004c5870
        joan::orig::as_height_grid(static_cast<void*>(grid_a))->free();
        joan::orig::as_file_txt(static_cast<void*>(stg))->destruct();       // 0x00589890
        joan::orig::as_archive(static_cast<void*>(archive))->dtor();   // 0x00417a30
        return 1;
    }
};

#define W5(name, addr, fn) JOAN_REPLACE("game/world5", name, addr, fn)
W5("Cell::load_files", 0x004f2f80, &H::load_files);
#undef W5

}  // namespace game_world5
}  // namespace

// ---------------------------------------- from modules/game_b83_01.cpp (game/b83_01)
//
//    - Cell::create_object 0x004f3df0 (thunk 0x00404273). thiscall (reader, flag, list),
//      ret 0xc, returns 1. Reads the object name, optional label, record name (looked up
//      in the 0xa58f50 name table, "object %1s not found!" otherwise), creates the unit
//      (0x0049ea90 / 0x00425d20) unless the record's 0x004a4050 matches flag or it is a
//      kind 6 / 0xc record, position (+ cell +0x44 / +0x48), "HD" heading, "V" / "VD" pitch,
//      three sizes (max), two more names, the treasure fields, then the "Scale" /
//      "InitState" / "FinalState" / "TriggerType" / "TriggerItemName" block up to "}".
//      Afterwards: kind 0xc on flag 1 -> 0xa48aa8 0x0041a650; kind 6 on flag 1 -> matrix
//      (identity 0xa18a78 * rotation 0x0060bc50) + 0x00560120 / 0x0055fa40 and the record
//      index appended to list; flag 1 with record byte +0x1e1 -> the first scene box
//      (0x005ef610) containing the position is grown by (max size / 2) * 1.2 and refreshed
//      (0x005eb030); flag 0 with +0x1c4 / 0x004a4180 -> a new 0x52-byte object
//      (0x00456eb0 ...) stored at unit +0xa2.
namespace {
namespace game_b83_01 {

using joan::orig::helpers::at;

struct alignas(4) Bytes24 {
    std::uint8_t b[0x18];
};

JOAN_COUNTER(c_create_object, "Cell::create_object");

// Emulated stack frame: buffer at [ebp - off] is at frame + N - off.
template <std::int32_t N>
struct Frame {
    alignas(8) std::uint8_t b[N];
    char* ebp(std::int32_t off) { return reinterpret_cast<char*>(b + N - off); }
};

// GCC cannot prove destination and source are different frame buffers; the original strcat never overlaps them.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wrestrict"
#endif
void strcat_(char* d, const char* s) { std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
using joan::orig::helpers::new_;

void* records_get(std::int32_t id) {  // 0x00425ce0 (reads the word)
    return joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<std::int32_t>(static_cast<std::int16_t>(id)))));
}

// Order0b +0xc4 written / read as one dword (the treasure level): drop_count (i16) and u8_c6 (the next two bytes of
// the packed layout, adjacent on every target: +0xc4 / +0xc6 on x86, +0xe8 / +0xea on x64).
std::int32_t& drop_level(jo::Order0b* o) { return *reinterpret_cast<std::int32_t*>(&o->drop_count); }

struct H {
    // ------------------------------------------------------------------ 0x004f3df0
    std::int32_t create_object(void* reader, std::int32_t flag, void* list) {
        c_create_object.hit();
        Frame<0x400> fr;
        std::memset(fr.b, 0, sizeof fr.b);
        char* const b158 = fr.ebp(0x158);
        char* const b25c = fr.ebp(0x25c);
        char* const b29c = fr.ebp(0x29c);
        char* const b33c = fr.ebp(0x33c);
        char* const b37c = fr.ebp(0x37c);
        char* tok;  // ebp-0x18

        tok = token(reader);
        tok = token(reader);
        read_str(reader, b158, 0x40);
        chop(b158);
        tok = token(reader);
        read_str(reader, b158, 0x40);
        chop(b158);
        if (static_cast<std::int8_t>(b158[0]) != 0)
            strcpy_(b29c, b158);
        else
            b29c[0] = 0;
        tok = token(reader);
        read_str(reader, b158, 0x40);
        chop(b158);
        tok = token(reader);
        read_str(reader, b25c, 0x104);
        chop(b25c);
        strcat_(b25c, b158);
        const std::int32_t rec_id = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(joan::orig::arg::str(b25c)));  // 0x004a36c0
        if (rec_id == 0)
            joan::orig::warning_box(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_object_1s_not_found), b25c);  // 0x00586bc0
        void* const rec = records_get(rec_id);  // ebp-0x2a8
        void* unit;                              // ebp-0x2ac
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_item_def(rec)->is_soldier()) == flag ||  // 0x004a4050
            jo::view<std::int16_t>(jo::as_item_def(rec)->type_id) == 6 || jo::view<std::int16_t>(jo::as_item_def(rec)->class_id) == 0xc) {
            unit = nullptr;
        } else {
            const std::int32_t handle = joan::orig::arg::as<std::int32_t>(joan::orig::as_units(obj(d::g_items))->create_49ea90(joan::orig::arg::u32(rec_id)));  // 0x0049ea90
            unit = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(handle));  // 0x00425d20
            if (static_cast<std::int8_t>(b29c[0]) != 0)
                strcpy_(jo::as_map_object(unit)->name, b29c);
            else
                strcpy_(jo::as_map_object(unit)->name, str(d::kEmpty));  // ""
            joan::orig::as_map_object(unit)->set_world(jo::self<jo::Cell>(this)->world);  // 0x004994b0
            joan::orig::as_cell(this)->attach_unit(joan::orig::arg::mptr(unit));                // 0x0043b190
        }

        Vec3 pos;  // ebp-0x2bc
        tok = token(reader);
        tok = token(reader);
        pos.x = static_cast<float>(kFpu.add(atof_(tok), static_cast<double>(jo::self<jo::Cell>(this)->origin_x)));
        tok = token(reader);
        tok = token(reader);
        pos.y = static_cast<float>(kFpu.add(atof_(tok), static_cast<double>(jo::self<jo::Cell>(this)->origin_y)));
        tok = token(reader);
        tok = token(reader);
        pos.z = static_cast<float>(atof_(tok));

        double scale_h = 1.0;  // ebp-0x2c8
        tok = token(reader);
        if (stricmp_(tok, str(d::s_HD)) == 0) scale_h = kDeg;  // "HD"
        tok = token(reader);
        const float heading = static_cast<float>(kFpu.mul(atof_(tok), scale_h));  // ebp-0x2c0

        float pitch = 0.0f;    // ebp-0x2cc
        double scale_v = 1.0;  // ebp-0x2d4
        tok = token(reader);
        if (stricmp_(tok, str(d::s_V_9d82ec)) == 0 || stricmp_(tok, str(d::s_VD)) == 0) {  // "V" / "VD"
            if (stricmp_(tok, str(d::s_VD)) == 0) scale_v = kDeg;
            tok = token(reader);
            pitch = static_cast<float>(kFpu.mul(atof_(tok), scale_v));
            tok = token(reader);
        }
        if (unit != nullptr) {
            joan::orig::as_map_object(unit)->set_yaw(joan::orig::arg::bits(heading));  // 0x00499fd0
            joan::orig::as_map_object(unit)->set_roll(joan::orig::arg::bits(pitch));    // 0x0049adc0
            joan::orig::as_map_object(unit)->set_position(pos);      // 0x0041ff50
        }

        std::int32_t size = 0;  // ebp-0x2d8
        tok = token(reader);  // one token only here (4f4293)
        atoi_(tok);  // ebp-0x2dc (dead)
        size = size > atoi_(tok) ? size : atoi_(tok);
        tok = token(reader);
        tok = token(reader);
        atoi_(tok);  // ebp-0x2e0 (dead)
        size = size > atoi_(tok) ? size : atoi_(tok);
        tok = token(reader);
        tok = token(reader);
        {
            const std::int32_t v = atoi_(tok);
            size = size > v ? size : v;
        }

        tok = token(reader);
        read_str(reader, b158, 0x40);
        chop(b158);
        const std::int32_t sound_ref = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(joan::orig::arg::str(b158)));  // 0x004b91a0
        const std::int32_t sound = joan::orig::arg::as<std::int32_t>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(sound_ref)));  // 0x004b67e0
        if (unit != nullptr) jo::as_map_object(unit)->side = static_cast<std::int16_t>(sound);

        tok = token(reader);
        read_str(reader, b158, 0x40);
        chop(b158);
        const std::int16_t treasure = static_cast<std::int16_t>(
            joan::orig::arg::as<std::int32_t>(joan::orig::as_names(obj(d::g_randList))->find_463ed0(joan::orig::arg::str(b158))));  // 0x00463ed0
        const std::int16_t item =
            static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(joan::orig::arg::str(b158))));  // 0x004a36c0
        if (unit != nullptr && joan::orig::arg::as<std::int32_t>(joan::orig::as_item_def(rec)->is_soldier()) != flag && jo::view<std::int16_t>(jo::as_item_def(rec)->class_id) == 0xa) {
            // ebp-0x2f8: vtable slot 0x50 returns `this` only in Order0b (vtable 0x9d37cc, 0x0048c0e0), 0 elsewhere.
            jo::Order0b* const data = static_cast<jo::Order0b*>(virtual_call<void*>(unit, 0x50));
            if (treasure != 0) {
                void* const t = joan::orig::arg::as<void*>(joan::orig::as_names(obj(d::g_randList))->at(joan::orig::arg::i16(static_cast<std::int32_t>(treasure))));  // 0x00464100
                joan::orig::as_grass(t)->pick(joan::orig::arg::mptr(&data->drop_item), joan::orig::arg::mptr(&data->drop_count));  // 0x00464030
                token(reader);
            } else if (item != 0) {
                data->drop_item = item;
                tok = token(reader);
                if (stricmp_(tok, str(d::s_TreasureLevel)) == 0) {  // "TreasureLevel"
                    tok = token(reader);
                    drop_level(data) = atoi_(tok);
                    if (drop_level(data) == -1)
                        drop_level(data) = jo::as_item_def(records_get(item))->min_level;
                    tok = token(reader);
                } else {
                    drop_level(data) = jo::as_item_def(records_get(item))->min_level;
                }
            }
        } else {
            token(reader);
        }
        if (stricmp_(tok, str(d::s_TreasureLevel)) == 0) {  // "TreasureLevel"
            token(reader);
            token(reader);
        }

        float scale = 1.0f;  // ebp-0x2fc
        while (stricmp_(tok, str(d::s_rbrace)) != 0) {  // "}"
            if (stricmp_(tok, str(d::s_Scale)) == 0) {  // "Scale"
                tok = token(reader);
                scale = static_cast<float>(atof_(tok));
                if (unit != nullptr) {
                    joan::orig::as_map_object(unit)->set_scale(joan::orig::arg::bits(scale));                // 0x0049cac0
                    const float h = joan::orig::arg::as<float>(joan::orig::as_map_object(unit)->yaw());               // 0x00499e20
                    joan::orig::as_map_object(unit)->set_yaw(joan::orig::arg::bits(h));                     // 0x00499fd0
                }
            } else if (stricmp_(tok, str(d::s_InitState)) == 0) {  // "InitState"
                tok = token(reader);
                if (unit != nullptr) {
                    if (jo::as_map_object(unit)->class_id == 1) {
                        const std::int32_t v = atoi_(tok);
                        jo::view<std::uint8_t>(static_cast<jo::Gate*>(virtual_call<void*>(unit, 0x4c))->state) = static_cast<std::uint8_t>(v);  // slot 0x4c: Gate (0x0048e900)
                    } else if (jo::as_map_object(unit)->class_id == 0xe) {
                        const std::int32_t v = atoi_(tok);
                        void* const s = virtual_call<void*>(unit, 0x64);
                        joan::orig::as_unit(s)->play_effect(static_cast<char>(joan::orig::arg::i32(v)));  // 0x00477360
                    }
                }
            } else if (stricmp_(tok, str(d::s_FinalState)) == 0) {  // "FinalState"
                tok = token(reader);
                if (unit != nullptr && jo::as_map_object(unit)->class_id == 1) {
                    const std::int32_t v = atoi_(tok);
                    jo::view<std::uint8_t>(static_cast<jo::Gate*>(virtual_call<void*>(unit, 0x4c))->side) = static_cast<std::uint8_t>(v);  // slot 0x4c: Gate (0x0048e900)
                }
            } else if (stricmp_(tok, str(d::s_TriggerType)) == 0) {  // "TriggerType"
                read_str(reader, b33c, 0x40);
                chop(b33c);
                if (unit != nullptr) jo::view<std::int32_t>(jo::as_map_object(unit)->interact_trigger) = joan::orig::arg::as<std::int32_t>(joan::orig::trigger_type_from_name(joan::orig::arg::str(b33c)));  // 0x00494dc0
            } else if (stricmp_(tok, str(d::s_TriggerItemName)) == 0) {  // "TriggerItemName"
                read_str(reader, b37c, 0x40);
                chop(b37c);
                if (unit != nullptr && jo::as_map_object(unit)->class_id == 0xd)
                    strcpy_(static_cast<jo::Order45*>(virtual_call<void*>(unit, 0x60))->target_name, b37c);  // slot 0x60: Order45 (0x004925c0)
            }
            tok = token(reader);
        }

        if (jo::view<std::int16_t>(jo::as_item_def(rec)->class_id) == 0xc && flag == 1)
            joan::orig::as_actor_list(obj(d::g_actor_list))->add_default(pos, joan::orig::arg::bits(heading), joan::orig::arg::bits(pitch), 0u);  // 0x0041a650

        if (jo::view<std::int16_t>(jo::as_item_def(rec)->type_id) == 6 && flag == 1) {
            alignas(4) float m[16];    // ebp-0x3bc
            alignas(4) float rot[16];  // ebp-0x3fc
            std::memcpy(m, img::obj(d::g_identity_matrix), sizeof m);
            joan::orig::matrix_rotation(joan::orig::arg::mptr(static_cast<void*>(rot)), joan::orig::arg::f32(heading));  // 0x0060bc50
            import_call<void*>(d::iat_v3d_87_matrix_multiply_2, static_cast<void*>(m), static_cast<void*>(rot));
            m[12] = pos.x;
            m[13] = pos.y;
            m[14] = pos.z;
            std::int32_t index = jo::view<std::int16_t>(jo::as_item_def(rec)->id);  // ebp-0x400
            joan::orig::as_item_def(rec)->acquire_anim(0, 0);      // 0x004a39f0
            joan::orig::dynarray(list)->linkin(&index);                // 0x00584620
            void* const model = joan::orig::arg::as<void*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_item_def(rec)->icon_sprite))));  // 0x0041a180
            if (jo::as_model_entry(model)->morph_count != 0) {
                void* const p = joan::orig::arg::as<void*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_item_def(rec)->icon_sprite))));
                joan::orig::as_world(this)->merge_animated(joan::orig::arg::mptr(p), joan::orig::arg::mptr(0), joan::orig::arg::mptr(static_cast<void*>(m)));  // 0x00560120
            } else {
                void* const p = joan::orig::arg::as<void*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_item_def(rec)->icon_sprite))));
                void* const mesh = jo::view<void*>(jo::as_actor_def(jo::as_model_entry(p)->actor_def)->model->mesh);
                joan::orig::as_world(this)->merge_static(joan::orig::arg::mptr(mesh), joan::orig::arg::mptr(0), joan::orig::arg::mptr(static_cast<void*>(m)));  // 0x0055fa40
            }
        }

        if (flag == 1 && jo::view<std::int16_t>(jo::as_item_def(rec)->type_id) != 6 && jo::view<std::int16_t>(jo::as_item_def(rec)->class_id) != 0xc &&
            jo::as_item_def(rec)->enlarge_scene_sector != 0) {
            std::int32_t count = 0;  // ebp-0x408
            Bytes24 box;             // ebp-0x420
            joan::orig::as_aabb(static_cast<void*>(&box))->set_point(&pos);  // 0x0060d430
            void* const scene = jo::view<void*>(jo::self<jo::Cell>(this)->world->scene);
            void** const list_ = joan::orig::arg::as<void**>(joan::orig::as_scene(scene)->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(&box)), &count));  // 0x005ef610
            for (std::int32_t i = 0; i < count; ++i) {
                void* const e = list_[i];
                if ((joan::orig::arg::as<std::uint32_t>(joan::orig::as_aabb(jo::as_scene_s(e)->inner_box)->contains(&pos)) & 0xffu) == 0) continue;  // 0x0060d5f0
                // fild (size / 2) * 1.2f (0x9d8290), one PC24 rounding; then float -/+ pos.
                // min side: fcompp + test ah,0x41 jne -> new value unless new > old (NaN: new);
                // max side: test ah,0x1 je -> new value when new >= old (NaN: keep). The kept
                // path's fld / fstp of the old value leaves it unchanged (it is not a NaN there).
                const float g = static_cast<float>(
                    kFpu.mul(static_cast<double>(size / 2), static_cast<double>(global<float>(d::flt_1_2))));
                {
                    const float a = pos.x - g;
                    if (!(a > jo::view<float>(jo::as_scene_s(e)->desc[0]))) jo::view<float>(jo::as_scene_s(e)->desc[0]) = a;
                }
                {
                    const float a = pos.y - g;
                    if (!(a > jo::view<float>(jo::as_scene_s(e)->desc[1]))) jo::view<float>(jo::as_scene_s(e)->desc[1]) = a;
                }
                {
                    const float a = g + pos.x;
                    if (a >= jo::view<float>(jo::as_scene_s(e)->desc[3])) jo::view<float>(jo::as_scene_s(e)->desc[3]) = a;
                }
                {
                    const float a = g + pos.y;
                    if (a >= jo::view<float>(jo::as_scene_s(e)->desc[4])) jo::view<float>(jo::as_scene_s(e)->desc[4]) = a;
                }
                joan::orig::as_scene(jo::as_scene_s(e)->scene)->delete_30();  // 0x005eb030
                break;
            }
        }

        if ((static_cast<std::int8_t>(jo::as_item_def(rec)->pickable) != 0 || joan::orig::arg::as<std::int32_t>(joan::orig::as_item_def(rec)->is_kind_1_or_4()) != 0) &&
            flag == 0) {  // 0x004a4180
            void* const mem = new_(static_cast<std::uint32_t>(sizeof(jo::WorldItem)));  // 0x52 on x86
            void* const o = mem != nullptr ? joan::orig::as_world_item(mem)->construct() : nullptr;  // 0x00456eb0
            const std::int32_t w2a = jo::view<std::int16_t>(jo::as_item_def(records_get(rec_id))->stats[3]);
            const std::int32_t w36 = *reinterpret_cast<const std::int16_t*>(&jo::as_item_def(records_get(rec_id))->min_level);  // low word
            joan::orig::as_item_stack(o)->init(joan::orig::arg::i16(static_cast<std::int32_t>(static_cast<std::int16_t>(rec_id))), joan::orig::arg::i16(w36), joan::orig::arg::i16(w2a), 0u, 0u, 0u);  // 0x00456cd0
            jo::as_world_item(o)->object_id = jo::as_map_object(unit)->index;
            jo::view<std::int16_t>(jo::as_world_item(o)->side) = static_cast<std::int16_t>(sound);
            joan::orig::as_world_item(o)->set_state(jo::self<jo::Cell>(this)->world);  // 0x00457120
            joan::orig::as_world_item(o)->set_pos(joan::orig::arg::bits(pos.x), joan::orig::arg::bits(pos.y), joan::orig::arg::bits(pos.z));  // 0x00456fe0
            joan::orig::as_world_item(o)->set_heading(joan::orig::arg::bits(heading));            // 0x004571b0
            jo::view<void*>(jo::as_map_object(unit)->world_item) = o;
        }
        return 1;
    }

    static constexpr double kDeg = 0.017453292519943295;  // qword 0x3f91df46a2529d39
};

#define B83(name, addr, fn) JOAN_REPLACE("game/b83_01", name, addr, fn)
B83("Cell::create_object", 0x004f3df0, &H::create_object);
#undef B83

}  // namespace game_b83_01
}  // namespace

// ---------------------------------------- from modules/game_b115_09.cpp (fix/uninit)
//
//    - World::build_grid_4f6eb0 0x004f6eb0 (thiscall(file, grid_a, grid_b, rows), ret 0x10,
//      world_s.cpp): the per-cell entry count [ebp-0x288c] is set only for cell type +0xf == 1 (9)
//      or 2 (0x19); any other nonzero type reuses the previous typed cell's count, the first one
//      reads stack garbage. Fix: the count starts at 0 (later carry-over exactly as the original).
//      The detail copy reads row bytes [0, count) only, count <= 0x19 < 0x1c: never past the row.
//    - The RLE row decoder 0x0059dc10 writes the whole decoded row (no bound) into the 0x1c-byte row
//      buffer [ebp-0x2884] of the second loop, running on into the locals above it up to this at
//      [ebp-0x10] (struct Frame: their offsets from the row). Bytes 0x1c..0x33 are the first loop's
//      locals (dead), so rows of up to 0x34 bytes have no further effect. A longer row (or one cut by
//      the end of the file) overwrites live locals exactly as in the original: the second loop keeps
//      their image in Frame and reads them back after such a decode. The one difference left: a
//      partly overwritten pointer to one of the frame's own tables (names / ids / texture names while
//      they fit their 256 / 256 / 32 entries) combines the row bytes with this frame's address, not
//      the original's. Rows longer than 0x2878 bytes reach the original's EH record / return address
//      (crash) and run past Frame here. The shipped rows are <= 0x19 bytes.
namespace {
namespace game_b115_09 {

i32 line(i32 k) { return static_cast<i32>(global<i16>(d::g_a0b798)) + k; }   // world_s.cpp __LINE__ base

void* const kHeap = obj(d::g_mem);

JOAN_COUNTER(c_grid, "World::build_grid_4f6eb0");

void* heap_alloc(u32 n, i32 k) {
    return joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(n), str(d::s_d_ww_world_s_cpp), joan::orig::arg::i32(line(k)));  // 0x0058e7b0
}
void heap_free(void* p, i32 k) { joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(p), str(d::s_d_ww_world_s_cpp), joan::orig::arg::i32(line(k))); }  // 0x0058ed70
// strncpy without a terminator, exactly as the original CRT call (0x00xxxxxx callers pass the field size).
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
void strncpy_(void* d, const void* s, u32 n) { std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }  // strncpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
void memset_(void* d, i32 v, u32 n) { std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }  // memset

using Name = char[0x20];

// ======================================================================= 0x004f6eb0
// The original's frame from the row buffer [ebp-0x2884] to this [ebp-0x10]: the byte image the second loop's
// decoder writes into, and the offsets of the locals that are live there.
struct Frame {
    static constexpr u32 kRow = 0x1c;         // the row buffer
    static constexpr u32 kDead = 0x34;        // [kRow, kDead): first-loop locals, dead in the second loop
    static constexpr u32 kRle = 0x34;         // the RLE plane buffer (heap)
    static constexpr u32 kTexturesPtr = 0x38; // texture names: kTextures or heap
    static constexpr u32 kTextures = 0x3c;    // 32 texture names
    static constexpr u32 kDefault = 0x444;    // id of "Default scene sector"
    static constexpr u32 kIdsPtr = 0x44c;     // sector ids: kIds or heap
    static constexpr u32 kIds = 0x450;        // 256 sector ids
    static constexpr u32 kNamesPtr = 0x850;   // sector names (its 256-entry table at 0x854 is dead by then)
    static constexpr u32 kRows = 0x285c;      // the rows argument
    static constexpr u32 kLastCol = 0x2860;
    static constexpr u32 kLastRow = 0x2864;
    static constexpr u32 kJ = 0x2868;
    static constexpr u32 kI = 0x286c;
    static constexpr u32 kSelf = 0x2874;
    static constexpr u32 kSize = 0x2878;

    alignas(4) u8 b[kSize];

    void put(u32 off, i32 v) { std::memcpy(b + off, &v, sizeof v); }
    void get(u32 off, i32& v) const { std::memcpy(&v, b + off, sizeof v); }
    // A pointer local is 4 bytes there: the original keeps the bytes as they are (on x86 this is the pointer).
    template <class T>
    void put(u32 off, T* p) {
        const std::uintptr_t v = reinterpret_cast<std::uintptr_t>(p);
        std::memcpy(b + off, &v, 4);  // the low 4 bytes: the x86 image of the pointer
    }
    template <class T>
    void get(u32 off, T*& p) const {
        u32 v;
        std::memcpy(&v, b + off, sizeof v);
        const std::uintptr_t cur = reinterpret_cast<std::uintptr_t>(p);
        u32 cur_low;
        std::memcpy(&cur_low, &cur, 4);
        if (v != cur_low) p = reinterpret_cast<T*>(static_cast<std::uintptr_t>(v));
    }
};
static_assert(Frame::kRow <= Frame::kDead && Frame::kSelf + 4 == Frame::kSize, "frame");

struct Grid {
    jo::Cell* self;              // [ebp-0x10]
    i32 last_row = 0;            // [ebp-0x20] (n20: rows 0..last_row)
    i32 last_col = 0;            // [ebp-0x24]
    GridCell** rows = nullptr;   // [ebp-0x28]
    i32 i = 0, j = 0;            // [ebp-0x18] / [ebp-0x1c]
    Name* names = nullptr;       // [ebp-0x2034] the sector names of the file
    void** ids = nullptr;        // [ebp-0x2438] their sectors (0x0054e6a0; pointers: the owner of a Sector)
    void* default_id = nullptr;  // [ebp-0x2440]
    Name* textures = nullptr;    // [ebp-0x284c] the texture names of the file
    u8* rle = nullptr;           // [ebp-0x2850] one plane of the cells, row by row
    Name names256[0x100];
    void* ids256[0x100];
    Name textures32[0x20];
    Frame fr;                    // fr.b: the row [ebp-0x2884]

    // The live locals into / out of the frame image.
    void store() {
        fr.put(Frame::kRle, rle);
        fr.put(Frame::kTexturesPtr, textures);
        std::memcpy(fr.b + Frame::kTextures, textures32, sizeof textures32);
        fr.put(Frame::kDefault, default_id);
        fr.put(Frame::kIdsPtr, ids);
        if constexpr (sizeof(void*) == 4) std::memcpy(fr.b + Frame::kIds, ids256, sizeof ids256);
        else for (u32 k = 0; k < 0x100; ++k) fr.put(Frame::kIds + k * 4, ids256[k]);  // 4-byte slots of the x86 frame image
        fr.put(Frame::kNamesPtr, names);
        fr.put(Frame::kRows, rows);
        fr.put(Frame::kLastCol, last_col);
        fr.put(Frame::kLastRow, last_row);
        fr.put(Frame::kJ, j);
        fr.put(Frame::kI, i);
        fr.put(Frame::kSelf, self);
    }
    void load() {
        fr.get(Frame::kRle, rle);
        fr.get(Frame::kTexturesPtr, textures);
        std::memcpy(textures32, fr.b + Frame::kTextures, sizeof textures32);
        fr.get(Frame::kDefault, default_id);
        fr.get(Frame::kIdsPtr, ids);
        if constexpr (sizeof(void*) == 4) std::memcpy(ids256, fr.b + Frame::kIds, sizeof ids256);
        else for (u32 k = 0; k < 0x100; ++k) fr.get(Frame::kIds + k * 4, ids256[k]);
        fr.get(Frame::kNamesPtr, names);
        fr.get(Frame::kRows, rows);
        fr.get(Frame::kLastCol, last_col);
        fr.get(Frame::kLastRow, last_row);
        fr.get(Frame::kJ, j);
        fr.get(Frame::kI, i);
        fr.get(Frame::kSelf, self);
    }
    // One row of the second loop into the frame (the loop indices are the only live locals changed since the
    // last store / load).
    void decode_row(void* file, void* reader) {
        fr.put(Frame::kJ, j);
        fr.put(Frame::kI, i);
        const i32 n = joan::orig::arg::as<i32>(joan::orig::as_rle(reader)->decode(joan::orig::arg::mptr(file), joan::orig::arg::mptr(static_cast<void*>(fr.b)), i32{0xff}));  // 0x0059dc10
        if (n < 0 || static_cast<u32>(n) > Frame::kDead) load();
    }

    i16 height(i32 r, i32 c) const { return rows[r][c].height; }
    i32 shade(i32 r, i32 c) const { return static_cast<i32>(rows[r][c].shade); }
    i16* grid_row(i32 r) const { return jo::view<i16**>(self->rows)[r]; }
    void place_sector(const GridCell* cell);
};

struct HWorld {
    u32 build_grid(void* file, void* grid_a, void* grid_b, GridCell** rows) {
        c_grid.hit();
        Grid g;  // the tables are filled before they are read (as the original's stack tables)
        g.self = jo::self<jo::Cell>(this);
        u8 reader[4];  // [ebp-0x14] (0x0059d190 object; no member is used)
        joan::orig::as_rle(static_cast<void*>(reader))->construct();  // 0x0059d190
        g.last_row = joan::orig::as_file_mem(file)->file_get_long();  // 0x005893c0 read int
        g.last_col = joan::orig::as_file_mem(file)->file_get_long();
        g.self->i32_50 = joan::orig::as_file_mem(file)->file_get_long();
        g.self->i32_50 = g.self->i32_50 < 1 ? 1 : g.self->i32_50;
        g.rows = rows;
        if (g.self->rows == nullptr) {
            constexpr u32 kPtr = sizeof(i16*);  // 4 in the original
            jo::view<void*>(g.self->rows) = heap_alloc(static_cast<u32>(g.last_row) * kPtr + kPtr, 0x13);
            for (g.i = 0; g.i <= g.last_row; ++g.i) {
                const u32 n = static_cast<u32>(g.last_col) * 2u + 2u;
                jo::view<i16**>(g.self->rows)[g.i] = static_cast<i16*>(heap_alloc(n, 0x16));
                memset_(g.grid_row(g.i), 0, static_cast<u32>(g.last_col) * 2u + 2u);
            }
        }
        const i32 nnames = joan::orig::as_file_mem(file)->file_get_long();
        g.names = g.names256;
        if (nnames > 0x100) g.names = static_cast<Name*>(heap_alloc(static_cast<u32>(nnames) << 5, 0x26));
        g.ids = g.ids256;
        if (nnames > 0x100) g.ids = static_cast<void**>(heap_alloc(static_cast<u32>(nnames) * sizeof(void*), 0x2b));
        for (i32 k = 0; k < nnames; ++k) {
            joan::orig::as_file_mem(file)->file_read(joan::orig::arg::mptr(g.names[k]), joan::orig::arg::u32(i32{0x20}));  // 0x00588a80
            g.ids[k] = joan::orig::as_world(g.self)->find_sector(joan::orig::arg::str(g.names[k]));  // 0x0054e6a0
        }
        g.default_id = joan::orig::as_world(g.self)->find_sector(str(d::s_Default_scene_sector));  // "Default scene sector"
        const i32 ntextures = joan::orig::as_file_mem(file)->file_get_long();
        g.textures = g.textures32;
        if (ntextures > 0x20) g.textures = static_cast<Name*>(heap_alloc(static_cast<u32>(ntextures) << 5, 0x3e));
        g.self->p84_count = ntextures;
        if (g.self->p_84 == nullptr) g.self->p_84 = heap_alloc(static_cast<u32>(ntextures) << 5, 0x43);
        Name* const cell_textures = static_cast<Name*>(g.self->p_84);
        for (i32 k = 0; k < ntextures; ++k) {
            joan::orig::as_file_mem(file)->file_read(joan::orig::arg::mptr(g.textures[k]), joan::orig::arg::u32(i32{0x20}));  // 0x00588a80
            joan::orig::as_file_mem(file)->file_read(joan::orig::arg::mptr(cell_textures[k]), joan::orig::arg::u32(i32{0x20}));
            strncpy_(cell_textures[k], g.textures[k], 0x20);
        }
        g.rle = static_cast<u8*>(heap_alloc(static_cast<u32>((g.last_row + 1) * (g.last_col + 1)), 0x53));
        // Planes 0..15: byte `plane` of every cell.
        for (u32 plane = 0; plane < 0x10; ++plane) {
            joan::orig::as_rle(static_cast<void*>(reader))->decode(joan::orig::arg::mptr(file), joan::orig::arg::mptr(g.rle), i32{0xff});  // 0x0059dc10
            for (i32 r = 0; r <= g.last_row; ++r) {
                const u8* const src = g.rle + r * (g.last_col + 1);
                for (i32 c = 0; c <= g.last_col; ++c) g.rows[r][c].planes()[plane] = src[c];
            }
        }
        g.store();
        i32 count = 0;  // [ebp-0x288c] (fix/uninit: 0 before the first typed cell, see header)
        for (g.i = 0; g.i <= g.last_row; ++g.i) {
            for (g.j = 0; g.j <= g.last_col; ++g.j) {
                GridCell* const cell = &g.rows[g.i][g.j];  // [ebp-0x2888]
                if (cell->level != 0) {
                    if (cell->level == 1) {
                        count = 9;
                    } else if (cell->level == 2) {
                        count = 0x19;
                    }
                    cell->detail = static_cast<u8*>(heap_alloc(static_cast<u32>(count * 5), 0x88));
                    for (u32 r = 0; r < 5; ++r) {
                        g.decode_row(file, reader);
                        for (i32 k = 0; k < count; ++k) cell->detail[k * 5 + static_cast<i32>(r)] = g.fr.b[k];
                    }
                }
                joan::orig::as_height_grid(grid_a)->set(joan::orig::arg::u32(g.i), joan::orig::arg::u32(g.j), joan::orig::arg::u16(static_cast<i32>(cell->height)));  // 0x004c5a20
                joan::orig::as_height_grid(grid_b)->set(joan::orig::arg::u32(g.i), joan::orig::arg::u32(g.j), joan::orig::arg::u16(static_cast<i32>(cell->shade)));   // 0x004c5a20
                if (cell->texture[0] != 0) strncpy_(cell->texture_name[0], g.textures[cell->texture[0] - 1], 0x20);
                if (cell->texture[1] != 0) strncpy_(cell->texture_name[1], g.textures[cell->texture[1] - 1], 0x20);
                if (cell->sector == 0) continue;
                g.place_sector(cell);
            }
        }
        heap_free(g.rle, 0xf0);
        if (g.ids != g.ids256) heap_free(g.ids, 0xf3);
        if (g.names != g.names256) heap_free(g.names, 0xf5);
        if (g.textures != g.textures32) heap_free(g.textures, 0xf7);
        joan::orig::as_rle(static_cast<void*>(reader))->destruct();  // 0x0059d1b0
        return 1;
    }
};

void Grid::place_sector(const GridCell* cell) {
    const i32 s = joan::orig::arg::as<i32>(joan::orig::world::alloc_objects(i32{1}));     // 0x0054e930
    void* const sector = joan::orig::arg::as<void*>(joan::orig::world_entry(joan::orig::arg::i32(s)));     // 0x0054eb20
    void* sector_id = default_id;                          // [ebp-0x28a4]
    if (cell->sector_name != 0) sector_id = ids[cell->sector_name - 1];
    const i32 ci = i, cj = j;
    float box[6];
    box[0] = static_cast<float>(static_cast<i32>(static_cast<u32>(ci << 6) + jo::view<u32>(self->origin_x)));
    box[1] = static_cast<float>(static_cast<i32>(static_cast<u32>(cj << 6) + jo::view<u32>(self->origin_y)));
    // Low z: min of the four corner heights - 1000.
    {
        const i32 a = cell->height, b = height(ci + 1, cj);
        float f = static_cast<float>(static_cast<i32>(static_cast<i16>(a < b ? a : b)));
        const float c = static_cast<float>(height(ci, cj + 1));
        f = c > f ? f : c;
        const float d = static_cast<float>(height(ci + 1, cj + 1));
        f = d > f ? f : d;
        box[2] = f - global<float>(d::flt_1000);
    }
    box[3] = static_cast<float>(static_cast<i32>(static_cast<u32>((ci + 1) << 6) + jo::view<u32>(self->origin_x)));
    box[4] = static_cast<float>(static_cast<i32>(static_cast<u32>((cj + 1) << 6) + jo::view<u32>(self->origin_y)));
    // High z: max of the four corner heights + 1000.
    {
        const i32 a = cell->height, b = height(ci + 1, cj);
        float f = static_cast<float>(static_cast<i32>(static_cast<i16>(a > b ? a : b)));
        const float c = static_cast<float>(height(ci, cj + 1));
        f = c < f ? f : c;
        const float d = static_cast<float>(height(ci + 1, cj + 1));
        f = d < f ? f : d;
        box[5] = f + global<float>(d::flt_1000);
    }
    alignas(4) u8 name[0x194];  // [ebp-0x2a50] (never destroyed, as original)
    joan::orig::as_string(name)->construct(str(d::s_newGrid));      // 0x005a11a0 "newGrid"
    joan::orig::as_string(name)->append(joan::orig::arg::i32(ci));   // 0x005a15d0
    joan::orig::as_string(name)->append(joan::orig::arg::i32(cj));
    const char* const name_p = joan::orig::as_string(name)->c_str();  // 0x00417d90
    joan::orig::as_sector(sector)->set(name_p, static_cast<void*>(box));       // 0x0054dbd0
    thiscall<void*>(joan::fn::X_set_24, sector, sector_id);                                             // 0x0054dc50 (sets the owner: a pointer)
    jo::as_world_sector(sector)->grid_x = static_cast<i16>(ci);  // +0x20
    jo::as_world_sector(sector)->grid_y = static_cast<i16>(cj);  // +0x22
    float avg = 0.0f;  // [ebp-0x2a54]
    if (cell->texture[1] != 0) {
        // the first corner through the cell pointer (as the original; differs only after a long row moved i / j)
        const i32 sum = static_cast<i32>(cell->shade) + shade(ci + 1, cj) + shade(ci, cj + 1) + shade(ci + 1, cj + 1);
        avg = static_cast<float>(sum) / global<float>(d::flt_1020);
    }
    i16* const row = grid_row(ci);
    if (cell->texture[1] != 0 && !(avg >= global<float>(d::flt_0_5))) {
        row[cj] = static_cast<i16>(cell->texture[1] - 1);
    } else {
        row[cj] = static_cast<i16>(cell->texture[0] - 1);
    }
    Vec3 pos;
    pos.x = static_cast<float>(self->origin_x);
    pos.y = static_cast<float>(self->origin_y);
    pos.z = 0.0f;
    alignas(4) u8 mat[0x40];
    joan::orig::matrix_translation(joan::orig::arg::mptr(static_cast<void*>(mat)), joan::orig::arg::mptr(static_cast<void*>(&pos)));  // 0x0060bb50
    joan::orig::as_sector(sector)->set_matrix_nop(0u, 0u, static_cast<void*>(mat));             // 0x0054dc80
}

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: per-cell entry count [ebp-0x288c] read before any typed cell set it -> 0.
FIX("World::build_grid_4f6eb0", 0x004f6eb0, &HWorld::build_grid);
#undef FIX

}  // namespace game_b115_09
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
//
//    - static initialisers (cdecl, no args): global float = global float / constant:
//      0x0051c310, 0x0052b320, 0x0052f900, 0x00543780, 0x0054d9c0; 0x00553a70
//      ([0xa3ce20] = atan(1) * 4.0); atexit (0x7b361a) registrations 0x005640c0, 0x005644d0,
//      0x00564830; TextDesc::clear 0x00623dc0 on a global: 0x00564ce0, 0x00564d70,
//      0x00565160, 0x005654c0, 0x00565790, 0x005659d0, 0x00565dc0, 0x005661b0, 0x00566240,
//      0x00566630.
//
//    - World::alloc_objects 0x0054e930 (cdecl, int n) (SEH frame: normal path only): grows the
//      pointer array [0xa3ce38] (capacity [0xa3ce34], *4 growth), creates n 0x66-byte objects
//      (0x54db70), returns the first 1-based index.
//
//  - 0x00553a70: CRT atan st0 taken as double, fmul qword 4.0 -> Fpu{true}.mul, as in the
//    earlier pi initialisers (game_b95_04.cpp 0x005436d0).
namespace {
namespace game_b100_08 {

void* const kMemory = obj(d::g_mem);

JOAN_COUNTER(c_alloc_objects, "World::alloc_objects");

void JOAN_CDECL pi_a3ce20() {  // 0x00553a70
    const Fpu fpu{true};
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));                                     // atan
    global<float>(d::g_a3ce20) = static_cast<float>(fpu.mul(a, global<double>(d::dbl_4)));  // 4.0
}

// 0x0054e930
i32 JOAN_CDECL alloc_objects(i32 n) {
    c_alloc_objects.hit();
    i32& count = global<i32>(d::g_a3ce30);
    i32& capacity = global<i32>(d::g_a3ce34);
    void**& array = global<void**>(d::g_a3ce38);
    if (count + n > capacity) {
        if (capacity == 0) capacity = 1;
        while (capacity < count + n) capacity = static_cast<i32>(static_cast<u32>(capacity) << 2);
        array = joan::orig::arg::as<void**>(joan::orig::as_mem(kMemory)->resize(static_cast<void*>(array), joan::orig::arg::u32(// 0x0058eb30
                                 static_cast<i32>(static_cast<u32>(capacity) * static_cast<u32>(sizeof(void*)))), str(d::s_d_ww_world_s_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a0b790)) + 0xb)));
    }
    i32 index = count + 1;
    const i32 first = index;
    for (i32 k = n; k > 0; --k, ++index) {
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::WorldSector)));  // operator new (0x66 on x86)
        void* const o = mem != nullptr ? joan::orig::as_sector(mem)->construct() : nullptr;  // 0x0054db70
        global<void**>(d::g_a3ce38)[index - 1] = o;
    }
    count = count + n;
    return first;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("World::alloc_objects", 0x0054e930, &alloc_objects);
B("pi_a3ce20", 0x00553a70, &pi_a3ce20);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b101_08.cpp (game/b101_08)
//
//    - world_entry 0x0054eb20 (cdecl, n): [0xa3ce38][n - 1] for 0 < n <= [0xa3ce30], else 0.
//
//    - World::unlink_all 0x0054f290 (ret 0): walk the +0x98 list by +0xb2, clearing +0xae /
//      +0xb2 / +0xaa of every node; +0x98 = +0x94 = 0. Returns this (eax).
//
//  - 0x00549dc0 / 0x0052b570 / 0x0054f290 / 0x0054a0e0 / 0x0054aa90 have no direct callers
//    (comparator pointer / CRT initialiser table / unreferenced).
namespace {
namespace game_b101_08 {


JOAN_COUNTER(c_unlink_all, "World::unlink_all");

struct H {

    // ------------------------------------------------------------------ 0x0054f290
    void* unlink_all() {
        c_unlink_all.hit();
        u8* p = jo::view<u8*>(jo::self<jo::Cell>(this)->objects_head);
        while (p != nullptr) {
            u8* const next = jo::view<u8*>(jo::as_map_object(p)->cell_next);
            jo::as_map_object(p)->cell_prev = nullptr;
            jo::as_map_object(p)->cell_next = nullptr;
            jo::as_map_object(p)->cell = nullptr;
            p = next;
        }
        jo::self<jo::Cell>(this)->objects_head = nullptr;
        jo::self<jo::Cell>(this)->objects_tail = nullptr;
        return this;
    }
};

// ---------------------------------------------------------------------- 0x0054eb20
static void* JOAN_CDECL world_entry(i32 n) {
    if (n <= 0 || n > global<i32>(d::g_a3ce30)) return nullptr;
    return global<jo::WorldSectorPtrArray*>(d::g_a3ce38)[n - 1].item;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b101_08", name, addr, fn)
B("world_entry", 0x0054eb20, &world_entry);
B("World::unlink_all", 0x0054f290, &H::unlink_all);
#undef B

}  // namespace game_b101_08
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
//
//    - Obj::get_8c 0x0054ee10; Ptr::valid 0x0056ddc0 ([0] != 0).
namespace {
namespace game_b90_04 {


struct H {

    // 0x0054ee10
    jo::Unit* get_8c() { return jo::self<jo::Cell>(this)->units_tail; }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Obj::get_8c", 0x0054ee10, &H::get_8c);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
//
//      0x0054ee70 get_33d              (cdecl(p), ret 0)
namespace {
namespace game_b98_04 {


jo::Unit* JOAN_CDECL get_33d(const void* p) { return jo::as_unit(p)->cell_prev; }  // 0x0054ee70

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("get_33d", 0x0054ee70, &get_33d);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b83_02.cpp (game/b83_02)
//
//    - World::create_water 0x00553aa0 (stream): name / material / sound tokens, position,
//      rotation, uv divisors, texture, flags, size, two effect names; builds a grid mesh
//      (0x005cbaf0) with one material (ice / waterpool / render-target textures by the
//      0xa58055 / 0xa58056 detail settings), the vertex grid (uv corners by rotation 0 / 90 /
//      180 / 270, lerp 0x0054a8a0 along the edges), the triangles, adds it to the sector and
//      registers the water object (0x00474df0 / 0x00425d90 / 0x00546bf0).
//
//    - World::read_anti_portal 0x00559470 (stream): four "(%f, %f, %f)" corners (+0x44 / +0x48
//      offset) into a new anti-portal mesh (0x005cb880) and occluder (0x00530ee0), then the
//      occluder plane (0x0060d920) and three edge vectors (0x0060cc90).
//
//    - World::read_scene_object 0x00551930 (stream, add): name, position, angle (degrees with
//      "HD" -> radians); when add != 0 the object "<name>.TXT" is placed in the sector found
//      by 0x005ef610.
//
//    - World::write_sector_mesh 0x0055f9b0 (file, sector).
//
//    - getters: 0x0054f980 (cdecl, +0x42), 0x0054f960 (+0xa0), 0x0054f530 (cdecl, +0xb2),
//      0x0054f510 (+0x98), 0x0054ef40 (cdecl, +0x341), 0x0054eec0 (+0x90), 0x0054dc20 (forwards
//      to the empty 0x0054dcc0, eax passed through).
//
//  - 0x00553aa0 reads the texture name with read_line(buf, 0x40) into [ebp-0x390]; a 64-char
//    line writes its NUL into the low byte of [ebp-0x350] (the v divisor, already copied to
//    the u divisor [ebp-0x34c]). Reproduced with the same byte layout (tex_raw).
//
//  - 0x00553aa0's SEH frame only guards operator new + the mesh constructor (normal path kept).
//
//  - 0x00559470: sscanf results are used as-is; on a malformed line the floats keep their
//    previous values (the first ones would be uninitialised stack in the original, here 0).
//    The shipped data always parses.
namespace {
namespace game_b83_02 {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
using joan::orig::helpers::at;

// Stream helpers (FileTxt).
char* get_token(void* s, std::int32_t advance) { return joan::orig::as_file_txt(s)->get_token(joan::orig::arg::i32(advance)); }  // 0x00589c70
// name[strlen(name) - 1] = 0 (the original's buffers have a dead byte in front).
void trim_last(char* name) { name[static_cast<std::int32_t>(strlen_(name)) - 1] = 0; }

// 0x0054a8a0 lerp(a, b, t) - cdecl, float in st0.
float lerp(float a, float b, float t) { return joan::orig::arg::as<float>(joan::orig::lerp_54a8a0(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(t))); }

using joan::orig::helpers::device;
constexpr std::int32_t kRelease = 2, kGetSurfaceLevel = 15, kSetRenderTarget = 31, kGetRenderTarget = 32,
                       kGetDepthStencilSurface = 33, kBeginScene = 34, kEndScene = 35, kClear = 36;

// 0x5a1170-family fixed string (0x191 bytes, no destructor).
struct alignas(4) String {
    char text[0x194];
};
const char* c_str(String& s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str()); }  // 0x00417d90

static_assert(sizeof(Box) == 0x18);

JOAN_COUNTER(c_water, "World::create_water");
JOAN_COUNTER(c_anti_portal, "World::read_anti_portal");
JOAN_COUNTER(c_scene_object, "World::read_scene_object");
JOAN_COUNTER(c_write_mesh, "World::write_sector_mesh");

// 0x0054f980 (cdecl): +0x42.
jo::WorldItem* JOAN_CDECL get_42(void* p) { return jo::as_world_item(p)->cell_next; }
// 0x0054f530 (cdecl): +0xb2.
jo::MapObject* JOAN_CDECL get_b2(void* p) { return jo::as_map_object(p)->cell_next; }
// 0x0054ef40 (cdecl): +0x341.
jo::Unit* JOAN_CDECL get_341(void* p) { return jo::as_unit(p)->cell_next; }

// x87 "equal or unordered" (fcomp; test ah,0x40; jnz).
bool eq_or_nan(float a, float b) { return !(a < b) && !(a > b); }

struct H {

    jo::WorldItem* get_a0() { return jo::self<jo::Cell>(this)->items_head; }  // 0x0054f960
    jo::MapObject* get_98() { return jo::self<jo::Cell>(this)->objects_head; }  // 0x0054f510
    jo::Unit* get_90() { return jo::self<jo::Cell>(this)->units_head; }  // 0x0054eec0

    // 0x0055f9b0 World::write_sector_mesh(file, sector).
    std::int32_t write_sector_mesh(void* file, std::int32_t sector_id) {
        c_write_mesh.hit();
        void* const sector = joan::orig::arg::as<void*>(joan::orig::as_world(this)->sector(joan::orig::arg::i32(sector_id)));  // 0x0054e1b0
        if (jo::view<void*>(jo::as_sector(sector)->plane) == nullptr) return 1;
        joan::orig::as_file(file)->file_put_long(1);                                                    // 0x00587da0
        const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_world(this)->sector_name(joan::orig::arg::i32(sector_id)));        // 0x0054e220
        if (joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(name), 0x21u) == 0) return 0;  // 0x005872e0
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_mesh(jo::view<void*>(jo::as_sector(sector)->plane))->write(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x005e2380
        return 1;
    }

    // 0x00551930 World::read_scene_object(stream, add).
    std::int32_t read_scene_object(void* stream, std::int32_t add) {
        c_scene_object.hit();
        const joan::hook::x87::Fpu fpu{true};
        alignas(4) char name_buf[0x108];  // [ebp-0x11c] (0xff + NUL), one pad byte in front
        char* const name = name_buf + 4;
        alignas(4) char line[0x104];  // [ebp-0x21c]
        float pos[3];                 // [ebp-0x14]

        get_token(stream, 1);
        get_token(stream, 1);
        read_line(stream, name, 0xff);
        trim_last(name);
        get_token(stream, 1);
        pos[0] = static_cast<float>(fpu.add(atof_(get_token(stream, 1)), static_cast<double>(jo::self<jo::Cell>(this)->origin_x)));
        get_token(stream, 1);
        pos[1] = static_cast<float>(fpu.add(atof_(get_token(stream, 1)), static_cast<double>(jo::self<jo::Cell>(this)->origin_y)));
        get_token(stream, 1);
        pos[2] = static_cast<float>(atof_(get_token(stream, 1)));
        double factor = 1.0;
        if (stricmp_(get_token(stream, 1), str(d::s_HD)) == 0) factor = 0.017453292519943295;  // "HD": pi / 180
        const float angle = static_cast<float>(fpu.mul(atof_(get_token(stream, 1)), factor));

        if (add != 0) {
            String path;
            joan::orig::as_string(&path)->construct();                   // 0x005a1170
            joan::orig::as_string(&path)->assign(joan::orig::arg::str(static_cast<char*>(name)));  // 0x005a1440 assign
            joan::orig::as_string(&path)->append(str(d::s_TXT));                        // 0x005a1550 append ".TXT"
            const char* const p = c_str(path);
            const std::int32_t id = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_list(obj(d::g_pemi_res))->find_4c2060(joan::orig::arg::str(p), static_cast<std::int8_t>(joan::orig::arg::i32(1))));  // 0x004c2060
            std::uint32_t found = 0;
            float range[6];
            joan::orig::as_aabb(static_cast<void*>(range))->set_point(joan::orig::arg::ptr(static_cast<float*>(pos)));  // 0x0060d430
            void* const scene = jo::view<void*>(jo::self<jo::Cell>(this)->world->scene);
            void* const hit = joan::orig::arg::as<void*>(joan::orig::as_scene(scene)->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(range)), &found));  // 0x005ef610
            if (found != 0) {
                void* const type = joan::orig::arg::as<void*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(id)));  // 0x0041b440
                void* const a2 = jo::as_pemi_script(type)->effect;  // the 0x0041b440 entry is a PemiScript (+0x24 effect)
                jo::Scene* const a1 = static_cast<jo::Scene* const*>(hit)[0];  // the first node found
                joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(a1, a2, obj(d::g_pos), static_cast<float*>(pos), static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::bits(angle), 0, 0);  // 0x0041baf0
            }
        }
        read_line(stream, line, 0xff);
        return 1;
    }

    // 0x00559470 World::read_anti_portal(stream).
    std::int32_t read_anti_portal(void* stream) {
        c_anti_portal.hit();
        const joan::hook::x87::Fpu fpu{true};
        alignas(4) char line[0x104];
        float v[3] = {0.0f, 0.0f, 0.0f};  // [ebp-0x114]

        const std::int32_t id = joan::orig::as_scene(jo::view<void*>(jo::self<jo::Cell>(this)->world->scene))->add_entry();                      // 0x005eeca0
        void* const mesh = joan::orig::arg::as<void*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Cell>(this)->world->scene))->occluder(id));                  // 0x005eed90
        joan::orig::as_mesh(mesh)->nothing();                                                                         // 0x005cb860
        joan::orig::as_mesh(mesh)->set_30_34(static_cast<void*>(this), 1u);                                            // 0x005cb8c0
        const std::int32_t oid = joan::orig::as_world_cell(this)->add_occluder();                                                // 0x00560c70
        std::uint8_t* const occ = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_world_cell(this)->occluder(oid));                           // 0x00560d30
        joan::orig::as_occluder(occ)->set_enabled();                                                                          // 0x00530eb0
        get_token(stream, 1);

        auto corner = [&](img::Addr format, std::int32_t slot) {
            read_line(stream, line, 0xff);
            joan::orig::libc::sscanf(joan::orig::arg::str(static_cast<char*>(line)), joan::orig::arg::str(str(format)), &v[0], &v[1], &v[2]);  // sscanf
            v[0] = static_cast<float>(fpu.add(static_cast<double>(jo::self<jo::Cell>(this)->origin_x), v[0]));
            v[1] = static_cast<float>(fpu.add(static_cast<double>(jo::self<jo::Cell>(this)->origin_y), v[1]));
            joan::orig::as_anti_portal(mesh)->set_corner(joan::orig::arg::i32(slot), joan::orig::arg::mptr(static_cast<float*>(v)));  // 0x005cb880
            joan::orig::as_occluder(occ)->set_corner(joan::orig::arg::i32(slot), joan::orig::arg::mptr(static_cast<float*>(v)));   // 0x00530ee0
        };
        corner(d::s_f_f_f_9d83c4, 0);  // "((%f, %f, %f),"
        corner(d::s_f_f_f_9d83b4, 1);  // "(%f, %f, %f),"
        corner(d::s_f_f_f_9d83b4, 3);
        corner(d::s_f_f_f_9d83a0, 2);  // "(%f, %f, %f)),"
        for (std::int32_t k = 0; k < 6; ++k) read_line(stream, line, 0xff);

        joan::orig::plane_from_points(joan::orig::arg::mptr(occ + 0x30), joan::orig::arg::ptr(occ), joan::orig::arg::ptr(occ + 0xc), joan::orig::arg::ptr(occ + 0x18));  // 0x0060d920
        auto edge = [&](std::int32_t dst, std::int32_t a, std::int32_t b) {
            float tmp[3];
            const std::uint32_t* const r =
                joan::orig::arg::as<std::uint32_t*>(joan::orig::vec_sub(joan::orig::arg::mptr(static_cast<float*>(tmp)), joan::orig::arg::ptr(occ + a), joan::orig::arg::ptr(occ + b)));  // 0x0060cc90
            std::uint32_t c[3] = {r[0], r[1], r[2]};
            std::memcpy(occ + dst, c, 12);
        };
        edge(0x40, 0xc, 0);
        edge(0x4c, 0x18, 0xc);
        edge(0x58, 0x24, 0x18);
        edge(0x64, 0, 0x24);
        return 1;
    }

    // 0x00553aa0 World::create_water(stream).
    std::int32_t create_water(void* stream) {
        c_water.hit();
        const joan::hook::x87::Fpu fpu{true};
        void* const tex_mgr = obj(d::g_texture_array);
        void* const materials = obj(d::g_texture_effect_res);

        alignas(4) char buf_name[0x48];  // [ebp-0x158] (reads of 0x40 / 0x40 / 0x30)
        char* const name = buf_name + 4;
        alignas(4) char buf_fx1[0x28];  // [ebp-0x17c]
        char* const fx1 = buf_fx1 + 4;
        alignas(4) char buf_fx2[0x28];  // [ebp-0x1a0]
        char* const fx2 = buf_fx2 + 4;
        alignas(4) char line[0x104];  // [ebp-0x118]
        // [ebp-0x394] .. [ebp-0x349] as raw bytes: +0 f394 slot (dead byte for name[-1]), +4 the
        // texture name (0x40), +0x44 the v divisor [ebp-0x350] (gets the NUL of a 64-char name),
        // +0x48 the u divisor [ebp-0x34c].
        alignas(4) std::uint8_t tex_raw[0x4c] = {};
        char* const tex_name = reinterpret_cast<char*>(tex_raw + 4);
        std::int32_t& div_v = at<std::int32_t>(tex_raw, 0x44);
        std::int32_t& div_u = at<std::int32_t>(tex_raw, 0x48);
        String material;  // [ebp-0x334]

        get_token(stream, 1);
        get_token(stream, 1);
        read_line(stream, name, 0x40);
        trim_last(name);
        get_token(stream, 1);
        read_line(stream, name, 0x40);
        trim_last(name);
        joan::orig::as_string(&material)->construct(joan::orig::arg::str(static_cast<char*>(name)));  // 0x005a11a0
        get_token(stream, 1);
        read_line(stream, name, 0x30);
        trim_last(name);
        std::int32_t sound;  // [ebp-0x338]
        if (stricmp_(name, str(d::s_none)) == 0)  // "none"
            sound = 0;
        else
            sound = joan::orig::arg::as<std::int16_t>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(joan::orig::arg::str(static_cast<char*>(name))));  // 0x004d12f0
        get_token(stream, 1);
        const float px = static_cast<float>(atof_(get_token(stream, 1)));  // [ebp-0x344]
        get_token(stream, 1);
        const float py = static_cast<float>(atof_(get_token(stream, 1)));  // [ebp-0x340]
        get_token(stream, 1);
        const float pz = static_cast<float>(atof_(get_token(stream, 1)));  // [ebp-0x33c]
        get_token(stream, 1);
        const std::int32_t rotation = atoi_(get_token(stream, 1));  // [ebp-0x348]
        div_u = 0x96;
        div_v = 0x96;
        get_token(stream, 1);
        div_v = atoi_(get_token(stream, 1));
        div_u = div_v;
        get_token(stream, 1);
        read_line(stream, tex_name, 0x40);
        trim_last(tex_name);
        get_token(stream, 1);
        const float f394 = static_cast<float>(atoi_(get_token(stream, 1)));
        get_token(stream, 1);
        const std::uint8_t b398 = atoi_(get_token(stream, 1)) != 0 ? 1 : 0;
        get_token(stream, 1);
        const float sx = static_cast<float>(atof_(get_token(stream, 1)));  // [ebp-0x3a4]
        get_token(stream, 1);
        const float sy = static_cast<float>(atof_(get_token(stream, 1)));  // [ebp-0x3a0]
        get_token(stream, 1);
        read_line(stream, fx1, 0x20);
        trim_last(fx1);
        get_token(stream, 1);
        read_line(stream, fx2, 0x20);
        trim_last(fx2);
        read_line(stream, line, 0xff);

        Box box;  // [ebp-0x3bc]
        box.v[0] = px - sx / 2.0f;
        box.v[1] = py - sy / 2.0f;
        box.v[2] = pz;
        box.v[3] = sx / 2.0f + px;
        box.v[4] = sy / 2.0f + py;
        box.v[5] = pz;

        const std::int32_t sector_id = joan::orig::arg::as<std::int32_t>(joan::orig::as_world(this)->add_entry());           // 0x0042ce10
        void* const sector = joan::orig::arg::as<void*>(joan::orig::as_map(this)->sector(joan::orig::arg::i32(sector_id)));                 // 0x0054aa00
        joan::orig::as_scene_source(sector)->reset(joan::orig::arg::mptr(static_cast<float*>(box.v)));                          // 0x005ef880
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));                          // 0x11c on x86; operator new
        void* const mesh = mem != nullptr ? joan::orig::as_plane(mem)->construct(1u, 1u) : nullptr;  // 0x005cbaf0
        joan::orig::as_plane(mesh)->init(0, 0, 0, 1u);                                           // 0x005cc7a0
        const std::int32_t mat_index = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_materials(1));       // 0x005cd470
        std::uint8_t* const mat = reinterpret_cast<std::uint8_t*>(&jo::as_plane(mesh)->materials[mat_index]);  // Material is 0x54 bytes on x86, larger on x64
        joan::orig::as_material(mat)->construct();  // 0x005d52a0
        jo::as_material(mat)->blend = 1;
        jo::as_material(mat)->pass_mask = 2;
        jo::as_material(mat)->raster |= 0x100;

        auto load_tex = [&](const char* tname) {
            return joan::orig::arg::as<void*>(joan::orig::as_texture_manager(tex_mgr)->load(joan::orig::arg::str(tname), 1, 0u));  // 0x006099c0
        };
        auto set_tex = [&](std::int32_t slot, void* t) { joan::orig::as_material(mat)->set_texture(joan::orig::arg::i32(slot), joan::orig::arg::mptr(t)); };  // 0x005d5650
        auto release_tex = [&](void* t) { joan::orig::as_texture_manager(tex_mgr)->release(joan::orig::arg::mptr(t)); };                   // 0x0060a550
        auto material_id = [&](const char* m) {
            return joan::orig::arg::as<std::uint32_t>(joan::orig::as_anim_db(materials)->find(joan::orig::arg::str(m)));  // 0x004dff70
        };
        auto waterpool = [&]() {
            void* const t = load_tex(str(d::s_waterpool));  // "waterpool"
            set_tex(0, t);
            release_tex(t);
            jo::as_material(mat)->texture_mode = material_id(str(d::s_NormalWater)) | 0x1000000;  // "NormalWater"
        };

        if (stricmp_(c_str(material), str(d::s_ice)) == 0) {  // "ice"
            void* const t = load_tex(str(d::s_icelake));          // "icelake"
            set_tex(0, t);
            release_tex(t);
            jo::as_material(mat)->texture_mode = material_id(c_str(material)) | 0x1000000;
        } else if (global<std::int8_t>(d::g_cfg_shadow_quality) == 0) {
            waterpool();
        } else if (global<std::int8_t>(d::g_a58056) != 0) {
            set_tex(0, nullptr);
            void* const t = load_tex(tex_name);
            set_tex(1, t);
            release_tex(t);
            std::uint32_t id = 0;
            std::uint8_t reflect = 0;
            if (global<std::int8_t>(d::g_cfg_shadow_quality) == 2) {
                String r;
                joan::orig::as_string(&r)->construct_copy(joan::orig::as_string(&material));  // 0x005a11f0 copy
                joan::orig::as_string(&r)->append(str(d::s_R));                // 0x005a1550 append "R"
                id = material_id(c_str(r));
                if (id != 0) reflect = 1;
            }
            if (id == 0 && tex_name[0] != 0 && global<std::uint32_t>(d::g_a52637) != 0) id = material_id(c_str(material));
            if (id != 0)
                jo::as_material(mat)->texture_mode = id | 0x1000000;
            else if (global<std::uint32_t>(d::g_a52637) != 0)
                jo::as_material(mat)->texture_mode = 0xc;
            if (reflect != 0) {
                jo::TexScriptRec* const def = joan::orig::arg::as<jo::TexScriptRec*>(joan::orig::as_anim_db(materials)->entry(joan::orig::arg::u32(static_cast<std::int16_t>(id))));  // 0x004b27b0
                for (std::int32_t i = 0; i < jo::view<std::int16_t>(def->stage_count[0]); ++i) {
                    if (def->projected_flags[i] == 0) continue;
                    joan::orig::as_material(mat)->set_14_18(joan::orig::arg::mptr(obj(d::g_init_white)), joan::orig::arg::bits(1.0f));  // 0x005d5610
                    void* const rt = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(tex_mgr)->new_render_target());  // 0x006096d0
                    joan::orig::as_render_target(rt)->create(0x400u, 0x400u, 1u);              // 0x00608080
                    void* old_rt;
                    void* old_zs;
                    void* surface;
                    com_call(device(), kGetRenderTarget, &old_rt);
                    com_call(device(), kGetDepthStencilSurface, &old_zs);
                    com_call(jo::view<void*>(jo::as_render_target(rt)->d3d_texture), kGetSurfaceLevel, 0u, &surface);
                    com_call(device(), kSetRenderTarget, surface, static_cast<void*>(nullptr));
                    if (com_call(device(), kBeginScene) == 0) {
                        alignas(4) std::uint8_t rgba[4] = {0x80, 0x80, 0x80, 0xff};  // [ebp-0x594]
                        std::uint32_t color;
                        joan::orig::as_colour(static_cast<void*>(rgba))->to_d3d(&color);  // 0x005e93b0
                        com_call(device(), kClear, 0u, static_cast<void*>(nullptr), 1u, color, 1.0f, 0u);
                        com_call(device(), kEndScene);
                    }
                    com_call(device(), kSetRenderTarget, old_rt, old_zs);
                    com_call(old_rt, kRelease);
                    com_call(old_zs, kRelease);
                    com_call(surface, kRelease);
                    set_tex(i, rt);
                    release_tex(rt);
                }
            } else if (id == 0) {
                waterpool();
            }
        } else {
            if (material_id(c_str(material)) != 0) {
                if (global<std::int8_t>(d::g_cfg_shadow_quality) == 2) {
                    joan::orig::as_material(mat)->set_14_18(joan::orig::arg::mptr(obj(d::g_init_white)), joan::orig::arg::bits(1.0f));  // 0x005d5610
                    jo::as_material(mat)->texture_mode = 0x20010;
                    void* const rt = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(tex_mgr)->new_render_target());  // 0x006096d0
                    joan::orig::as_render_target(rt)->create(0x400u, 0x400u, 1u);              // 0x00608080
                    set_tex(0, rt);
                    release_tex(rt);
                } else if (global<std::int8_t>(d::g_cfg_shadow_quality) == 1) {
                    jo::as_material(mat)->texture_mode = 0xc;
                    void* const t = load_tex(tex_name);
                    set_tex(1, t);
                    release_tex(t);
                }
            } else {
                waterpool();
            }
        }

        // Grid size.
        std::int32_t step = 800;  // [ebp-0x5ac]
        if (!(sx >= 800.0f) || !(sy >= 800.0f)) {
            const float m = !(sx >= sy) ? sx : sy;  // fcomp; test ah,1 (less or unordered) -> sx
            step = joan::hook::x87::ftol(fpu.div(m, 3.0));
        }
        const std::int32_t w = joan::hook::x87::ftol(sx) / step + 2;  // [ebp-0x5b0]
        const std::int32_t h = joan::hook::x87::ftol(sy) / step + 2;  // [ebp-0x5b4]
        const float x0 = px - sx / 2.0f;
        const float y0 = py - sy / 2.0f;
        const float dx = static_cast<float>(fpu.div(sx, static_cast<double>(w - 1)));
        const float dy = static_cast<float>(fpu.div(sy, static_cast<double>(h - 1)));
        joan::orig::as_plane(mesh)->add_vertices(joan::orig::arg::i32(w * h));  // 0x005cd7f0

        auto vtx = [&](std::int32_t k) -> std::uint8_t* { return reinterpret_cast<std::uint8_t*>(&jo::as_plane(mesh)->vertices_f1[k]); };
        for (std::int32_t j = 0; j < h; ++j) {
            for (std::int32_t i = 0; i < w; ++i) {
                jo::as_vertex_f1(vtx(w * j + i))->pos[0] =
                    static_cast<float>(fpu.add(fpu.mul(static_cast<double>(i), dx), x0));
                jo::as_vertex_f1(vtx(w * j + i))->pos[1] =
                    static_cast<float>(fpu.add(fpu.mul(static_cast<double>(j), dy), y0));
                jo::as_vertex_f1(vtx(w * j + i))->pos[2] = pz;
                jo::VertexF1* const n = jo::as_vertex_f1(vtx(w * j + i));
                jo::view<std::uint32_t>(n->attr[0]) = global<std::uint32_t>(d::g_a18a60);
                jo::view<std::uint32_t>(n->attr[1]) = global<std::uint32_t>(d::g_a18a64);
                jo::view<std::uint32_t>(n->attr[2]) = global<std::uint32_t>(d::g_a18a68);
                if (stricmp_(c_str(material), str(d::s_BumpEnvWaterSea)) == 0) {  // "BumpEnvWaterSea"
                    vcol1(vtx(w * j + i))[2] = 0x6f;
                    vcol1(vtx(w * j + i))[0] = 0xd5;
                    vcol1(vtx(w * j + i))[1] = 0xc8;
                } else {
                    vcol1(vtx(w * j + i))[2] = 0x41;
                    vcol1(vtx(w * j + i))[0] = 0x50;
                    vcol1(vtx(w * j + i))[1] = 0x33;
                }
                vcol1(vtx(w * j + i))[3] = 0xb4;
            }
        }

        // Corner uvs by rotation.
        auto U = [&]() { return static_cast<float>(joan::hook::x87::ftol(sx) / div_u); };
        auto V = [&]() { return static_cast<float>(joan::hook::x87::ftol(sy) / div_v); };
        const std::int32_t ca = 0, cb = w - 1, cc = (h - 1) * w, cd = (h - 1) * w + w - 1;
        auto set_u = [&](std::int32_t k, float val) { jo::as_vertex_f1(vtx(k))->attr[4] = val; };
        auto set_v = [&](std::int32_t k, float val) { jo::as_vertex_f1(vtx(k))->attr[5] = val; };
        if (rotation == 0) {
            set_u(ca, 0.0f);
            set_v(ca, 0.0f);
            set_u(cb, U());
            set_v(cb, 0.0f);
            set_u(cc, 0.0f);
            set_v(cc, V());
            set_u(cd, U());
            set_v(cd, V());
        } else if (rotation == 0x5a) {
            set_u(ca, V());
            set_v(ca, 0.0f);
            set_u(cb, V());
            set_v(cb, U());
            set_u(cc, 0.0f);
            set_v(cc, 0.0f);
            set_u(cd, 0.0f);
            set_v(cd, U());
        } else if (rotation == 0xb4) {
            set_u(ca, U());
            set_v(ca, V());
            set_u(cb, 0.0f);
            set_v(cb, V());
            set_u(cc, U());
            set_v(cc, 0.0f);
            set_u(cd, 0.0f);
            set_v(cd, 0.0f);
        } else if (rotation == 0x10e) {
            set_u(ca, 0.0f);
            set_v(ca, U());
            set_u(cb, 0.0f);
            set_v(cb, 0.0f);
            set_u(cc, V());
            set_v(cc, U());
            set_u(cd, V());
            set_v(cd, 0.0f);
        }

        // Interpolate along the edges whose corners agree.
        for (std::int32_t j = 0; j < h; ++j) {
            for (std::int32_t i = 0; i < w; ++i) {
                const std::int32_t k = w * j + i;
                auto tj = [&]() { return static_cast<float>(fpu.div(static_cast<double>(j), static_cast<double>(h - 1))); };
                auto ti = [&]() { return static_cast<float>(fpu.div(static_cast<double>(i), static_cast<double>(w - 1))); };
                for (std::int32_t a = 4; a <= 5; ++a) {  // u, v
                    if (eq_or_nan(jo::as_vertex_f1(vtx(ca))->attr[a], jo::as_vertex_f1(vtx(cb))->attr[a])) {
                        const float t = tj();
                        jo::as_vertex_f1(vtx(k))->attr[a] = lerp(jo::as_vertex_f1(vtx(ca))->attr[a], jo::as_vertex_f1(vtx(cc))->attr[a], t);
                    } else if (eq_or_nan(jo::as_vertex_f1(vtx(ca))->attr[a], jo::as_vertex_f1(vtx(cc))->attr[a])) {
                        const float t = ti();
                        jo::as_vertex_f1(vtx(k))->attr[a] = lerp(jo::as_vertex_f1(vtx(ca))->attr[a], jo::as_vertex_f1(vtx(cb))->attr[a], t);
                    }
                }
                if (vcol1(vtx(ca))[3] == vcol1(vtx(cb))[3]) {
                    const float t = tj();
                    const float b = static_cast<float>(vcol1(vtx(cc))[3]);
                    const float a = static_cast<float>(vcol1(vtx(ca))[3]);
                    vcol1(vtx(k))[3] = static_cast<std::uint8_t>(joan::hook::x87::ftol(lerp(a, b, t)));
                } else if (vcol1(vtx(ca))[3] == vcol1(vtx(cc))[3]) {
                    const float t = ti();
                    const float b = static_cast<float>(vcol1(vtx(cb))[3]);
                    const float a = static_cast<float>(vcol1(vtx(ca))[3]);
                    vcol1(vtx(k))[3] = static_cast<std::uint8_t>(joan::hook::x87::ftol(lerp(a, b, t)));
                }
            }
        }

        // Triangles.
        joan::orig::as_plane(mesh)->add_triangles(joan::orig::arg::i32((w - 1) * (h - 1) * 2));  // 0x005ce6f0
        for (std::int32_t j = 0; j < h - 1; ++j) {
            for (std::int32_t i = 0; i < w - 1; ++i) {
                const std::int32_t tri = i * 2 + (w - 1) * j * 2;
                joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(tri), joan::orig::arg::u32(mat_index), joan::orig::arg::u32(w * j + i), joan::orig::arg::u32(w * j + i + 1), joan::orig::arg::u32(w * (j + 1) + i));  // 0x005ce800
                joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(tri + 1), joan::orig::arg::u32(mat_index), joan::orig::arg::u32(w * j + i + 1), joan::orig::arg::u32(w * (j + 1) + i + 1), joan::orig::arg::u32(w * (j + 1) + i));
            }
        }
        joan::orig::as_plane(mesh)->set_format(2);  // 0x005cdc70
        joan::orig::as_plane(mesh)->upload();     // 0x005cfa10
        jo::as_plane(mesh)->origin[0] = px;
        jo::as_plane(mesh)->origin[1] = py;
        jo::as_plane(mesh)->origin[2] = pz;
        if (global<std::uint32_t>(d::g_a52633) != 0) joan::orig::as_plane(mesh)->set_mode(2u);  // 0x005d7420
        joan::orig::as_plane(mesh)->invalidate(joan::orig::arg::u32(-1));         // 0x005cf920
        joan::orig::as_plane(mesh)->sort_by_material(1);          // 0x005cf1d0
        joan::orig::as_scene_source(sector)->set_38(joan::orig::arg::mptr(mesh));     // 0x005efbe0
        // [Graphics] Water=1 (not part of the original game): its surface, for the modern water pass.
        if (stricmp_(c_str(material), str(d::s_ice)) != 0) joan::render::water_register(mesh, {box.v[0], box.v[1], box.v[3], box.v[4], pz});

        void* const waters = obj(d::g_water_array);
        const std::int32_t water_id =
            thiscall<std::int32_t>(joan::fn::List_add_546af0, waters, static_cast<char>(joan::orig::arg::i32(b398)), static_cast<char>(joan::orig::arg::i32(0)), box, joan::orig::arg::bits(f394), joan::orig::arg::u32(sound), sector);  // 0x00474df0 (the sector is a pointer: Water::u32_30)
        void* const water = joan::orig::as_dynarray(waters)->ptr_at(joan::orig::arg::i32(water_id));  // 0x00425d90
        if (strlen_(fx1) != 0) joan::orig::as_water(water)->add_sound(joan::orig::arg::str(static_cast<char*>(fx1)));        // 0x00546bf0
        if (strlen_(fx2) != 0) joan::orig::as_water(water)->add_sound(joan::orig::arg::str(static_cast<char*>(fx2)));
        return 1;
    }
};

#define B83(name, addr, fn) JOAN_REPLACE("game/b83_02", name, addr, fn)
B83("World::create_water", 0x00553aa0, &H::create_water);
B83("World::read_anti_portal", 0x00559470, &H::read_anti_portal);
B83("World::read_scene_object", 0x00551930, &H::read_scene_object);
B83("World::write_sector_mesh", 0x0055f9b0, &H::write_sector_mesh);
B83("get_42", 0x0054f980, &get_42);
B83("Obj::get_a0", 0x0054f960, &H::get_a0);
B83("get_b2", 0x0054f530, &get_b2);
B83("Obj::get_98", 0x0054f510, &H::get_98);
B83("get_341", 0x0054ef40, &get_341);
B83("Obj::get_90", 0x0054eec0, &H::get_90);
#undef B83

}  // namespace game_b83_02
}  // namespace

// ---------------------------------------- from modules/game_b82_10.cpp (game/b82_10)
//
//    - World::load_pln 0x0055f320 (path): File + Reader locals, "invalid pln format", the
//      sector table, one mesh per used sector (0x005cbaf0 / 0x005e26c0), progress ticks.
//
//    - World::read_scene_sector 0x00551440 (stream, add): name + two boxes (+-128 margin) from
//      the tokens; when add != 0, the next sector id gets the boxes (0x005ef880 / +0x20) and
//      the name (0x0041baa0).
//
//    - Unit::set_sector 0x0054f110 (unit; the new sector is a constant 0: only the unlink
//      half is live).
//
//    - World::read_portals 0x005593e0 ("Anti-Portal" -> 0x00559470), World::read_nodes
//      0x0054a6c0 ("CreateNode" -> 0x0047ad40, "CreateWater" -> 0x00553aa0, then +0x60 = 1).
namespace {
namespace game_b82_10 {


// Stream helpers (FileTxt).
char* get_token(void* s, std::int32_t advance) { return joan::orig::as_file_txt(s)->get_token(joan::orig::arg::i32(advance)); }  // 0x00589c70

// Binary reader helpers.
std::int32_t read_int(void* r) { return joan::orig::as_file_mem(r)->file_get_long(); }                // 0x005893c0
void read_bytes(void* r, void* buf, std::int32_t n) { joan::orig::as_file_mem(r)->file_read(joan::orig::arg::mptr(buf), joan::orig::arg::u32(n)); }  // 0x00588a80
void seek(void* r, std::int32_t off, std::int32_t whence) { joan::orig::as_file_mem(r)->file_seek(joan::orig::arg::i32(off), joan::orig::arg::i32(whence)); }  // 0x005888c0
void* mem_alloc(std::int32_t size, std::int32_t line) {  // 0x0058e7b0
    return joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size), str(d::s_d_ww_world_s_cpp), joan::orig::arg::i32(line));
}
void mem_free(void* p, std::int32_t line) {  // 0x0058ed70
    joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(p), str(d::s_d_ww_world_s_cpp), joan::orig::arg::i32(line));
}
void progress_tick() {
    joan::orig::as_sound_stream(obj(d::g_snd))->update();  // 0x0059b1c0
    joan::orig::as_ambient_streams(obj(d::g_mus))->update();  // 0x005866d0
}

// World::load_pln's File / FileMem locals: 0x11c bytes room on x86 (File 0x10e, FileMem 0x11c); x64: File 0x112,
// FileMem 0x12c.
struct alignas(alignof(void*)) File {
    std::uint8_t bytes[((sizeof(joan::orig::File) > 0x11c ? sizeof(joan::orig::File) : 0x11c) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};
struct alignas(alignof(void*)) Reader {
    std::uint8_t bytes[((sizeof(joan::orig::FileMem) > 0x11c ? sizeof(joan::orig::FileMem) : 0x11c) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};

JOAN_COUNTER(c_load_pln, "World::load_pln");
JOAN_COUNTER(c_scene_sector, "World::read_scene_sector");
JOAN_COUNTER(c_set_sector, "Unit::set_sector");
JOAN_COUNTER(c_portals, "World::read_portals");

struct H {

    // 0x0054f110 Unit::set_sector(unit): new sector is the constant 0, so only the unlink from
    // the current sector (+0xaa, list links +0xae prev / +0xb2 next, sector head +0x94 /
    // tail +0x98) runs.
    void set_sector(void* unit) {
        c_set_sector.hit();
        void* const old_sector = jo::view<void*>(jo::as_map_object(unit)->cell);
        if (old_sector == nullptr) return;  // new (0) == old
        void* const prev = jo::view<void*>(jo::as_map_object(unit)->cell_prev);
        void* const next = jo::view<void*>(jo::as_map_object(unit)->cell_next);
        if (prev != nullptr)
            jo::view<void*>(jo::as_map_object(prev)->cell_next) = next;
        else
            jo::view<void*>(jo::as_cell(old_sector)->objects_head) = next;
        if (next != nullptr)
            jo::view<void*>(jo::as_map_object(next)->cell_prev) = prev;
        else
            jo::view<void*>(jo::as_cell(old_sector)->objects_tail) = prev;
        jo::view<void*>(jo::as_map_object(unit)->cell_prev) = nullptr;
        jo::view<void*>(jo::as_map_object(unit)->cell_next) = nullptr;
        jo::view<void*>(jo::as_map_object(unit)->cell) = nullptr;
    }

    // 0x005593e0 World::read_portals(stream).
    std::int32_t read_portals(void* stream) {
        c_portals.hit();
        alignas(4) char line[0x100];
        read_line(stream, line, 0xff);
        for (;;) {
            char* const tok = get_token(stream, 1);
            if (tok == nullptr) break;
            if (stricmp_(tok, str(d::s_Anti_Portal)) == 0) joan::orig::as_world(this)->read_anti_portal(joan::orig::arg::mptr(stream));  // "Anti-Portal" 0x00559470
        }
        return 1;
    }

    // 0x00551440 World::read_scene_sector(stream, add).
    std::int32_t read_scene_sector(void* stream, std::int32_t add) {
        c_scene_sector.hit();
        const joan::hook::x87::Fpu fpu{true};
        const double margin = 128.0f;  // 0x009d7c50
        // name: 0x40 chars + NUL may be written (one byte past the original's buffer, into a
        // dead local); one pad byte in front for name[strlen - 1] with an empty name.
        alignas(4) char name_buf[0x48];
        char* const name = name_buf + 4;
        alignas(4) char line[0x100];
        float inner[6];  // [ebp-0x7c]: x0 y0 z0 x1 y1 z1
        float outer[6];  // [ebp-0x64]

        get_token(stream, 1);
        get_token(stream, 1);
        read_line(stream, name, 0x40);
        name[static_cast<std::int32_t>(strlen_(name)) - 1] = 0;

        auto plus = [&](std::int32_t v, std::int32_t base) {
            return fpu.add(static_cast<double>(v), static_cast<double>(base));
        };
        get_token(stream, 1);
        char* t = get_token(stream, 1);
        inner[0] = static_cast<float>(plus(atoi_(t), jo::self<jo::Cell>(this)->origin_x));
        outer[0] = static_cast<float>(fpu.sub(plus(atoi_(t), jo::self<jo::Cell>(this)->origin_x), margin));
        get_token(stream, 1);
        t = get_token(stream, 1);
        inner[3] = static_cast<float>(plus(atoi_(t), jo::self<jo::Cell>(this)->origin_x));
        outer[3] = static_cast<float>(fpu.add(plus(atoi_(t), jo::self<jo::Cell>(this)->origin_x), margin));
        get_token(stream, 1);
        t = get_token(stream, 1);
        inner[1] = static_cast<float>(plus(atoi_(t), jo::self<jo::Cell>(this)->origin_y));
        outer[1] = static_cast<float>(fpu.sub(plus(atoi_(t), jo::self<jo::Cell>(this)->origin_y), margin));
        get_token(stream, 1);
        t = get_token(stream, 1);
        inner[4] = static_cast<float>(plus(atoi_(t), jo::self<jo::Cell>(this)->origin_y));
        outer[4] = static_cast<float>(fpu.add(plus(atoi_(t), jo::self<jo::Cell>(this)->origin_y), margin));
        get_token(stream, 1);
        t = get_token(stream, 1);
        outer[2] = static_cast<float>(atoi_(t));
        inner[2] = outer[2];
        get_token(stream, 1);
        t = get_token(stream, 1);
        outer[5] = static_cast<float>(atoi_(t));
        inner[5] = outer[5];
        for (std::int32_t k = 0; k < 6; ++k) read_line(stream, line, 0xff);

        if (add != 0) {
            const std::int32_t id = joan::orig::arg::as<std::int32_t>(joan::orig::as_cell(this)->add_scene_node());         // 0x0042b270
            void* const sector = joan::orig::arg::as<void*>(joan::orig::as_world(this)->sector(joan::orig::arg::i32(id)));              // 0x0054e1b0
            joan::orig::as_scene_source(sector)->reset(joan::orig::arg::mptr(static_cast<void*>(outer)));                 // 0x005ef880
            std::memcpy(jo::as_scene_s(sector)->inner_box, inner, 0x18);
            joan::orig::as_names(this)->set_41baa0(joan::orig::arg::i32(id), joan::orig::arg::str(static_cast<char*>(name)));                // 0x0041baa0
        }
        return 1;
    }

    // 0x0055f320 World::load_pln(path).
    std::int32_t load_pln(const char* path) {
        joan::hitch::Span hitch_span("load_pln");  // [Debug] HitchLog
        c_load_pln.hit();
        joan::item_preload::once();  // [Game] ItemPreload: the first mission load builds the drop items' models
        File file;
        joan::orig::as_file(&file)->construct();  // 0x004179c0
        Reader reader;
        joan::orig::as_file_mem(&reader)->construct();  // 0x00587f80
        // (constant-false branch with 0x00586da0 / 0x005885e0 omitted: unreachable.)
        joan::orig::as_file_mem(&reader)->open_588000(joan::orig::arg::str(path), 1, static_cast<char>(joan::orig::arg::i32(0)));  // 0x00588000 open

        const std::uint8_t flag =
            (global<std::uint32_t>(d::g_a52508) >= 3 && (global<std::uint32_t>(d::g_a524fc) & 0xffffu) >= 4) ? 1 : 0;
        const std::int32_t v = read_int(&reader);
        if ((flag != 0 && v == 0) || (flag == 0 && v != 0)) {
            joan::orig::as_file_mem(&reader)->destruct();  // 0x00587fd0
            joan::orig::as_file(&file)->destruct();  // 0x005872b0
            return 0;
        }
        if (read_int(&reader) != 3) {
            joan::orig::warning_box(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_invalid_pln_format));  // 0x00586bc0 "invalid pln format"
            joan::orig::as_file_mem(&reader)->destruct();
            joan::orig::as_file(&file)->destruct();
            return 0;
        }
        const std::int32_t n_entries = read_int(&reader);
        const std::int32_t n_used = read_int(&reader);
        const std::int32_t table_size =
            static_cast<std::int32_t>(static_cast<std::uint32_t>(n_entries + 1) * static_cast<std::uint32_t>(sizeof(PlnEntry)));
        std::uint8_t* const table =
            static_cast<std::uint8_t*>(mem_alloc(table_size, global<std::int16_t>(d::g_a0b7a4) + 0x2c));
        read_bytes(&reader, table, table_size);
        progress_tick();

        std::int32_t i = 0;
        for (; i < n_used; ++i) {
            const PlnEntry* const e = reinterpret_cast<const PlnEntry*>(table) + i;
            void* sector;
            const std::int32_t w = e->sector;
            if (w > 0 && w <= jo::self<jo::Cell>(this)->sector_count &&
                std::strcmp(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_world(this)->sector_name(joan::orig::arg::i32(w)))), joan::orig::arg::str(// 0x0054e220
                                         e->name)) == 0)                  // strcmp
                sector = joan::orig::arg::as<void*>(joan::orig::as_world(this)->sector(joan::orig::arg::i32(w)));                                     // 0x0054e1b0
            else
                sector = joan::orig::arg::as<void*>(joan::orig::as_world(this)->find_sector(joan::orig::arg::str(e->name)));                 // 0x0054e6a0
            std::int32_t found = 0;
            if (sector != nullptr) {
                found = 1;
                progress_tick();
            }
            if (found == 0) continue;
            seek(&reader, e->offset, 0);
            if (read_int(&reader) == 1) {
                alignas(4) std::uint8_t name[0x24];
                read_bytes(&reader, name, 0x21);
                void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86; operator new
                void* const mesh = mem != nullptr ? joan::orig::as_plane(mem)->construct(1u, 0u) : nullptr;  // 0x005cbaf0
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_mesh(mesh)->read(joan::orig::arg::mptr(static_cast<void*>(&reader)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)))) != 0) {  // 0x005e26c0
                    jo::view<std::uint32_t>(jo::as_plane(mesh)->list_kind) = 2;
                    joan::orig::as_plane(mesh)->set_owner(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x005d03f0
                    joan::orig::as_plane(mesh)->set_10c(1u);                        // 0x005e4320
                    joan::orig::as_plane(mesh)->sort_by_material(0);                        // 0x005cf1d0
                    joan::orig::as_scene_source(sector)->set_38(joan::orig::arg::mptr(mesh));                   // 0x005efbe0
                } else if (mesh != nullptr) {
                    joan::orig::as_plane(mesh)->destroy(1u);  // 0x00459c40 deleting dtor
                }
            }
            progress_tick();
        }
        seek(&reader, (reinterpret_cast<const PlnEntry*>(table) + i)->offset, 0);
        mem_free(table, global<std::int16_t>(d::g_a0b7a4) + 0x93);
        joan::orig::as_file_mem(&reader)->destruct();
        joan::orig::as_file(&file)->destruct();
        return 1;
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_10", name, addr, fn)
B82("World::load_pln", 0x0055f320, &H::load_pln);
B82("World::read_scene_sector", 0x00551440, &H::read_scene_sector);
B82("Unit::set_sector", 0x0054f110, &H::set_sector);
B82("World::read_portals", 0x005593e0, &H::read_portals);
#undef B82

}  // namespace game_b82_10
}  // namespace

// ---------------------------------------- from modules/game_b102_06.cpp (game/b102_06)
//
//    - World::get_94 0x0054f460 / World::get_a4 0x0054fc80 (ret 0): dword getters. Dead code.
//
//    - World::clear_a0 0x0054f700 (ret 0): +0x3e / +0x42 / +0x3a cleared on every node of the
//      +0xa0 list (next = +0x42), then +0xa0 = +0x9c = 0. Returns this. Dead code.
//
//    - World::clear_a8 0x0054fb20 (ret 0): 0x0043edb0(p) on each node of the +0xa8 list
//      (next = +0x1ea read before the call), then +0xa8 = +0xa4 = 0. Returns this. Dead code.
//
//    - Obj::clear_34_4 0x0054fdb0 (ret 0): [+0x34]+4 = 0 when +0x34 != 0. Returns this. Dead.
namespace {
namespace game_b102_06 {


struct H {

    // 0x0054f460
    jo::MapObject* get_94() { return jo::self<jo::Cell>(this)->objects_tail; }
    // 0x0054fc80
    jo::Place* get_a4() { return jo::self<jo::Cell>(this)->places_tail; }

    // 0x0054f700
    H* clear_a0() {
        for (u8* p = jo::view<u8*>(jo::self<jo::Cell>(this)->items_head); p != nullptr;) {
            u8* const next = jo::view<u8*>(jo::as_world_item(p)->cell_next);
            jo::as_world_item(p)->cell_prev = nullptr;
            jo::as_world_item(p)->cell_next = nullptr;
            jo::as_world_item(p)->cell = nullptr;
            p = next;
        }
        jo::self<jo::Cell>(this)->items_head = nullptr;
        jo::self<jo::Cell>(this)->items_tail = nullptr;
        return this;
    }

    // 0x0054fb20
    H* clear_a8() {
        for (u8* p = jo::view<u8*>(jo::self<jo::Cell>(this)->places_head); p != nullptr;) {
            u8* const next = jo::view<u8*>(jo::as_place(p)->cell_next);
            joan::orig::as_place(this)->unlink_owner(joan::orig::arg::mptr(p));  // 0x0043edb0
            p = next;
        }
        jo::self<jo::Cell>(this)->places_head = nullptr;
        jo::self<jo::Cell>(this)->places_tail = nullptr;
        return this;
    }

    // 0x0054fdb0
    H* clear_34_4() {
        if (jo::self<jo::BoxTree>(this)->item != nullptr) jo::as_cell(jo::self<jo::BoxTree>(this)->item)->ptr_4 = nullptr;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_06", name, addr, fn)
B("World::get_94", 0x0054f460, &H::get_94);
B("World::clear_a0", 0x0054f700, &H::clear_a0);
B("World::clear_a8", 0x0054fb20, &H::clear_a8);
B("World::get_a4", 0x0054fc80, &H::get_a4);
B("Obj::clear_34_4", 0x0054fdb0, &H::clear_34_4);
#undef B

}  // namespace game_b102_06
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
//
//    - World::load_units 0x00558890 ("CreateUnit" then unit links 0x00447600 / 0x00438f80),
//      World::load_places 0x00559ff0 ("CreatePlace" / "CreateFacility").
//
//    - spline point 0x00429a60 (hidden out, p0, p1, p2, t) via 0x0060cc90 / 0x0060cd60 /
//      0x0060cc20; list unlinks 0x0054f580 (+0x3a / +0x3e / +0x42, head +0x9c / +0xa0) and
//      0x0054de20 (+0x46 / +0x4a / +0x4e, head +0x5e / +0x62).
//
//  - 0x0054f580 / 0x0054de20: the "link into new owner" half is dead (new owner is the
//    constant 0) and has no calls - dropped.
namespace {
namespace game_b85_06 {


JOAN_COUNTER(c_load_units, "World::load_units");
JOAN_COUNTER(c_load_places, "World::load_places");
JOAN_COUNTER(c_unlink_54f580, "List54f580::unlink");

struct H {

    // ---- world_s.cpp loaders ------------------------------------------------------------
    std::int32_t load_units(void* reader) {  // 0x00558890
        c_load_units.hit();
        for (;;) {
            const char* const line = joan::orig::arg::as<const char*>(joan::orig::as_file_txt(reader)->get_token(1));  // 0x00589c70
            if (line == nullptr) break;
            if (stricmp_(line, str(d::s_CreateUnit)) == 0) joan::orig::as_world(this)->create_unit(joan::orig::arg::mptr(reader));  // "CreateUnit" 0x00558a60
        }
        std::uint8_t* next = nullptr;
        for (std::uint8_t* u = jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->units_tail); u != nullptr; u = next) {
            next = jo::view<std::uint8_t*>(jo::as_unit(u)->cell_prev);
            if (jo::as_unit(u)->rank >= 3 && jo::as_unit(u)->group_id != 0) joan::orig::as_unit(u)->sync_shadow();  // 0x00447600
        }
        for (std::uint8_t* u = jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->units_tail); u != nullptr; u = next) {
            next = jo::view<std::uint8_t*>(jo::as_unit(u)->cell_prev);
            if (jo::as_unit(u)->rank >= 3 || jo::as_unit(u)->group_id == 0) continue;
            std::uint8_t* next2 = nullptr;
            for (std::uint8_t* v = jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->units_tail); v != nullptr; v = next2) {
                next2 = jo::view<std::uint8_t*>(jo::as_unit(v)->cell_prev);
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(v)->leads()) == 0) continue;  // 0x0044a660
                if (jo::as_unit(v)->group_id != jo::as_unit(u)->group_id) continue;
                joan::orig::as_unit(u)->link_to(static_cast<std::int32_t>(jo::as_unit(v)->id));  // 0x00438f80
            }
        }
        return 1;
    }

    std::int32_t load_places(void* reader) {  // 0x00559ff0
        c_load_places.hit();
        for (;;) {
            const char* const line = joan::orig::arg::as<const char*>(joan::orig::as_file_txt(reader)->get_token(1));  // 0x00589c70
            if (line == nullptr) break;
            if (stricmp_(line, str(d::s_CreatePlace)) == 0) joan::orig::as_world(this)->create_place(joan::orig::arg::mptr(reader));  // "CreatePlace" 0x0055a090
            if (stricmp_(line, str(d::s_CreateFacility)) == 0) joan::orig::as_world(this)->create_facility(joan::orig::arg::mptr(reader));  // "CreateFacility" 0x0055ae40
        }
        return 1;
    }

    // ---- list unlinks (new owner is the constant 0) ------------------------------------------
    void unlink_54f580(std::uint8_t* n) {  // 0x0054f580
        c_unlink_54f580.hit();
        std::uint8_t* const owner = jo::view<std::uint8_t*>(jo::as_world_item(n)->cell);
        if (owner == nullptr) return;
        std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::as_world_item(n)->cell_prev);
        std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_world_item(n)->cell_next);
        if (prev != nullptr) jo::view<std::uint8_t*>(jo::as_world_item(prev)->cell_next) = next;
        else jo::view<std::uint8_t*>(jo::as_cell(owner)->items_head) = next;
        if (next != nullptr) jo::view<std::uint8_t*>(jo::as_world_item(next)->cell_prev) = prev;
        else jo::view<std::uint8_t*>(jo::as_cell(owner)->items_tail) = prev;
        jo::as_world_item(n)->cell_prev = nullptr;
        jo::as_world_item(n)->cell_next = nullptr;
        jo::as_world_item(n)->cell = nullptr;
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("World::load_units", 0x00558890, &H::load_units);
B85("List54f580::unlink", 0x0054f580, &H::unlink_54f580);
B85("World::load_places", 0x00559ff0, &H::load_places);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
//
//      0x0054f890 X::get_9c                (thiscall, ret 0)
//
//      0x00560060 X::flags_54_58           (thiscall, ret 0) - ax = (+0x54 != 0) + 2 (+0x58 != 0),
//                 upper eax = this.
//
//  - Void functions return void (their eax is not a value); narrow returns keep the original's
//    upper eax bits where they are deterministic (0x004d6a10, 0x00560060).
namespace {
namespace game_b93_02 {


struct H {

    // ------------------------------------------------------------------------ 0x0054f890
    jo::WorldItem* get_9c() { return jo::self<jo::Cell>(this)->items_tail; }

    // ------------------------------------------------------------------------ 0x00560060
    u32 flags_54_58() {
        u16 v = 0;
        if (jo::self<jo::Cell>(this)->path_space != nullptr) v = static_cast<u16>(v + 1);
        if (jo::self<jo::Cell>(this)->path_space_b != nullptr) v = static_cast<u16>(v + 2);
        return (joan::call::register_low_bits(this) & 0xffff0000u) | v;  // eax: the upper half of this (callers use ax: unit_stats.cpp, gamedraw.cpp)
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("X::get_9c", 0x0054f890, &H::get_9c);
B93("X::flags_54_58", 0x00560060, &H::flags_54_58);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b105_07.cpp (game/b105_07)
//
//    - getters (ret 0): 0x0054dfe0 dword +0x5e, 0x0054e020 dword +0x62, 0x0054fd40 dword +0xa8,
//
//    - CRT initialiser table entries (0x009f43ec..0x009f4448), cdecl forwarders returning the
//      callee's eax: 0x0052da80 -> 0x0052f900, 0x005436b0 -> 0x005436d0, 0x00543710 ->
//      0x00543730, 0x00543760 -> 0x00543780, 0x005437b0 -> 0x005437d0, 0x00552590 ->
//      0x00553a70, 0x00555e30 -> 0x00555e80, 0x00556280 -> 0x005568b0, 0x00563ec0 ->
//      0x00563ee0, 0x00563f10 -> 0x00563f30 (all direct calls, no thunk).
//
//    0x0054dca0 and the getters 0x0054dfe0 / 0x0054e000 / 0x0054e020 / 0x0054fd40 /
namespace {
namespace game_b105_07 {

using joan::orig::helpers::at;

struct H {

    jo::Place* get_a8() { return jo::self<jo::Cell>(this)->places_head; }
};

// CRT initialiser forwarders (direct calls)
template <fn::Code Target>
u32 JOAN_CDECL forward() {
    return cdecl_call<u32>(Target);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_07", name, addr, fn)
B("Obj::get_a8_54fd40", 0x0054fd40, &H::get_a8);
B("init_552590", 0x00552590, &forward<fn::pi_a3ce20>);
B("init_555e30", 0x00555e30, &forward<fn::init_a3ce24>);
B("init_556280", 0x00556280, &forward<fn::init_a3ce28>);
#undef B

}  // namespace game_b105_07
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
//
//    - BoxNode::collect 0x0054ff30 (3 args, ret 0xc, world_s.cpp): recursive, no outside
//      reference (dead code apart from its own recursion); eax as in the original.
namespace {
namespace game_b108_03 {

using joan::orig::helpers::at;
u32 uptr(const void* p) { return joan::call::register_low_bits(p); }  // dead result: eax of a function nobody reads

struct H {

    // ------------------------------------------------------------------ 0x0054ff30
    u32 box_collect(void* q, i32* n, void** out) {
        if ((joan::orig::arg::as<u32>(joan::orig::as_box(q)->overlaps(static_cast<void*>(jo::self<jo::BoxTree>(this)->box))) & 0xff) == 0) return 0;  // 0x0060d6b0
        if (jo::self<jo::BoxTree>(this)->item != nullptr) {
            out[*n] = jo::self<jo::BoxTree>(this)->item;
            *n = *n + 1;
        }
        u32 eax = uptr(this);
        if (jo::view<void*>(jo::self<jo::BoxTree>(this)->left) != nullptr) eax = joan::orig::arg::as<u32>(joan::orig::as_box_tree(jo::view<void*>(jo::self<jo::BoxTree>(this)->left))->collect_54ff30(joan::orig::arg::mptr(q), joan::orig::arg::mptr(n), joan::orig::arg::mptr(out)));  // 0x0054ff30
        if (jo::view<void*>(jo::self<jo::BoxTree>(this)->right) != nullptr) eax = joan::orig::arg::as<u32>(joan::orig::as_box_tree(jo::view<void*>(jo::self<jo::BoxTree>(this)->right))->collect_54ff30(joan::orig::arg::mptr(q), joan::orig::arg::mptr(n), joan::orig::arg::mptr(out)));  // 0x0054ff30
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("BoxNode::collect", 0x0054ff30, &H::box_collect);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b109_09.cpp (game/b109_09)
//
//    - Building::on_event_54ffd0 0x0054ffd0 (short id), ret 4 -> al = 1: vtable 0x9d3c88 slot
//      0x48 (callers at 0x004ef4e2 / 0x004ef5c6 discard eax). Looks the event up (0x00485f20 on
//      0xa5ccd0) and, for types 0x40 / 0x2000 (sub-type 1), 0x4000 (sub-type 2) and 1 (sub-type
//      1, target kind 0xb), runs the damage stage step: per-stage action byte (+0x96 of the
namespace {
namespace game_b109_09 {

using i8 = std::int8_t;

// Game virtual (thiscall) through the object's vtable.
template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}
using joan::orig::helpers::at;

struct V3 {
    float v[3];
};

JOAN_COUNTER(c_event, "Building::on_event_54ffd0");

void* const kEffects = obj(d::g_particle_effect_array);
void* const kEffectDefs = obj(d::g_pemi_res);
void* const kRecords = obj(d::g_item_res);

// ---------------------------------------------------------------------------------------------
// 0x0054ffd0 helpers (this = building).
void vt18(void* self, i32 v) { vcall<void>(self, 0x18, v); }

// 0x0041b440 effect definition lookup, then 0x0041baf0 spawn (8 args). The arguments after
// the definition are read before the lookup like the original pushes them.
// Emitter k of the record: script emitter_scripts[k], position emitter_pos[3k..], scale emitter_scale[k].
u32 spawn_effect(void* self, u8* rec, i32 k) {
    const u32 a6 = jo::view<u32>(jo::as_item_def(rec)->emitter_scale[k]);
    u8* const def = joan::orig::arg::as<u8*>(joan::orig::as_name_list(kEffectDefs)->entry(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(rec)->emitter_scripts[k]))));  // 0x0041b440
    return joan::orig::arg::as<u32>(joan::orig::as_model_list(kEffects)->add(jo::as_wall(self)->actor->scene, jo::as_pemi_script(def)->effect, &jo::as_item_def(rec)->emitter_pos[k * 3], jo::as_wall(self)->pos, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::i32(a6), 0, 0x1388));  // 0x0041baf0 (def: the 0x0041b440 PemiScript entry)
}

// The stage step shared by the three event kinds. Returns false when the action byte is not
// 0..4 (the original then jumps straight to the common tail).
bool stage_step(void* self, u8* rec) {
    const auto action = [&]() { return static_cast<i32>(jo::view<i8>(jo::as_item_def(rec)->model_kinds[jo::view<i32>(jo::as_wall(self)->busy) - 1])); };  // busy >= 1 here (stage_gate)
    const auto marker = [&]() {
        if (jo::view<i8>(jo::as_wall(self)->active) != 0 && jo::as_wall(self)->actor != nullptr) {
            const u32 colour = global<u32>(d::g_now);
            const char* const m = jo::as_item_def(rec)->anim_names[jo::view<i32>(jo::as_wall(self)->busy)];
            joan::orig::as_actor(jo::view<void*>(jo::as_wall(self)->actor))->start_anim(jo::as_wall(self)->actor->anim, m, joan::orig::arg::u32(colour), 0u, 0u, 0u);  // 0x005abe30
        }
    };
    if (action() == 0) {
        vt18(self, jo::view<i32>(jo::as_wall(self)->busy) - 1);
        marker();
    } else if (action() == 1) {
        if (jo::as_wall(self)->actor != nullptr) {
            jo::as_wall(self)->level_a = jo::view<i32>(jo::as_wall(self)->busy);
            jo::as_wall(self)->actor_a = jo::as_wall(self)->actor;
            jo::as_wall(self)->actor_a->fade_38 = global<u32>(d::g_now);
            jo::view<u32>(jo::as_wall(self)->actor_a->fade) = 0xbe99999au;  // -0.3f
            jo::as_wall(self)->actor = nullptr;
            vt18(self, jo::view<i32>(jo::as_wall(self)->busy) - 1);
            vcall<void>(self, 0x20);
            vcall<void>(self, 0x1c);
        } else {
            vt18(self, jo::view<i32>(jo::as_wall(self)->busy) - 1);
        }
    } else if (action() == 2) {
        vt18(self, jo::view<i32>(jo::as_wall(self)->busy) - 1);
        marker();
    } else if (action() == 3) {
        jo::as_wall(self)->level_b = jo::view<i32>(jo::as_wall(self)->busy) - 1;
        joan::orig::as_unit(self)->attach_model(joan::orig::arg::i32(jo::view<i32>(jo::as_wall(self)->busy) - 1));  // 0x00492e20
        vt18(self, jo::view<i32>(jo::as_wall(self)->busy) - 1);
        vt18(self, jo::view<i32>(jo::as_wall(self)->busy) - 1);
    } else if (action() == 4) {
        vt18(self, jo::view<i32>(jo::as_wall(self)->busy) - 1);
    } else {
        return false;
    }
    if (jo::view<i32>(jo::as_wall(self)->busy) == 0 && jo::view<u32>(jo::as_item_def(rec)->model_count) != 0 && jo::as_wall(self)->u32_c6 == 0 && jo::view<i8>(jo::as_wall(self)->active) != 0 &&
        jo::as_wall(self)->actor != nullptr) {
        if (jo::view<u32>(jo::as_item_def(rec)->emitter_scripts[2]) != 0) spawn_effect(self, rec, 2);
        if (jo::view<u32>(jo::as_item_def(rec)->emitter_scripts[3]) != 0) spawn_effect(self, rec, 3);
    }
    if (jo::view<i32>(jo::as_wall(self)->busy) == 0 && jo::view<u32>(jo::as_item_def(rec)->model_count) != 0 && jo::as_wall(self)->u32_c6 == 0 && jo::view<u32>(jo::as_item_def(rec)->sounds[2]) != 0) {
        V3 pos;
        std::memcpy(&pos, jo::as_wall(self)->pos, sizeof pos);
        joan::orig::as_sound_list(obj(d::g_sfx_player))->play_at(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(rec)->sounds[2])), pos);  // 0x004950a0
    }
    return true;
}

// Stage gate: 0 = closed (stage +0x52 is 0, or the factor is zero / NaN), 1 = +0xc6 above the
// stage limit, 2 = open.
i32 stage_gate(void* self, float f, u8* rec) {
    if (jo::view<i32>(jo::as_wall(self)->busy) == 0) return 0;
    if (!(f < 0.0f || f > 0.0f)) return 0;
    if (jo::view<i32>(jo::as_wall(self)->u32_c6) > jo::as_item_def(rec)->model_values[jo::view<i32>(jo::as_wall(self)->busy) - 1]) return 1;
    return 2;
}

u8* record_of(void* self) {
    return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kRecords)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_wall(self)->type_id))));  // 0x00425ce0
}

// Tower fire / smoke effect into sound_slots[slot].
void tower_effect(void* self, i32 slot, img::Addr name) {
    const u32 a6 = jo::view<u32>(jo::as_wall(self)->yaw_);
    const u32 id = joan::orig::arg::as<u32>(joan::orig::as_name_list(kEffectDefs)->find_4c2060(joan::orig::arg::str(str(name)), static_cast<std::int8_t>(joan::orig::arg::i32(1))));    // 0x004c2060
    u8* const def = joan::orig::arg::as<u8*>(joan::orig::as_name_list(kEffectDefs)->entry(joan::orig::arg::i32(id)));            // 0x0041b440
    const u32 r = joan::orig::arg::as<u32>(joan::orig::as_model_list(kEffects)->add(jo::as_wall(self)->actor->scene, jo::as_pemi_script(def)->effect, obj(d::g_pos), jo::as_wall(self)->pos, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::i32(a6), 0, 0));  // 0x0041baf0 (def: the 0x0041b440 PemiScript entry)
    jo::view<u16>(jo::as_wall(self)->sound_slots[slot]) = static_cast<u16>(r);
    void* const owner = jo::view<void*>(jo::as_wall(self)->actor);
    void* const e = joan::orig::as_dynarray(kEffects)->ptr_at(static_cast<i32>(jo::as_wall(self)->sound_slots[slot]));  // 0x0041bd60
    joan::orig::as_effect(e)->set_owner(joan::orig::arg::mptr(owner), joan::orig::arg::mptr(0));                                                         // 0x0041b0d0
}

struct H {

    // 0x0054ffd0
    u8 on_event_54ffd0(i32 id_arg) {
        c_event.hit();
        void* const self = this;
        const i32 id = static_cast<i16>(id_arg);
        u8* const ev = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_a5ccd0))->at_485f20(joan::orig::arg::u32(id)));  // 0x00485f20 (a Trigger)
        const i32 type = jo::view<i32>(jo::as_trigger(ev)->flags);
        if (type == 0x2000 || type == 0x40) {
            if (jo::view<i8>(jo::self<jo::Wall>(this)->invincible) == 0 && !(jo::view<i32>(jo::as_trigger(ev)->flags) == 0x2000 && global<i32>(d::g_mission) != 4) &&
                jo::view<i32>(jo::as_trigger(ev)->ref_kind) == 1) {
                const float k = joan::orig::arg::as<float>(joan::orig::as_building(self)->take_half(static_cast<i32>(jo::as_trigger(ev)->ref_id), 0));  // 0x00492b10
                u8* const rec = record_of(self);
                const i32 gate = stage_gate(self, k, rec);
                // Above the stage limit the step is skipped but the player-unit check still runs;
                // an action byte outside 0..4 skips both.
                if (gate == 1 || (gate == 2 && stage_step(self, rec))) {
                    if (jo::as_trigger(ev)->ref_id == global<i16>(d::g_a47c36))
                        joan::orig::as_hud(obj(d::g_player_hud))->set_unit_portrait(static_cast<i32>(jo::self<jo::Wall>(this)->portrait_unit));  // 0x006fe340
                }
            }
        } else if (type == 0x4000) {
            if (jo::view<i8>(jo::self<jo::Wall>(this)->invincible) == 0 && jo::view<i32>(jo::as_trigger(ev)->ref_kind) == 2) {
                const float k = joan::orig::arg::as<float>(joan::orig::as_building(self)->take_amount(static_cast<i32>(jo::as_trigger(ev)->ref_id), 0));  // 0x00492bb0
                u8* const rec = record_of(self);
                if (stage_gate(self, k, rec) == 2) stage_step(self, rec);
            }
        } else if (type == 1) {
            if (jo::view<i8>(jo::self<jo::Wall>(this)->invincible) == 0 && jo::view<i32>(jo::as_trigger(ev)->ref_kind) == 1 &&
                (joan::orig::arg::as<u32>(joan::orig::as_unit(self)->can_reach_49a010(joan::orig::arg::u32(static_cast<i32>(jo::as_trigger(ev)->ref_id)), 0u)) & 0xffu) != 0) {  // 0x0049a010
                u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(static_cast<i32>(jo::as_trigger(ev)->ref_id)));  // 0x0041b3c0
                u8* const k = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(o)->type_id))));  // 0x0042d0a0
                if (jo::as_unit_type(k)->kind == 0xb) {
                    const float v = joan::orig::arg::as<float>(joan::orig::as_building(self)->drain_hp(static_cast<i32>(jo::as_trigger(ev)->ref_id), 0));  // 0x00451f20
                    u8* const rec = record_of(self);
                    if (stage_gate(self, v, rec) == 2) stage_step(self, rec);
                }
            }
        }
        // Tower fire / smoke below 50 % health.
        if (global<i32>(d::g_mission) != 4 && jo::view<i32>(jo::self<jo::Wall>(this)->u32_c6) != 0 && jo::view<i32>(jo::self<jo::Wall>(this)->max_hp) != 0 &&
            static_cast<i32>(static_cast<u32>(jo::view<i32>(jo::self<jo::Wall>(this)->u32_c6)) * 100u) / jo::view<i32>(jo::self<jo::Wall>(this)->max_hp) < 0x32) {
            u8* const q = record_of(self);
            if (jo::view<i32>(jo::as_item_def(q)->struction_type) == 1 && jo::self<jo::Wall>(this)->actor != nullptr) {
                if (jo::self<jo::Wall>(this)->sound_slots[0] == 0) tower_effect(self, 0, d::s_towerfire_txt);  // 'towerfire.txt'
                if (jo::self<jo::Wall>(this)->sound_slots[1] == 0) tower_effect(self, 1, d::s_towerfiresmoke_txt);  // 'towerfiresmoke.txt'
            }
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_09", name, addr, fn)
B("Building::on_event_54ffd0", 0x0054ffd0, &H::on_event_54ffd0);
#undef B

}  // namespace game_b109_09
}  // namespace

// ---------------------------------------- from modules/game_b84_02.cpp (game/b84_02)
//
//    - World::merge_animated 0x00560120 (model, node, matrix): finds the scene node holding the
//      matrix translation (0x0060d5f0) when node == 0; creates (or reuses, node +0x3c) the
//      actor (0x005aab20 / 0x005af390 body, mesh 0x005cbaf0 with bounding sphere center /
//      radius, "ANIM\" sequences 0x005adf50 / 0x005ae400) or re-adds the bones (0x005aec90 /
//      0x005c9b10), then appends the model's mesh (materials, vertices transformed by the
//      matrix, triangles) to the actor plane (+0x90) - shared with merge_static.
//
//    - World::merge_static 0x0055fa40 (mesh, node, matrix): same for a static mesh into the
//      node's merged mesh (node +0x40, created with 0x005cbaf0 when missing; 0xffc vertex cap).
//
//    - Occluders::add 0x00560c70: grows the 0x74-byte record array +0x74 (cap +0x7c, count
//      +0x78), zeroes the new record and returns its index; Occluders::get 0x00560d30.
//
//    - lerp 0x005513f0 and 0x0054a8a0 (cdecl, a + (b - a) * t when b > a, else
//      b + (1 - t) * (a - b)); both are called directly (no thunk) - installed by entry patch.
//
//  - 0x00560120 / 0x0055fa40 / 0x00547bf0: the SEH frames only guard operator new + ctor
//    (normal path kept). 0x00560120 / 0x0055fa40 eax return is garbage and unused (only
//    caller 0x004f3df0 ignores it) - void here; 0x00547bf0 returns the first new index.
namespace {
namespace game_b84_02 {


void* const kMemory = obj(d::g_mem);  // allocator (0x0058eb30 realloc / 0x0058ed70 free)
const char* const kWorldSCpp = str(d::s_d_ww_world_s_cpp);  // "d:\ww\world_s.cpp"

using joan::orig::helpers::game_new;  // operator new
using joan::orig::helpers::memset_;
using joan::orig::helpers::memcpy_;
void* mem_realloc(void* p, std::int32_t size, const char* file, std::int32_t line) {
    return joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
}
void mem_free(void* p, const char* file, std::int32_t line) { joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70
using joan::orig::helpers::line_base;

// 0x5a1170-family fixed string (no destructor); 0x194 bytes as in 0x00560120's frame.
struct alignas(4) String {
    std::uint8_t bytes[0x198];
};
const char* c_str(String& s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str()); }  // 0x00417d90

struct Sphere {  // ebp-0x54 .. ebp-0x48 in 0x00560120 (center, radius)
    float x, y, z, r;
};
static_assert(sizeof(Sphere) == 16);

JOAN_COUNTER(c_merge_animated, "World::merge_animated");
JOAN_COUNTER(c_merge_static, "World::merge_static");
JOAN_COUNTER(c_occ_add, "Occluders::add");

std::uint8_t* material(void* mesh, std::int32_t i) { return reinterpret_cast<std::uint8_t*>(&jo::as_plane(mesh)->materials[i]); }

// Finds the world node (this +0x6c[0 .. +0x64)) containing the matrix translation. The nodes are the cell's SceneS
// sectors (Scene* and SectorNode* first, the box at +0x8 on x86; WorldSector::owner points at them): read as
// WorldSector the box sat 8 bytes early on x64, so no static object was merged into its sector there.
void* find_node(void* world, const float* matrix) {
    Vec3 v;  // ebp-0x24 (copied as dwords)
    std::memcpy(&v, matrix + 12, sizeof v);
    for (std::int32_t i = 0; i < jo::as_cell(world)->sector_count; ++i) {
        void* const n = jo::view<void**>(jo::as_cell(world)->sectors)[i];
        if (n == nullptr) continue;
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(jo::view<float[6]>(jo::as_scene_s(n)->desc))->contains(&v)) != 0) return n;  // 0x0060d5f0
    }
    return nullptr;
}

// Common tail of 0x00560120 / 0x0055fa40: appends src's materials (remapped / added), vertices
// (point_transform; normals vector_transform when the format has them) and triangles to dst.
// `remap` / `cap` are the caller's ebp-0x18 / ebp-0x14 (both 0 on entry).
void append_mesh(void* src, void* dst, void* matrix, void*& remap, std::int32_t& cap, std::int32_t line) {
    if (jo::as_plane(src)->material_count > cap) {
        cap = jo::as_plane(src)->material_count + 8;
        remap = mem_realloc(remap, cap << 2, kWorldSCpp, line);
    }
    for (std::int32_t i = 0; i < jo::as_plane(src)->material_count; ++i) {
        jo::as_material(material(src, i))->diffuse = global<std::uint32_t>(d::g_init_white);
        std::int32_t j = 0;
        for (; j < jo::as_plane(dst)->material_count; ++j) {
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_material(material(src, i))->equals(joan::orig::arg::mptr(material(dst, j)))) != 0) break;  // 0x005d5500
        }
        if (j >= jo::as_plane(dst)->material_count) {
            j = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(dst)->add_materials(1));           // 0x005cd470
            joan::orig::as_material(material(dst, j))->assign(joan::orig::arg::mptr(material(src, i)));  // 0x005d53e0
        }
        static_cast<std::int32_t*>(remap)[i] = j;
    }

    const std::int32_t nv = jo::as_plane(src)->vertex_count;                     // ebp-0x3c
    const std::int32_t vbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(dst)->add_vertices(joan::orig::arg::i32(nv)));  // 0x005cd7f0
    std::int32_t stride = 0;                                                 // ebp-0x44
    std::uint8_t* const vdst0 = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(dst)->buffer(&stride));  // 0x005cdae0
    std::uint8_t* const vdst = vdst0 + vbase * stride;
    std::uint8_t* const vsrc = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(src)->buffer(joan::orig::arg::mptr(static_cast<std::int32_t*>(nullptr))));
    memcpy_(vdst, vsrc, static_cast<std::uint32_t>(jo::as_plane(src)->vertex_count * stride));
    {
        std::uint8_t* v = vdst;
        for (std::int32_t k = 0; k < nv; ++k, v += stride) import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(v), matrix);
    }
    if (jo::as_vertex_format(joan::orig::arg::as<std::uint8_t*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(jo::as_plane(dst)->vertex_format)))))->prelit == 0) {  // 0x00600200
        std::uint8_t* v = vdst + jo::as_vertex_format(joan::orig::arg::as<std::uint8_t*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(jo::as_plane(dst)->vertex_format)))))->normal_offset;
        for (std::int32_t k = 0; k < nv; ++k, v += stride) import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(v), matrix);
    }
    jo::view<std::int32_t>(jo::as_plane(dst)->transformed) = 1;

    const std::int32_t nt = jo::as_plane(src)->triangle_count;
    const std::int32_t tbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(dst)->add_triangles(joan::orig::arg::i32(nt)));  // 0x005ce6f0
    std::uint16_t* mat_src = jo::view<std::uint16_t*>(jo::as_plane(src)->triangle_materials);
    std::uint16_t* mat_dst = jo::view<std::uint16_t*>(jo::as_plane(dst)->triangle_materials) + tbase;
    std::uint16_t* idx_src = jo::view<std::uint16_t*>(jo::as_plane(src)->triangles);
    std::uint16_t* idx_dst = jo::view<std::uint16_t*>(jo::as_plane(dst)->triangles) + tbase * 3;
    joan::stream_cache_api::indices_written(jo::view<void*>(jo::as_plane(dst)->triangles));
    for (std::int32_t t = 0; t < nt; ++t, ++mat_src, ++mat_dst) {
        for (std::int32_t k = 0; k < 3; ++k, ++idx_src, ++idx_dst) {
            if (static_cast<std::int32_t>(*idx_src) < jo::as_plane(src)->vertex_count)
                *idx_dst = static_cast<std::uint16_t>(static_cast<std::int32_t>(*idx_src) + vbase);
            else
                *idx_dst = *idx_src;
        }
        *mat_dst = static_cast<std::uint16_t>(static_cast<std::int32_t*>(remap)[*mat_src]);
    }
}

// 0x005513f0 / 0x0054a8a0 lerp (cdecl; fld b; fcomp a; test ah,0x41; jne -> mirrored form).
float JOAN_CDECL lerp_5513f0(float a, float b, float t) {
    if (b > a) return (b - a) * t + a;
    return (1.0f - t) * (a - b) + b;
}

struct H {

    // 0x00560120 World::merge_animated(model, node, matrix), ret 0xc.
    void merge_animated(std::uint8_t* model, std::uint8_t* node, float* matrix) {
        c_merge_animated.hit();
        std::int32_t cap = 0;     // ebp-0x14
        void* remap = nullptr;    // ebp-0x18
        if (node == nullptr) node = static_cast<std::uint8_t*>(find_node(this, matrix));
        if (node == nullptr) return;
        void* actor = nullptr;  // ebp-0x30
        void* const tmpl = jo::view<void*>(jo::as_actor_def(jo::as_model_entry(model)->actor_def)->model->mesh);  // ebp-0x38
        if (jo::view<void*>(jo::as_scene_s(node)->object) != nullptr) {
            actor = jo::view<void*>(jo::as_scene_s(node)->object);
            if (jo::view<std::int8_t>(jo::as_actor(actor)->plane->buffer_mode) != 0 && jo::as_actor(actor)->plane->vertex_count >= 0xccc)
                return;
            if (jo::view<std::int8_t>(jo::as_actor(actor)->plane->buffer_mode) != 0 && jo::as_actor(actor)->plane->triangle_count * 3 >= 0x3333)
                return;
            for (std::int32_t i = 0; i < jo::as_model_entry(model)->morph_count; ++i) {
                void* const r = joan::orig::arg::as<void*>(joan::orig::as_actor(actor)->find_morph(joan::orig::arg::str(jo::as_model_entry(model)->morphs[i].name)));  // 0x005aec90
                joan::orig::as_morph(r)->add(joan::orig::arg::str(jo::view<void*>(jo::as_model_entry(model)->dir)), joan::orig::arg::str(jo::as_model_entry(model)->morphs[i].file), joan::orig::arg::mptr(static_cast<void*>(matrix)));  // 0x005c9b10
            }
        } else {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
            void* const mesh =  // ebp-0x44
                mem != nullptr ? joan::orig::as_plane(mem)->construct(joan::orig::arg::u32(jo::as_plane(tmpl)->vertex_format), joan::orig::arg::u32(static_cast<std::int8_t>(1)))
                               : nullptr;  // 0x005cbaf0
            joan::orig::as_plane(mesh)->init(0, 0, 0, 1u);  // 0x005cc7a0
            Sphere c;                                    // ebp-0x54
            c.x = (jo::view<float[6]>(jo::as_scene_s(node)->desc)[0] + jo::view<float[6]>(jo::as_scene_s(node)->desc)[3]) * 0.5f;
            c.y = (jo::view<float[6]>(jo::as_scene_s(node)->desc)[1] + jo::view<float[6]>(jo::as_scene_s(node)->desc)[4]) * 0.5f;
            c.z = (jo::view<float[6]>(jo::as_scene_s(node)->desc)[2] + jo::view<float[6]>(jo::as_scene_s(node)->desc)[5]) * 0.5f;
            Vec3 tmp;  // ebp-0x254
            Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(jo::view<float[6]>(jo::as_scene_s(node)->desc)), static_cast<void*>(&c)));  // 0x0060cc90
            c.r = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &d)) * 0.5f;  // length
            void* const amem = game_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // 0x10c on x86
            actor = amem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(amem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
            void* const bmem = game_new(static_cast<std::uint32_t>(sizeof(jo::ActorDef)));  // 0x2d8 on x86
            void* const body = bmem != nullptr ? joan::orig::as_actor_def(bmem)->construct() : nullptr;  // 0x005af390
            jo::view<void*>(jo::as_actor(actor)->def) = body;
            joan::orig::as_actor_def(jo::view<void*>(jo::as_actor(actor)->def))->create(static_cast<jo::Plane*>(mesh), static_cast<void*>(&c));  // 0x005b02f0
            joan::orig::as_scene_object(actor)->init();                                             // 0x005aab70
            joan::orig::as_actor(actor)->set_position_5ac4b0(0u, 0u, 0u);                                    // 0x005ac4b0
            String path;  // ebp-0x1e8
            joan::orig::as_string(&path)->construct(joan::orig::arg::str(jo::view<void*>(jo::as_model_entry(model)->dir)));  // 0x005a11a0
            joan::orig::as_string(&path)->append(str(d::s_ANIM));             // 0x005a1550 "ANIM\"
            for (std::int32_t i = 0; i < jo::as_model_entry(model)->morph_count; ++i) {
                joan::orig::as_actor(actor)->add_part(joan::orig::arg::str(jo::as_model_entry(model)->morphs[i].name), joan::orig::arg::str(jo::view<void*>(jo::as_model_entry(model)->dir)), joan::orig::arg::str(jo::as_model_entry(model)->morphs[i].file), static_cast<void*>(matrix));  // 0x005adf50
                if (strlen_(static_cast<const char*>(jo::as_model_entry(model)->morphs[i].anim)) != 0) {
                    char* const seq = jo::as_model_entry(model)->morphs[i].anim;
                    const char* const dir = c_str(path);
                    joan::orig::as_actor(actor)->attach(joan::orig::arg::str(jo::as_model_entry(model)->morphs[i].name), dir, joan::orig::arg::str(seq));  // 0x005ae400
                }
            }
            jo::view<void*>(jo::as_scene_s(node)->object) = actor;
        }

        void* const plane = jo::view<void*>(jo::as_actor(actor)->plane);  // ebp-0x34
        append_mesh(tmpl, plane, matrix, remap, cap, line_base(d::g_a0b7ac) + 0x7d);
        if (plane != nullptr) {
            joan::orig::as_plane(plane)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
            joan::orig::as_plane(plane)->sort_by_material(1);   // 0x005cf1d0
            jo::view<void*>(jo::as_scene_s(node)->object) = actor;
        }
        if (remap != nullptr) {
            mem_free(remap, kWorldSCpp, line_base(d::g_a0b7ac) + 0xde);
            remap = nullptr;
        }
        cap = 0;
    }

    // 0x0055fa40 World::merge_static(mesh, node, matrix), ret 0xc.
    void merge_static(std::uint8_t* src, std::uint8_t* node, float* matrix) {
        c_merge_static.hit();
        std::int32_t cap = 0;   // ebp-0x14
        void* remap = nullptr;  // ebp-0x18
        if (node == nullptr) node = static_cast<std::uint8_t*>(find_node(this, matrix));
        if (node == nullptr) return;
        void* dst = nullptr;  // ebp-0x30
        if (jo::view<void*>(jo::as_scene_s(node)->plane2) != nullptr) {
            dst = jo::view<void*>(jo::as_scene_s(node)->plane2);
        } else {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
            dst = mem != nullptr ? joan::orig::as_plane(mem)->construct(joan::orig::arg::u32(jo::as_plane(src)->vertex_format), joan::orig::arg::u32(static_cast<std::int8_t>(0)))
                                 : nullptr;  // 0x005cbaf0
            joan::orig::as_plane(dst)->init(0, 0, 0, 1u);  // 0x005cc7a0
            jo::as_plane(dst)->list_kind = 0;
            joan::orig::as_plane(dst)->set_owner(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x005d03f0
            joan::orig::as_plane(dst)->set_10c(1u);                         // 0x005e4320
        }
        if (jo::as_plane(dst)->vertex_count > 0xffc) return;
        append_mesh(src, dst, matrix, remap, cap, line_base(d::g_a0b7a8) + 0x55);
        if (dst != nullptr) {
            joan::orig::as_plane(dst)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
            joan::orig::as_plane(dst)->sort_by_material(1);   // 0x005cf1d0
            jo::view<void*>(jo::as_scene_s(node)->plane2) = dst;
        }
        if (remap != nullptr) {
            mem_free(remap, kWorldSCpp, line_base(d::g_a0b7a8) + 0xb6);
            remap = nullptr;
        }
        cap = 0;
    }

    // 0x00560c70 Occluders::add() -> index of the new zeroed 0x74-byte record.
    std::int32_t occ_add() {
        c_occ_add.hit();
        if (jo::self<jo::Cell>(this)->occluder_cap == jo::self<jo::Cell>(this)->occluder_count) {
            jo::self<jo::Cell>(this)->occluder_cap = jo::self<jo::Cell>(this)->occluder_cap + 8;
            jo::view<void*>(jo::self<jo::Cell>(this)->occluders) = mem_realloc(jo::view<void*>(jo::self<jo::Cell>(this)->occluders), jo::self<jo::Cell>(this)->occluder_cap * static_cast<std::int32_t>(sizeof(jo::Occluder)), kWorldSCpp, line_base(d::g_a0b7b0) + 4);
        }
        memset_(jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->occluders) + jo::self<jo::Cell>(this)->occluder_count * sizeof(jo::Occluder), 0, sizeof(jo::Occluder));
        const std::int32_t r = jo::self<jo::Cell>(this)->occluder_count;
        jo::self<jo::Cell>(this)->occluder_count = jo::self<jo::Cell>(this)->occluder_count + 1;
        return r;
    }
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("World::merge_animated", 0x00560120, &H::merge_animated);
B84("World::merge_static", 0x0055fa40, &H::merge_static);
B84("Occluders::add", 0x00560c70, &H::occ_add);
B84("lerp_5513f0", 0x005513f0, &lerp_5513f0);
#undef B84

}  // namespace game_b84_02
}  // namespace

// ---------------------------------------- from modules/game_b104_07.cpp (game/b104_07)
//
//    - static initialisers (cdecl, void, reached from the CRT initialiser wrappers whose
//      results _initterm ignores; the original eax is returned anyway):
//      0x00555e80 [0xa3ce24] = [0xa3ce20] / 2.0f, 0x005568b0 [0xa3ce28] = [0xa3ce20] / 3.0f,
//
//  - 0x00555e80 / 0x005568b0 run at static-init time (PC53): float / float rounded to double
//    then float is innocuous double rounding (53 >= 2 * 24 + 2), so C++ float division is
//    exact.
namespace {
namespace game_b104_07 {

// 0x00555e80 / 0x005568b0
void JOAN_CDECL init_a3ce24() { global<float>(d::g_a3ce24) = global<float>(d::g_a3ce20) / global<float>(d::flt_2); }
void JOAN_CDECL init_a3ce28() { global<float>(d::g_a3ce28) = global<float>(d::g_a3ce20) / global<float>(d::flt_3); }

#define B(name, addr, fn) JOAN_REPLACE("game/b104_07", name, addr, fn)
B("init_a3ce24", 0x00555e80, &init_a3ce24);
B("init_a3ce28", 0x005568b0, &init_a3ce28);
#undef B

}  // namespace game_b104_07
}  // namespace

// ---------------------------------------- from modules/game_b107_12.cpp (game/b107_12)
//
//    - StaticInit::init_* (cdecl, ret 0) - CRT initialiser-table entries (no direct callers):
//      one or two direct calls (no args), eax of the last one:
//        0x0051adb0 (0x0051b7e0), 0x00556df0 (0x005570b0),
//        0x00564400 (0x00564420, 0x00564440), 0x00564e70 (0x00564e90, 0x00564ec0),
//        0x00565890 (0x005658b0, 0x005658e0), 0x005662b0 (0x005662d0, 0x00566300),
//        0x00566d60 (0x00566d80, 0x00566db0), 0x00567790 (0x005677c0, 0x005677e0),
//        0x00567c20 (0x00567c40, 0x00567c70), 0x00568640 (0x00568660, 0x00568690),
//        0x005690f0 (0x00569110, 0x00569140), 0x00569ba0 (0x00569bc0, 0x00569bf0),
//        0x0056a650 (0x0056a670, 0x0056a690), 0x0056b0d0 (0x0056b0f0, 0x0056b110).
//      (0x0051adb0 / 0x00556df0 are listed as ~2.6K / 700 bytes, but the code is 21 bytes; the
//      rest is int3 padding.)
namespace {
namespace game_b107_12 {

// Direct call (cdecl, no args), its eax.
template <fn::Code A>
u32 JOAN_CDECL init_one() {
    return cdecl_call<u32>(A);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_12", name, addr, fn)
B("StaticInit::init_556df0", 0x00556df0, (&init_one<fn::init_a3ce2c>));
#undef B

}  // namespace game_b107_12
}  // namespace

// ---------------------------------------- from modules/game_b106_17.cpp (game/b106_17)
//
//    - init_a3ce2c 0x005570b0 (world_s.cpp, cdecl, void; called only by the CRT initialiser
//
//  - 0x005570b0 runs at static-init time (PC53): float / float rounded to double then to
//    float is innocuous double rounding, so the C++ float division is exact.
namespace {
namespace game_b106_17 {

// 0x005570b0
void JOAN_CDECL init_a3ce2c() { global<float>(d::g_a3ce2c) = global<float>(d::g_a3ce20) / global<float>(d::flt_4); }

#define B(name, addr, fn) JOAN_REPLACE("game/b106_17", name, addr, fn)
B("init_a3ce2c", 0x005570b0, &init_a3ce2c);
#undef B

}  // namespace game_b106_17
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
//
//    - Obj::forward_558a30 0x00558a30 (a, ret 4): returns 0x00558890(a).
namespace {
namespace game_b96_04 {

struct H {
    // ------------------------------------------------------------------------ 0x00558a30
    u32 forward_558a30(void* a) { return joan::orig::arg::as<u32>(joan::orig::as_world(this)->load_units(a)); }  // 0x00558890
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("Obj::forward_558a30", 0x00558a30, &H::forward_558a30);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b86_03.cpp (game/b86_03)
//
//      0x00558a60 World::create_unit(reader) - thiscall, ret 4: the "CreateUnit" block of a
//                 world script (world_s.cpp): unit type name / short name, position (+ this
//                 +0x44 / +0x48 offsets), angle (optionally in degrees, "HD"), speed, side
//                 name, level word +0x31, then TASK / Range / Treasure / TreasureLevel /
//                 UnitLevel keys up to "}"; creates the unit and applies task, treasure, level.
//
// 0x00558a60 stack layout: after each 64-byte reader string the original strips the last
namespace {
namespace game_b86_03 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_create_unit, "World::create_unit");

// ---------------------------------------------------------------------------- 0x00558a60
// Emulated stack frame: [ebp - off] is at b + N - off.
template <std::int32_t N>
struct Frame {
    alignas(8) std::uint8_t b[N];
    char* ebp(std::int32_t off) { return reinterpret_cast<char*>(b + N - off); }
};

std::int32_t word_arg(std::int32_t v) { return static_cast<std::int32_t>(static_cast<std::int16_t>(v)); }

struct H {
    std::int32_t create_unit(void* reader) {
        c_create_unit.hit();
        Frame<0x2c0> fr;
        std::memset(fr.b, 0, sizeof fr.b);
        char* const b108 = fr.ebp(0x108);  // 0x100 bytes
        char* const b148 = fr.ebp(0x148);
        char* const b188 = fr.ebp(0x188);
        char* const b1c8 = fr.ebp(0x1c8);
        char* const b24c = fr.ebp(0x24c);
        char* const b28c = fr.ebp(0x28c);
        void* const task_rec = fr.ebp(0x2a8);  // 0x1c bytes
        std::int32_t& unit_level = at<std::int32_t>(fr.ebp(0x20c), 0);
        char* tok;  // ebp-0x8

        tok = token(reader);
        tok = token(reader);
        read_str(reader, b148, 0x40);
        chop(b148);
        tok = token(reader);
        read_str(reader, b148, 0x40);
        chop(b148);
        b188[0] = 0;
        tok = token(reader);
        if (stricmp_(tok, str(d::s_UnitShortName)) == 0) {  // "UnitShortName"
            read_str(reader, b188, 0x40);
            chop(b188);
            tok = token(reader);
        }
        read_str(reader, b1c8, 0x40);
        chop(b1c8);
        const std::int32_t type = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(static_cast<const char*>(b1c8))));  // 0x005270b0
        const std::int32_t handle = joan::orig::arg::as<std::int32_t>(joan::orig::as_units(obj(d::g_units))->create_51dfc0(joan::orig::arg::i32(type)));                        // 0x0051dfc0
        std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(handle)));  // 0x0041b3c0
        void* const def = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(word_arg(type))));  // 0x0042d0a0
        joan::orig::as_unit(unit)->set_level_state(static_cast<std::int32_t>(jo::as_unit_type(def)->rank));                           // 0x005089e0
        jo::as_unit(unit)->layer = 1;
        strcpy_(jo::as_unit(unit)->name_, b148);  // +0x3d on x86
        if (strlen_(b188) > 0) strcpy_(jo::as_unit(unit)->name_b, b188);  // +0x7f on x86
        joan::orig::as_unit(unit)->set_group(jo::self<jo::Cell>(this)->world);  // 0x004fed00
        read_str(reader, b108, 0xff);

        tok = token(reader);
        tok = token(reader);
        const float x = static_cast<float>(kFpu.add(atof_(tok), static_cast<double>(jo::self<jo::Cell>(this)->origin_x)));  // ebp-0x1e0
        tok = token(reader);
        tok = token(reader);
        const float y = static_cast<float>(kFpu.add(atof_(tok), static_cast<double>(jo::self<jo::Cell>(this)->origin_y)));  // ebp-0x1dc
        tok = token(reader);
        tok = token(reader);
        const float z = static_cast<float>(atof_(tok));  // ebp-0x1d8
        joan::orig::as_ground_object(unit)->set_position(Vec3{x, y, z});        // 0x004fedf0

        double scale = 1.0;  // ebp-0x1ec
        tok = token(reader);
        if (stricmp_(tok, str(d::s_HD)) == 0) scale = 0.017453292519943295;  // qword 0x3f91df46a2529d39
        tok = token(reader);
        const float angle = static_cast<float>(kFpu.mul(atof_(tok), scale));  // ebp-0x1e4
        joan::orig::as_unit(unit)->set_target(joan::orig::arg::f32(angle));                                     // 0x004ff870
        joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(static_cast<float>(atof_(tok))));            // 0x0050abd0
        read_str(reader, b108, 0xff);
        read_str(reader, b108, 0xff);
        read_str(reader, b108, 0xff);

        tok = token(reader);
        read_str(reader, b148, 0x40);
        chop(b148);
        const std::int32_t side_name = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(joan::orig::arg::str(static_cast<const char*>(b148))));  // 0x004b91a0
        const std::int32_t side = joan::orig::arg::as<std::int32_t>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(side_name)));                           // 0x004b67e0
        joan::orig::as_unit(unit)->set_side(joan::orig::arg::i32(word_arg(side)));                                                                          // 0x00509250
        tok = token(reader);
        tok = token(reader);
        jo::as_unit(unit)->group_id = static_cast<std::int16_t>(atoi_(tok));

        std::int32_t task = 0;            // ebp-0x1f8
        std::int32_t range = 0;           // ebp-0x1fc
        std::int32_t treasure = 0;        // ebp-0x200
        std::int32_t treasure_name = 0;   // ebp-0x204
        std::int32_t treasure_level = 0;  // ebp-0x208
        unit_level = 1;                   // ebp-0x20c (in the frame)
        tok = token(reader);
        while (stricmp_(tok, str(d::s_rbrace)) != 0) {  // "}"
            if (stricmp_(tok, str(d::s_TASK)) == 0) {  // "TASK"
                read_str(reader, b24c, 0x40);
                chop(b24c);
                task = joan::orig::arg::as<std::int32_t>(joan::orig::ai_task_from_name(joan::orig::arg::str(static_cast<const char*>(b24c))));  // 0x004e4610
            } else if (stricmp_(tok, str(d::s_Range)) == 0) {  // "Range"
                tok = token(reader);
                range = atoi_(tok);
            } else if (stricmp_(tok, str(d::s_Treasure)) == 0) {  // "Treasure"
                read_str(reader, b28c, 0x40);
                chop(b28c);
                treasure = joan::orig::arg::as<std::int32_t>(joan::orig::as_names(obj(d::g_randList))->find_463ed0(joan::orig::arg::str(static_cast<const char*>(b28c))));  // 0x00463ed0
                if (treasure == 0)
                    treasure_name = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(joan::orig::arg::str(static_cast<const char*>(b28c))));  // 0x004a36c0
            } else if (stricmp_(tok, str(d::s_TreasureLevel)) == 0) {  // "TreasureLevel"
                treasure_level = atoi_(token(reader));        // not stored to ebp-0x8
            } else if (stricmp_(tok, str(d::s_UnitLevel)) == 0) {  // "UnitLevel"
                tok = token(reader);
                unit_level = atoi_(tok);
            }
            tok = token(reader);
        }

        if (task != 0) {
            if (task == 8) {
                joan::orig::as_unit_task(task_rec)->set(joan::orig::arg::u32(task), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<std::int8_t>(1))), joan::orig::arg::u32(range), 0u, 0u, 0u);  // 0x004e4520
            } else {
                const std::int32_t iy = ftol(y);
                const std::int32_t ix = ftol(x);
                joan::orig::as_unit_task(task_rec)->set(joan::orig::arg::u32(task), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<std::int8_t>(1))), joan::orig::arg::u32(ix), joan::orig::arg::u32(iy), joan::orig::arg::u32(range), 0u);
            }
            joan::orig::as_unit(unit)->set_task(joan::orig::arg::mptr(task_rec));  // 0x00527fb0
        }
        if (treasure != 0) {
            // The original pushes +0x1c7 / +0x1c5 (arguments of 0x00464030) before the word
            // argument of 0x00464100 (ret 4), then calls 0x00464030 (ret 8) on its result.
            void* const t = joan::orig::arg::as<void*>(joan::orig::as_names(obj(d::g_randList))->at(joan::orig::arg::i16(word_arg(treasure))));  // 0x00464100
            joan::orig::as_grass(t)->pick(joan::orig::arg::mptr(&jo::as_unit(unit)->drop_item), joan::orig::arg::mptr(&jo::as_unit(unit)->treasure_level));                                // 0x00464030
        } else if (treasure_name != 0) {
            jo::as_unit(unit)->drop_item = static_cast<std::int16_t>(treasure_name);
            if (treasure_level == -1) {
                void* const rec = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(word_arg(treasure_name))));  // 0x00425ce0
                jo::as_unit(unit)->treasure_level = jo::as_item_def(rec)->min_level;
            } else {
                jo::as_unit(unit)->treasure_level = treasure_level;
            }
        }
        joan::orig::as_unit(unit)->set_level_523190(joan::orig::arg::i32(unit_level));  // 0x00523190
        if (jo::as_unit(unit)->rank < 4) joan::orig::as_unit(unit)->distribute_levels();  // 0x004a95a0
        const std::int32_t v = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hit_points_max());   // 0x00523320
        joan::orig::as_unit(unit)->set_value_135(joan::orig::arg::i32(v));                                      // 0x004fe510
        return 1;
    }
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_03", name, addr, fn)
B86("World::create_unit", 0x00558a60, &H::create_unit);
#undef B86

}  // namespace game_b86_03
}  // namespace

// ---------------------------------------- from modules/game_b111_18.cpp (fix/uninit)
//
//    - WorldShape::read_corners 0x00559a90 (thiscall(Reader*), ret 4; no code or data reference:
//      dead code): reads four "(%f, %f, %f)" corner lines with sscanf (0x007b3ec2) into the
//      stack floats [ebp-0x114..-0x10c], offsets x / y by the ints +0x44 / +0x48, stores them
//      with 0x00530ee0 (0, 1, 3, 2), skips 6 lines, then the plane 0x0060d920 and the four edge
//      vectors +0x40..+0x64 (0x0060cc90). UNINIT READ: the three floats are never initialised;
//      when the first line does not match, the stored corner is int + stack garbage. We use
//      0.0f (later mismatches keep the previous line's values, as in the original).
namespace {
namespace game_b111_18 {


struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_corners, "WorldShape::read_corners_559a90");

struct H {
    // ------------------------------------------------------------ 0x00559a90
    i32 read_corners(void* reader) {
        c_corners.hit();
        using Scan = int(JOAN_CDECL*)(const char*, const char*, ...);
        const Scan sscanf_ = reinterpret_cast<Scan>(static_cast<std::uintptr_t>(fn::code_value(fn::crt_sscanf)));
        const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_world_cell(this)->add_occluder());        // 0x00560c70
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(this)->occluder(joan::orig::arg::i32(idx)));     // 0x00560d30
        joan::orig::as_occluder(p)->set_enabled();                                // 0x00530eb0
        joan::orig::as_file_txt(reader)->get_token(1);             // 0x00589c70 (result unused)
        char buf[0x108];
        // fix/uninit: [ebp-0x114..-0x10c] are never initialised in the original; 0.0f here.
        V3 c{0.0f, 0.0f, 0.0f};
        const Fpu fpu{true};
        const img::Addr fmts[4] = {d::s_f_f_f_9d83c4, d::s_f_f_f_9d83b4, d::s_f_f_f_9d83b4, d::s_f_f_f_9d83a0};
        const i32 slots[4] = {0, 1, 3, 2};
        for (int k = 0; k < 4; ++k) {
            joan::orig::as_file_txt(reader)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), 0xff);  // 0x00589f10 read_line
            sscanf_(buf, str(fmts[k]), &c.x, &c.y, &c.z);
            c.x = static_cast<float>(fpu.add(static_cast<double>(jo::self<jo::WorldCell>(this)->origin_x), c.x));
            c.y = static_cast<float>(fpu.add(static_cast<double>(jo::self<jo::WorldCell>(this)->origin_y), c.y));
            joan::orig::as_occluder(p)->set_corner(joan::orig::arg::i32(slots[k]), &c);  // 0x00530ee0
        }
        for (int k = 0; k < 6; ++k) joan::orig::as_file_txt(reader)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), 0xff);  // 0x00589f10
        joan::orig::plane_from_points(joan::orig::arg::mptr(p + 0x30), joan::orig::arg::ptr(p), joan::orig::arg::ptr(p + 0xc), joan::orig::arg::ptr(p + 0x18));         // 0x0060d920
        V3 tmp;
        V3 e = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(p + 0xc), joan::orig::arg::ptr(p)));  // 0x0060cc90
        std::memcpy(p + 0x40, &e, sizeof e);
        e = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(p + 0x18), joan::orig::arg::ptr(p + 0xc)));
        std::memcpy(p + 0x4c, &e, sizeof e);
        e = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(p + 0x24), joan::orig::arg::ptr(p + 0x18)));
        std::memcpy(p + 0x58, &e, sizeof e);
        e = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(p), joan::orig::arg::ptr(p + 0x24)));
        std::memcpy(p + 0x64, &e, sizeof e);
        return 1;
    }
};

// fix/uninit: sscanf targets [ebp-0x114..-0x10c] never initialised (first line not matching);
// 0.0f used. Dead code (no references).
JOAN_REPLACE("fix/uninit", "WorldShape::read_corners", 0x00559a90, &H::read_corners);

}  // namespace game_b111_18
}  // namespace

// ---------------------------------------- from modules/game_b86_02.cpp (game/b86_02)
//
//    - World::create_place 0x0055a090 (reader): skips the header token, reads the place kind
//      line (unused), the name (0x40) and the place type (VILLAGE 1 / TOWN 2 / CASTLE 3 /
//      UMBRELLA 4), the position (+0x44 / +0x48 offset), the angle ("HD" -> radians), the
//      size, the entrances (until "ENTRANCE_END;", 8 slots), the parking places (until
//      "PARKING_END;", 13 slots, each with a direction, "PARKING_DIR" -> radians) and the
//      nation ("NATION" n / "NATION_ID" id / "Kingdom" name -> 0xa57f80 lookup); builds the
//      bounding box (absolute with |cos| / |sin| extents when the angle is 0, else relative
//      +-size/2, z -64 .. size+64), allocates a place in 0xa484d0 (0x004c5370 / 0x00456c70)
//      and initialises it (0x00433ea0, 0x004366f0, 0x0042b350, 0x004233c0, 0x004c22b0,
//      0x00438420, 0x0044f5c0).
//
//    - World::create_facility 0x0055ae40 (reader): facility class (0x0046d820), facility
//      type (0xa484d0 0x004c54a0), name, position, angle, size, entrances (until
//      "ENTRANCE_END;", 2 slots), absolute box built with cos(0) / sin(0); creates the facility
//      through 0xa5d100 (0x0046ce70 / 0x0043c450), vtable +4 (class, name), attaches it to the
//      type (0x004c39d0), 0x0046d6b0 / 0x0046d720 / 0x0046d750, vtable +0xc (reader).
//
//  - Both readers keep the original frame layout from the first array up to the token
//    pointer in one byte block (fr): name[strlen - 1] = 0 on an empty line writes the byte in
//    front of the buffer (0x0055ae40: the high byte of the facility class [ebp-0x14c], which
//    is used later; 0x0055a090: a dead dword), the 0xff-line strip writes name[0x3f], the
//    0x40-line read writes its NUL into line[0], and too many entrances / parking places
//    overwrite the counters exactly as in the original (after which both write wild).
namespace {
namespace game_b86_02 {

using joan::orig::helpers::at;

// Stream helpers (FileTxt).
const char* get_token(void* s) { return joan::orig::arg::as<const char*>(joan::orig::as_file_txt(s)->get_token(1)); }  // 0x00589c70
double cos_(double v) { return joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(v))); }
double sin_(double v) { return joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(v))); }
double fabs_(double v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(v))); }
// buf[strlen(buf) - 1] = 0 (writes buf[-1] on an empty line).
void trim_last(char* buf) { buf[static_cast<std::int32_t>(strlen_(buf)) - 1] = 0; }

constexpr std::uint64_t kDegBits = 0x3f91df46a2529d39ull;  // pi / 180
double deg_factor() {
    double d;
    std::memcpy(&d, &kDegBits, 8);
    return d;
}
float half() { return global<float>(d::flt_2); }    // 2.0f divisor
float margin() { return global<float>(d::flt_64); }  // 64.0f

// x87 "equal or unordered" (fcomp; test ah,0x40; je skips the equal branch).
bool eq_or_nan(float a, float b) { return !(a < b) && !(a > b); }

static_assert(sizeof(Vec3) == 12);
static_assert(sizeof(Box) == 0x18);

// Absolute box: centre +- (|cos(a)| * sx / 2 + |sin(a)| * sy / 2, |sin(a)| * sx / 2 + |cos(a)| *
// sy / 2), z - 64 .. z + sz + 64, x87 order of 0x0055a710.. / 0x0055b1c4...
void absolute_box(const joan::hook::x87::Fpu& fpu, Box& box, const Vec3& pos, double angle, float sx, float sy,
                  float sz) {
    // a.x
    box.v[0] = pos.x;
    box.v[1] = pos.y;
    box.v[2] = pos.z;
    {
        const double base = box.v[0];
        const double q1 = static_cast<double>(sx / half());
        const double m1 = fpu.mul(fabs_(cos_(angle)), q1);
        const double q2 = static_cast<double>(sy / half());
        const double m2 = fpu.mul(fabs_(sin_(angle)), q2);
        box.v[0] = static_cast<float>(fpu.sub(base, fpu.add(m2, m1)));
    }
    {
        const double base = box.v[1];
        const double q1 = static_cast<double>(sy / half());
        const double m1 = fpu.mul(fabs_(cos_(angle)), q1);
        const double q2 = static_cast<double>(sx / half());
        const double m2 = fpu.mul(fabs_(sin_(angle)), q2);
        box.v[1] = static_cast<float>(fpu.sub(base, fpu.add(m2, m1)));
    }
    box.v[2] = box.v[2] - margin();
    // b
    box.v[3] = pos.x;
    box.v[4] = pos.y;
    box.v[5] = pos.z;
    {
        const double base = box.v[3];
        const double q1 = static_cast<double>(sx / half());
        const double m1 = fpu.mul(fabs_(cos_(angle)), q1);
        const double q2 = static_cast<double>(sy / half());
        const double m2 = fpu.mul(fabs_(sin_(angle)), q2);
        box.v[3] = static_cast<float>(fpu.add(fpu.add(m2, m1), base));
    }
    {
        const double base = box.v[4];
        const double q1 = static_cast<double>(sy / half());
        const double m1 = fpu.mul(fabs_(cos_(angle)), q1);
        const double q2 = static_cast<double>(sx / half());
        const double m2 = fpu.mul(fabs_(sin_(angle)), q2);
        box.v[4] = static_cast<float>(fpu.add(fpu.add(m2, m1), base));
    }
    box.v[5] = (sz + margin()) + box.v[5];
}

JOAN_COUNTER(c_place, "World::create_place");
JOAN_COUNTER(c_facility, "World::create_facility");

struct H {

    // ---- 0x0055a090 World::create_place(reader) ----------------------------------------------
    std::int32_t create_place(void* reader) {
        c_place.hit();
        const joan::hook::x87::Fpu fpu{true};
        // [ebp-0x2ac] .. [ebp-0x9] of the original frame.
        alignas(8) std::uint8_t fr[0x2a4] = {};
        auto V = [&fr](std::int32_t ebp_off) -> std::uint8_t* { return fr + (0x2ac - ebp_off); };
        char* const name = reinterpret_cast<char*>(V(0x148));  // 0x40
        char* const line = reinterpret_cast<char*>(V(0x108));  // 0x100
        std::uint8_t& type = at<std::uint8_t>(V(0x150), 0);
        float& px = at<float>(V(0x15c), 0);
        float& py = at<float>(V(0x158), 0);
        float& pz = at<float>(V(0x154), 0);
        float& angle = at<float>(V(0x160), 0);
        double& factor = at<double>(V(0x168), 0);
        float& sx = at<float>(V(0x174), 0);
        float& sy = at<float>(V(0x170), 0);
        float& sz = at<float>(V(0x16c), 0);
        std::int32_t& n_ent = at<std::int32_t>(V(0x178), 0);
        std::int32_t& n_park = at<std::int32_t>(V(0x1dc), 0);
        auto ent = [&](std::int32_t i) { return reinterpret_cast<float*>(V(0x1d8) + i * 0xc); };
        auto park = [&](std::int32_t i) { return reinterpret_cast<float*>(V(0x278) + i * 0xc); };
        auto dir = [&](std::int32_t i) { return reinterpret_cast<float*>(V(0x2ac) + i * 4); };

        get_token(reader);
        std::memset(V(0x14c), 0, 4);  // [ebp-0x14c] = 0 in the local frame buffer (not a game object)
        get_token(reader);
        read_line(reader, line, 0xff);
        trim_last(line);
        get_token(reader);
        read_line(reader, name, 0x40);
        trim_last(name);
        type = 0;
        get_token(reader);
        read_line(reader, line, 0xff);
        trim_last(line);
        if (stricmp_(line, str(d::s_VILLAGE)) == 0) {  // "VILLAGE"
            type = 1;
        } else if (stricmp_(line, str(d::s_TOWN)) == 0) {  // "TOWN"
            type = 2;
        } else if (stricmp_(line, str(d::s_CASTLE)) == 0) {  // "CASTLE"
            type = 3;
        } else if (stricmp_(line, str(d::s_UMBRELLA)) == 0) {  // "UMBRELLA"
            type = 4;
        }
        get_token(reader);
        px = static_cast<float>(fpu.add(atof_(get_token(reader)), static_cast<double>(jo::self<jo::Cell>(this)->origin_x)));
        get_token(reader);
        py = static_cast<float>(fpu.add(atof_(get_token(reader)), static_cast<double>(jo::self<jo::Cell>(this)->origin_y)));
        get_token(reader);
        pz = static_cast<float>(atof_(get_token(reader)));
        factor = 1.0;
        if (stricmp_(get_token(reader), str(d::s_HD)) == 0) factor = deg_factor();  // "HD"
        angle = static_cast<float>(fpu.mul(atof_(get_token(reader)), factor));
        get_token(reader);
        sx = static_cast<float>(atof_(get_token(reader)));
        get_token(reader);
        sy = static_cast<float>(atof_(get_token(reader)));
        get_token(reader);
        sz = static_cast<float>(atof_(get_token(reader)));

        n_ent = 0;
        while (stricmp_(get_token(reader), str(d::s_ENTRANCE_END)) != 0) {  // "ENTRANCE_END;"
            ent(n_ent)[0] =
                static_cast<float>(fpu.add(atof_(get_token(reader)), static_cast<double>(jo::self<jo::Cell>(this)->origin_x)));
            get_token(reader);
            ent(n_ent)[1] =
                static_cast<float>(fpu.add(atof_(get_token(reader)), static_cast<double>(jo::self<jo::Cell>(this)->origin_y)));
            get_token(reader);
            ent(n_ent)[2] = static_cast<float>(atof_(get_token(reader)));
            n_ent = n_ent + 1;
        }
        n_park = 0;
        while (stricmp_(get_token(reader), str(d::s_PARKING_END)) != 0) {  // "PARKING_END;"
            park(n_park)[0] =
                static_cast<float>(fpu.add(atof_(get_token(reader)), static_cast<double>(jo::self<jo::Cell>(this)->origin_x)));
            get_token(reader);
            park(n_park)[1] =
                static_cast<float>(fpu.add(atof_(get_token(reader)), static_cast<double>(jo::self<jo::Cell>(this)->origin_y)));
            get_token(reader);
            park(n_park)[2] = static_cast<float>(atof_(get_token(reader)));
            if (stricmp_(get_token(reader), str(d::s_PARKING_DIR)) == 0) {  // "PARKING_DIR"
                factor = deg_factor();
            } else {
                factor = 1.0;
            }
            dir(n_park)[0] = static_cast<float>(fpu.mul(atof_(get_token(reader)), factor));
            n_park = n_park + 1;
        }

        std::int32_t nation = 0;  // [ebp-0x2b0]
        const char* const key = get_token(reader);
        if (stricmp_(key, str(d::s_NATION)) == 0) {  // "NATION"
            nation = atoi_(get_token(reader));
        } else if (stricmp_(key, str(d::s_NATION_ID)) == 0) {  // "NATION_ID"
            const std::int32_t id = atoi_(get_token(reader));
            if (id != 0) nation = joan::orig::arg::as<std::int32_t>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(id)));  // 0x004b67e0
        } else if (stricmp_(key, str(d::s_Kingdom)) == 0) {  // "Kingdom"
            read_line(reader, line, 0xff);
            trim_last(line);
            const std::int32_t k = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(joan::orig::arg::str(line)));  // 0x004b91a0
            if (k != 0) nation = joan::orig::arg::as<std::int32_t>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(k)));       // 0x004b67e0
        }
        read_line(reader, line, 0xff);

        Box box;  // [ebp-0x2d0]
        if (eq_or_nan(angle, global<float>(d::k_9cf5c4))) {  // 0.0
            const Vec3 pos{px, py, pz};
            absolute_box(fpu, box, pos, static_cast<double>(angle), sx, sy, sz);
        } else {
            box.v[0] = -sx * global<float>(d::flt_0_5);  // 0.5
            box.v[1] = -sy * global<float>(d::flt_0_5);
            std::uint32_t bits = 0xc2800000u;  // -64.0f
            std::memcpy(&box.v[2], &bits, 4);
            box.v[3] = sx * global<float>(d::flt_0_5);
            box.v[4] = sy * global<float>(d::flt_0_5);
            box.v[5] = sz + margin();
        }

        void* const places = obj(d::g_place_array);
        const std::int32_t index = joan::orig::arg::as<std::int32_t>(joan::orig::as_place_list(places)->alloc());   // 0x004c5370
        void* const place = joan::orig::arg::as<void*>(joan::orig::as_list(places)->get_c70(joan::orig::arg::i32(index)));          // 0x00456c70
        joan::orig::as_group(place)->init(joan::orig::arg::i16(static_cast<std::int32_t>(0)), static_cast<std::int8_t>(joan::orig::arg::i32(type)), joan::orig::arg::str(name));   // 0x00433ea0
        joan::orig::as_place(place)->set_owner(jo::self<jo::Cell>(this)->world);                         // 0x004366f0
        const Vec3 pos{px, py, pz};
        joan::orig::as_place(place)->set_pose(pos, &box);                                   // 0x0042b350
        joan::orig::as_place(place)->set_angle(joan::orig::arg::bits(angle));                                       // 0x004233c0
        for (std::int32_t i = 0; i < n_ent; ++i) joan::orig::as_place(place)->add_entrance(joan::orig::arg::mptr(ent(i)));  // 0x004c22b0
        for (std::int32_t i = 0; i < n_park; ++i) {
            joan::orig::as_place(place)->add_parking(joan::orig::arg::mptr(park(i)), joan::orig::arg::bits(dir(i)[0]));  // 0x00438420
        }
        joan::orig::as_place(place)->set_nation(joan::orig::arg::i32(nation));  // 0x0044f5c0
        return 1;
    }

    // ---- 0x0055ae40 World::create_facility(reader) -------------------------------------------
    std::int32_t create_facility(void* reader) {
        c_facility.hit();
        const joan::hook::x87::Fpu fpu{true};
        // [ebp-0x190] .. [ebp-0x9] of the original frame.
        alignas(8) std::uint8_t fr[0x188] = {};
        auto V = [&fr](std::int32_t ebp_off) -> std::uint8_t* { return fr + (0x190 - ebp_off); };
        char* const name = reinterpret_cast<char*>(V(0x148));  // 0x40
        char* const line = reinterpret_cast<char*>(V(0x108));  // 0x100
        std::int32_t& kind = at<std::int32_t>(V(0x14c), 0);
        std::int32_t& ftype = at<std::int32_t>(V(0x150), 0);
        float& px = at<float>(V(0x15c), 0);
        float& py = at<float>(V(0x158), 0);
        float& pz = at<float>(V(0x154), 0);
        float& angle = at<float>(V(0x160), 0);
        double& factor = at<double>(V(0x168), 0);
        float& sx = at<float>(V(0x174), 0);
        float& sy = at<float>(V(0x170), 0);
        float& sz = at<float>(V(0x16c), 0);
        std::int32_t& n_ent = at<std::int32_t>(V(0x178), 0);
        auto ent = [&](std::int32_t i) { return reinterpret_cast<float*>(V(0x190) + i * 0xc); };

        get_token(reader);
        kind = 0;
        get_token(reader);
        read_line(reader, line, 0xff);
        trim_last(line);
        kind = joan::orig::arg::as<std::int32_t>(joan::orig::facility_kind(joan::orig::arg::str(static_cast<char*>(line))));  // 0x0046d820
        ftype = 0;
        get_token(reader);
        read_line(reader, name, 0x40);
        trim_last(name);
        ftype = joan::orig::arg::as<std::int32_t>(joan::orig::as_place_list(obj(d::g_place_array))->find(joan::orig::arg::str(name)));  // 0x004c54a0
        get_token(reader);
        read_line(reader, name, 0x40);
        trim_last(name);
        get_token(reader);
        px = static_cast<float>(fpu.add(atof_(get_token(reader)), static_cast<double>(jo::self<jo::Cell>(this)->origin_x)));
        get_token(reader);
        py = static_cast<float>(fpu.add(atof_(get_token(reader)), static_cast<double>(jo::self<jo::Cell>(this)->origin_y)));
        get_token(reader);
        pz = static_cast<float>(atof_(get_token(reader)));
        factor = 1.0;
        if (stricmp_(get_token(reader), str(d::s_HD)) == 0) factor = deg_factor();  // "HD"
        angle = static_cast<float>(fpu.mul(atof_(get_token(reader)), factor));
        get_token(reader);
        sx = static_cast<float>(atof_(get_token(reader)));
        get_token(reader);
        sy = static_cast<float>(atof_(get_token(reader)));
        get_token(reader);
        sz = static_cast<float>(atof_(get_token(reader)));

        n_ent = 0;
        while (stricmp_(get_token(reader), str(d::s_ENTRANCE_END)) != 0) {  // "ENTRANCE_END;"
            ent(n_ent)[0] =
                static_cast<float>(fpu.add(atof_(get_token(reader)), static_cast<double>(jo::self<jo::Cell>(this)->origin_x)));
            get_token(reader);
            ent(n_ent)[1] =
                static_cast<float>(fpu.add(atof_(get_token(reader)), static_cast<double>(jo::self<jo::Cell>(this)->origin_y)));
            get_token(reader);
            ent(n_ent)[2] = static_cast<float>(atof_(get_token(reader)));
            n_ent = n_ent + 1;
        }

        Box box;  // [ebp-0x1a8]
        const Vec3 pos{px, py, pz};
        absolute_box(fpu, box, pos, 0.0, sx, sy, sz);  // cos(0) / sin(0): pushes of two zero dwords

        void* const factory = obj(d::g_places);
        const std::int32_t proto = joan::orig::as_facility_list(factory)->create(joan::orig::arg::i32(kind));  // 0x0046ce70
        void* const fac = joan::orig::arg::as<void*>(joan::orig::as_list(factory)->get_e(proto));   // 0x0043c450
        virtual_call(fac, 0x4, kind, name);
        void* const type = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(joan::orig::arg::i32(ftype)));  // 0x00456c70
        joan::orig::as_place(type)->link_facility(joan::orig::arg::mptr(fac));                                         // 0x004c39d0
        joan::orig::as_facility(fac)->set_placement(pos, &box);                                    // 0x0046d6b0
        joan::orig::as_facility(fac)->set_angle(joan::orig::arg::bits(angle));                                        // 0x0046d720
        for (std::int32_t i = 0; i < n_ent; ++i) joan::orig::as_facility(fac)->add_entrance(joan::orig::arg::mptr(ent(i)));  // 0x0046d750
        virtual_call(fac, 0xc, reader);
        read_line(reader, line, 0xff);
        return 1;
    }
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_02", name, addr, fn)
B86("World::create_place", 0x0055a090, &H::create_place);
B86("World::create_facility", 0x0055ae40, &H::create_facility);
#undef B86

}  // namespace game_b86_02
}  // namespace

// ---------------------------------------- from modules/game_b82_03.cpp (game/b82_03)
//
//    - Cell::build_ground 0x0055b720 (world_s.cpp; called by Cell::load_files 0x004f2f80 through
//      thunk 0x00405079). thiscall, 4 stack args (ret 0x10), no used return value:
//        grid_a (ebp+0x8)  the cell's height field (0x004c7100 slope / 0x004c7b70 height),
//        grid_b (ebp+0xc)  the shade field (0x004c7b70 -> vertex alpha),
//        rows   (ebp+0x10) grid cell columns (rows[x][y], GridCell),
//        path   (ebp+0x14) the lightmap cache file name (file_mem 0x00586f00 when 0xa0b6b0).
//      The 0xa3ce38 tile list (count 0xa3ce30) is copied and sorted (qsort 0x007b2d55 with
//      0x0055f2c0), grouped by the tile's +0x24 owner; per group: an optional lightmap texture
//      ("<dir>_<name>" when the 0xa52508 / 0xa524fc caps allow, else "lightmap_default"),
//      the flat quads of level-0 cells (mesh ebp-0x284), the subdivided level-1 / level-2
//      cells (mesh ebp-0x3a0 + the 0x44-byte height field ebp-0x168), each appended to a new
//      0x11c-byte mesh (materials merged through 0x005d5500, vertices translated by the
//      cell offset, heights / normals tilted from the height fields, neighbour cells looked
//      up through World::cells_in_box 0x00429090 on the borders), then attached to the owner
//      (0x005efbe0) and optionally saved to the cache file (records of 0x28 bytes).
namespace {
namespace game_b82_03 {

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

using joan::orig::helpers::at;

constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kTextures(d::g_texture_array);
constexpr img::Addr kWorldSCpp(d::s_d_ww_world_s_cpp);  // "d:\ww\world_s.cpp"

JOAN_COUNTER(c_build_ground, "Cell::build_ground");

std::int32_t line(std::int32_t delta) { return static_cast<std::int32_t>(global<std::int16_t>(d::g_a0b7a0)) + delta; }
void* mem_alloc(std::int32_t size, std::int32_t delta) {
    return joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(size), joan::orig::arg::str(img::str(kWorldSCpp)), joan::orig::arg::i32(line(delta)));  // 0x0058e7b0
}
void* mem_realloc(void* p, std::int32_t size, std::int32_t delta) {
    return joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(img::str(kWorldSCpp)), joan::orig::arg::i32(line(delta)));  // 0x0058eb30
}
void mem_free(void* p, std::int32_t delta) {
    joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(img::str(kWorldSCpp)), joan::orig::arg::i32(line(delta)));  // 0x0058ed70
}
void* tex_get(const void* name) {
    return joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(name), 1, 0u));  // 0x006099c0
}
void tex_release(void* tex) { joan::orig::as_texture_manager(img::obj(kTextures))->release(joan::orig::arg::mptr(tex)); }  // 0x0060a550
void fm_write(void* fm, std::int32_t v) { joan::orig::as_file(fm)->file_put_long(joan::orig::arg::i32(v)); }  // 0x00587da0
std::int32_t fm_tell(void* fm) { return joan::orig::as_file(fm)->file_pos(); }   // 0x00587ef0
void mesh_set_flags(void* mesh, std::int32_t v) { joan::orig::as_plane(mesh)->set_format(joan::orig::arg::i32(v)); }  // 0x005cdc70
void mesh_reset(void* mesh, const void* cell) {
    joan::orig::as_plane(mesh)->clear();  // 0x005cd2d0
    jo::as_plane(mesh)->list_kind = 2;
    joan::orig::as_plane(mesh)->set_owner(joan::orig::arg::mptr(cell));  // 0x005d03f0
}
std::int32_t mesh_add_vertices(void* mesh, std::int32_t n) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_vertices(joan::orig::arg::i32(n))); }  // 0x005cd7f0
std::int32_t mesh_add_triangles(void* mesh, std::int32_t n) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_triangles(joan::orig::arg::i32(n))); }  // 0x005ce6f0
std::int32_t mesh_add_materials(void* mesh, std::int32_t n) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_materials(joan::orig::arg::i32(n))); }  // 0x005cd470
void mesh_triangle(void* mesh, std::int32_t tri, std::int32_t mat, std::int32_t a, std::int32_t b, std::int32_t c) {
    joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(tri), joan::orig::arg::u32(mat), joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c));  // 0x005ce800
}
void material_set_texture(void* m, std::int32_t stage, void* tex) { joan::orig::as_material(m)->set_texture(joan::orig::arg::i32(stage), joan::orig::arg::mptr(tex)); }  // 0x005d5650
float slope(void* field, float x, float y, float* dx, float* dy) {
    return joan::orig::arg::as<float>(joan::orig::as_height_grid(field)->sample(joan::orig::arg::f32(x), joan::orig::arg::f32(y), joan::orig::arg::mptr(dx), joan::orig::arg::mptr(dy)));  // 0x004c7100 (st0 discarded)
}
double height(void* field, float x, float y) {
    return joan::orig::arg::as<double>(joan::orig::as_terrain(field)->height_4c7b70(joan::orig::arg::f32(x), joan::orig::arg::f32(y), static_cast<char>(joan::orig::arg::i32(0))));  // 0x004c7b70
}
float crt_atan(double v) { return joan::orig::arg::as<float>(joan::orig::libc::atan(joan::orig::arg::f64(v))); }
void rotation_x(void* m, float a) { joan::orig::matrix_rotation_60bc00(joan::orig::arg::mptr(m), joan::orig::arg::f32(a)); }  // 0x0060bc00
void rotation_y(void* m, float a) { joan::orig::matrix_from_quat(joan::orig::arg::mptr(m), joan::orig::arg::mptr(a)); }  // 0x0060bbb0
void vector_transform(void* v, void* m) { import_call<void*>(d::iat_v3d_87_vector_transform_2, v, m); }
std::uint8_t* vertex_format(std::uint8_t kind) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<std::int32_t>(kind)))));  // 0x00600200
}
// x87 fild / fidiv of two ints at PC24.
double idiv_f(std::int32_t a, std::int32_t b) { return kFpu.div(static_cast<double>(a), static_cast<double>(b)); }
// x87 compare (fcomp + test ah,0x40): equal or unordered.
bool eq_or_nan(double a, double b) { return !(a < b) && !(a > b); }

struct H {
    void build_ground(void* grid_a, void* grid_b, GridCell** rows, const char* path) {
        if (global<std::int32_t>(d::g_a3ce30) == 0) return;
        if (!fix_uninit()) {
            // Uninitialised subdivision count for a cell level > 2: keep the original body.
            const std::int32_t n = global<std::int32_t>(d::g_a3ce30);
            void** const list = global<void**>(d::g_a3ce38);
            for (std::int32_t i = 0; i < n; ++i) {
                void* const e = list[i];
                if (!e || jo::as_world_sector(e)->level != 0) continue;
                const std::int32_t x = jo::as_world_sector(e)->grid_x, y = jo::as_world_sector(e)->grid_y;
                if (rows[x][y].level > 2) {
                    using Fn = void (H::*)(void*, void*, GridCell**, const char*);
                    (this->*joan::hook::original<Fn>(fn::Cell_build_ground_body))(grid_a, grid_b, rows, path);
                    return;
                }
            }
        }
        c_build_ground.hit();
        void* const self = this;
        auto cell_at = [rows](std::int32_t x, std::int32_t y) -> GridCell* { return &rows[x][y]; };

        const std::uint8_t caps = (global<std::uint32_t>(d::g_a52508) >= 3 && (global<std::uint32_t>(d::g_a524fc) & 0xffff) >= 4) ? 1 : 0;
        alignas(8) jo::File fm_obj;  // ebp-0x124 file_mem (0x110-byte slot); a real File (x64: 0x112 bytes)
        void* const fm = &fm_obj;
        joan::orig::as_file(static_cast<void*>(fm))->construct();  // 0x004179c0
        std::int32_t rec_pos = 0;                             // ebp-0x128
        std::int32_t rec_count = 0;                           // ebp-0x12c
        std::uint8_t* recs = nullptr;                         // ebp-0x130
        if (global<std::int32_t>(d::g_world_write_pln) != 0) {
            joan::orig::as_file(static_cast<void*>(fm))->file_create(joan::orig::arg::str(path), 1, static_cast<char>(0));  // 0x00586f00
            fm_write(fm, static_cast<std::int32_t>(caps));
            recs = static_cast<std::uint8_t*>(mem_alloc((jo::as_cell(self)->sector_count + 1) * static_cast<std::int32_t>(sizeof(PlnEntry)), 0x17));
            std::memset(static_cast<void*>(recs), 0, joan::orig::arg::u32((jo::as_cell(self)->sector_count + 1) * static_cast<std::int32_t>(sizeof(PlnEntry))));  // memset
            fm_write(fm, 3);
            fm_write(fm, jo::as_cell(self)->sector_count);
            rec_pos = fm_tell(fm);
            fm_write(fm, rec_count);
            joan::orig::as_file(static_cast<void*>(fm))->file_write(static_cast<void*>(recs), joan::orig::arg::u32((jo::as_cell(self)->sector_count + 1) * static_cast<std::int32_t>(sizeof(PlnEntry))));  // 0x005872e0
            joan::orig::as_sound_stream(img::obj(d::g_snd))->update();  // 0x0059b1c0
            joan::orig::as_ambient_streams(img::obj(d::g_mus))->update();  // 0x005866d0
        }
        void** const tiles = static_cast<void**>(mem_alloc(global<std::int32_t>(d::g_a3ce30) * static_cast<std::int32_t>(sizeof(void*)), 0x24));  // ebp-0x134
        std::memcpy(static_cast<void*>(tiles), joan::orig::arg::ptr(global<void*>(d::g_a3ce38)), joan::orig::arg::u32(global<std::int32_t>(d::g_a3ce30) * static_cast<std::int32_t>(sizeof(void*))));  // memcpy
        if (joan::replay::stable_ground_order()) joan::ground_order::g_cell = self;  // recordings: tiles by sector index (see compare_24)
        joan::orig::libc::qsort(joan::orig::arg::mptr(static_cast<void*>(tiles)), joan::orig::arg::u32(global<std::int32_t>(d::g_a3ce30)), static_cast<std::uint32_t>(sizeof(void*)), fn::code_ptr(fn::fn_55f2c0));  // qsort
        joan::ground_order::g_cell = nullptr;
        std::int32_t remap_cap = 0;          // ebp-0x138
        std::int32_t* remap = nullptr;       // ebp-0x13c
        const float f140 = 1.0f;
        const float f144 = 64.0f;
        const float f148 = jo::as_cell(self)->box[0];
        const float f14c = 64.0f;
        const float f150 = jo::as_cell(self)->box[1];
        std::int32_t subdivided = 0;  // ebp-0x154
        alignas(8) std::uint8_t field[0x44];   // ebp-0x168
        alignas(8) std::uint8_t quads[((sizeof(jo::Plane) > 0x11c ? sizeof(jo::Plane) : 0x11c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // ebp-0x284 (x64: the larger Plane)
        alignas(8) std::uint8_t sub[((sizeof(jo::Plane) > 0x11c ? sizeof(jo::Plane) : 0x11c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];    // ebp-0x3a0 (x64: the larger Plane)
        alignas(8) std::uint8_t dyn[((sizeof(jo::DynArray) > 0x20 ? sizeof(jo::DynArray) : 0x20) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];     // ebp-0x3c0 (x64: the larger DynArray)
        joan::orig::as_height_grid(static_cast<void*>(field))->construct();  // 0x004c56b0
        joan::orig::as_plane(static_cast<void*>(quads))->construct(1u, 0u);  // 0x005cbaf0
        joan::orig::as_plane(static_cast<void*>(quads))->init(0, 0, 0, 1u);  // 0x005cc7a0
        jo::as_plane(quads)->list_kind = 2;
        joan::orig::as_plane(static_cast<void*>(quads))->set_owner(joan::orig::arg::mptr(self));  // 0x005d03f0
        joan::orig::as_plane(static_cast<void*>(sub))->construct(1u, 0u);
        joan::orig::as_plane(static_cast<void*>(sub))->init(0, 0, 0, 1u);
        jo::as_plane(sub)->list_kind = 2;
        joan::orig::as_plane(static_cast<void*>(sub))->set_owner(joan::orig::arg::mptr(self));
        joan::orig::dynarray(static_cast<void*>(dyn))->construct(4, 0x1e, 0);  // 0x005843d0
        void* default_tex = nullptr;  // ebp-0x3c4
        if (caps != 0) default_tex = tex_get(&img::global<const char>(d::s_lightmap_default));  // "lightmap_default"

        // ebp-0x5d4: set before use, see the gate above - except with fix/uninit, where a cell
        // level > 2 keeps the previous subdivided cell's count (as the original) and the
        // never-written initial value is 1 (no subdivision; 0 would divide by zero below).
        std::int32_t n = 1;
        std::int32_t i = 0;         // ebp-0x3c8
        std::int32_t end = 0;       // ebp-0x3cc
        for (;;) {
            if (i >= global<std::int32_t>(d::g_a3ce30)) break;
            void* const first = tiles[i];  // ebp-0x3d0
            std::int32_t idx = jo::as_cell(self)->sector_count;  // ebp-0x3d4
            while (idx >= 1) {
                if (jo::as_world_sector(first)->owner == jo::as_cell(self)->sectors[idx - 1].item) break;
                --idx;
            }
            void* tex = nullptr;  // ebp-0x3d8
            if (caps != 0) {
                alignas(8) std::uint8_t name[0x194];  // ebp-0x56c (no destructor)
                joan::orig::as_string(static_cast<void*>(name))->construct();             // 0x005a1170
                joan::orig::as_string(static_cast<void*>(name))->assign(joan::orig::arg::str(cell_name(self)));  // 0x005a1440
                joan::orig::as_string(static_cast<void*>(name))->append(joan::orig::arg::str(&img::global<const char>(d::s_us_9cf774)));  // 0x005a1550 "_"
                joan::orig::as_string(static_cast<void*>(name))->append(joan::orig::arg::str(jo::view<std::uint8_t*>(jo::as_cell(self)->sector_names) + (idx - 1) * 0x21));
                tex = tex_get(joan::orig::arg::as<const char*>(joan::orig::as_string(static_cast<void*>(name))->c_str()));  // 0x00417d90
            }
            if (tex) {
                mesh_set_flags(quads, 2);
                mesh_set_flags(sub, 2);
            } else if (default_tex) {
                mesh_set_flags(quads, 2);
                mesh_set_flags(sub, 2);
                joan::orig::as_texture_manager(img::obj(kTextures))->use(joan::orig::arg::mptr(default_tex));  // 0x0060a500
                tex = default_tex;
            } else {
                mesh_set_flags(quads, 1);
                mesh_set_flags(sub, 1);
            }
            for (end = i + 1; end < global<std::int32_t>(d::g_a3ce30); ++end) {
                if (jo::as_world_sector(tiles[end])->owner != jo::as_world_sector(first)->owner) break;
            }
            if (first == nullptr) {
                i = end;
                continue;
            }

            void* mesh = nullptr;  // ebp-0x570
            const std::int32_t start = i;  // ebp-0x574
            std::int32_t min_x = -1, max_x = -1, min_y = -1, max_y = -1;  // ebp-0x578 / 0x57c / 0x580 / 0x584
            std::int32_t scale = 0;  // ebp-0x588
            {
                const jo::SceneS* const owner = jo::as_world_sector(first)->owner;
                float t = owner->inner_box[3] - owner->inner_box[0];
                t = t / 64.0f;  // 0x9d0748
                scale = ftol(t);
            }
            for (; i < end; ++i) {
                void* const p = tiles[i];
                if (jo::as_world_sector(p)->level != 0) continue;
                const std::int32_t x = jo::as_world_sector(p)->grid_x;
                if (min_x == -1 || min_x > x) min_x = x;
                if (max_x == -1 || max_x < x) max_x = x;
                const std::int32_t y = jo::as_world_sector(p)->grid_y;
                if (min_y == -1 || min_y > y) min_y = y;
                if (max_y == -1 || max_y < y) max_y = y;
            }

            // Flat quads of the level-0 cells.
            if (min_x != -1 && min_y != -1) {
                mesh_reset(quads, self);
                const std::int32_t w = max_x - min_x + 2;  // ebp-0x58c
                const std::int32_t h = max_y - min_y + 2;  // ebp-0x590
                const std::int32_t base = mesh_add_vertices(quads, w * h);  // ebp-0x594
                if (tex) {
                    for (std::int32_t y = min_y; y <= max_y + 1; ++y) {
                        for (std::int32_t x = min_x; x <= max_x + 1; ++x) {
                            std::uint8_t* const v = reinterpret_cast<std::uint8_t*>(&jo::as_plane(quads)->vertices_f2[(y - min_y) * w + x - min_x + base]);
                            jo::as_vertex_f2(v)->pos[0] = static_cast<float>(x << 6);
                            jo::as_vertex_f2(v)->pos[1] = static_cast<float>(y << 6);
                            jo::view<std::uint32_t>(jo::as_vertex_f2(v)->pos[2]) = 0;
                            vcol2(v)[2] = cell_at(x, y)->rgb[0];
                            vcol2(v)[1] = cell_at(x, y)->rgb[1];
                            vcol2(v)[0] = cell_at(x, y)->rgb[2];
                            vcol2(v)[3] = static_cast<std::uint8_t>(0xff - cell_at(x, y)->shade);
                            jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[0]) = 0;
                            jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[1]) = 0;
                            jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[2]) = 0x3f800000;
                            jo::as_vertex_f2(v)->attr[4] = static_cast<float>(idiv_f(max_y + 1 - y, jo::as_cell(self)->i32_50));
                            jo::as_vertex_f2(v)->attr[5] = static_cast<float>(idiv_f(max_x + 1 - x, jo::as_cell(self)->i32_50));
                            jo::as_vertex_f2(v)->attr[6] = static_cast<float>(idiv_f(x - min_x, scale));
                            jo::as_vertex_f2(v)->attr[7] = static_cast<float>(kFpu.sub(1.0, idiv_f(y - min_y, scale)));
                        }
                    }
                } else {
                    for (std::int32_t y = min_y; y <= max_y + 1; ++y) {
                        for (std::int32_t x = min_x; x <= max_x + 1; ++x) {
                            std::uint8_t* const v = reinterpret_cast<std::uint8_t*>(&jo::as_plane(quads)->vertices_f1[(y - min_y) * w + x - min_x + base]);
                            jo::as_vertex_f2(v)->pos[0] = static_cast<float>(x << 6);
                            jo::as_vertex_f2(v)->pos[1] = static_cast<float>(y << 6);
                            jo::view<std::uint32_t>(jo::as_vertex_f2(v)->pos[2]) = 0;
                            vcol2(v)[2] = cell_at(x, y)->rgb[0];
                            vcol2(v)[1] = cell_at(x, y)->rgb[1];
                            vcol2(v)[0] = cell_at(x, y)->rgb[2];
                            vcol2(v)[3] = static_cast<std::uint8_t>(0xff - cell_at(x, y)->shade);
                            jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[0]) = 0;
                            jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[1]) = 0;
                            jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[2]) = 0x3f800000;
                            jo::as_vertex_f2(v)->attr[4] = static_cast<float>(idiv_f(max_y + 1 - y, jo::as_cell(self)->i32_50));
                            jo::as_vertex_f2(v)->attr[5] = static_cast<float>(idiv_f(max_x + 1 - x, jo::as_cell(self)->i32_50));
                        }
                    }
                }
                for (i = start; i < end; ++i) {
                    const std::int32_t x = jo::as_world_sector(tiles[i])->grid_x;  // ebp-0x5b0
                    const std::int32_t y = jo::as_world_sector(tiles[i])->grid_y;  // ebp-0x5b4
                    if (cell_at(x, y)->level != 0) continue;
                    const std::int32_t tri = mesh_add_triangles(quads, 2);  // ebp-0x5b8
                    const std::int32_t mat = mesh_add_materials(quads, 1);  // ebp-0x5bc
                    std::uint8_t* const m = reinterpret_cast<std::uint8_t*>(&jo::as_plane(quads)->materials[mat]);
                    joan::orig::as_material(m)->construct();  // 0x005d52a0
                    jo::view<std::int32_t>(jo::as_material(m)->blend) = 0;
                    joan::orig::as_material(m)->set_colour4(joan::orig::arg::mptr(img::obj(d::g_init_white)));  // 0x005d5570
                    if (tex) {
                        if (cell_at(x, y)->texture[1] != 0) {
                            jo::view<std::int32_t>(jo::as_material(m)->texture_mode) = 0x13;
                            material_set_texture(m, 2, tex);
                        } else {
                            jo::view<std::int32_t>(jo::as_material(m)->texture_mode) = 0x12;
                            material_set_texture(m, 1, tex);
                        }
                    } else if (cell_at(x, y)->texture[1] != 0) {
                        jo::view<std::int32_t>(jo::as_material(m)->texture_mode) = 0x11;
                    } else {
                        jo::view<std::int32_t>(jo::as_material(m)->texture_mode) = 0x20000;
                    }
                    jo::view<std::int32_t>(jo::as_material(m)->pass_mask) = 1;
                    void* t = tex_get(cell_at(x, y)->texture_name[0]);
                    material_set_texture(m, 0, t);
                    tex_release(t);
                    if (cell_at(x, y)->texture[1] != 0) {
                        t = tex_get(cell_at(x, y)->texture_name[1]);
                        material_set_texture(m, 1, t);
                        tex_release(t);
                    }
                    auto V = [&](std::int32_t dx, std::int32_t dy) { return (y + dy - min_y) * w + x + dx - min_x + base; };
                    if ((x + y) % 2 == 0) {
                        mesh_triangle(quads, tri, mat, V(0, 0), V(1, 0), V(1, 1));
                        mesh_triangle(quads, tri + 1, mat, V(0, 0), V(1, 1), V(0, 1));
                    } else {
                        mesh_triangle(quads, tri, mat, V(0, 0), V(1, 0), V(0, 1));
                        mesh_triangle(quads, tri + 1, mat, V(0, 1), V(1, 0), V(1, 1));
                    }
                }
            }

            std::int32_t has_quads = (min_x != -1 && min_y != -1) ? 1 : 0;  // ebp-0x5c8
            void* src = nullptr;  // ebp-0x5d8
            std::int32_t cx = 0, cy = 0;  // ebp-0x5cc / 0x5d0
            for (i = start; i < end; ++i) {
                if (jo::as_world_sector(tiles[i])->level != 0) continue;
                cx = jo::as_world_sector(tiles[i])->grid_x;
                cy = jo::as_world_sector(tiles[i])->grid_y;
                if (cell_at(cx, cy)->level != 0) {
                    // Subdivided cell (level 1: 2x2, level 2: 4x4).
                    mesh_reset(sub, self);
                    const std::uint8_t level = cell_at(cx, cy)->level;
                    if (level == 1) {
                        n = 2;
                    } else if (level == 2) {
                        n = 4;
                    }
                    const std::int32_t step = 0x40 / n;  // ebp-0x5dc
                    const std::int32_t base = mesh_add_vertices(sub, (n + 1) * (n + 1));  // ebp-0x5e0
                    if (tex) {
                        for (std::int32_t r = 0; r <= n; ++r) {
                            for (std::int32_t c = 0; c <= n; ++c) {
                                std::uint8_t* const v = reinterpret_cast<std::uint8_t*>(&jo::as_plane(sub)->vertices_f2[r * (n + 1) + c + base]);
                                jo::as_vertex_f2(v)->pos[0] = static_cast<float>((cx << 6) + c * step);
                                jo::as_vertex_f2(v)->pos[1] = static_cast<float>((cy << 6) + r * step);
                                jo::view<std::uint32_t>(jo::as_vertex_f2(v)->pos[2]) = 0;
                                const std::int32_t k = (c * (n + 1) + r) * 5;
                                vcol2(v)[2] = cell_at(cx, cy)->detail[k];
                                vcol2(v)[1] = cell_at(cx, cy)->detail[k + 1];
                                vcol2(v)[0] = cell_at(cx, cy)->detail[k + 2];
                                float u = jo::as_vertex_f2(v)->pos[0] - f148;
                                u = u / f144;
                                float t = jo::as_vertex_f2(v)->pos[1] - f150;
                                t = t / f14c;
                                vcol2(v)[3] = static_cast<std::uint8_t>(ftol(height(grid_b, u, t)));
                                jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[0]) = 0;
                                jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[1]) = 0;
                                jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[2]) = 0x3f800000;
                                double a = kFpu.add(static_cast<double>(max_y), 1.0);
                                a = kFpu.sub(a, static_cast<double>(cy));
                                a = kFpu.sub(a, idiv_f(r, n));
                                jo::as_vertex_f2(v)->attr[4] = static_cast<float>(kFpu.div(a, static_cast<double>(jo::as_cell(self)->i32_50)));
                                a = kFpu.add(static_cast<double>(max_x), 1.0);
                                a = kFpu.sub(a, static_cast<double>(cx));
                                a = kFpu.sub(a, idiv_f(c, n));
                                jo::as_vertex_f2(v)->attr[5] = static_cast<float>(kFpu.div(a, static_cast<double>(jo::as_cell(self)->i32_50)));
                                a = kFpu.add(static_cast<double>(cx), idiv_f(c, n));
                                a = kFpu.sub(a, static_cast<double>(min_x));
                                jo::as_vertex_f2(v)->attr[6] = static_cast<float>(kFpu.div(a, static_cast<double>(scale)));
                                a = kFpu.add(static_cast<double>(cy), idiv_f(r, n));
                                a = kFpu.sub(a, static_cast<double>(min_y));
                                a = kFpu.div(a, static_cast<double>(scale));
                                jo::as_vertex_f2(v)->attr[7] = static_cast<float>(kFpu.sub(1.0, a));
                            }
                        }
                    } else {
                        for (std::int32_t r = 0; r <= n; ++r) {
                            for (std::int32_t c = 0; c <= n; ++c) {
                                std::uint8_t* const v = reinterpret_cast<std::uint8_t*>(&jo::as_plane(sub)->vertices_f1[r * (n + 1) + c + base]);
                                jo::as_vertex_f2(v)->pos[0] = static_cast<float>((cx << 6) + c * step);
                                jo::as_vertex_f2(v)->pos[1] = static_cast<float>((cy << 6) + r * step);
                                jo::view<std::uint32_t>(jo::as_vertex_f2(v)->pos[2]) = 0;
                                const std::int32_t k = (c * (n + 1) + r) * 5;
                                vcol2(v)[2] = cell_at(cx, cy)->detail[k];
                                vcol2(v)[1] = cell_at(cx, cy)->detail[k + 1];
                                vcol2(v)[0] = cell_at(cx, cy)->detail[k + 2];
                                float u = jo::as_vertex_f2(v)->pos[0] - f148;
                                u = u / f144;
                                float t = jo::as_vertex_f2(v)->pos[1] - f150;
                                t = t / f14c;
                                vcol2(v)[3] = static_cast<std::uint8_t>(ftol(height(grid_b, u, t)));
                                jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[0]) = 0;
                                jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[1]) = 0;
                                jo::view<std::uint32_t>(jo::as_vertex_f2(v)->attr[2]) = 0x3f800000;
                                double a = kFpu.add(static_cast<double>(max_y), 1.0);
                                a = kFpu.sub(a, static_cast<double>(cy));
                                a = kFpu.sub(a, idiv_f(r, n));
                                jo::as_vertex_f2(v)->attr[4] = static_cast<float>(kFpu.div(a, static_cast<double>(jo::as_cell(self)->i32_50)));
                                a = kFpu.add(static_cast<double>(max_x), 1.0);
                                a = kFpu.sub(a, static_cast<double>(cx));
                                a = kFpu.sub(a, idiv_f(c, n));
                                jo::as_vertex_f2(v)->attr[5] = static_cast<float>(kFpu.div(a, static_cast<double>(jo::as_cell(self)->i32_50)));
                            }
                        }
                    }
                    const std::int32_t mat = mesh_add_materials(sub, 1);  // ebp-0x60c
                    std::uint8_t* const m = reinterpret_cast<std::uint8_t*>(&jo::as_plane(sub)->materials[mat]);
                    joan::orig::as_material(m)->construct();  // 0x005d52a0
                    jo::view<std::int32_t>(jo::as_material(m)->blend) = 0;
                    joan::orig::as_material(m)->set_colour4(joan::orig::arg::mptr(img::obj(d::g_init_white)));  // 0x005d5570
                    if (tex) {
                        if (cell_at(cx, cy)->texture[1] != 0) {
                            jo::view<std::int32_t>(jo::as_material(m)->texture_mode) = 0x13;
                            material_set_texture(m, 2, tex);
                        } else {
                            jo::view<std::int32_t>(jo::as_material(m)->texture_mode) = 0x12;
                            material_set_texture(m, 1, tex);
                        }
                    } else if (cell_at(cx, cy)->texture[1] != 0) {
                        jo::view<std::int32_t>(jo::as_material(m)->texture_mode) = 0x11;
                    } else {
                        jo::view<std::int32_t>(jo::as_material(m)->texture_mode) = 0x20000;
                    }
                    void* t = tex_get(cell_at(cx, cy)->texture_name[0]);
                    material_set_texture(m, 0, t);
                    tex_release(t);
                    if (cell_at(cx, cy)->texture[1] != 0) {
                        t = tex_get(cell_at(cx, cy)->texture_name[1]);
                        material_set_texture(m, 1, t);
                        tex_release(t);
                    }
                    const std::int32_t tri = mesh_add_triangles(sub, n * n * 2);  // ebp-0x618
                    for (std::int32_t a = 0; a < n; ++a) {
                        for (std::int32_t b = 0; b < n; ++b) {
                            auto W = [&](std::int32_t da, std::int32_t db) { return (a + da) * (n + 1) + b + db + base; };
                            const std::int32_t T = tri + b * 2 + a * n * 2;
                            // Both (cx + cy) parities issue the same calls in the original.
                            if ((cx + cy) % 2 == 0) {
                                if ((b + a) % 2 == 0) {
                                    mesh_triangle(sub, T, mat, W(0, 0), W(0, 1), W(1, 1));
                                    mesh_triangle(sub, T + 1, mat, W(0, 0), W(1, 1), W(1, 0));
                                } else {
                                    mesh_triangle(sub, T, mat, W(0, 0), W(0, 1), W(1, 0));
                                    mesh_triangle(sub, T + 1, mat, W(1, 0), W(0, 1), W(1, 1));
                                }
                            } else {
                                if ((b + a) % 2 == 0) {
                                    mesh_triangle(sub, T, mat, W(0, 0), W(0, 1), W(1, 1));
                                    mesh_triangle(sub, T + 1, mat, W(0, 0), W(1, 1), W(1, 0));
                                } else {
                                    mesh_triangle(sub, T, mat, W(0, 0), W(0, 1), W(1, 0));
                                    mesh_triangle(sub, T + 1, mat, W(1, 0), W(0, 1), W(1, 1));
                                }
                            }
                        }
                    }
                    src = sub;
                    joan::orig::as_height_grid(static_cast<void*>(field))->create(joan::orig::arg::i16(static_cast<std::int16_t>(n)), joan::orig::arg::i16(static_cast<std::int16_t>(n)), joan::orig::arg::i16(static_cast<std::int16_t>(0)));  // 0x004c58a0
                    for (std::int32_t a = 0; a <= n; ++a) {
                        for (std::int32_t c = 0; c <= n; ++c) {
                            const std::int16_t hv = reinterpret_cast<const DetailEntry*>(cell_at(cx, cy)->detail)[c * (n + 1) + a].height;
                            joan::orig::as_height_grid(static_cast<void*>(field))->set(joan::orig::arg::u32(static_cast<std::int16_t>(c)), joan::orig::arg::u32(static_cast<std::int16_t>(a)), joan::orig::arg::u16(hv));  // 0x004c5a20
                        }
                    }
                    subdivided = 1;
                } else {
                    subdivided = 0;
                    if (has_quads == 0) continue;
                    src = quads;
                    has_quads = 0;
                }

                // Append src to the group mesh.
                if (mesh == nullptr) {
                    void* const memory = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86; operator new
                    void* made;
                    if (memory) {
                        const std::int32_t kind = tex ? 2 : static_cast<std::int32_t>(jo::as_plane(src)->vertex_format);
                        made = joan::orig::as_plane(memory)->construct(joan::orig::arg::u32(static_cast<std::int32_t>(static_cast<std::uint8_t>(kind))), 0u);  // 0x005cbaf0
                    } else {
                        made = nullptr;
                    }
                    mesh = made;
                    joan::orig::as_plane(mesh)->init(0, 0, 0, 1u);  // 0x005cc7a0
                    jo::as_plane(mesh)->list_kind = 2;
                    joan::orig::as_plane(mesh)->set_owner(joan::orig::arg::mptr(self));  // 0x005d03f0
                    joan::orig::as_plane(mesh)->set_10c(1u);     // 0x005e4320
                }
                if (jo::as_plane(src)->material_count > remap_cap) {
                    remap_cap = jo::as_plane(src)->material_count + 8;
                    remap = static_cast<std::int32_t*>(mem_realloc(remap, remap_cap << 2, 0x22a));
                }
                for (std::int32_t k = 0; k < jo::as_plane(src)->material_count; ++k) {
                    jo::view<std::int32_t>(jo::as_plane(src)->materials[k].diffuse) = global<std::int32_t>(d::g_init_white);
                    std::int32_t mm = 0;  // ebp-0x62c
                    for (; mm < jo::as_plane(mesh)->material_count; ++mm) {
                        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_material(reinterpret_cast<std::uint8_t*>(&jo::as_plane(src)->materials[k]))->equals(joan::orig::arg::mptr(static_cast<void*>(reinterpret_cast<std::uint8_t*>(&jo::as_plane(mesh)->materials[mm]))))) != 0)  // 0x005d5500
                            break;
                    }
                    if (mm >= jo::as_plane(mesh)->material_count) {
                        mm = mesh_add_materials(mesh, 1);
                        joan::orig::as_material(reinterpret_cast<std::uint8_t*>(&jo::as_plane(mesh)->materials[mm]))->assign(joan::orig::arg::mptr(static_cast<void*>(reinterpret_cast<std::uint8_t*>(&jo::as_plane(src)->materials[k]))));  // 0x005d53e0
                    }
                    remap[k] = mm;
                }
                const std::int32_t nv = jo::as_plane(src)->vertex_count;  // ebp-0x630
                const std::int32_t vbase = mesh_add_vertices(mesh, nv);  // ebp-0x634
                std::int32_t stride = 0;  // ebp-0x638
                std::uint8_t* verts = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(mesh)->buffer(&stride));  // 0x005cdae0
                verts += vbase * stride;
                std::uint8_t* const from = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(src)->buffer(joan::orig::arg::mptr(static_cast<std::int32_t*>(nullptr))));
                std::memcpy(static_cast<void*>(verts), static_cast<void*>(from), joan::orig::arg::u32(jo::as_plane(src)->vertex_count * stride));         // memcpy
                alignas(8) std::uint8_t mat684[0x40];
                float off[3];  // ebp-0x690
                off[0] = static_cast<float>(jo::as_cell(self)->origin_x);
                off[1] = static_cast<float>(jo::as_cell(self)->origin_y);
                jo::view<std::uint32_t>(off[2]) = 0;
                joan::orig::matrix_translation(joan::orig::arg::mptr(static_cast<void*>(mat684)), joan::orig::arg::mptr(static_cast<void*>(off)));  // 0x0060bb50
                {
                    std::uint8_t* p = verts;
                    for (std::int32_t q = 0; q < nv; ++q, p += stride)
                        import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(p), static_cast<void*>(mat684));  // point_transform
                }
                if (jo::as_vertex_format(vertex_format(jo::view<std::uint8_t>(jo::as_plane(mesh)->vertex_format)))->prelit == 0) {
                    std::uint8_t* p = verts + jo::as_vertex_format(vertex_format(jo::view<std::uint8_t>(jo::as_plane(mesh)->vertex_format)))->normal_offset;
                    for (std::int32_t q = 0; q < nv; ++q, p += stride) vector_transform(p, mat684);
                }
                if (subdivided != 0) {
                    std::uint8_t* p = verts;      // ebp-0x644
                    std::uint8_t* nrm = nullptr;  // ebp-0x6a0
                    if (jo::as_vertex_format(vertex_format(jo::view<std::uint8_t>(jo::as_plane(mesh)->vertex_format)))->prelit == 0)
                        nrm = verts + jo::as_vertex_format(vertex_format(jo::view<std::uint8_t>(jo::as_plane(mesh)->vertex_format)))->normal_offset;
                    const std::int32_t step = 0x40 / n;  // ebp-0x6a4
                    for (std::int32_t q = 0; q < nv; ++q, p += stride, nrm = nrm ? nrm + stride : nullptr) {
                        const float u = static_cast<float>(kFpu.div(kFpu.sub(at<float>(p, 0x0), static_cast<double>(cx << 6)), static_cast<double>(step)));
                        const float t = static_cast<float>(kFpu.div(kFpu.sub(at<float>(p, 0x4), static_cast<double>(cy << 6)), static_cast<double>(step)));
                        float dx = 0.0f, dy = 0.0f;  // ebp-0x6b4 / 0x6b8 (always written by 0x004c7100)
                        slope(field, u, t, &dx, &dy);
                        const double hv = height(field, u, t);
                        at<float>(p, 0x8) = static_cast<float>(kFpu.add(kFpu.mul(hv, f140), at<float>(p, 0x8)));
                        if (nrm) {
                            float s = f140 / f144;
                            dx = s * dx;
                            s = f140 / f14c;
                            dy = s * dy;
                            alignas(8) std::uint8_t mx[0x40];  // ebp-0x6fc
                            alignas(8) std::uint8_t my[0x40];  // ebp-0x73c
                            rotation_x(mx, -crt_atan(static_cast<double>(dx * 1.0f)));
                            rotation_y(my, crt_atan(static_cast<double>(dy * 1.0f)));
                            vector_transform(nrm, mx);
                            vector_transform(nrm, my);
                        }
                    }
                } else {
                    std::uint8_t* p = verts;      // ebp-0x644
                    std::uint8_t* nrm = nullptr;  // ebp-0x740
                    if (jo::as_vertex_format(vertex_format(jo::view<std::uint8_t>(jo::as_plane(mesh)->vertex_format)))->prelit == 0)
                        nrm = verts + jo::as_vertex_format(vertex_format(jo::view<std::uint8_t>(jo::as_plane(mesh)->vertex_format)))->normal_offset;
                    for (std::int32_t q = 0; q < nv; ++q, p += stride, nrm = nrm ? nrm + stride : nullptr) {
                        float u = at<float>(p, 0x0) - f148;  // ebp-0x748
                        u = u / f144;
                        float t = at<float>(p, 0x4) - f150;  // ebp-0x74c
                        t = t / f14c;
                        float dx = 0.0f, dy = 0.0f;  // ebp-0x750 / 0x754 (always written by 0x004c7100)
                        slope(grid_a, u, t, &dx, &dy);
                        const double hv = height(grid_a, u, t);
                        at<float>(p, 0x8) = static_cast<float>(kFpu.add(kFpu.mul(hv, f140), at<float>(p, 0x8)));
                        if (nrm == nullptr) continue;
                        std::int32_t found = 0;  // ebp-0x75c
                        if (eq_or_nan(u, 0.0) || eq_or_nan(t, 0.0) ||
                            eq_or_nan(static_cast<double>(jo::as_height_grid(grid_a)->width), u) ||
                            eq_or_nan(static_cast<double>(jo::as_height_grid(grid_a)->height), t)) {
                            // Border vertex: slope of the neighbouring cell.
                            // 0x00429090 reads a box (min, max): the degenerate box {copy, p} (ebp-0x77c, then p at ebp-0x770).
                            float pos[6];
                            std::memcpy(pos, p, 12);
                            std::memcpy(pos + 3, p, 12);
                            std::int32_t count = 0;  // ebp-0x764 (written by 0x00429090)
                            void** const cells = joan::orig::arg::as<void**>(joan::orig::as_world(jo::view<void*>(jo::as_cell(self)->world))->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(pos)), &count));  // 0x00429090
                            for (std::int32_t k = 0; k < count; ++k) {
                                if (cells[k] == self) continue;
                                if (jo::as_cell(cells[k])->path_space_b == nullptr) continue;
                                u = at<float>(p, 0x0) - jo::as_cell(cells[k])->box[0];
                                u = u / f144;
                                t = at<float>(p, 0x4) - jo::as_cell(cells[k])->box[1];
                                t = t / f14c;
                                slope(jo::as_cell(cells[k])->path_space_b->grid, u, t, &dx, &dy);
                                float s = f140 / f144;
                                dx = s * dx;
                                s = f140 / f14c;
                                dy = s * dy;
                                alignas(8) std::uint8_t mx[0x40];  // ebp-0x7c4
                                alignas(8) std::uint8_t my[0x40];  // ebp-0x804
                                rotation_x(mx, -crt_atan(static_cast<double>(dx * 1.0f)));
                                rotation_y(my, crt_atan(static_cast<double>(dy * 1.0f)));
                                vector_transform(nrm, mx);
                                vector_transform(nrm, my);
                                found = 1;
                                break;
                            }
                        }
                        if (found == 0) {
                            float s = f140 / f144;
                            dx = s * dx;
                            s = f140 / f14c;
                            dy = s * dy;
                            alignas(8) std::uint8_t mx[0x40];  // ebp-0x844
                            alignas(8) std::uint8_t my[0x40];  // ebp-0x884
                            rotation_x(mx, -crt_atan(static_cast<double>(dx * 1.0f)));
                            rotation_y(my, crt_atan(static_cast<double>(dy * 1.0f)));
                            vector_transform(nrm, mx);
                            vector_transform(nrm, my);
                        }
                    }
                }
                jo::view<std::int32_t>(jo::as_plane(mesh)->transformed) = 1;
                const std::int32_t ntri = jo::as_plane(src)->triangle_count;  // ebp-0x888
                const std::int32_t tbase = mesh_add_triangles(mesh, ntri);  // ebp-0x88c
                std::uint16_t* smat = jo::view<std::uint16_t*>(jo::as_plane(src)->triangle_materials);
                std::uint16_t* dmat = jo::view<std::uint16_t*>(jo::as_plane(mesh)->triangle_materials) + tbase;
                std::uint16_t* sidx = jo::view<std::uint16_t*>(jo::as_plane(src)->triangles);
                std::uint16_t* didx = jo::view<std::uint16_t*>(jo::as_plane(mesh)->triangles) + tbase * 3;
                for (std::int32_t q = 0; q < ntri; ++q, ++smat, ++dmat) {
                    for (std::int32_t k = 0; k < 3; ++k, ++sidx, ++didx) {
                        if (static_cast<std::int32_t>(*sidx) >= jo::as_plane(src)->vertex_count)
                            *didx = *sidx;
                        else
                            *didx = static_cast<std::uint16_t>(static_cast<std::int32_t>(*sidx) + vbase);
                    }
                    *dmat = static_cast<std::uint16_t>(remap[*smat]);
                }
            }

            if (mesh) {
                joan::orig::as_plane(mesh)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
                joan::orig::as_plane(mesh)->sort_by_material(1);   // 0x005cf1d0
                joan::orig::as_scene_source(jo::as_world_sector(first)->owner)->set_38(joan::orig::arg::mptr(mesh));  // 0x005efbe0
                if (global<std::int32_t>(d::g_world_write_pln) != 0) {
                    const char* const nm = joan::orig::arg::as<const char*>(joan::orig::as_world(self)->sector_name(joan::orig::arg::i32(idx)));  // 0x0054e220
                    std::strcpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<PlnEntry*>(recs)[rec_count].name)), joan::orig::arg::str(nm));  // strcpy
                    reinterpret_cast<PlnEntry*>(recs)[rec_count].sector = static_cast<std::int16_t>(idx);
                    reinterpret_cast<PlnEntry*>(recs)[rec_count].offset = fm_tell(fm);
                    ++rec_count;
                    joan::orig::as_world(self)->write_sector_mesh(joan::orig::arg::mptr(static_cast<void*>(fm)), joan::orig::arg::i32(idx));  // 0x0055f9b0
                    reinterpret_cast<PlnEntry*>(recs)[rec_count].offset = fm_tell(fm);
                }
            }
            if (tex) tex_release(tex);
            i = end;
        }

        tex_release(default_tex);
        if (global<std::int32_t>(d::g_world_write_pln) != 0) {
            const std::int32_t pos = fm_tell(fm);
            joan::orig::as_file(static_cast<void*>(fm))->file_seek(joan::orig::arg::i32(rec_pos), joan::orig::arg::i32(-1));  // 0x00587ea0
            fm_write(fm, rec_count);
            joan::orig::as_file(static_cast<void*>(fm))->file_write(static_cast<void*>(recs), joan::orig::arg::u32((jo::as_cell(self)->sector_count + 1) * static_cast<std::int32_t>(sizeof(PlnEntry))));  // 0x005872e0
            joan::orig::as_file(static_cast<void*>(fm))->file_seek(joan::orig::arg::i32(pos), joan::orig::arg::i32(-1));
        }
        for (std::int32_t k = joan::orig::as_dynarray(static_cast<void*>(dyn))->size(); k > 0; --k) {  // 0x00417ad0
            const std::int16_t v = *joan::orig::arg::as<std::int16_t*>(joan::orig::as_dynarray(static_cast<void*>(dyn))->get(joan::orig::arg::i32(k)));  // 0x00417200
            joan::orig::as_model_res_mgr(img::obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(v));  // 0x004b16a0
        }
        if (remap) {
            mem_free(remap, 0x342);
            remap = nullptr;
        }
        remap_cap = 0;
        mem_free(tiles, 0x346);
        if (recs) mem_free(recs, 0x349);
        recs = nullptr;
        joan::orig::dynarray(static_cast<void*>(dyn))->destruct();  // 0x005844b0
        joan::orig::as_plane(static_cast<void*>(sub))->dtor();    // 0x005cc750
        joan::orig::as_plane(static_cast<void*>(quads))->dtor();  // 0x005cc750
        joan::orig::as_height_grid(static_cast<void*>(field))->free();  // 0x004c5870
        joan::orig::as_file(static_cast<void*>(fm))->destruct();     // 0x005872b0
    }
};

JOAN_REPLACE("game/b82_03", "Cell::build_ground", 0x0055b720, &H::build_ground);

}  // namespace game_b82_03
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
//
//    - WorldCell::grid (0x005600c0): one of the two grid pointers (+0x54 / +0x58) by a flag,
//      falling back to the other one when null (world_s.cpp).
namespace {
namespace game_leaves80b {


struct H {

    // 0x005600c0
    void* cell_grid(std::int16_t second) {
        if (static_cast<std::int32_t>(second) != 0) return jo::view<void*>(jo::self<jo::Cell>(this)->path_space) != nullptr ? jo::view<void*>(jo::self<jo::Cell>(this)->path_space) : jo::view<void*>(jo::self<jo::Cell>(this)->path_space_b);
        return jo::view<void*>(jo::self<jo::Cell>(this)->path_space_b) != nullptr ? jo::view<void*>(jo::self<jo::Cell>(this)->path_space_b) : jo::view<void*>(jo::self<jo::Cell>(this)->path_space);
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("WorldCell::grid", 0x005600c0, &H::cell_grid);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/game_b88_05.cpp (game/b88_05)
//
//    - World::remove_entry 0x005482c0 (index 1..n), ret 4: 0x0054a040, deletes entry +0x7a[i-1]
//      (0x00548430, 1), removes it (0x00591070), --+0x72.
namespace {
namespace game_b88_05 {

using joan::call::thiscall;
using joan::img::obj;

void* const kArrays = obj(d::g_misc);     // 0x00591070 remove, 0x005911a0 search, 0x00590660 random

void array_remove(void* data, std::int32_t count, std::int32_t size, std::int32_t index) {
    joan::orig::as_misc(kArrays)->del_array_rec_591070(joan::orig::arg::str(data), joan::orig::arg::i32(count), joan::orig::arg::i32(size), joan::orig::arg::i32(index));  // 0x00591070
}

JOAN_COUNTER(c_remove_entry, "World::remove_entry");

struct H {

    // ------------------------------------------------------------ 0x005482c0 (ret 4)
    void remove_entry(std::int32_t index) {
        c_remove_entry.hit();
        if (index <= 0 || index > jo::self<jo::World>(this)->cell_count) return;
        joan::orig::as_map(this)->release_tree();  // 0x0054a040
        void* const entry = jo::view<void**>(jo::self<jo::World>(this)->cells)[index - 1];
        if (entry != nullptr) joan::orig::as_world_cell(entry)->destroy(1u);  // 0x00548430
        const std::int32_t count = jo::self<jo::World>(this)->cell_count;
        array_remove(jo::view<void*>(jo::self<jo::World>(this)->cells), count, static_cast<std::int32_t>(sizeof(void*)), index);
        jo::self<jo::World>(this)->cell_count = jo::self<jo::World>(this)->cell_count - 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_05", name, addr, fn)
B("World::remove_entry", 0x005482c0, &H::remove_entry);
#undef B

}  // namespace game_b88_05
}  // namespace

// ---------------------------------------- from modules/game_b82_10.cpp (game/b82_10)
//
//    - World::read_portals 0x005593e0 ("Anti-Portal" -> 0x00559470), World::read_nodes
//      0x0054a6c0 ("CreateNode" -> 0x0047ad40, "CreateWater" -> 0x00553aa0, then +0x60 = 1).
//
//    - World::sector 0x0054e1b0 (1-based, 0 when out of range), Blob::data 0x0056de20,
//      Obj54dba0 0x0054dba0 (forwards to 0x0054dc20, eax passed through).
namespace {
namespace game_b82_10_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::str;

// Stream helpers (FileTxt).
char* get_token(void* s, std::int32_t advance) { return joan::orig::as_file_txt(s)->get_token(joan::orig::arg::i32(advance)); }  // 0x00589c70
void read_line(void* s, char* buf, std::int32_t max) { joan::orig::as_file_txt(s)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(max)); }  // 0x00589f10
using joan::orig::helpers::stricmp_;

JOAN_COUNTER(c_nodes, "World::read_nodes");

struct H {

    // 0x0054e1b0 World::sector(i), 1-based.
    void* sector(std::int32_t i) {  // returns the sector pointer (was u32: truncated on x64)
        if (i <= 0 || i > jo::self<jo::Cell>(this)->sector_count) return nullptr;
        return jo::self<jo::Cell>(this)->sectors[i - 1].item;
    }

    // 0x0054a6c0 World::read_nodes(stream).
    std::int32_t read_nodes(void* stream) {
        c_nodes.hit();
        alignas(4) char line[0x100];
        get_token(stream, 1);
        get_token(stream, 1);
        get_token(stream, 1);
        read_line(stream, line, 0xff);
        get_token(stream, 1);
        read_line(stream, line, 0xff);
        get_token(stream, 1);
        for (;;) {
            char* const tok = get_token(stream, 1);
            if (tok == nullptr) break;
            if (stricmp_(tok, str(d::s_CreateNode)) == 0) joan::orig::as_scene(this)->create_node(joan::orig::arg::mptr(stream));  // "CreateNode" 0x0047ad40
            if (stricmp_(tok, str(d::s_CreateWater)) == 0) joan::orig::as_world(this)->create_water(joan::orig::arg::mptr(stream));  // "CreateWater" 0x00553aa0
        }
        jo::self<jo::World>(this)->u8_60 = 1;
        return 1;
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_10", name, addr, fn)
B82("World::read_nodes", 0x0054a6c0, &H::read_nodes);
B82("World::sector", 0x0054e1b0, &H::sector);
#undef B82

}  // namespace game_b82_10_2
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
//
//      0x0054db70 Obj::zero_66            (thiscall, ret 0) - memset 0x66, returns this.
namespace {
namespace game_b99_05 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ 0x0054db70
    void* zero_66() {
        memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::WorldSector)));  // 0x66 on x86: the WorldSector tiles of World::alloc_objects 0x0054e930 (0xa3ce38 holds WorldSector pointers)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Obj::zero_66", 0x0054db70, &H::zero_66);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b83_02.cpp (game/b83_02)
//
//    - World::find_sector 0x0054e6a0 (name, stricmp), World::sector_name 0x0054e220 (1-based,
//      0x21-byte entries at +0x70).
namespace {
namespace game_b83_02_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::stricmp_;

JOAN_COUNTER(c_find_sector, "World::find_sector");

struct H {

    // 0x0054e220 World::sector_name(i), 1-based.
    char* sector_name(std::int32_t i) {
        if (i <= 0 || i > jo::self<jo::Cell>(this)->sector_count) return nullptr;
        return jo::self<jo::Cell>(this)->sector_names + (i - 1) * 0x21;
    }

    // 0x0054e6a0 World::find_sector(name).
    void* find_sector(const char* name) {
        c_find_sector.hit();
        for (std::int32_t i = 1; i <= jo::self<jo::Cell>(this)->sector_count; ++i) {
            if (stricmp_(name, joan::orig::arg::as<const char*>(joan::orig::as_world(this)->sector_name(joan::orig::arg::i32(i)))) == 0)  // 0x0054e220
                return jo::view<void**>(jo::self<jo::Cell>(this)->sectors)[i - 1];
        }
        return nullptr;
    }
};

#define B83(name, addr, fn) JOAN_REPLACE("game/b83_02", name, addr, fn)
B83("World::find_sector", 0x0054e6a0, &H::find_sector);
B83("World::sector_name", 0x0054e220, &H::sector_name);
#undef B83

}  // namespace game_b83_02_2
}  // namespace

// ---------------------------------------- from modules/game_b84_02.cpp (game/b84_02)
//
//    - Occluders::add 0x00560c70: grows the 0x74-byte record array +0x74 (cap +0x7c, count
//      +0x78), zeroes the new record and returns its index; Occluders::get 0x00560d30.
namespace {
namespace game_b84_02_2 {


struct H {

    // 0x00560d30 Occluders::get(i), ret 4.
    std::uint8_t* occ_get(std::int32_t i) { return reinterpret_cast<std::uint8_t*>(&jo::self<jo::Cell>(this)->occluders[i]); }
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("Occluders::get", 0x00560d30, &H::occ_get);
#undef B84

}  // namespace game_b84_02_2
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
//
//    - getters (cdecl, one pointer arg): 0x0054e040 (+0x4e), 0x0054f4e0 (+0xae), 0x0054f930
//      (+0x3e), 0x0054fd10 (+0x1e6), 0x0054fd60 (+0x1ea).
//
//    - compare_24 0x0055f2c0 (cdecl qsort comparator on pointers): unsigned dword +0x24.
//
//  - 0x0051fce0, 0x00522c10, 0x00530c60, 0x00530c90 and the getters 0x0054e040 .. 0x0054fd60 have
//    no references (no call, no data dword for the function or its thunk): dead code, replaced for
//    completeness.
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


jo::MapObject* JOAN_CDECL get_ae(const u8* p) { return jo::as_map_object(p)->cell_prev; }
jo::WorldItem* JOAN_CDECL get_3e(const u8* p) { return jo::as_world_item(p)->cell_prev; }
jo::Place* JOAN_CDECL get_1e6(const u8* p) { return jo::as_place(p)->cell_prev; }
jo::Place* JOAN_CDECL get_1ea(const u8* p) { return jo::as_place(p)->cell_next; }

// 0x0055f2c0: qsort order of the tiles (WorldSector) by their owner sector (pointers compared unsigned).
// While recording / replaying (joan::replay::stable_ground_order), the owners are compared by their index in the cell's
// sector list instead: the address order is the heap's, and the x64 heap lays the sectors out differently.
i32 JOAN_CDECL compare_24(u8* const* a, u8* const* b) {
    if (const void* const cell = joan::ground_order::g_cell) {
        const std::int32_t ia = joan::ground_order::sector_index(cell, jo::as_world_sector(*a)->owner);
        const std::int32_t ib = joan::ground_order::sector_index(cell, jo::as_world_sector(*b)->owner);
        return ia < ib ? -1 : (ia > ib ? 1 : 0);
    }
    const std::uintptr_t pa = reinterpret_cast<std::uintptr_t>(jo::as_world_sector(*a)->owner);
    const std::uintptr_t pb = reinterpret_cast<std::uintptr_t>(jo::as_world_sector(*b)->owner);
    if (pa < pb) return -1;
    if (pa > pb) return 1;
    return 0;
}

JOAN_REPLACE(M, "get_ae", 0x0054f4e0, &get_ae);
JOAN_REPLACE(M, "get_3e", 0x0054f930, &get_3e);
JOAN_REPLACE(M, "get_1e6", 0x0054fd10, &get_1e6);
JOAN_REPLACE(M, "get_1ea", 0x0054fd60, &get_1ea);
JOAN_REPLACE(M, "compare_24", 0x0055f2c0, &compare_24);

}  // namespace game_b103_06
}  // namespace
#undef M

// ---------------------------------------- from modules/game_b106_12.cpp (game/b106_12)
//
//    - load_throw_ini 0x005525b0 (cdecl, void; called by 0x0046f350 which ignores eax):
//      [THROWI] ARW_ / PROJ_ / EXPL_ / STM_ / STC_ / CAN_ keys TL DL M MXD MND FC TR AV GL ZE
//      (atoi / atof, key order kept per weapon) into 0xa39cac..0xa39dd8, and per weapon the
//      flight time: GL == 0 (or NaN) -> sqrt term 0, time = TL; else
//      s = sqrt(2*GL*9.81*ZE*1/46.1), time = ftol((sqrt(s*s + x) + s) / GL / 9.81 * 1000)
//      (x = the same product again), stored as float (fild qword of the unsigned dword) or,
//      for PROJ_, as int.
//
// Notes:
//  - 0x005525b0: x87 at the current precision (PC24 via Fpu, any other state as double; the
//    CRT sqrt wrapper 0x004256d0 taken as double). The original's final eax is stale
//    (fnstsw / _ftol leftovers); the only caller ignores it.
namespace {
namespace game_b106_12 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using joan::hook::x87::Precision;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_throw_ini, "load_throw_ini");

// ------------------------------------------------------------------ 0x005525b0 helpers
const char* ini_entry(img::Addr key) {
    void* const sec = joan::orig::arg::as<void*>(joan::orig::as_ini(obj(d::g_steer))->section(str(d::s_THROWI)));  // 0x0047cbf0 "THROWI"
    char* const e = joan::orig::arg::as<char*>(joan::orig::as_ini(sec)->value(joan::orig::arg::str(str(key))));                       // 0x0047c180
    return e + 9;
}
i32 ini_int(img::Addr key) { return joan::crt::atoi(joan::orig::arg::str(ini_entry(key))); }  // atoi
float ini_float(img::Addr key) {
    return static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(ini_entry(key)))));  // atof, fstp dword
}

// fld 2.0; fmul GL; fmul 9.81; fmul ZE; fmul 1.0; fdiv 46.1
double throw_product(const Fpu& fpu, img::Addr gl, img::Addr ze) {
    double x = fpu.mul(static_cast<double>(global<float>(d::flt_2)), static_cast<double>(global<float>(gl)));
    x = fpu.mul(x, static_cast<double>(global<float>(d::flt_9_81)));
    x = fpu.mul(x, static_cast<double>(global<float>(ze)));
    x = fpu.mul(x, static_cast<double>(global<float>(d::flt_1)));
    return fpu.div(x, static_cast<double>(global<float>(d::flt_46_1)));
}
double sqrt_w(double x) {  // push ecx; fstp dword [esp]; call 0x004256d0 (CRT sqrt wrapper, st0)
    return joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(static_cast<float>(x))));
}

// The flight-time block (GL != 0 path): sqrt term stored to `s_out`, returns _ftol(...).
i32 throw_time(const Fpu& fpu, img::Addr gl, img::Addr ze, img::Addr s_out) {
    const double s = sqrt_w(throw_product(fpu, gl, ze));
    global<float>(s_out) = static_cast<float>(s);  // fst dword
    double y = fpu.mul(s, static_cast<double>(global<float>(s_out)));
    y = fpu.add(y, throw_product(fpu, gl, ze));  // faddp st(1), st
    double z = sqrt_w(y);
    z = fpu.add(z, static_cast<double>(global<float>(s_out)));
    z = fpu.div(z, static_cast<double>(global<float>(gl)));
    z = fpu.div(z, static_cast<double>(global<float>(d::flt_9_81)));
    z = fpu.mul(z, static_cast<double>(global<float>(d::flt_1000)));  // 1000.0f
    return joan::hook::x87::ftol(z);
}
// fld GL; fcomp 0.0f; fnstsw; test ah, 0x40; je -> "equal or unordered" takes the zero path.
bool gl_zero(img::Addr gl) {
    const float g = global<float>(gl), k = global<float>(d::k_9cf5c4);
    return !(g < k) && !(g > k);
}
// Float-stored flight time: zero path fild dword TL; else fild qword {ftol, 0} (unsigned).
void throw_time_float(const Fpu& fpu, img::Addr gl, img::Addr ze, img::Addr s_out,
                      img::Addr tl, img::Addr out) {
    if (gl_zero(gl)) {
        global<u32>(s_out) = 0;
        global<float>(out) = static_cast<float>(static_cast<double>(global<i32>(tl)));
        return;
    }
    const i32 t = throw_time(fpu, gl, ze, s_out);
    global<float>(out) = static_cast<float>(static_cast<double>(static_cast<u32>(t)));
}

void load_throw_ini_body(const Fpu& fpu) {
    global<i32>(d::g_a39d88) = ini_int(d::s_ARW_TL);  // 'ARW_TL'
    global<i32>(d::g_a39cb4) = ini_int(d::s_ARW_DL);  // 'ARW_DL'
    global<float>(d::g_throw_arw_m) = ini_float(d::s_ARW_M);  // 'ARW_M'
    global<i32>(d::g_a39d74) = ini_int(d::s_ARW_MXD);  // 'ARW_MXD'
    global<i32>(d::g_a39d84) = ini_int(d::s_ARW_MND);  // 'ARW_MND'
    global<float>(d::g_a39d60) = ini_float(d::s_ARW_FC);  // 'ARW_FC'
    global<i32>(d::g_a39cc0) = ini_int(d::s_ARW_TR);  // 'ARW_TR'
    global<float>(d::g_a39d5c) = ini_float(d::s_ARW_AV);  // 'ARW_AV'
    global<float>(d::g_throw_arw_gl) = ini_float(d::s_ARW_GL);  // 'ARW_GL'
    global<float>(d::g_a39d8c) = ini_float(d::s_ARW_ZE);  // 'ARW_ZE'
    throw_time_float(fpu, d::g_throw_arw_gl, d::g_a39d8c, d::g_a39d48, d::g_a39d88, d::g_a39d30);  // ARW
    global<i32>(d::g_a39db8) = ini_int(d::s_PROJ_TL);  // 'PROJ_TL'
    global<i32>(d::g_a39db4) = ini_int(d::s_PROJ_DL);  // 'PROJ_DL'
    global<float>(d::g_throw_proj_m) = ini_float(d::s_PROJ_M);  // 'PROJ_M'
    global<i32>(d::g_a39dc0) = ini_int(d::s_PROJ_MXD);  // 'PROJ_MXD'
    global<i32>(d::g_a39da4) = ini_int(d::s_PROJ_MND);  // 'PROJ_MND'
    global<float>(d::g_a39cd4) = ini_float(d::s_PROJ_FC);  // 'PROJ_FC'
    global<i32>(d::g_a39dc4) = ini_int(d::s_PROJ_TR);  // 'PROJ_TR'
    global<float>(d::g_a39cd8) = ini_float(d::s_PROJ_AV);  // 'PROJ_AV'
    global<float>(d::g_throw_proj_gl) = ini_float(d::s_PROJ_GL);  // 'PROJ_GL'
    global<float>(d::g_a39ce8) = ini_float(d::s_PROJ_ZE);  // 'PROJ_ZE'
    if (gl_zero(d::g_throw_proj_gl)) {  // PROJ: int flight time
        global<u32>(d::g_a39cac) = 0;
        global<i32>(d::g_a39d90) = global<i32>(d::g_a39db8);
    } else {
        global<i32>(d::g_a39d90) = throw_time(fpu, d::g_throw_proj_gl, d::g_a39ce8, d::g_a39cac);
    }
    global<i32>(d::g_a39d7c) = ini_int(d::s_EXPL_TL);  // 'EXPL_TL'
    global<i32>(d::g_a39d3c) = ini_int(d::s_EXPL_DL);  // 'EXPL_DL'
    global<float>(d::g_a39d50) = ini_float(d::s_EXPL_M);  // 'EXPL_M'
    global<i32>(d::g_a39d0c) = ini_int(d::s_EXPL_MXD);  // 'EXPL_MXD'
    global<i32>(d::g_a39d1c) = ini_int(d::s_EXPL_MND);  // 'EXPL_MND'
    global<float>(d::g_a39cf0) = ini_float(d::s_EXPL_FC);  // 'EXPL_FC'
    global<i32>(d::g_a39d24) = ini_int(d::s_EXPL_TR);  // 'EXPL_TR'
    global<float>(d::g_a39d18) = ini_float(d::s_EXPL_AV);  // 'EXPL_AV'
    global<float>(d::g_throw_expl_gl) = ini_float(d::s_EXPL_GL);  // 'EXPL_GL'
    global<float>(d::g_a39d34) = ini_float(d::s_EXPL_ZE);  // 'EXPL_ZE'
    throw_time_float(fpu, d::g_throw_expl_gl, d::g_a39d34, d::g_a39cd0, d::g_a39d7c, d::g_a39ccc);  // EXPL
    global<i32>(d::g_a39d38) = ini_int(d::s_STM_TL);  // 'STM_TL'
    global<i32>(d::g_a39d08) = ini_int(d::s_STM_DL);  // 'STM_DL'
    global<float>(d::g_throw_stm_m) = ini_float(d::s_STM_M);  // 'STM_M'
    global<i32>(d::g_a39d94) = ini_int(d::s_STM_MXD);  // 'STM_MXD'
    global<i32>(d::g_a39d10) = ini_int(d::s_STM_MND);  // 'STM_MND'
    global<i32>(d::g_a39cf4) = ini_int(d::s_STM_TR);  // 'STM_TR'
    global<float>(d::g_a39dc8) = ini_float(d::s_STM_FC);  // 'STM_FC'
    global<float>(d::g_a39d20) = ini_float(d::s_STM_AV);  // 'STM_AV'
    global<float>(d::g_a39ce0) = ini_float(d::s_STM_GL);  // 'STM_GL'
    global<float>(d::g_a39d2c) = ini_float(d::s_STM_ZE);  // 'STM_ZE'
    throw_time_float(fpu, d::g_a39ce0, d::g_a39d2c, d::g_a39cb0, d::g_a39d38, d::g_a39cc4);  // STM
    global<i32>(d::g_a39da8) = ini_int(d::s_STC_TL);  // 'STC_TL'
    global<i32>(d::g_a39d80) = ini_int(d::s_STC_DL);  // 'STC_DL'
    global<float>(d::g_throw_stc_m) = ini_float(d::s_STC_M);  // 'STC_M'
    global<i32>(d::g_a39d58) = ini_int(d::s_STC_MXD);  // 'STC_MXD'
    global<i32>(d::g_a39ce4) = ini_int(d::s_STC_MND);  // 'STC_MND'
    global<i32>(d::g_a39d68) = ini_int(d::s_STC_TR);  // 'STC_TR'
    global<float>(d::g_a39cbc) = ini_float(d::s_STC_FC);  // 'STC_FC'
    global<float>(d::g_a39cec) = ini_float(d::s_STC_AV);  // 'STC_AV'
    global<float>(d::g_throw_stc_gl) = ini_float(d::s_STC_GL);  // 'STC_GL'
    global<float>(d::g_a39d04) = ini_float(d::s_STC_ZE);  // 'STC_ZE'
    throw_time_float(fpu, d::g_throw_stc_gl, d::g_a39d04, d::g_a39dd0, d::g_a39da8, d::g_a39db0);  // STC
    global<i32>(d::g_a39dcc) = ini_int(d::s_CAN_TL);  // 'CAN_TL'
    global<i32>(d::g_a39d70) = ini_int(d::s_CAN_DL);  // 'CAN_DL'
    global<float>(d::g_throw_can_m) = ini_float(d::s_CAN_M);  // 'CAN_M'
    global<i32>(d::g_a39d00) = ini_int(d::s_CAN_MXD);  // 'CAN_MXD'
    global<i32>(d::g_a39d64) = ini_int(d::s_CAN_MND);  // 'CAN_MND'
    global<i32>(d::g_a39d54) = ini_int(d::s_CAN_TR);  // 'CAN_TR'
    global<float>(d::g_a39d28) = ini_float(d::s_CAN_FC);  // 'CAN_FC'
    global<float>(d::g_a39cc8) = ini_float(d::s_CAN_AV);  // 'CAN_AV'
    global<float>(d::g_throw_can_gl) = ini_float(d::s_CAN_GL);  // 'CAN_GL'
    global<float>(d::g_a39d6c) = ini_float(d::s_CAN_ZE);  // 'CAN_ZE'
    throw_time_float(fpu, d::g_throw_can_gl, d::g_a39d6c, d::g_a39d4c, d::g_a39dcc, d::g_a39d40);  // CAN
}

// 0x005525b0
void JOAN_CDECL load_throw_ini() {
    c_throw_ini.hit();
    load_throw_ini_body(Fpu{joan::hook::x87::current_precision() == Precision::Single});
}

#define B(name, addr, fn) JOAN_REPLACE("game/b106_12", name, addr, fn)
B("load_throw_ini", 0x005525b0, &load_throw_ini);
#undef B

}  // namespace game_b106_12
}  // namespace

// ---------------------------------------- from modules/game_b115_11.cpp (game/b115_11)
//
//  * module fix/uninit:
//    - WorldS::load_grid_558140 0x00558140 (thiscall (reader), ret 4, returns 1; caller
//      0x004f3756 ignores eax): the 0x610d0-byte frame is emulated byte for byte in a static
//      buffer (every local that a grid / RLE overrun can reach lives in it at its original
//      offset). The corner average reads row y + 1 / column x + 1 of the 257 x 257 x 5-byte
//      grid, which the loader never writes for the last row / column: the emulated frame
//      starts zeroed, so those cells read 0 (the original reads stack garbage). Reads beyond
//      the frame (saved registers / caller frame, only for a 257-row map) read 0, writes there
//      are dropped. The SEH "next" slot [ebp-0xc] holds 0.
namespace {
namespace game_b115_11 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
template <class T>
T rd(const void* p) {
    T v{};
    std::memcpy(&v, p, sizeof v);
    return v;
}

void* const kMemory = obj(d::g_mem);
const Fpu fpu{true};

// Memory manager alloc 0x0058e7b0 (thunk 0x0040a95c): (size, file, line).
void* mem_alloc(u32 size, const char* file, i32 line) {
    return joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));
}

JOAN_COUNTER(c_grid, "WorldS::load_grid_558140 (fix/uninit)");

// ============================================================ 0x00558140 frame emulation
// Frame bytes [ebp - 0x610d0, ebp) plus a guard for RLE overruns past ebp.
constexpr u32 kFrame = 0x610d0;
constexpr u32 kGuard = 0x10000;
alignas(16) u8 g_frame[kFrame + kGuard];
// Frame index of ebp + off (off < 0).
constexpr u32 fi(i32 off) { return static_cast<u32>(static_cast<i32>(kFrame) + off); }
u8* fp(i32 off) { return g_frame + fi(off); }
i32 f32(i32 off) { return rd<i32>(fp(off)); }
void fw32(i32 off, i32 v) { std::memcpy(fp(off), &v, 4); }
// A pointer local of the original frame: only its 4 x86 bytes are kept in the frame image (overrunning reads see
// them as the original did); the code below never reads a pointer back from the frame.
void fwptr(i32 off, const void* p) {
    const std::uintptr_t v = reinterpret_cast<std::uintptr_t>(p);
    std::memcpy(fp(off), &v, 4);  // the low 4 bytes: the x86 image of the pointer
}
// Byte reads / writes at an arbitrary (wrapping) frame index: beyond ebp read 0 / drop.
u8 frd8(u32 idx) { return idx < kFrame ? g_frame[idx] : u8{0}; }
void fwr8(u32 idx, u8 v) {
    if (idx < kFrame) g_frame[idx] = v;
}
i16 frd16(u32 idx) { return static_cast<i16>(frd8(idx) | (frd8(idx + 1) << 8)); }

constexpr i32 kGrid = -0x50a2c;  // 257 rows of 0x505 bytes (257 cells of 5 bytes)
struct H {

    // ======================================================== 0x00558140
    u32 load_grid(void* reader) {
        c_grid.hit();
        std::memset(g_frame, 0, sizeof g_frame);
        fw32(-0xc, 0);           // SEH next (unknown)
        fwptr(-8, fn::code_ptr(fn::fn_7df6c9));    // SEH handler
        fwptr(-0x10, this);
        void* const rle = fp(-0x14);
        joan::orig::as_rle(rle)->construct();  // 0x0059d190 ctor
        fw32(-4, 0);
        auto read_int = [reader]() { return joan::orig::as_file_mem(reader)->file_get_long(); };  // 0x005893c0
        auto self = [this]() { return this; };  // [ebp-0x10]
        const char* const src_file = str(d::s_d_ww_world_s_cpp);
        auto line = [](i32 d) { return static_cast<i32>(global<i16>(d::g_a0b79c)) + d; };

        fw32(-0x20, read_int());
        fw32(-0x24, read_int());
        jo::as_cell(self())->i32_50 = read_int();
        const i32 lim = jo::as_cell(self())->i32_50 < 1 ? 1 : jo::as_cell(self())->i32_50;
        jo::as_cell(self())->i32_50 = lim;
        if (jo::as_cell(self())->rows == nullptr) {
            jo::view<void*>(jo::as_cell(self())->rows) =
                mem_alloc(static_cast<u32>(f32(-0x20)) * static_cast<u32>(sizeof(jo::U8PtrArray)) + static_cast<u32>(sizeof(jo::U8PtrArray)), src_file, line(0x11));
            for (fw32(-0x18, 0); f32(-0x18) <= f32(-0x20); fw32(-0x18, f32(-0x18) + 1)) {
                void* const row = mem_alloc(static_cast<u32>(f32(-0x24)) * 2u + 2u, src_file, line(0x14));
                jo::as_cell(self())->rows[f32(-0x18)].buffer = static_cast<u8*>(row);
                std::memset(joan::orig::arg::mptr(jo::as_cell(self())->rows[f32(-0x18)].buffer), 0, joan::orig::arg::u32(static_cast<u32>(f32(-0x24)) * 2u + 2u));  // memset
            }
        }
        fw32(-0x50a50, read_int());
        for (fw32(-0x50a54, 0); f32(-0x50a54) < f32(-0x50a50); fw32(-0x50a54, f32(-0x50a54) + 1))
            joan::orig::as_file_mem(reader)->file_read(static_cast<void*>(fp(-0x50a4c)), 0x20u);  // 0x00588a80
        fw32(-0x50a58, read_int());
        fw32(-0x50a5c, 0x20);
        u8* names = fp(-0x50e5c);  // [ebp-0x50e60]
        if (f32(-0x50a58) > 0x20) names = static_cast<u8*>(mem_alloc(static_cast<u32>(f32(-0x50a58)) << 5, src_file, line(0x2d)));
        fwptr(-0x50e60, names);
        jo::as_cell(self())->p84_count = f32(-0x50a58);
        if (jo::as_cell(self())->p_84 == nullptr)
            jo::as_cell(self())->p_84 = mem_alloc(static_cast<u32>(f32(-0x50a58)) << 5, src_file, line(0x32));
        for (fw32(-0x50a54, 0); f32(-0x50a54) < f32(-0x50a58); fw32(-0x50a54, f32(-0x50a54) + 1)) {
            const u32 o = static_cast<u32>(f32(-0x50a54)) << 5;
            joan::orig::as_file_mem(reader)->file_read(joan::orig::arg::mptr(names + o), 0x20u);
            joan::orig::as_file_mem(reader)->file_read(joan::orig::arg::mptr(static_cast<u8*>(jo::as_cell(self())->p_84) + o), 0x20u);
            std::strncpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<u8*>(jo::as_cell(self())->p_84) + o)), joan::orig::arg::str(names + o), 0x20u);  // strncpy
        }
        // Layers: RLE (0x0059dc10) into the buffer at [ebp-0x61064]; layers 4 / 11..14 go to the
        // grid byte 0 / 1..4.
        const u32 buf = fi(-0x61064);
        i32 off = 0;  // [ebp-0x61074]: assigned before every use
        for (u32 layer = 0; layer < 0x10; ++layer) {
            joan::orig::as_rle(rle)->decode(joan::orig::arg::mptr(reader), joan::orig::arg::mptr(static_cast<void*>(g_frame + buf)), 0xff);  // 0x0059dc10
            const i32 l = static_cast<i32>(layer);
            if (l != 4 && (l < 0xb || l > 0xe)) continue;
            if (l == 4) {
                off = 0;
            } else if (l > 0xa && l <= 0xe) {
                off = l - 0xa;
            }
            for (i32 y = 0; y <= f32(-0x20); ++y) {
                u32 dst = fi(kGrid) + static_cast<u32>(y) * 0x505u + static_cast<u32>(off);
                u32 src = buf + static_cast<u32>(y) * (static_cast<u32>(f32(-0x24)) + 1u);
                for (i32 x = 0; x <= f32(-0x24); ++x, dst += 5, ++src) fwr8(dst, frd8(src));
            }
        }
        // Corner average -> per-cell value.
        for (fw32(-0x18, 0); f32(-0x18) <= f32(-0x20); fw32(-0x18, f32(-0x18) + 1)) {
            for (fw32(-0x1c, 0); f32(-0x1c) <= f32(-0x24); fw32(-0x1c, f32(-0x1c) + 1)) {
                const u32 y = static_cast<u32>(f32(-0x18));
                const u32 x = static_cast<u32>(f32(-0x1c));
                const u32 g = fi(kGrid);
                const u32 cell = g + y * 0x505u + x * 5u;
                float avg = 0.0f;
                if (frd16(cell + 3) != 0) {
                    const i32 sum = frd8(cell) + frd8(g + (y + 1) * 0x505u + x * 5u) +
                                    frd8(g + y * 0x505u + (x + 1) * 5u) + frd8(g + (y + 1) * 0x505u + (x + 1) * 5u);
                    avg = static_cast<float>(fpu.div(static_cast<double>(sum), 1020.0));
                }
                i16* const row = reinterpret_cast<i16*>(jo::as_cell(self())->rows[y].buffer);
                if (frd16(cell + 3) != 0 && !(avg >= 0.5f)) {
                    row[x] = static_cast<i16>(frd16(cell + 3) - 1);
                } else {
                    row[x] = static_cast<i16>(frd16(cell + 1) - 1);
                }
            }
        }
        fw32(-4, -1);
        joan::orig::as_rle(rle)->destruct();  // 0x0059d1b0 dtor
        return 1;
    }

};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: unwritten last grid row / column (stack garbage) read as 0 (emulated frame).
F("WorldS::load_grid_558140", 0x00558140, &H::load_grid);
#undef F

}  // namespace game_b115_11
}  // namespace

// ---------------------------------------- from modules/game_b105_07_2.cpp (game/b105_07)
namespace {
namespace game_b105_07_2 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {

    u32 get_78() { return jo::view<u32>(jo::self<jo::Cell>(this)->occluder_count); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_07", name, addr, fn)
B("Obj::get_78_560d60", 0x00560d60, &H::get_78);
#undef B

}  // namespace game_b105_07_2
}  // namespace

// ---------------------------------------- from modules/game_b101_08_2.cpp (game/b101_08)
namespace {
namespace game_b101_08_2 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::x87::Fpu;
using i32 = std::int32_t;

struct Vec3 {
    float x, y, z;
};


static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_any_hit, "Zones::any_hit");

struct H {

    // ------------------------------------------------------------------ 0x00560d80
    bool any_hit(Vec3 a, Vec3 b, i32 dz) {
        c_any_hit.hit();
        a.z = static_cast<float>(Fpu{true}.add(static_cast<double>(dz), static_cast<double>(a.z)));
        // `this` is the Cell: occluders (+0x74, 0x74-byte records) and occluder_count (+0x78).
        for (i32 i = static_cast<i32>(static_cast<u32>(jo::self<jo::Cell>(this)->occluder_count) - 1u); i >= 0; --i) {
            u8* const zone = reinterpret_cast<u8*>(&jo::self<jo::Cell>(this)->occluders[i]);
            if (joan::orig::arg::as<u8>(joan::orig::as_occluder(zone)->ray_hit(a, b)) != 0) return true;  // 0x00530f20
        }
        return false;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_08", name, addr, fn)
B("Zones::any_hit", 0x00560d80, &H::any_hit);
#undef B

}  // namespace game_b101_08_2
}  // namespace

// ---------------------------------------- from modules/game_b91_01_2.cpp (game/b91_01)
namespace {
namespace game_b91_01_2 {

using joan::hook::x87::Fpu;

struct Vec3 {
    float x, y, z;
};


static_assert(sizeof(Vec3) == 12, "vec3");

struct H {

    // 0x00560e30
    void cell_of(std::int32_t* gx, std::int32_t* gy, Vec3 pos) {
        const joan::hook::x87::Fpu fpu{true};
        *gx = joan::hook::x87::ftol(
            fpu.div(fpu.sub(pos.x, static_cast<double>(jo::self<jo::Cell>(this)->origin_x)), 64.0));  // 0x9d0748
        *gy = joan::hook::x87::ftol(
            fpu.div(fpu.sub(pos.y, static_cast<double>(jo::self<jo::Cell>(this)->origin_y)), 64.0));
    }

    // 0x00560e90
    void* cell(std::int32_t i, std::int32_t j) {
        const std::int32_t idx = jo::view<std::int16_t**>(jo::self<jo::Cell>(this)->rows)[i][j];
        // signed: a cell without terrain holds -1 and the original reads the record before the table (32-bit
        // wrap-around); a u32 offset would put it 4 GB further on x64
        return static_cast<std::uint8_t*>(jo::self<jo::Cell>(this)->p_84) + static_cast<std::ptrdiff_t>(idx) * 32;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_01", name, addr, fn)
B("Terrain::cell_of", 0x00560e30, &H::cell_of);
B("Terrain::cell", 0x00560e90, &H::cell);
#undef B

}  // namespace game_b91_01_2
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

using joan::call::thiscall;
using joan::img::obj;

struct Vec3 {
    float x, y, z;
};


static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_name_at, "Terrain::name_at");

struct H {

    // 0x00560ee0
    void* name_at(Vec3 pos) {
        c_name_at.hit();
        std::int32_t gx, gy;
        joan::orig::as_world_cell(this)->grid_cell_of(&gx, &gy, pos);  // 0x00560e30
        gx = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(obj(d::g_misc))->limit_int(joan::orig::arg::i32(gx), 0, static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_rows)));  // 0x00590aa0
        gy = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(obj(d::g_misc))->limit_int(joan::orig::arg::i32(gy), 0, static_cast<std::int32_t>(jo::self<jo::Cell>(this)->sector_cols)));  // 0x00590aa0
        return joan::orig::arg::as<void*>(joan::orig::as_terrain(this)->cell(joan::orig::arg::i32(gx), joan::orig::arg::i32(gy)));  // 0x00560e90
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("Terrain::name_at", 0x00560ee0, &H::name_at);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b83_01.cpp (game/b83_01)
namespace {
namespace game_b83_01_2 {


using i32 = std::int32_t;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;

using joan::call::thiscall;

using joan::img::obj;

using joan::hook::global;

using joan::call::cdecl_call;

using joan::img::str;

// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    using Fn = R(JOAN_STDCALL*)(void*, A...);
    return reinterpret_cast<Fn>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}

constexpr Fpu kFpu{true};

struct Box6 {
    float v[6];
};

JOAN_COUNTER(c_create_water, "Cell::create_water");

// Emulated stack frame: buffer at [ebp - off] is at frame + N - off.
template <std::int32_t N>
struct Frame {
    alignas(8) std::uint8_t b[N];
    char* ebp(std::int32_t off) { return reinterpret_cast<char*>(b + N - off); }
};

// CRT.
std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
using joan::orig::helpers::stricmp_;
double atof_(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
std::int32_t atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
using joan::orig::helpers::new_;

// Reader.
char* token(void* reader) { return joan::orig::as_file_txt(reader)->get_token(1); }  // 0x00589c70
void read_str(void* reader, char* buf, std::int32_t max) {
    joan::orig::as_file_txt(reader)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(max));  // 0x00589f10
}
// buf[strlen(buf) - 1] = 0 (writes buf[-1] for an empty string, inside the frame).
void chop(char* buf) { *(buf + static_cast<std::int32_t>(strlen_(buf)) - 1) = 0; }

const char* c_str(void* path) { return joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str()); }  // 0x00417d90
void* tex_get(const char* name) {
    return joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(name), 1, 0u));  // 0x006099c0
}
void tex_release(void* tex) { joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(tex)); }  // 0x0060a550
void mat_set_texture(void* mat, std::int32_t stage, void* tex) {
    joan::orig::as_material(mat)->set_texture(joan::orig::arg::i32(stage), joan::orig::arg::mptr(tex));  // 0x005d5650
}
std::int32_t effect_find(const char* name) {
    return joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_db(obj(d::g_texture_effect_res))->find(joan::orig::arg::str(name)));  // 0x004dff70
}
float lerp(float a, float b, float t) { return joan::orig::arg::as<float>(joan::orig::lerp_5513f0(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(t))); }

struct H {
    // ------------------------------------------------------------------ 0x004f5210
    std::int32_t create_water(void* reader, std::int32_t flag) {
        c_create_water.hit();
        Frame<0x580> fr;
        std::memset(fr.b, 0, sizeof fr.b);
        char* const b118 = fr.ebp(0x118);  // 0x100
        char* const b158 = fr.ebp(0x158);
        char* const b17c = fr.ebp(0x17c);  // sound 1
        char* const b1a0 = fr.ebp(0x1a0);  // sound 2
        char* const b390 = fr.ebp(0x390);  // bump texture
        void* const path = fr.ebp(0x334);  // string object (0x194)
        void* const path_r = fr.ebp(0x578);
        char* tok;  // ebp-0x18

        tok = token(reader);
        tok = token(reader);
        read_str(reader, b158, 0x40);
        chop(b158);
        tok = token(reader);
        read_str(reader, b158, 0x40);
        chop(b158);
        joan::orig::as_string(path)->construct(joan::orig::arg::str(b158));  // 0x005a11a0
        tok = token(reader);
        read_str(reader, b158, 0x30);
        chop(b158);
        std::int32_t effect;  // ebp-0x338
        if (stricmp_(b158, str(d::s_none)) == 0)  // "none"
            effect = 0;
        else
            effect = static_cast<std::int16_t>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(b158));  // 0x004d12f0

        tok = token(reader);
        tok = token(reader);
        const float px = static_cast<float>(kFpu.add(atof_(tok), static_cast<double>(jo::self<jo::Cell>(this)->origin_x)));  // ebp-0x344
        tok = token(reader);
        tok = token(reader);
        const float py = static_cast<float>(kFpu.add(atof_(tok), static_cast<double>(jo::self<jo::Cell>(this)->origin_y)));  // ebp-0x340
        tok = token(reader);
        tok = token(reader);
        const float pz = static_cast<float>(atof_(tok));  // ebp-0x33c
        tok = token(reader);
        tok = token(reader);
        const std::int32_t rotation = atoi_(tok);  // ebp-0x348
        // tile_v lives in the frame: the 0x40-char read into ebp-0x390 below puts its
        // terminator on ebp-0x350 (tile_v's low byte) when the name fills the buffer.
        std::int32_t tile_u = 0x96;  // ebp-0x34c
        std::int32_t& tile_v = *reinterpret_cast<std::int32_t*>(fr.ebp(0x350));
        tile_v = 0x96;
        tok = token(reader);
        tok = token(reader);
        tile_v = atoi_(tok);
        tile_u = tile_v;
        tok = token(reader);
        read_str(reader, b390, 0x40);
        chop(b390);
        tok = token(reader);
        tok = token(reader);
        const float f394 = static_cast<float>(atoi_(tok));  // fild + fstp
        tok = token(reader);
        tok = token(reader);
        const std::uint8_t b398 = atoi_(tok) != 0 ? 1 : 0;
        tok = token(reader);
        tok = token(reader);
        const float w = static_cast<float>(atof_(tok));  // ebp-0x3a4
        tok = token(reader);
        tok = token(reader);
        const float h = static_cast<float>(atof_(tok));  // ebp-0x3a0
        tok = token(reader);
        read_str(reader, b17c, 0x20);
        chop(b17c);
        tok = token(reader);
        read_str(reader, b1a0, 0x20);
        chop(b1a0);
        read_str(reader, b118, 0xff);

        const float two = global<float>(d::flt_2);  // 2.0
        Box6 box;                                     // ebp-0x3bc
        box.v[0] = px - w / two;
        box.v[1] = py - h / two;
        box.v[2] = pz;
        box.v[3] = w / two + px;
        box.v[4] = h / two + py;
        box.v[5] = pz;

        void* node = nullptr;  // ebp-0x3c0 (only read when flag != 0, set below)
        if (flag != 0) {
            const std::int32_t slot = joan::orig::arg::as<std::int32_t>(joan::orig::as_cell(this)->add_scene_node());  // 0x0042b270
            node = joan::orig::arg::as<void*>(joan::orig::as_world(this)->sector(joan::orig::arg::i32(slot)));                      // 0x0054e1b0
            joan::orig::as_scene_source(node)->reset(joan::orig::arg::mptr(static_cast<void*>(&box)));  // 0x005ef880
            joan::orig::as_names(this)->set_41baa0(joan::orig::arg::i32(slot), str(d::s_Water));     // 0x0041baa0 "Water"
            void* const mem = new_(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
            void* const mesh = mem != nullptr ? joan::orig::as_plane(mem)->construct(1u, 1u) : nullptr;  // 0x005cbaf0
            joan::orig::as_plane(mesh)->init(0, 0, 0, 1u);                                 // 0x005cc7a0
            const std::int32_t mat_index = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_materials(1));           // 0x005cd470
            void* const mat = &jo::as_plane(mesh)->materials[mat_index];  // Material is 0x54 bytes on x86, larger on x64
            joan::orig::as_material(mat)->construct();  // 0x005d52a0
            jo::view<std::int32_t>(jo::as_material(mat)->blend) = 1;
            jo::view<std::int32_t>(jo::as_material(mat)->pass_mask) = 2;
            jo::as_material(mat)->raster = jo::as_material(mat)->raster | 0x100u;

            auto waterpool = [&]() {
                void* const tex = tex_get(str(d::s_waterpool));  // "waterpool"
                mat_set_texture(mat, 0, tex);
                tex_release(tex);
                jo::as_material(mat)->texture_mode =
                    static_cast<std::uint32_t>(effect_find(str(d::s_NormalWater))) | 0x1000000u;  // "NormalWater"
            };
            if (stricmp_(c_str(path), str(d::s_ice)) == 0) {  // "ice"
                void* const tex = tex_get(str(d::s_icelake));     // "icelake"
                mat_set_texture(mat, 0, tex);
                tex_release(tex);
                jo::as_material(mat)->texture_mode = static_cast<std::uint32_t>(effect_find(c_str(path))) | 0x1000000u;
            } else if (global<std::int8_t>(d::g_cfg_shadow_quality) == 0) {
                waterpool();
            } else if (global<std::int8_t>(d::g_a58056) != 0) {
                mat_set_texture(mat, 0, nullptr);
                {
                    void* const tex = tex_get(b390);
                    mat_set_texture(mat, 1, tex);
                    tex_release(tex);
                }
                std::int32_t fx = 0;      // ebp-0x3e0
                std::uint8_t refl = 0;    // ebp-0x3e4
                if (global<std::int8_t>(d::g_cfg_shadow_quality) == 2) {
                    joan::orig::as_string(path_r)->construct_copy(joan::orig::as_string(path));  // 0x005a11f0 copy
                    joan::orig::as_string(path_r)->append(str(d::s_R));                      // 0x005a1550 append "R"
                    fx = effect_find(c_str(path_r));
                    if (fx != 0) refl = 1;
                }
                if (fx == 0 && static_cast<std::int8_t>(b390[0]) != 0 && global<std::int32_t>(d::g_a52637) != 0)
                    fx = effect_find(c_str(path));
                if (fx != 0)
                    jo::as_material(mat)->texture_mode = static_cast<std::uint32_t>(fx) | 0x1000000u;
                else if (global<std::int32_t>(d::g_a52637) != 0)
                    jo::view<std::int32_t>(jo::as_material(mat)->texture_mode) = 0xc;
                if (refl != 0) {
                    jo::TexScriptRec* const def = joan::orig::arg::as<jo::TexScriptRec*>(joan::orig::as_anim_db(obj(d::g_texture_effect_res))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(static_cast<std::int16_t>(fx)))));  // 0x004b27b0
                    for (std::int32_t i = 0; i < jo::view<std::int16_t>(def->stage_count[0]); ++i) {
                        if (jo::view<std::int32_t>(def->projected_flags[i]) == 0) continue;
                        joan::orig::as_material(mat)->set_14_18(joan::orig::arg::mptr(obj(d::g_init_white)), joan::orig::arg::bits(1.0f));  // 0x005d5610
                        void* const rt = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->new_render_target());   // 0x006096d0
                        joan::orig::as_render_target(rt)->create(0x400u, 0x400u, 1u);         // 0x00608080
                        void* const dev = global<void*>(d::g_game_device);
                        void* old_target;   // ebp-0x588
                        void* old_depth;    // ebp-0x58c
                        void* surface;      // ebp-0x590
                        com_call(dev, 0x80, &old_target);  // GetRenderTarget
                        com_call(global<void*>(d::g_game_device), 0x84, &old_depth);  // GetDepthStencilSurface
                        {
                            void* const t = jo::view<void*>(jo::as_render_target(rt)->d3d_texture);
                            com_call(t, 0x3c, 0, &surface);  // GetSurfaceLevel(0)
                        }
                        com_call(global<void*>(d::g_game_device), 0x7c, surface, static_cast<void*>(nullptr));  // SetRenderTarget
                        if (com_call(global<void*>(d::g_game_device), 0x88) == 0) {  // BeginScene
                            alignas(4) std::uint8_t colour[4] = {0x80, 0x80, 0x80, 0xff};  // ebp-0x594
                            std::uint32_t d3d_colour;                                      // ebp-0x598
                            joan::orig::as_colour(static_cast<void*>(colour))->to_d3d(&d3d_colour);  // 0x005e93b0
                            com_call(global<void*>(d::g_game_device), 0x90, 0u, static_cast<void*>(nullptr), 1u, d3d_colour,
                                     1.0f, 0u);                                   // Clear(target)
                            com_call(global<void*>(d::g_game_device), 0x8c);            // EndScene
                        }
                        com_call(global<void*>(d::g_game_device), 0x7c, old_target, old_depth);  // SetRenderTarget
                        com_call<std::uint32_t>(old_target, 0x8);                          // Release
                        com_call<std::uint32_t>(old_depth, 0x8);
                        com_call<std::uint32_t>(surface, 0x8);
                        mat_set_texture(mat, i, rt);
                        tex_release(rt);
                    }
                } else if (fx == 0) {
                    waterpool();
                }
            } else {
                const std::int32_t fx = effect_find(c_str(path));  // ebp-0x5a0
                if (fx != 0) {
                    if (global<std::int8_t>(d::g_cfg_shadow_quality) == 2) {
                        joan::orig::as_material(mat)->set_14_18(joan::orig::arg::mptr(obj(d::g_init_white)), joan::orig::arg::bits(1.0f));  // 0x005d5610
                        jo::view<std::int32_t>(jo::as_material(mat)->texture_mode) = 0x20010;
                        void* const rt = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->new_render_target());  // 0x006096d0
                        joan::orig::as_render_target(rt)->create(0x400u, 0x400u, 1u);        // 0x00608080
                        mat_set_texture(mat, 0, rt);
                        tex_release(rt);
                    } else if (global<std::int8_t>(d::g_cfg_shadow_quality) == 1) {
                        jo::view<std::int32_t>(jo::as_material(mat)->texture_mode) = 0xc;
                        void* const tex = tex_get(b390);
                        mat_set_texture(mat, 1, tex);
                        tex_release(tex);
                    }
                } else {
                    waterpool();
                }
            }

            // Grid step: 800 unless a side is < 800 (or NaN): min side / 3.
            std::int32_t step = 0x320;  // ebp-0x5ac
            const float k800 = global<float>(d::flt_800);
            if (!(w >= k800) || !(h >= k800)) {
                const float m = !(w >= h) ? w : h;
                step = ftol(static_cast<double>(m / global<float>(d::flt_3)));
            }
            const std::int32_t nx = ftol(static_cast<double>(w)) / step + 2;  // ebp-0x5b0
            const std::int32_t ny = ftol(static_cast<double>(h)) / step + 2;  // ebp-0x5b4
            const float x0 = px - w / two;                                    // ebp-0x5b8
            const float y0 = py - h / two;                                    // ebp-0x5bc
            const float dx = static_cast<float>(kFpu.div(static_cast<double>(w), static_cast<double>(nx - 1)));  // ebp-0x5c0
            const float dy = static_cast<float>(kFpu.div(static_cast<double>(h), static_cast<double>(ny - 1)));  // ebp-0x5c4
            joan::orig::as_plane(mesh)->add_vertices(static_cast<std::int32_t>(static_cast<std::uint32_t>(nx) *
                                                                               static_cast<std::uint32_t>(ny)));  // 0x005cd7f0
            auto vtx = [&](std::int32_t i) -> std::uint8_t* { return reinterpret_cast<std::uint8_t*>(&jo::as_plane(mesh)->vertices_f1[i]); };
            auto idx = [&](std::int32_t row, std::int32_t col) {
                return static_cast<std::int32_t>(static_cast<std::uint32_t>(nx) * static_cast<std::uint32_t>(row) +
                                                 static_cast<std::uint32_t>(col));
            };
            for (std::int32_t r = 0; r < ny; ++r) {
                for (std::int32_t c = 0; c < nx; ++c) {
                    jo::as_vertex_f1(vtx(idx(r, c)))->pos[0] = static_cast<float>(
                        kFpu.add(kFpu.mul(static_cast<double>(c), static_cast<double>(dx)), static_cast<double>(x0)));
                    jo::as_vertex_f1(vtx(idx(r, c)))->pos[1] = static_cast<float>(
                        kFpu.add(kFpu.mul(static_cast<double>(r), static_cast<double>(dy)), static_cast<double>(y0)));
                    jo::as_vertex_f1(vtx(idx(r, c)))->pos[2] = pz;
                    {
                        jo::VertexF1* const n = jo::as_vertex_f1(vtx(idx(r, c)));
                        jo::view<std::uint32_t>(n->attr[0]) = global<std::uint32_t>(d::g_a18a60);
                        jo::view<std::uint32_t>(n->attr[1]) = global<std::uint32_t>(d::g_a18a64);
                        jo::view<std::uint32_t>(n->attr[2]) = global<std::uint32_t>(d::g_a18a68);
                    }
                    if (stricmp_(c_str(path), str(d::s_BumpEnvWaterSea)) == 0) {  // "BumpEnvWaterSea"
                        vcol1(vtx(idx(r, c)))[2] = 0x6f;
                        vcol1(vtx(idx(r, c)))[0] = 0xd5;
                        vcol1(vtx(idx(r, c)))[1] = 0xc8;
                    } else {
                        vcol1(vtx(idx(r, c)))[2] = 0x41;
                        vcol1(vtx(idx(r, c)))[0] = 0x50;
                        vcol1(vtx(idx(r, c)))[1] = 0x33;
                    }
                    vcol1(vtx(idx(r, c)))[3] = 0x80;
                }
            }

            // Corner UVs by rotation (fild of ftol(size) / tile).
            const std::int32_t last_x = nx - 1;                  // (nx - 1)
            const std::int32_t first_y = idx(ny - 1, 0);         // (ny - 1) * nx
            const std::int32_t corner = idx(ny - 1, 0) + nx - 1; // (ny - 1) * nx + nx - 1
            auto U = [&]() { return static_cast<float>(ftol(static_cast<double>(w)) / tile_u); };
            auto V = [&]() { return static_cast<float>(ftol(static_cast<double>(h)) / tile_v); };
            auto u = [&](std::int32_t i) -> float& { return jo::as_vertex_f1(vtx(i))->attr[4]; };
            auto v = [&](std::int32_t i) -> float& { return jo::as_vertex_f1(vtx(i))->attr[5]; };
            if (rotation == 0) {
                u(0) = 0.0f;
                v(0) = 0.0f;
                u(last_x) = U();
                v(last_x) = 0.0f;
                u(first_y) = 0.0f;
                v(first_y) = V();
                u(corner) = U();
                v(corner) = V();
            } else if (rotation == 0x5a) {
                u(0) = V();
                v(0) = 0.0f;
                u(last_x) = V();
                v(last_x) = U();
                u(first_y) = 0.0f;
                v(first_y) = 0.0f;
                u(corner) = 0.0f;
                v(corner) = U();
            } else if (rotation == 0xb4) {
                u(0) = U();
                v(0) = V();
                u(last_x) = 0.0f;
                v(last_x) = V();
                u(first_y) = U();
                v(first_y) = 0.0f;
                u(corner) = 0.0f;
                v(corner) = 0.0f;
            } else if (rotation == 0x10e) {
                u(0) = 0.0f;
                v(0) = U();
                u(last_x) = 0.0f;
                v(last_x) = 0.0f;
                u(first_y) = V();
                v(first_y) = U();
                u(corner) = V();
                v(corner) = 0.0f;
            }

            // Interpolate u / v / alpha along the axis on which the corners differ.
            // fcomp + test ah,0x40 je: taken when equal or unordered.
            auto eq_or_nan = [](float a, float b) { return !(a < b) && !(a > b); };
            auto t_of = [](std::int32_t i, std::int32_t n) {
                return static_cast<float>(kFpu.div(static_cast<double>(i), static_cast<double>(n)));
            };
            for (std::int32_t r = 0; r < ny; ++r) {
                for (std::int32_t c = 0; c < nx; ++c) {
                    for (std::int32_t a = 4; a <= 5; ++a) {  // u, v
                        if (eq_or_nan(jo::as_vertex_f1(vtx(0))->attr[a], jo::as_vertex_f1(vtx(last_x))->attr[a])) {
                            const float t = t_of(r, ny - 1);
                            const float res = lerp(jo::as_vertex_f1(vtx(0))->attr[a], jo::as_vertex_f1(vtx(first_y))->attr[a], t);
                            jo::as_vertex_f1(vtx(idx(r, c)))->attr[a] = res;
                        } else if (eq_or_nan(jo::as_vertex_f1(vtx(0))->attr[a], jo::as_vertex_f1(vtx(first_y))->attr[a])) {
                            const float t = t_of(c, nx - 1);
                            const float res = lerp(jo::as_vertex_f1(vtx(0))->attr[a], jo::as_vertex_f1(vtx(last_x))->attr[a], t);
                            jo::as_vertex_f1(vtx(idx(r, c)))->attr[a] = res;
                        }
                    }
                    if (vcol1(vtx(0))[3] == vcol1(vtx(last_x))[3]) {
                        const float t = t_of(r, ny - 1);
                        const float b = static_cast<float>(vcol1(vtx(first_y))[3]);
                        const float a = static_cast<float>(vcol1(vtx(0))[3]);
                        const float res = lerp(a, b, t);
                        vcol1(vtx(idx(r, c)))[3] = static_cast<std::uint8_t>(ftol(static_cast<double>(res)));
                    } else if (vcol1(vtx(0))[3] == vcol1(vtx(first_y))[3]) {
                        const float t = t_of(c, nx - 1);
                        const float b = static_cast<float>(vcol1(vtx(last_x))[3]);
                        const float a = static_cast<float>(vcol1(vtx(0))[3]);
                        const float res = lerp(a, b, t);
                        vcol1(vtx(idx(r, c)))[3] = static_cast<std::uint8_t>(ftol(static_cast<double>(res)));
                    }
                }
            }

            joan::orig::as_plane(mesh)->add_triangles(static_cast<std::int32_t>(static_cast<std::uint32_t>(nx - 1) *
                                                             static_cast<std::uint32_t>(ny - 1) * 2u));  // 0x005ce6f0
            for (std::int32_t r = 0; r < ny - 1; ++r) {
                for (std::int32_t c = 0; c < nx - 1; ++c) {
                    const std::int32_t tri = static_cast<std::int32_t>(
                        static_cast<std::uint32_t>(c) * 2u +
                        static_cast<std::uint32_t>(nx - 1) * static_cast<std::uint32_t>(r) * 2u);
                    joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(tri), joan::orig::arg::u32(mat_index), joan::orig::arg::u32(idx(r, c)), joan::orig::arg::u32(idx(r, c) + 1), joan::orig::arg::u32(idx(r + 1, c)));  // 0x005ce800
                    joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(tri + 1), joan::orig::arg::u32(mat_index), joan::orig::arg::u32(idx(r, c) + 1), joan::orig::arg::u32(idx(r + 1, c) + 1), joan::orig::arg::u32(idx(r + 1, c)));
                }
            }
            joan::orig::as_plane(mesh)->set_format(2);  // 0x005cdc70
            joan::orig::as_plane(mesh)->upload();     // 0x005cfa10
            jo::as_plane(mesh)->origin[0] = px;
            jo::as_plane(mesh)->origin[1] = py;
            jo::as_plane(mesh)->origin[2] = pz;
            if (global<std::int32_t>(d::g_a52633) != 0) joan::orig::as_plane(mesh)->set_mode(2u);  // 0x005d7420
            joan::orig::as_plane(mesh)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
            joan::orig::as_plane(mesh)->sort_by_material(1);   // 0x005cf1d0
            joan::orig::as_scene_source(node)->set_38(joan::orig::arg::mptr(mesh));  // 0x005efbe0
            // [Graphics] Water=1 (not part of the original game): its surface, for the modern water pass.
            if (stricmp_(c_str(path), str(d::s_ice)) != 0) joan::render::water_register(mesh, {box.v[0], box.v[1], box.v[3], box.v[4], pz});
        }
        if (flag != 0) {
            const std::int32_t id = thiscall<std::int32_t>(joan::fn::List_add_546af0, obj(d::g_water_array), static_cast<char>(joan::orig::arg::i32(static_cast<std::int32_t>(b398))), static_cast<char>(joan::orig::arg::i32(0)), box, joan::orig::arg::bits(f394), joan::orig::arg::u32(effect), node);  // 0x00474df0
            void* const water = joan::orig::as_dynarray(obj(d::g_water_array))->ptr_at(joan::orig::arg::i32(id));  // 0x00425d90
            if (strlen_(b17c) != 0) joan::orig::as_water(water)->add_sound(joan::orig::arg::str(b17c));  // 0x00546bf0
            if (strlen_(b1a0) != 0) joan::orig::as_water(water)->add_sound(joan::orig::arg::str(b1a0));
        }
        return 1;
    }
};

#define B83(name, addr, fn) JOAN_REPLACE("game/b83_01", name, addr, fn)
B83("Cell::create_water", 0x004f5210, &H::create_water);
#undef B83

}  // namespace game_b83_01_2
}  // namespace

// ---------------------------------------- from modules/game_b82_10_3.cpp (game/b82_10)
namespace {
namespace game_b82_10_3 {

using joan::call::thiscall;

struct H {
    // 0x0054dba0: forwards to 0x0054dc20 (eax passed through).
    std::uint32_t forward_54dba0() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_sector(this)->destruct_base()); }  // 0x0054dc20
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_10", name, addr, fn)
B82("Obj54dba0::forward", 0x0054dba0, &H::forward_54dba0);
#undef B82

}  // namespace game_b82_10_3
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;


struct H {

    // ------------------------------------------------------------------------ 0x0054dbd0
    void* set_54dbd0(const char*, const void* src) {  // (name, box): the name is unused
        std::memcpy(reinterpret_cast<u8*>(this) + 8, src, 24);
        jo::self<jo::WorldSector>(this)->u32_52 = 0;
        jo::self<jo::WorldSector>(this)->u32_4e = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("Obj54d::set", 0x0054dbd0, &H::set_54dbd0);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b83_02.cpp (game/b83_02)
namespace {
namespace game_b83_02_3 {

using joan::call::thiscall;

struct H {
    // 0x0054dc20: forwards to the empty 0x0054dcc0 (eax passed through).
    std::uint32_t forward_54dc20() { joan::orig::as_sector(this)->destruct_empty(); return 0; }  // 0x0054dcc0 (void; destructor chain, result unused)
};

#define B83(name, addr, fn) JOAN_REPLACE("game/b83_02", name, addr, fn)
B83("Obj54dc20::forward", 0x0054dc20, &H::forward_54dc20);
#undef B83

}  // namespace game_b83_02_3
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

using u32 = std::uint32_t;


struct H {

    void* set_24(void* v) {  // 0x0054dc50
        jo::self<jo::WorldSector>(this)->owner = static_cast<jo::SceneS*>(v);  // the owner sector
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("X::set_24", 0x0054dc50, &H::set_24);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b105_07_3.cpp (game/b105_07)
namespace {
namespace game_b105_07_3 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {

    void nop_54dc80(u32, u32, const void*) {}  // (.., .., matrix)
    void nop_54dca0() {}

    // getters
    jo::WorldItem* get_5e() { return jo::self<jo::WorldSector>(this)->nodes_tail; }
    jo::WorldItem* get_62() { return jo::self<jo::WorldSector>(this)->nodes_head; }
};

jo::WorldItem* JOAN_CDECL get_4a(void* p) { return jo::as_world_item(p)->sector_prev; }  // 0x0054e000

#define B(name, addr, fn) JOAN_REPLACE("game/b105_07", name, addr, fn)
B("Obj::nop_54dc80", 0x0054dc80, &H::nop_54dc80);
B("Obj::nop_54dca0", 0x0054dca0, &H::nop_54dca0);
B("Obj::get_5e_54dfe0", 0x0054dfe0, &H::get_5e);
B("get_4a_54e000", 0x0054e000, &get_4a);
B("Obj::get_62_54e020", 0x0054e020, &H::get_62);
#undef B

}  // namespace game_b105_07_3
}  // namespace

// ---------------------------------------- from modules/game_b84_02_3.cpp (game/b84_02)
namespace {
namespace game_b84_02_3 {

struct H {
    // 0x0054dcc0 empty.
    void empty_54dcc0() {}
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("Obj54dcc0::empty", 0x0054dcc0, &H::empty_54dcc0);
#undef B84

}  // namespace game_b84_02_3
}  // namespace

// ---------------------------------------- from modules/game_b102_06_2.cpp (game/b102_06)
namespace {
namespace game_b102_06_2 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;


JOAN_COUNTER(c_attach, "Node::attach_54dce0");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // 0x0054dce0
    void attach_54dce0(u8* node) {
        c_attach.hit();
        u8* const me = self();
        u8* const old = jo::view<u8*>(jo::as_world_item(node)->sector);
        if (me == old) return;
        ++joan::en3d::g_scene_list_edits;  // (a sector's actor list: SceneS::draw_lists' layer lists)
        if (old != nullptr) {
            u8* const prev = jo::view<u8*>(jo::as_world_item(node)->sector_prev);
            u8* const next = jo::view<u8*>(jo::as_world_item(node)->sector_next);
            if (prev != nullptr)
                jo::view<u8*>(jo::as_world_item(prev)->sector_next) = next;
            else
                jo::view<u8*>(jo::as_world_sector(old)->nodes_head) = next;
            if (next != nullptr)
                jo::view<u8*>(jo::as_world_item(next)->sector_prev) = prev;
            else
                jo::view<u8*>(jo::as_world_sector(old)->nodes_tail) = prev;
            jo::view<u8*>(jo::as_world_item(node)->sector_prev) = nullptr;
            jo::view<u8*>(jo::as_world_item(node)->sector_next) = nullptr;
            jo::view<u8*>(jo::as_world_item(node)->sector) = nullptr;
        }
        if (me != nullptr) {
            u8* const next = jo::view<u8*>(jo::as_world_sector(me)->nodes_head);
            jo::view<u8*>(jo::as_world_sector(me)->nodes_head) = node;  // prev is always null: head insertion
            if (next != nullptr)
                jo::view<u8*>(jo::as_world_item(next)->sector_prev) = node;
            else
                jo::view<u8*>(jo::as_world_sector(me)->nodes_tail) = node;
            jo::view<u8*>(jo::as_world_item(node)->sector_prev) = nullptr;
            jo::view<u8*>(jo::as_world_item(node)->sector_next) = next;
            jo::view<u8*>(jo::as_world_item(node)->sector) = me;
        }
    }

    // 0x0054df60
    H* clear_54df60() {
        for (u8* p = jo::view<u8*>(jo::self<jo::WorldSector>(this)->nodes_head); p != nullptr;) {
            u8* const next = jo::view<u8*>(jo::as_world_item(p)->sector_next);
            jo::as_world_item(p)->sector_prev = nullptr;
            jo::as_world_item(p)->sector_next = nullptr;
            jo::as_world_item(p)->sector = nullptr;
            p = next;
        }
        jo::self<jo::WorldSector>(this)->nodes_head = nullptr;
        jo::self<jo::WorldSector>(this)->nodes_tail = nullptr;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_06", name, addr, fn)
B("Node::attach_54dce0", 0x0054dce0, &H::attach_54dce0);
B("Owner::clear_54df60", 0x0054df60, &H::clear_54df60);
#undef B

}  // namespace game_b102_06_2
}  // namespace

// ---------------------------------------- from modules/game_b85_06_2.cpp (game/b85_06)
namespace {
namespace game_b85_06_2 {


JOAN_COUNTER(c_unlink_54de20, "List54de20::unlink");

struct H {
    void unlink_54de20(std::uint8_t* n) {  // 0x0054de20
        c_unlink_54de20.hit();
        std::uint8_t* const owner = jo::view<std::uint8_t*>(jo::as_world_item(n)->sector);
        if (owner == nullptr) return;
        std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::as_world_item(n)->sector_prev);
        std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_world_item(n)->sector_next);
        if (prev != nullptr) jo::view<std::uint8_t*>(jo::as_world_item(prev)->sector_next) = next;
        else jo::view<std::uint8_t*>(jo::as_world_sector(owner)->nodes_head) = next;
        if (next != nullptr) jo::view<std::uint8_t*>(jo::as_world_item(next)->sector_prev) = prev;
        else jo::view<std::uint8_t*>(jo::as_world_sector(owner)->nodes_tail) = prev;
        jo::as_world_item(n)->sector_prev = nullptr;
        jo::as_world_item(n)->sector_next = nullptr;
        jo::as_world_item(n)->sector = nullptr;
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("List54de20::unlink", 0x0054de20, &H::unlink_54de20);
#undef B85

}  // namespace game_b85_06_2
}  // namespace

// ---------------------------------------- from modules/game_b90_02_2.cpp (game/b90_02)
namespace {
namespace game_b90_02_2 {

using joan::call::thiscall;

struct H {
    // 0x0054e080
    void* forward_43d340() { return joan::orig::arg::as<void*>(joan::orig::as_world(this)->release()); }  // 0x0043d340
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("forward_43d340", 0x0054e080, &H::forward_43d340);
#undef B

}  // namespace game_b90_02_2
}  // namespace

// ---------------------------------------- from modules/game_b103_06_2.cpp (game/b103_06)
#define M "game/b103_06"

namespace {
namespace game_b103_06_2 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;


// ---- getters (cdecl) ----
jo::WorldItem* JOAN_CDECL get_4e(const u8* p) { return jo::as_world_item(p)->sector_next; }

JOAN_REPLACE(M, "get_4e", 0x0054e040, &get_4e);

}  // namespace game_b103_06_2
}  // namespace
#undef M
