// game/unknown_f_work.cpp: functions whose original source file is unknown (formerly game/_unplaced/f_work.cpp;
// re/attribution.csv: between ww/f_shop.cpp and ww/flocksys.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

namespace {
// v3d "M" results (length, length_sq, dot_product): the replacements (src/lib/v3d/v3d87.cpp) return double. On x86
// the caller reads st0 as the float it always used (same code as before); x64 returns the double in xmm0, so it is
// read as a double there.
template <class... A>
float v3d_float(joan::img::Addr slot, A... args) {
#if UINTPTR_MAX == 0xffffffffu
    return joan::call::import_call<float>(slot, args...);
#else
    return static_cast<float>(joan::call::import_call<double>(slot, args...));
#endif
}
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using u32 = std::uint32_t;

using i32 = std::int32_t;

using u8 = std::uint8_t;

using joan::call::cdecl_call;

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;

using joan::hook::x87::Fpu;

// ---------------------------------------------------------------- shared game helpers
i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

void* const kUnits = obj(d::g_units);

using joan::call::import_call;

}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using joan::orig::helpers::memset_;

struct H {
    void* self() { return static_cast<void*>(this); }

    void* ctor_base(img::Addr vtable, std::uint32_t n) {
        joan::orig::as_facility(self())->construct();  // 0x0046d520
        jo::self<jo::Siege>(this)->vtable = img::obj(vtable);
        memset_(&jo::self<jo::Siege>(this)->i32_79, 0, n);
        return self();
    }
    // The Siege members after the Facility base (0x2e9b bytes at +0x79 on x86; life_bar is a pointer).
    void* ctor_469e90() { return ctor_base(d::vt_Obj_9d0980, static_cast<std::uint32_t>(sizeof(jo::Siege) - sizeof(jo::Facility))); }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Obj::ctor_469e90", 0x00469e90, &H::ctor_469e90);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b103_02.cpp (game/b103_02)
namespace {
namespace game_b103_02 {

struct Vec3 {
    u32 x, y, z;
};
static_assert(sizeof(Vec3) == 12, "by-value 12-byte argument");

void* const kTextures = obj(d::g_texture_array);

JOAN_COUNTER(c_life_bar, "Obj::create_life_bar");

using joan::orig::helpers::game_new;  // operator new
using joan::orig::helpers::game_delete;            // operator delete

// One 24-byte vertex copy + offsets as in 0x0046ba20 (rep movsd of 6 dwords, then float ops).
void quad_vertex(u8* verts, i32 dst, i32 src, float dx, bool dx_add, float dy, bool dy_add, u32 u, u32 v) {
    u32* const d = reinterpret_cast<u32*>(verts + dst * 0x18);
    const u32* const s = reinterpret_cast<const u32*>(verts + src * 0x18);
    for (i32 k = 0; k < 6; ++k) d[k] = s[k];
    float x = jo::view<float>(d[1]);
    x = dx_add ? x + dx : x - dx;
    jo::view<float>(d[1]) = x;
    float y = jo::view<float>(d[2]);
    y = dy_add ? y + dy : y - dy;
    jo::view<float>(d[2]) = y;
    d[4] = u;
    d[5] = v;
}

struct H {
    void* sdd_469ee0(u32 flags) {
        joan::orig::as_workshop_facility(this)->destruct();  // 0x00469f30
        if ((flags & 1u) != 0) game_delete(this);
        return this;
    }

    // ------------------------------------------------------------------ 0x0046a6b0 (dead)
    void set_7d(i32 v) {
        if (v > 0 && v <= 0xc) jo::view<i32>(jo::self<jo::Siege>(this)->u32_7d) = v;
    }

    // ------------------------------------------------------------------ 0x0046ba20
    std::uintptr_t create_life_bar() {
        c_life_bar.hit();
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Billboard)));  // 0x37 on x86
        void* const bar = mem != nullptr ? joan::orig::as_billboard(mem)->construct() : nullptr;  // 0x006351a0
        jo::view<void*>(jo::self<jo::Siege>(this)->life_bar) = bar;

        Vec3 pos = jo::view<Vec3>(jo::self<jo::Siege>(this)->pos);
        float z;
        std::memcpy(&z, &pos.z, 4);
        z = z + global<float>(d::flt_200);
        std::memcpy(&pos.z, &z, 4);

        u32 tmp = 0;
        u32* const c = joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(0xffu, 0xffu, 0xffu, 0xffu));  // 0x0042a260
        const u32 colour = *c;
        u32 q[3];
        std::memcpy(q, &pos, sizeof q);
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Siege>(this)->life_bar))->create(joan::orig::arg::u32(q[0]), joan::orig::arg::u32(q[1]), joan::orig::arg::u32(q[2]), 0x41800000u, 0x40000000u,
                 str(d::s_LifeBarre), joan::orig::arg::i16(0), joan::orig::arg::u32(colour), 0u, 0u, 0u, 0u, 6u, 0x20000u, 1u);  // 0x00635260

        u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Siege>(this)->life_bar))->plane());  // 0x00635e80
        jo::as_plane(m)->materials->raster = 0x10;

        const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kTextures)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kTextures)->set_mode(1u);                // 0x0060a740
        void* const tex = joan::orig::as_texture_manager(kTextures)->load(str(d::s_bar_workshop), 1, 0u);  // the texture (x64: was kept in a u32)  // 0x006099c0 "bar_workshop"
        joan::orig::as_texture_manager(kTextures)->set_mode(joan::orig::arg::u32(saved));  // 0x0060a740

        const i32 k = joan::orig::arg::as<i32>(joan::orig::as_plane(m)->add_materials(1));  // 0x005cd470
        {
            u8* const mats = jo::view<u8*>(jo::as_plane(m)->materials);
            joan::orig::as_material(&jo::as_plane(m)->materials[k])->assign(joan::orig::arg::mptr(mats));  // 0x005d53e0
        }
        joan::orig::as_material(&jo::as_plane(m)->materials[k])->set_texture(0, tex);  // 0x005d5650
        jo::as_plane(m)->materials[k].raster = 0x20;
        joan::orig::as_texture_manager(kTextures)->release(tex);  // 0x0060a550

        const float c3c = global<float>(d::flt_0_046875);
        const float c38 = global<float>(d::flt_0_1875);
        const float c34 = global<float>(d::flt_0_46875);
        const float c30 = global<float>(d::flt_0_171875);
        for (i32 pass = 0; pass < 2; ++pass) {
            const i32 n = joan::orig::arg::as<i32>(joan::orig::as_plane(m)->add_vertices(4));  // 0x005cd7f0
            quad_vertex(jo::view<u8*>(jo::as_plane(m)->vertices_f4), n + 0, 0, c3c, true, c38, false, 0u, 0u);
            quad_vertex(jo::view<u8*>(jo::as_plane(m)->vertices_f4), n + 1, 1, c3c, true, c34, true, 0u, 0x3f200000u);
            quad_vertex(jo::view<u8*>(jo::as_plane(m)->vertices_f4), n + 2, 2, c30, false, c38, false, 0x3f480000u, 0u);
            quad_vertex(jo::view<u8*>(jo::as_plane(m)->vertices_f4), n + 3, 3, c30, false, c34, true, 0x3f480000u, 0x3f200000u);
            const i32 t = joan::orig::arg::as<i32>(joan::orig::as_plane(m)->add_triangles(2));                   // 0x005ce6f0
            joan::orig::as_plane(m)->set_triangle(joan::orig::arg::i32(t), joan::orig::arg::u32(k), joan::orig::arg::u32(n), joan::orig::arg::u32(n + 1), joan::orig::arg::u32(n + 2));              // 0x005ce800
            joan::orig::as_plane(m)->set_triangle(joan::orig::arg::i32(t + 1), joan::orig::arg::u32(k), joan::orig::arg::u32(n + 2), joan::orig::arg::u32(n + 1), joan::orig::arg::u32(n + 3));      // 0x005ce800
        }
        joan::orig::as_plane(m)->sort_by_material(1);                                     // 0x005cf1d0
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Siege>(this)->life_bar))->set_19(0x3c23d70au);             // 0x00635db0
        return joan::orig::arg::as<u32>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Siege>(this)->life_bar))->set_15(0x3c23d70au));      // 0x00635d80
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_02", name, addr, fn)
B("Obj::sdd_469ee0", 0x00469ee0, &H::sdd_469ee0);
B("Obj::set_7d", 0x0046a6b0, &H::set_7d);
B("Obj::create_life_bar", 0x0046ba20, &H::create_life_bar);
#undef B

}  // namespace game_b103_02
}  // namespace

// ---------------------------------------- from modules/game_b101_02.cpp (game/b101_02)
namespace {
namespace game_b101_02 {

JOAN_COUNTER(c_remove_slot, "Obj::remove_slot");
JOAN_COUNTER(c_recruit, "Obj::parse_recruit_list");

i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
using joan::orig::helpers::crt_atoi;
const char* token(void* rd, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::as_file_txt(rd)->get_token(joan::orig::arg::i32(n))); }  // 0x00589c70
void read_line(void* rd, char* buf, i32 max) { joan::orig::as_file_txt(rd)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(max)); }   // 0x00589f10

struct H {
    // ------------------------------------------------------------------ 0x00469f30
    u32 dtor_469f30() {
        jo::self<jo::Siege>(this)->vtable = img::obj(d::vt_Obj_9d0980);
        joan::orig::as_workshop_facility(this)->release();              // 0x0046a010
        return joan::orig::arg::as<u32>(joan::orig::as_facility(this)->destruct());  // 0x0046d5b0
    }

    // ------------------------------------------------------------------ 0x0046a980
    u8 remove_slot(i32 id) {
        c_remove_slot.hit();
        if (id == 0 && static_cast<i32>(jo::self<jo::Siege>(this)->n_slots) != 0) {
            jo::self<jo::Siege>(this)->n_slots = static_cast<i16>(jo::view<u16>(jo::self<jo::Siege>(this)->n_slots) - 1);
            jo::self<jo::Siege>(this)->slots[static_cast<i32>(jo::self<jo::Siege>(this)->n_slots)] = 0;
        } else {
            if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
            i16 idx = -1;
            for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Siege>(this)->n_slots); ++i) {
                if (jo::self<jo::Siege>(this)->slots[i] == id) idx = static_cast<i16>(i);
            }
            if (static_cast<i32>(idx) == -1) return 0;
            jo::self<jo::Siege>(this)->slots[static_cast<i32>(idx)] = jo::self<jo::Siege>(this)->slots[static_cast<i32>(jo::self<jo::Siege>(this)->n_slots) - 1];
            jo::self<jo::Siege>(this)->n_slots = static_cast<i16>(jo::view<u16>(jo::self<jo::Siege>(this)->n_slots) - 1);
        }
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Siege>(this)->n_types); ++i) {
            if (!(static_cast<i32>(jo::self<jo::Siege>(this)->type_need[i]) > static_cast<i32>(jo::self<jo::Siege>(this)->n_slots))) continue;
            while (static_cast<i32>(static_cast<i8>(
                       joan::orig::arg::as<u8>(joan::orig::as_workshop_facility(this)->take_item(static_cast<i32>(jo::self<jo::Siege>(this)->type_kind[i]))))) != 0) {  // 0x0046ae20
            }
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x0046b840
    u32 parse_recruit_list(void* rd) {
        c_recruit.hit();
        struct {
            char pad[4];   // [ebp-0x10c] (dead): target of buf[-1] on an empty line
            char buf[0x101];  // [ebp-0x108], NUL of read_line at buf[0x100]
        } s;
        if (crt_stricmp(token(rd, 0), str(d::s_RECRUIT_LIST)) != 0) return 1;  // "RECRUIT_LIST"
        token(rd, 1);
        read_line(rd, s.buf, 0x100);
        {
            const u32 len = crt_strlen(s.buf);
            reinterpret_cast<char*>(&s)[4 + static_cast<i32>(len) - 1] = 0;
        }
        const i32 grp = joan::orig::as_table(obj(d::g_recruit_res))->find_b4(joan::orig::arg::str(static_cast<char*>(s.buf)));  // 0x004cad00 (a group id)
        if (grp != 0) joan::orig::as_workshop_facility(this)->load_types(grp);  // 0x0046b730
        token(rd, 1);
        read_line(rd, s.buf, 0x100);
        {
            const u32 len = crt_strlen(s.buf);
            reinterpret_cast<char*>(&s)[4 + static_cast<i32>(len) - 1] = 0;
        }
        if (crt_stricmp(s.buf, str(d::s_NORMAL)) == 0) {  // "NORMAL"
            jo::self<jo::Siege>(this)->i32_79 = 0;
        } else if (crt_stricmp(s.buf, str(d::s_IRON)) == 0) {  // "IRON"
            jo::self<jo::Siege>(this)->i32_79 = 1;
        } else if (crt_stricmp(s.buf, str(d::s_WOOD)) == 0) {  // "WOOD"
            jo::self<jo::Siege>(this)->i32_79 = 2;
        }
        token(rd, 1);
        const char* const t = token(rd, 1);
        jo::self<jo::Siege>(this)->i32_2f0a = crt_atoi(t);
        return 1;
    }
};

#define B101(name, addr, fn) JOAN_REPLACE("game/b101_02", name, addr, fn)
B101("Obj::dtor_469f30", 0x00469f30, &H::dtor_469f30);
B101("Obj::remove_slot", 0x0046a980, &H::remove_slot);
B101("Obj::parse_recruit_list", 0x0046b840, &H::parse_recruit_list);
#undef B101

}  // namespace game_b101_02
}  // namespace

// ---------------------------------------- from modules/game_b104_02.cpp (game/b104_02)
namespace {
namespace game_b104_02 {

struct H {
    // ------------------------------------------------------------------ 0x00469fa0 / 0x004803c0
    u32 reset_469fa0(u32 a, const char* b) {
        joan::orig::as_facility(this)->set_kind_name(joan::orig::arg::i16(a), b);  // 0x0046d5e0
        jo::view<u16>(jo::self<jo::Siege>(this)->current) = 0xffff;
        jo::self<jo::Siege>(this)->u32_7d = 0xc;
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_workshop_facility(this)->create_life_bar());  // 0x0046ba20
        jo::self<jo::Siege>(this)->u8_2f0e = 0;
        jo::self<jo::Siege>(this)->u8_2f0f = 0;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_02", name, addr, fn)
B("Obj::reset_469fa0", 0x00469fa0, &H::reset_469fa0);
#undef B

}  // namespace game_b104_02
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02 {

JOAN_COUNTER(c_take_item, "Obj::take_item");

struct H {
    // ------------------------------------------------------------------ 0x0046a010
    u32 call_46d630_46c3e0() {
        joan::orig::as_facility(this)->detach();              // 0x0046d630
        return joan::orig::arg::as<u32>(joan::orig::as_workshop_facility(this)->destroy_life_bar());  // 0x0046c3e0
    }

    // ------------------------------------------------------------------ 0x0046ad90
    u32 remove_member(i32 id) {
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_talk(this)->get_msg(joan::orig::arg::i32(id)));                                   // 0x004e3fe0
        (void)joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(static_cast<i32>(jo::as_treaty(e)->to));     // 0x0047b600
        const i32 side = global<i32>(d::g_player_side);
        if (static_cast<i32>(jo::as_treaty(e)->to) == side || static_cast<i32>(jo::as_treaty(e)->from) == side) {
            joan::orig::as_news_list(obj(d::g_tasks))->remove_match(1, joan::orig::arg::i32(id), 0);  // 0x004b95f0
            joan::orig::as_news_list(obj(d::g_tasks))->remove_match(1, joan::orig::arg::i32(id), 1);
        }
        return thiscall<u32>(fn::linkout_5858c0, &jo::self<jo::Talk>(this)->msgs, id);  // 0x005858c0
    }

    // ------------------------------------------------------------------ 0x0046ae20
    u8 take_item(i32 id) {
        c_take_item.hit();
        i16 slot = -1;
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Siege>(this)->n_types); ++i) {
            if (static_cast<i32>(jo::self<jo::Siege>(this)->type_kind[i]) == id) {
                slot = static_cast<i16>(i);
                break;
            }
        }
        if (static_cast<i32>(slot) == -1) return 0;
        if (jo::self<jo::Siege>(this)->type_stock[static_cast<i32>(slot)] == 0) return 0;
        u32 best = 0;
        i16 bi = -1;
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Siege>(this)->n_queue); ++i) {
            if (static_cast<i32>(jo::self<jo::Siege>(this)->queue[i].kind) == id && best < jo::self<jo::Siege>(this)->queue[i].queued_time) {
                bi = static_cast<i16>(i);
                best = jo::self<jo::Siege>(this)->queue[i].queued_time;
            }
        }
        if (static_cast<i32>(bi) == -1) return 0;
        if (static_cast<i32>(jo::self<jo::Siege>(this)->queue[static_cast<i32>(bi)].started) != 0) jo::self<jo::Siege>(this)->current = -1;
        if (static_cast<i32>(jo::self<jo::Siege>(this)->current) >= 0 &&
            static_cast<i32>(jo::self<jo::Siege>(this)->current) == static_cast<i32>(jo::self<jo::Siege>(this)->n_queue) - 1)
            jo::self<jo::Siege>(this)->current = bi;
        std::memmove(&jo::self<jo::Siege>(this)->queue[static_cast<i32>(bi)],
                     &jo::self<jo::Siege>(this)->queue[static_cast<i32>(jo::self<jo::Siege>(this)->n_queue) - 1], sizeof(jo::SiegeOrder));
        jo::self<jo::Siege>(this)->type_stock[static_cast<i32>(slot)] = jo::self<jo::Siege>(this)->type_stock[static_cast<i32>(slot)] - 1;
        jo::self<jo::Siege>(this)->n_queue = static_cast<i16>(jo::self<jo::Siege>(this)->n_queue - 1);
        return 1;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("Obj::call_46d630_46c3e0", 0x0046a010, &H::call_46d630_46c3e0);
B100("Obj::remove_member", 0x0046ad90, &H::remove_member);
B100("Obj::take_item", 0x0046ae20, &H::take_item);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

struct V3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers
    float x, y, z;
};

JOAN_COUNTER(c_siege, "Siege::update_46a040");

struct H {
    // -------------------------------------------------------------- 0x0046a040
    jo::SiegeOrder& cur() { return jo::self<jo::Siege>(this)->queue[jo::self<jo::Siege>(this)->current]; }

    std::uintptr_t siege_update() {
        c_siege.hit();
        if (jo::self<jo::Siege>(this)->current < 0) {
            if (jo::self<jo::Siege>(this)->n_queue == 0) return 0;
            i16 best = -1;
            u32 oldest = jo::self<jo::Siege>(this)->queue[0].queued_time;
            for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Siege>(this)->n_queue); ++i) {
                if (!(jo::self<jo::Siege>(this)->queue[i].queued_time > oldest)) {
                    oldest = jo::self<jo::Siege>(this)->queue[i].queued_time;
                    best = static_cast<i16>(i);
                }
            }
            if (best == -1) return 0xffffffffu;
            jo::self<jo::Siege>(this)->current = best;
            jo::self<jo::Siege>(this)->queue[best].started = 1;
            jo::self<jo::Siege>(this)->queue[best].start_time = global<u32>(d::g_now);
            if (joan::orig::arg::as<i8>(joan::orig::as_sound(obj(d::g_sfx_player))->is_playing(str(d::s_RTS_SIEGE_009))) == 0) {      // 0x00473140
                joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_RTS_SIEGE_008));            // 0x004ceef0
            }
            return joan::orig::arg::as<u32>(joan::orig::as_workshop_facility(this)->show_life_bar());  // 0x0046c4d0
        }
        const u32 due = cur().start_time + static_cast<u32>(static_cast<i32>(cur().duration));
        if (due > global<u32>(d::g_now)) return joan::orig::arg::as<u32>(joan::orig::as_workshop_facility(this)->show_life_bar());  // 0x0046c4d0
        joan::orig::as_workshop_facility(this)->hide_life_bar();  // 0x0046c460
        i16 slot = -1;
        for (i32 j = 0; j < static_cast<i32>(jo::self<jo::Siege>(this)->n_types); ++j) {
            if (static_cast<i32>(jo::self<jo::Siege>(this)->type_kind[j]) == static_cast<i32>(cur().kind)) {
                slot = static_cast<i16>(j);
                break;
            }
        }
        if (slot < 0) return reinterpret_cast<std::uintptr_t>(this);
        void* const units = obj(d::g_units);
        const u32 uid = joan::orig::arg::as<u32>(joan::orig::as_units(units)->create_51dfc0(static_cast<i32>(cur().kind)));  // 0x0051dfc0
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(uid)));  // 0x0041b3c0
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(cur().kind))));  // 0x0042d0a0
        joan::orig::as_unit(unit)->set_level_state(static_cast<i32>(jo::self<jo::UnitType>(type)->rank));  // 0x005089e0
        jo::view<u16>(jo::as_unit(unit)->layer) = 1;
        joan::orig::as_unit(unit)->set_group(global<jo::World*>(d::g_world));                        // 0x004fed00
        joan::orig::as_unit(unit)->set_side(static_cast<i32>(jo::view<i16[2]>(jo::self<jo::Siege>(this)->owner->nation)[0]));  // 0x00509250
        jo::view<u16>(jo::as_unit(unit)->group_id) = cur().group;
        i16* const g = joan::orig::arg::as<i16*>(joan::orig::as_list(units)->find_by_31(static_cast<i32>(jo::as_unit(unit)->group_id)));  // 0x0051fe60
        if (g != nullptr) joan::orig::as_unit(unit)->link_to(static_cast<i32>(*g));  // 0x00438f80
        V3 pos{0, 0, 0};
        float angle = 0.0f;
        if (joan::orig::arg::as<i8>(joan::orig::as_workshop_facility(this)->find_exit_spot(&pos, &angle, joan::orig::arg::u32(uid))) == 0) {  // 0x0046b150
            return joan::orig::arg::as<u32>(joan::orig::as_units(units)->remove_51e1f0(joan::orig::arg::i32(uid)));              // 0x0051e1f0
        }
        joan::orig::as_ground_object(unit)->set_position(pos);    // 0x004fedf0
        joan::orig::as_unit(unit)->set_target(joan::orig::arg::f32(angle));  // 0x004ff870
        joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(angle / global<float>(d::flt_0_017453292)));  // 0x0050abd0
        if (jo::self<jo::Siege>(this)->order_arg != 0) {
            if (jo::as_unit(unit)->orders == nullptr) {
                void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Order)));  // 0x3e on x86
                void* const o = mem != nullptr ? joan::orig::as_order(mem)->construct() : nullptr;  // 0x004bab50
                jo::as_unit(unit)->orders = static_cast<jo::Order*>(o);
            }
            joan::orig::as_order(jo::view<u8*>(jo::as_unit(unit)->orders))->add(unit, joan::orig::arg::i32(jo::self<jo::Siege>(this)->order_arg), joan::orig::arg::i32(jo::self<jo::Siege>(this)->order_b), joan::orig::arg::i32(// 0x004bac60
                          jo::self<jo::Siege>(this)->order_c), joan::orig::arg::i32(jo::self<jo::Siege>(this)->order_d), joan::orig::arg::i32(jo::self<jo::Siege>(this)->order_e), joan::orig::arg::i32(jo::self<jo::Siege>(this)->order_f), joan::orig::arg::i32(global<u32>(d::g_now) + jo::self<jo::Siege>(this)->delay), i32{0}, i32{0});
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_scenario(obj(d::g_mission))->mode()) == 2) jo::view<u32>(jo::as_unit(unit)->formation_slot) = 2;  // 0x004cd510
        jo::self<jo::Siege>(this)->n_queue = static_cast<i16>(jo::self<jo::Siege>(this)->n_queue - 1);
        std::memmove(&cur(), &jo::self<jo::Siege>(this)->queue[jo::self<jo::Siege>(this)->n_queue], sizeof(jo::SiegeOrder));
        jo::self<jo::Siege>(this)->type_stock[slot] = jo::self<jo::Siege>(this)->type_stock[slot] - 1;
        jo::self<jo::Siege>(this)->type_made[slot] = jo::self<jo::Siege>(this)->type_made[slot] + 1;
        jo::self<jo::Siege>(this)->current = -1;
        const char* const x = joan::orig::as_txt_res(obj(d::g_strings))->workshop_text_17();                 // 0x004ddff0
        joan::orig::as_sms(obj(d::g_sms))->add(x, i32{0}, i32{0});           // 0x006e5700
        return joan::orig::arg::as<u32>(joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_RTS_SIEGE_009)));     // 0x004ceef0
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("Siege::update_46a040", 0x0046a040, &H::siege_update);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

JOAN_COUNTER(c_sync_colours, "Obj::sync_colours");

struct H {
    // ------------------------------------------------------------------ 0x0046a560
    void sync_colours() {
        c_sync_colours.hit();
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::LightSys>(this)->current.light_count); ++i) {
            const jo::LightDef* const o = &jo::self<jo::LightSys>(this)->current.lights[i];
            u8* const dst = jo::view<u8*>(jo::self<jo::LightSys>(this)->lights[i]);
            jo::view<u32>(jo::as_light(dst)->orbit_a) = jo::view<u32>(o->angle_a);
            jo::view<u32>(jo::as_light(jo::self<jo::LightSys>(this)->lights[i])->orbit_b) = jo::view<u32>(o->angle_b);
            const i32 a = joan::hook::x87::ftol(static_cast<double>(o->colour[3]));
            const i32 bb = joan::hook::x87::ftol(static_cast<double>(o->colour[2]));
            const i32 g = joan::hook::x87::ftol(static_cast<double>(o->colour[1]));
            const i32 r = joan::hook::x87::ftol(static_cast<double>(o->colour[0]));
            u32 colour = 0;
            const u32* const c = joan::orig::arg::as<u32*>(joan::orig::as_colour(&colour)->construct(r, g, bb, a));  // 0x0042a260
            jo::view<u32>(jo::as_light(jo::self<jo::LightSys>(this)->lights[i])->colour) = *c;
            jo::as_light(jo::self<jo::LightSys>(this)->lights[i])->active_ = jo::view<i8>(o->enabled) != 0 ? 1 : 0;
            joan::orig::as_light(jo::self<jo::LightSys>(this)->lights[i])->invalidate();  // 0x005c5c00
        }
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::sync_colours", 0x0046a560, &H::sync_colours);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

JOAN_COUNTER(c_add_type, "Workshop::add_type");

i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

struct H {
    // ------------------------------------------------------------------ 0x0046a6f0
    u8 add_type(i32 id, i32 a, i32 b, i32 c) {
        c_add_type.hit();
        if (static_cast<i32>(jo::self<jo::Siege>(this)->n_types) == 4) return 0;
        if (id == 0) return 0;
        i16 slot = -1;  // ebp-0x8
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Siege>(this)->n_types); ++i) {
            if (static_cast<i32>(jo::self<jo::Siege>(this)->type_kind[i]) == id) {
                slot = static_cast<i16>(i);
                break;
            }
        }
        if (static_cast<i32>(slot) == -1) {
            const i32 n = static_cast<i32>(jo::self<jo::Siege>(this)->n_types);
            jo::view<u16>(jo::self<jo::Siege>(this)->type_kind[n]) = static_cast<u16>(id);
            jo::view<u16>(jo::self<jo::Siege>(this)->type_cost[n]) = static_cast<u16>(b);
            jo::view<u16>(jo::self<jo::Siege>(this)->type_need[n]) = static_cast<u16>(a);
            jo::self<jo::Siege>(this)->type_rate[n] = sx16(c);
            jo::view<u16>(jo::self<jo::Siege>(this)->n_types) = static_cast<u16>(jo::view<u16>(jo::self<jo::Siege>(this)->n_types) + 1u);
        } else {
            const i32 n = static_cast<i32>(slot);
            jo::view<u16>(jo::self<jo::Siege>(this)->type_kind[n]) = static_cast<u16>(id);
            jo::view<u16>(jo::self<jo::Siege>(this)->type_cost[n]) = static_cast<u16>(b);
            jo::view<u16>(jo::self<jo::Siege>(this)->type_need[n]) = static_cast<u16>(a);
            jo::self<jo::Siege>(this)->type_rate[n] = sx16(c);
        }
        return 1;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Workshop::add_type", 0x0046a6f0, &H::add_type);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
namespace {
namespace game_b102_02 {

JOAN_COUNTER(c_area_free, "Obj::area_free");
JOAN_COUNTER(c_time_left, "Obj::time_left");

void* const kItems = obj(d::g_units);

void* list_get(const void* list, i32 i) { return joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0

struct H {
    // 0x0046a8a0
    u8 add_slot(u32 id) {
        if (static_cast<i32>(jo::self<jo::Siege>(this)->n_slots) == 8) return 0;
        if (joan::orig::as_dynarray(kItems)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Siege>(this)->n_slots); ++i)
            if (jo::view<u32>(jo::self<jo::Siege>(this)->slots[i]) == id) return 0;
        jo::view<u32>(jo::self<jo::Siege>(this)->slots[static_cast<i32>(jo::self<jo::Siege>(this)->n_slots)]) = id;
        jo::self<jo::Siege>(this)->n_slots = static_cast<i16>(jo::self<jo::Siege>(this)->n_slots + 1);
        return 1;
    }

    // 0x0046b080
    i32 area_free() {
        c_area_free.hit();
        for (i32 i = list_count(kItems); i > 0; --i) {
            u8* const e = static_cast<u8*>(list_get(kItems, i));
            if (e == nullptr) continue;
            // fcomp; test ah,1 / jne = below or unordered -> next; test ah,0x41 / je = above -> next.
            if (!(jo::view<float[3]>(jo::self<jo::Unit>(e)->position_)[0] >= jo::self<jo::Siege>(this)->bounds[0])) continue;
            if (jo::view<float[3]>(jo::self<jo::Unit>(e)->position_)[0] > jo::self<jo::Siege>(this)->bounds[3]) continue;
            if (!(jo::view<float[3]>(jo::self<jo::Unit>(e)->position_)[1] >= jo::self<jo::Siege>(this)->bounds[1])) continue;
            if (jo::view<float[3]>(jo::self<jo::Unit>(e)->position_)[1] > jo::self<jo::Siege>(this)->bounds[4]) continue;
            return 0;
        }
        return 1;
    }

    // 0x0046b600
    double time_left() {
        c_time_left.hit();
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->actor) == nullptr) return static_cast<double>(global<float>(d::k_9cf5c4));
        u8* const t = jo::view<u8*>(jo::self<jo::MapObject>(this)->actor);
        const u32 dt = global<u32>(d::g_now) - jo::as_actor(t)->start_time;
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::MapObject>(this)->type_id))));  // 0x00425ce0
        const i32 v = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_anim_def(jo::view<void*>(jo::self<jo::MapObject>(this)->actor->anim))->lookup_word(joan::orig::arg::str(jo::as_item_def(rec)->anim_names[jo::view<i32>(jo::self<jo::MapObject>(this)->busy)]), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), jo::view<void*>(jo::self<jo::MapObject>(this)->actor))));  // 0x005b4a00
        void* const a1 = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::view<void*>(jo::self<jo::MapObject>(this)->actor->anim))->frame(joan::orig::arg::i32(v), 0u));  // 0x005b5dd0
        const double r = joan::orig::arg::as<double>(joan::orig::as_range(a1)->span());                                // 0x0042d130
        if (!(static_cast<double>(dt) >= r)) {  // fild qword (zero-extended) / fcompp: dt < r or unordered
            void* const a2 = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::view<void*>(jo::self<jo::MapObject>(this)->actor->anim))->frame(joan::orig::arg::i32(v), 0u));
            const double r2 = joan::orig::arg::as<double>(joan::orig::as_range(a2)->span());
            return Fpu{true}.sub(r2, static_cast<double>(static_cast<i32>(dt)));  // fisub dword
        }
        return static_cast<double>(global<float>(d::k_9cf5c4));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("Obj::add_slot", 0x0046a8a0, &H::add_slot);
B("Obj::area_free", 0x0046b080, &H::area_free);
B("Obj::time_left", 0x0046b600, &H::time_left);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

JOAN_COUNTER(c_news_add, "News::add");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
using joan::orig::helpers::strcpy_;

struct H {
    // ------------------------------------------------------------------ 0x0046ab40
    bool news_add(i32 id, i32 w, const char* s1, const char* s2) {
        c_news_add.hit();
        if (static_cast<i32>(jo::self<jo::Siege>(this)->n_queue) == 0x50) return false;
        const i32 player = global<i16>(d::g_a47c36);
        if (unit_missing(player) != 0) return false;
        unit_get(static_cast<i32>(global<i16>(d::g_a47c36)));
        i16 idx = -1;
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Siege>(this)->n_types); ++i) {
            if (static_cast<i32>(jo::self<jo::Siege>(this)->type_kind[i]) == id) {
                idx = static_cast<i16>(i);
                break;
            }
        }
        if (static_cast<i32>(idx) == -1) return false;
        if (static_cast<i32>(jo::self<jo::Siege>(this)->n_slots) < static_cast<i32>(jo::self<jo::Siege>(this)->type_need[idx])) return false;
        if (jo::self<jo::Siege>(this)->type_stock[idx] >= 0x14) return false;
        u32 t = global<u32>(d::g_now);
        if (!(jo::self<jo::Siege>(this)->time > t)) t = jo::self<jo::Siege>(this)->time + 1;
        auto rec = [&]() -> jo::NewsRecord& { return jo::view<jo::NewsRecord>(jo::self<jo::Siege>(this)->queue[jo::self<jo::Siege>(this)->n_queue]); };
        rec().read = 0;
        strcpy_(rec().text1, s1);
        strcpy_(rec().text2, s2);
        rec().id = static_cast<u16>(id);
        rec().time = t;
        rec().weight = static_cast<u16>(w);
        rec().icon = jo::view<u16[2]>(jo::self<jo::Siege>(this)->type_rate[idx])[0];
        jo::self<jo::Siege>(this)->type_stock[idx] = jo::self<jo::Siege>(this)->type_stock[idx] + 1;
        jo::view<u16>(jo::self<jo::Siege>(this)->n_queue) = static_cast<u16>(jo::view<u16>(jo::self<jo::Siege>(this)->n_queue) + 1);
        jo::self<jo::Siege>(this)->time = t;
        return true;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("News::add", 0x0046ab40, &H::news_add);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using joan::hook::x87::ftol;

u32 bits(float v) {
    u32 r = 0;
    std::memcpy(&r, &v, 4);
    return r;
}

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kKinds = obj(d::g_types);

JOAN_COUNTER(c_exit_spot, "Building::find_exit_spot");

struct H {
    // ------------------------------------------------------------------ 0x0046b150
    u32 find_exit_spot(Vec3* out, float* ang, u32 id) {
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70 (0/1)
        c_exit_spot.hit();
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        const u8* const kind = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
        float r = 0.0f;  // [ebp-0x10]
        switch (jo::view<i32>(jo::self<jo::UnitType>(kind)->shape_kind)) {
            case 1: r = jo::self<jo::UnitType>(kind)->shape_sphere[3]; break;
            case 2: {
                const Vec3 a{jo::self<jo::UnitType>(kind)->shape_box[0], jo::self<jo::UnitType>(kind)->shape_box[1], 0.0f};
                const Vec3 b{jo::self<jo::UnitType>(kind)->shape_box[3], jo::self<jo::UnitType>(kind)->shape_box[4], 0.0f};
                Vec3 tmp{0.0f, 0.0f, 0.0f};  // [ebp-0x5c]
                const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &a, &b));  // 0x0060cc90
                const double len = import_call<double>(d::iat_v3d_87_length, &d);       // v3d length
                const Fpu fpu{true};
                r = static_cast<float>(fpu.div(len, static_cast<double>(global<float>(d::flt_2))));
                break;
            }
            case 3: r = jo::self<jo::UnitType>(kind)->shape_cylinder[3]; break;
            default: break;
        }
        const float c = global<float>(d::flt_200);  // 200
        out->x = jo::self<jo::Facility>(this)->bounds[0] - c;
        out->y = jo::self<jo::Facility>(this)->bounds[1] - c - r;
        jo::view<u32>(ang[0]) = 0x4096cbe4u;
        i16 state = 0;  // [ebp-0x2c]
        while (state < 4) {
            u8* const map = jo::view<u8*>(jo::self<jo::Facility>(this)->owner->cell);
            if (map != nullptr) {
                u8* const cell = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(map)->grid(joan::orig::arg::i16(0)));  // 0x005600c0
                if (cell != nullptr) {
                    const Fpu fpu{true};
                    const float fx = static_cast<float>(fpu.div(
                        fpu.sub(static_cast<double>(out->x), static_cast<double>(jo::as_world_cell(map)->origin_x)),
                        static_cast<double>(jo::as_path_space(cell)->cell_w)));
                    const float fy = static_cast<float>(fpu.div(
                        fpu.sub(static_cast<double>(out->y), static_cast<double>(jo::as_world_cell(map)->origin_y)),
                        static_cast<double>(jo::as_path_space(cell)->cell_h)));
                    const i32 ix = ftol(static_cast<double>(fx));
                    const i32 iy = ftol(static_cast<double>(fy));
                    const i32 even = ((static_cast<u32>(ix) + static_cast<u32>(iy)) & 1u) == 0 ? 1 : 0;
                    out->z = joan::orig::arg::as<float>(joan::orig::as_terrain(jo::view<void*>(jo::as_path_space(cell)->grid))->height_4c7b70(joan::orig::arg::f32(fx), joan::orig::arg::f32(fy), static_cast<char>(joan::orig::arg::i32(even))));  // 0x004c7b70
                } else {
                    out->z = jo::self<jo::Facility>(this)->pos[2];
                }
            } else {
                out->z = jo::self<jo::Facility>(this)->pos[2];
            }
            joan::orig::as_ground_object(unit)->set_position(*out);              // 0x004fedf0
            joan::orig::as_unit(unit)->set_target(joan::orig::arg::f32(jo::view<u32>(ang[0])));   // 0x004ff870
            joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(*ang / global<float>(d::flt_0_017453292)));  // 0x0050abd0
            if ((joan::orig::arg::as<u32>(joan::orig::as_formation(obj(d::g_collision_grid))->can_place(joan::orig::arg::i32(id), 1, joan::orig::arg::i32(bits(r)))) & 0xffu) != 0) break;  // 0x006932d0
            const float m = c > r ? 200.0f : r;  // fcomp + test ah,0x41 jne: 200 only when 200 > r
            switch (state) {
                case 0:
                    out->x = out->x + c;
                    out->y = jo::self<jo::Facility>(this)->bounds[1] - m;
                    jo::view<u32>(ang[0]) = 0x4096cbe4u;
                    if (!(jo::self<jo::Facility>(this)->bounds[3] + c >= out->x)) {  // test ah,1 je: taken unless below / NaN
                        out->x = jo::self<jo::Facility>(this)->bounds[3] + c;
                        state = 1;
                    }
                    break;
                case 1:
                    out->y = out->y + c;
                    out->x = m + jo::self<jo::Facility>(this)->bounds[3];
                    jo::view<u32>(ang[0]) = 0;
                    if (!(jo::self<jo::Facility>(this)->bounds[4] + c >= out->y)) {
                        out->y = jo::self<jo::Facility>(this)->bounds[4] + c;
                        state = 2;
                    }
                    break;
                case 2:
                    out->x = out->x - c;
                    out->y = m + jo::self<jo::Facility>(this)->bounds[4];
                    jo::view<u32>(ang[0]) = 0x3fc90fdbu;
                    if (jo::self<jo::Facility>(this)->bounds[0] - c > out->x) {
                        out->x = jo::self<jo::Facility>(this)->bounds[0] - c;
                        state = 3;
                    }
                    break;
                case 3:
                    out->y = out->y - c;
                    out->x = jo::self<jo::Facility>(this)->bounds[0] - m;
                    jo::view<u32>(ang[0]) = 0x40490fdbu;
                    if (jo::self<jo::Facility>(this)->bounds[1] - c > out->y) {
                        out->y = jo::self<jo::Facility>(this)->bounds[1] - c;
                        state = 4;
                    }
                    break;
                default: break;
            }
        }
        return state < 4 ? 1u : 0u;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Building::find_exit_spot", 0x0046b150, &H::find_exit_spot);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
namespace {
namespace game_b99_02 {

using joan::orig::helpers::u;  // eax residue as a full pointer

JOAN_COUNTER(c_load_group, "Obj::load_group");

struct H {
    // ------------------------------------------------------------------ 0x0046b730
    std::uintptr_t load_group(i32 id) {
        c_load_group.hit();
        u8* const grp = joan::orig::arg::as<u8*>(joan::orig::as_recruit_res(obj(d::g_recruit_res))->group_at(joan::orig::arg::u32(id)));  // 0x00469520 (low word)
        if (jo::view<void*>(jo::as_recruit_entry(grp)->rows) == nullptr) return u(grp);
        jo::self<jo::Siege>(this)->group_id = id;
        jo::self<jo::Siege>(this)->n_types = 0;
        i32 n = 0;
        for (i32 i = 1; i <= (n = list_count(jo::view<void*>(jo::as_recruit_entry(grp)->rows))); ++i) {
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::as_recruit_entry(grp)->rows))->get(joan::orig::arg::i32(i)));  // 0x00417200
            const float prod = jo::as_recruit_row_item(rec)->rate * global<float>(d::flt_1000);
            const i32 v = joan::hook::x87::ftol(static_cast<double>(prod));
            u8* const res = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_recruit_row_item(rec)->kind))));  // 0x0042d0a0
            joan::orig::as_workshop_facility(this)->add_type(static_cast<i32>(jo::as_recruit_row_item(rec)->kind), static_cast<i32>(jo::as_recruit_row_item(rec)->count), static_cast<i32>(jo::view<i16[2]>(jo::self<jo::UnitType>(res)->cost)[0]), joan::orig::arg::i32(v));  // 0x0046a6f0
        }
        jo::self<jo::Siege>(this)->n_queue = 0;
        return static_cast<u32>(n);
    }

    // ------------------------------------------------------------------ 0x0046c3e0
    std::uintptr_t delete_2f10() {
        if (jo::view<void*>(jo::self<jo::Siege>(this)->life_bar) == nullptr) return u(this);
        joan::orig::as_workshop_facility(this)->hide_life_bar();  // 0x0046c460
        void* const p = jo::view<void*>(jo::self<jo::Siege>(this)->life_bar);
        if (p != nullptr) thiscall<u32>(static_cast<const std::uintptr_t*>(jo::self<jo::Billboard>(p)->vtable)[0], p, 1);  // deleting dtor
        jo::self<jo::Siege>(this)->life_bar = nullptr;
        return u(this);  // mov eax,[ebp-4] before the final store
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::load_group", 0x0046b730, &H::load_group);
B99("Obj::delete_2f10", 0x0046c3e0, &H::delete_2f10);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02 {

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_target_a, "Unit::nearest_target_a");

bool target_better_flag(void* self, void* cand, const Vec3* spos, float* best, void* info) {
    Vec3 cpos;
    joan::orig::as_unit(cand)->position(joan::orig::arg::mptr(static_cast<void*>(&cpos)));  // 0x004fedb0 position
    Vec3 tmp;
    const Vec3* const d = joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &cpos, joan::orig::arg::ptr(spos)));  // 0x0060cc90 sub
    Vec3 diff = *d;
    const float len = v3d_float(d::iat_v3d_87_length, static_cast<const void*>(&diff));  // length
    const float ang = joan::orig::arg::as<float>(joan::orig::as_unit(self)->heading_to_unit(static_cast<std::int32_t>(jo::as_unit(cand)->id)));  // 0x00505960
    const float hi = 90.0f, lo = -90.0f;
    if (len > *best) return false;
    if ((jo::self<jo::UnitType>(info)->trigger_flags & 0x200u) == 0) return false;
    if (!(ang >= lo)) return false;
    if (ang > hi) return false;
    *best = len;
    return true;
}

struct H {
    // 0x0046c1f0
    std::int32_t target_a(std::int32_t range) {
        c_target_a.hit();
        Vec3 spos;
        joan::orig::as_unit(this)->position(joan::orig::arg::mptr(static_cast<void*>(&spos)));  // 0x004fedb0
        float best = static_cast<float>(range);
        std::int32_t found = 0;
        if (jo::self<jo::Unit>(this)->cell == nullptr) return 0;
        for (void* c = joan::orig::arg::as<void*>(joan::orig::as_world_cell(jo::self<jo::Unit>(this)->cell)->get_8c()); c != nullptr; c = jo::as_unit(c)->cell_prev) {  // 0x0054ee10
            if (jo::as_unit(c)->can_interact == 0) continue;
            if (c == static_cast<void*>(this)) continue;
            if (static_cast<std::int32_t>(jo::as_unit(c)->id) == global<std::int32_t>(d::g_player_unit_recno)) continue;
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(c)->dead()) != 0) continue;  // 0x0042d0e0
            void* const info = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<std::int32_t>(jo::as_unit(c)->type_id))));  // 0x0042d0a0
            if (target_better_flag(this, c, &spos, &best, info)) found = jo::as_unit(c)->id;
        }
        return found;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Unit::nearest_target_a", 0x0046c1f0, &H::target_a);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

void* const kScene = obj(d::g_texture_array);

JOAN_COUNTER(c_msg_init, "MessageBox::init");

struct H {
    // ---- misc ------------------------------------------------------------------------------
    std::uintptr_t message_init() {  // 0x0046c360
        c_msg_init.hit();
        const std::uint32_t saved = joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(kScene)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kScene)->set_mode(1u);                          // 0x0060a740
        // `this` is the MessageList 0xa55a88 (callers 0x0046f7f5 / 0x004ae6d7), not the MessageBox popup
        jo::view<void*>(jo::self<jo::MessageList>(this)->icons) = img::obj(d::g_icons);
        jo::self<jo::MessageList>(this)->message_texture = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(kScene)->load(str(d::s_MESSAGE), 1, 0u));  // 0x006099c0 "MESSAGE"
        jo::self<jo::MessageList>(this)->flag_3f = 1;
        jo::self<jo::MessageList>(this)->dirty = 1;  // +0x44 on x86 (layout round r9b)
        jo::view<std::uint32_t>(jo::self<jo::MessageList>(this)->message_count) = 0;
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(kScene)->set_mode_60a740(joan::orig::arg::u32(saved)));
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("MessageBox::init", 0x0046c360, &H::message_init);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

struct H {
    // ------------------------------------------------------------------------ 0x0046c460
    void release_sound() {
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Siege>(this)->u8_2f0e)) == 0) return;
        if (jo::self<jo::Siege>(this)->life_bar == nullptr) return;
        void* const mgr = jo::view<void*>(jo::as_world(global<void*>(d::g_world))->scene);
        joan::orig::as_scene(mgr)->remove_sorted(joan::orig::arg::mptr(jo::self<jo::Siege>(this)->life_bar));  // 0x005eded0
        jo::self<jo::Siege>(this)->u8_2f0e = 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Obj::release_sound", 0x0046c460, &H::release_sound);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

struct H {
    // ------------------------------------------------------------------ 0x0046c4d0
    void* start_2f10() {  // eax = this on every path
        if (jo::view<i8>(jo::self<jo::Siege>(this)->u8_2f0e) != 0) return this;
        if (jo::self<jo::Siege>(this)->life_bar == nullptr) return this;
        joan::orig::as_scene(jo::as_world(global<u8*>(d::g_world))->scene)->register_material(joan::orig::arg::mptr(jo::self<jo::Siege>(this)->life_bar));  // 0x005eddc0
        jo::self<jo::Siege>(this)->u8_2f0e = 1;
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::start_2f10", 0x0046c4d0, &H::start_2f10);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_units.cpp (game/units2)
namespace {
namespace game_units {

std::uint32_t now() { return global<std::uint32_t>(d::g_now); }
void* camera() { return global<void*>(d::g_camera); }

JOAN_COUNTER(c_bar, "Building::progress_bar");

// The scale of an overlay at pos that keeps it a constant size on screen: the camera eye
// from its orbit frame (0x006140a0: +4 target, +0x10 pitch, +0x14 yaw, +0x1c distance),
// the ground plane (0x00617150), l1 = |eye - pos|, l2 = |eye - ground hit|.
struct Scale {
    float l1, l2;
};
Scale screen_scale(const float pos[3]) {
    const joan::hook::x87::Fpu fpu{true};
    std::uint32_t frame[10];
    joan::orig::as_camera(camera())->frame(joan::orig::arg::mptr(static_cast<void*>(frame)));  // 0x006140a0
    float f[10];
    std::memcpy(f, frame, sizeof f);
    float eye[3] = {f[1], f[2], f[3]};
    eye[2] = static_cast<float>(fpu.add(fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(f[5])))), f[7]), eye[2]));
    const float t = static_cast<float>(fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(f[5])))), f[7]));
    eye[0] = static_cast<float>(fpu.sub(eye[0], fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(f[4])))), t)));
    eye[1] = static_cast<float>(fpu.sub(eye[1], fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(f[4])))), t)));
    float plane[8] = {};
    joan::orig::as_plane_equation(static_cast<void*>(plane))->init_425700();                          // 0x00425700 Plane4::Plane4
    joan::orig::as_camera(camera())->plane_617150(0, joan::orig::arg::mptr(static_cast<void*>(plane)));             // 0x00617150 ground plane
    const float seg[6] = {eye[0], eye[1], eye[2], pos[0], pos[1], pos[2]};
    float k = 0.0f;  // (the original leaves it unset when the ray misses the plane)
    joan::orig::line_plane(&k, joan::orig::arg::mptr(seg), joan::orig::arg::mptr(static_cast<const float*>(plane)));  // 0x0060e590
    const float hit[3] = {(seg[3] - seg[0]) * k + seg[0], (seg[4] - seg[1]) * k + seg[1], (seg[5] - seg[2]) * k + seg[2]};
    float a[3] = {eye[0] - pos[0], eye[1] - pos[1], eye[2] - pos[2]};
    float b[3] = {eye[0] - hit[0], eye[1] - hit[1], eye[2] - hit[2]};
    Scale s;
    s.l1 = v3d_float(d::iat_v3d_87_length, static_cast<void*>(a));  // length
    s.l2 = v3d_float(d::iat_v3d_87_length, static_cast<void*>(b));
    return s;
}

// Billboard (0x635xxx) setters through their thunks.
void bb_position(void* bb, const float p[3]) {
    std::uint32_t v[3];
    std::memcpy(v, p, sizeof v);
    joan::orig::as_billboard(bb)->set_position(joan::orig::arg::u32(v[0]), joan::orig::arg::u32(v[1]), joan::orig::arg::u32(v[2]));  // 0x00635d40
}
void bb_scale_y(void* bb, float v) { joan::orig::as_billboard(bb)->set_19(joan::orig::arg::bits(v)); }  // 0x00635db0
void bb_scale_x(void* bb, float v) { joan::orig::as_billboard(bb)->set_15(joan::orig::arg::bits(v)); }  // 0x00635d80
constexpr float kTiny = 0.01f;                                        // 0x3c23d70a

// The bar part of a plate / bar mesh: vertices +0x94 / +0xac (left) and +0xa0 / +0xb8 and
// +0xf4 / +0x10c / +0x100 / +0x118 follow the fill level.
void set_bar(void* plane, float left, float right, float width) {
    jo::VertexF4* const v = jo::as_plane(plane)->vertices_f4;  // 0x18-byte vertices: +0x94 = [6].pos[1], +0xa0 = [6].attr[1]
    v[10].pos[1] = left;
    jo::view<std::uint32_t>(v[6].pos[1]) = jo::view<std::uint32_t>(v[10].pos[1]);
    v[11].pos[1] = right;
    jo::view<std::uint32_t>(v[7].pos[1]) = jo::view<std::uint32_t>(v[11].pos[1]);
    v[10].attr[1] = width;
    jo::view<std::uint32_t>(v[6].attr[1]) = jo::view<std::uint32_t>(v[10].attr[1]);
    v[11].attr[1] = width;
    jo::view<std::uint32_t>(v[7].attr[1]) = jo::view<std::uint32_t>(v[11].attr[1]);
    joan::orig::as_plane(plane)->invalidate(2u);  // 0x005cf920 Plane::invalidate
}

void colour(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {  // 0x0042a260 into a dead local
    std::uint32_t c = 0;
    (void)joan::orig::as_colour(&c)->construct(r, g, b, a);
}

struct H {
    // 0x0046c540 Building::progress_bar: shown (+0x2f0e) while a job (+0x2f04 index into
    // 0x93-byte jobs: +0x11b start, +0x11f duration) runs; billboard +0x2f10 above +0x6d.
    void progress_bar() {
        if (jo::view<void*>(jo::self<jo::Siege>(this)->life_bar) == nullptr) return;
        if (static_cast<std::int8_t>(jo::self<jo::Siege>(this)->u8_2f0e) == 0) return;
        c_bar.hit();
        float pos[3];
        std::memcpy(pos, jo::self<jo::Siege>(this)->owner->position_, sizeof pos);
        pos[2] = pos[2] + global<float>(d::flt_200);
        bb_position(jo::view<void*>(jo::self<jo::Siege>(this)->life_bar), pos);
        void* const plane = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Siege>(this)->life_bar))->plane());  // 0x00635e80
        const jo::VertexF4* const v = jo::as_plane(plane)->vertices_f4;
        const float span = (v[2].pos[1] - 0.171875f) - (v[0].pos[1] + 0.046875f);
        const jo::SiegeOrder* const job = &jo::self<jo::Siege>(this)->queue[jo::self<jo::Siege>(this)->current];
        const std::int32_t duration = job->duration;
        if (job->start_time + static_cast<std::uint32_t>(duration) > now()) {
            const joan::hook::x87::Fpu fpu{true};
            float done = static_cast<float>(fpu.div(static_cast<double>(now() - job->start_time), static_cast<double>(duration)));
            done = joan::orig::arg::as<float>(joan::orig::as_misc(img::obj(d::g_misc))->limit_double(joan::orig::arg::f64(static_cast<double>(done)), joan::orig::arg::f64(0.0), joan::orig::arg::f64(1.0)));  // 0x00590b00 clamp
            const float left = (span * done + v[0].pos[1]) + 0.046875f;
            const float right = (span * done + v[1].pos[1]) + 0.046875f;
            const float width = 0.78125f * done;
            set_bar(plane, left, right, width);
        }
        colour(0x57, 0xa7, 0x43, 0xff);
        const Scale s = screen_scale(pos);
        const joan::hook::x87::Fpu fpu{true};
        const float q = s.l1 / s.l2;
        bb_scale_y(jo::view<void*>(jo::self<jo::Siege>(this)->life_bar), static_cast<float>(fpu.mul(q, 0.06)));
        bb_scale_x(jo::view<void*>(jo::self<jo::Siege>(this)->life_bar), static_cast<float>(fpu.mul(q, 0.24)));
        if (s.l1 > 1000.0f && global<std::int32_t>(d::g_control_mode) != 4) {
            bb_scale_y(jo::view<void*>(jo::self<jo::Siege>(this)->life_bar), kTiny);
            bb_scale_x(jo::view<void*>(jo::self<jo::Siege>(this)->life_bar), kTiny);
        }
    }
};

#define U2(name, addr, fn) JOAN_REPLACE("game/units2", name, addr, fn)
U2("Building::progress_bar", 0x0046c540, &H::progress_bar);
#undef U2

}  // namespace game_units
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

// Model record table 0xa58f50 (word index).
u8* model_rec(i16 idx) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(idx)))); }  // 0x00425ce0

JOAN_COUNTER(c_anim_left, "Unit::anim_time_left");

struct H {
    // ------------------------------------------------------------------ 0x0046ca50
    float anim_time_left() {
        if (jo::self<jo::Wall>(this)->actor_b == nullptr) return global<float>(d::k_9cf5c4);
        c_anim_left.hit();
        u8* const anim = jo::view<u8*>(jo::self<jo::Wall>(this)->actor_b);
        const u32 diff = global<u32>(d::g_now) - jo::as_actor(anim)->start_time;
        model_rec(jo::self<jo::Wall>(this)->type_id);  // 0x00425ce0, result unused
        const i32 idx = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_anim_def(jo::self<jo::Wall>(this)->actor_b->anim)->lookup_word(str(d::s_default_9d3d60), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), jo::view<u8*>(jo::self<jo::Wall>(this)->actor_b))));  // 0x005b4a00 'default'
        void* const seq = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::self<jo::Wall>(this)->actor_b->anim)->frame(joan::orig::arg::i32(idx), 0u));  // 0x005b5dd0
        const float total = joan::orig::arg::as<float>(joan::orig::as_range(seq)->span());                               // 0x0042d130
        if (!(static_cast<double>(diff) >= static_cast<double>(total))) {
            void* const seq2 = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::self<jo::Wall>(this)->actor_b->anim)->frame(joan::orig::arg::i32(idx), 0u));  // 0x005b5dd0
            const float total2 = joan::orig::arg::as<float>(joan::orig::as_range(seq2)->span());                              // 0x0042d130
            return static_cast<float>(Fpu{true}.sub(total2, static_cast<double>(static_cast<i32>(diff))));
        }
        return global<float>(d::k_9cf5c4);
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Unit::anim_time_left", 0x0046ca50, &H::anim_time_left);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

struct H {
    void* self() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("Obj::self_46cb90", 0x0046cb90, &H::self);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::WorkshopFacility::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::ctor_469e90>(this)); }  // 0x469e90
auto joan::orig::WorkshopFacility::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_02::H::dtor_469f30>(this)); }  // 0x469f30
auto joan::orig::WorkshopFacility::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_02::H::call_46d630_46c3e0>(this)); }  // 0x46a010
auto joan::orig::LightSys::sync_lights() -> void { joan::bind::member<&::game_b95_01::H::sync_colours>(this); }  // 0x46a560
auto joan::orig::WorkshopFacility::add_type(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b98_01::H::add_type>(this, a0, a1, a2, a3)); }  // 0x46a6f0
auto joan::orig::WorkshopFacility::queue_build(std::int32_t a0, std::int32_t a1, const char* a2, const char* a3) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b96_01::H::news_add>(this, a0, a1, a2, a3)); }  // 0x46ab40
auto joan::orig::Talk::remove_message(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_02::H::remove_member>(this, a0)); }  // 0x46ad90
auto joan::orig::WorkshopFacility::take_item(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b100_02::H::take_item>(this, a0)); }  // 0x46ae20
auto joan::orig::WorkshopFacility::find_exit_spot(void* a0, void* a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_06::H::find_exit_spot>(this, a0, a1, a2)); }  // 0x46b150
auto joan::orig::WorkshopFacility::load_types(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::load_group>(this, a0)); }  // 0x46b730
auto joan::orig::WorkshopFacility::create_life_bar() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_02::H::create_life_bar>(this)); }  // 0x46ba20
auto joan::orig::Unit::nearest_target_a(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_02::H::target_a>(this, a0)); }  // 0x46c1f0
auto joan::orig::Messages::init() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b85_06::H::message_init>(this)); }  // 0x46c360
auto joan::orig::WorkshopFacility::destroy_life_bar() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::delete_2f10>(this)); }  // 0x46c3e0
auto joan::orig::WorkshopFacility::hide_life_bar() -> void { joan::bind::member<&::game_b93_01::H::release_sound>(this); }  // 0x46c460
auto joan::orig::WorkshopFacility::show_life_bar() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_01::H::start_2f10>(this)); }  // 0x46c4d0
auto joan::orig::WorkshopFacility::progress_bar() -> void { joan::bind::member<&::game_units::H::progress_bar>(this); }  // 0x46c540
auto joan::orig::Unit::anim_time_left_46ca50() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b95_02::H::anim_time_left>(this)); }  // 0x46ca50
