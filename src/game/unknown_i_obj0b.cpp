// game/unknown_i_obj0b.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_obj0b.cpp;
// re/attribution.csv: between ww/help.cpp and ww/i_gate.cpp). They move to their original file when evidence names it
// (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstddef>
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;

using joan::img::obj;

using joan::img::str;

void* const kTypes = obj(d::g_item_res);

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using joan::orig::helpers::memset_;  // memset

// Order constructors: base 0x00416a50, vtable, memset(this + 0xc2, 0, n).
using joan::orig::helpers::order_ctor;

struct H {
    void* order_489ea0() { return order_ctor(this, d::vt_Obj_9d37cc, static_cast<u32>(sizeof(jo::Order0b) - sizeof(jo::MapObject))); }  // 0xb
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Order0b::ctor", 0x00489ea0, &H::order_489ea0);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b105_04.cpp (game/b105_04)
namespace {
namespace game_b105_04 {

struct H {
    // ---------------------------------------------------------------- empty stubs
    void stub0() {}
    void stub4(u32) {}

    // ---------------------------------------------------------------- getters
    i32 zero0() { return 0; }
    H* self0() { return this; }

    // ---------------------------------------------------------------- scalar deleting dtors
    template <fn::Code DtorThunk>
    H* scalar_dtor(u32 flags) {
        thiscall<u32>(DtorThunk, this);
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(this));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_04", name, addr, fn)
B("Obj::stub_48bf20", 0x0048bf20, &H::stub0);
B("Obj::stub_48bf40", 0x0048bf40, &H::stub0);
B("Obj::stub_48bf60", 0x0048bf60, &H::stub0);
B("Obj::stub_48bf80", 0x0048bf80, &H::stub0);
// empty stubs, ret 4
B("Obj::stub_48bfa0", 0x0048bfa0, &H::stub4);
B("Obj::stub_48bfc0", 0x0048bfc0, &H::stub4);
// return 0
B("Obj::zero_48bfe0", 0x0048bfe0, &H::zero0);
B("Obj::zero_48c000", 0x0048c000, &H::zero0);
B("Obj::zero_48c020", 0x0048c020, &H::zero0);
B("Obj::zero_48c040", 0x0048c040, &H::zero0);
B("Obj::zero_48c060", 0x0048c060, &H::zero0);
B("Obj::zero_48c080", 0x0048c080, &H::zero0);
B("Obj::zero_48c0a0", 0x0048c0a0, &H::zero0);
B("Obj::zero_48c0c0", 0x0048c0c0, &H::zero0);
// return this
B("Obj::self_48c0e0", 0x0048c0e0, &H::self0);
// scalar deleting dtors
B("Obj::scalar_dtor_489ef0", 0x00489ef0, &H::scalar_dtor<fn::Obj489_dtor>);  // 0x00489f40
#undef B

}  // namespace game_b105_04
}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
namespace {
namespace game_b104_03 {

struct Vec3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers (Unit::set_position 0x0041ff50)
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");
Vec3 vec_at(const float* p) {
    Vec3 v;
    std::memcpy(&v, p, sizeof v);  // bit copy
    return v;
}

void* const kObjects = obj(d::g_items);
void* const kRandList = obj(d::g_randList);

u32 now() { return global<u32>(d::g_now); }
bool eq_or_nan(double v) { return !(v < 0.0 || v > 0.0); }  // fcomp 0 / test ah,0x40 jnz
u8* types(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0 (reads the low word)
using joan::orig::helpers::op_new;
double crt_fabs(double v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(v))); }

// fabs + 1000.0 timer test on an anim block (+0x30 speed, +0x38 start): true = the code path the
// original takes when the speed is 0 / NaN or 1000 / |speed| < elapsed (or unordered).
bool timer_done(u8* a_now, jo::Order0b* self) {
    if (eq_or_nan(static_cast<double>(jo::as_actor(a_now)->fade))) return true;
    const double dt = static_cast<double>(static_cast<u32>(now() - self->actor->fade_38));
    const double m = crt_fabs(static_cast<double>(self->actor->fade));
    const double q = Fpu{true}.div(global<double>(d::dbl_1000), m);  // 1000.0 / fabs (fdivr qword)
    return !(q >= dt);
}

JOAN_COUNTER(c_update_48ba70, "Obj489::update");

struct H {
    i16& drop_item() { return jo::self<jo::Order0b>(this)->drop_item; }
    i16& drop_count() { return jo::self<jo::Order0b>(this)->drop_count; }

    // ------------------------------------------------------------------ destructors
    u32 dtor_489f40() {
        jo::self<jo::Order0b>(this)->vtable = img::obj(d::vt_Obj_9d37cc);
        joan::orig::as_map_object(this)->release();         // 0x0048a040
        return joan::orig::arg::as<u32>(joan::orig::as_map_object(this)->destruct());  // 0x00415790
    }

    // ------------------------------------------------------------------ 0x0048ba70
    void update_48ba70() {
        c_update_48ba70.hit();
        u8* const t = types(jo::self<jo::Order0b>(this)->type_id);
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(t)->open_with_interact)) == 0) return;
        if (jo::self<jo::Order0b>(this)->busy != 0) return;
        if (!eq_or_nan(joan::orig::arg::as<double>(joan::orig::as_unit(this)->anim_time_left_48a070()))) return;  // 0x0048a070 (st0)
        if (jo::self<jo::Order0b>(this)->actor != nullptr && !eq_or_nan(static_cast<double>(jo::self<jo::Order0b>(this)->actor->fade))) {
            if (timer_done(jo::view<u8*>(jo::self<jo::Order0b>(this)->actor), jo::self<jo::Order0b>(this))) jo::self<jo::Order0b>(this)->active = 1;
            return;
        }
        if (static_cast<i32>(drop_item()) != 0) {
            u8* t2 = types(drop_item());
            if (static_cast<i32>(jo::view<i8>(jo::as_item_def(t2)->unique_item)) != 0 &&
                static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_game(obj(d::g_game_state))->has_word(joan::orig::arg::i16(static_cast<i32>(drop_item()))))) != 0) {  // 0x00472740
                const i16 list = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_names(kRandList)->find_463ed0(str(d::s_unique_random_list))));  // 0x00463ed0
                void* const r = joan::orig::arg::as<void*>(joan::orig::as_names(kRandList)->at(joan::orig::arg::i16(static_cast<i32>(list))));            // 0x00464100
                joan::orig::as_grass(r)->pick(joan::orig::arg::mptr(static_cast<void*>(&drop_item())), joan::orig::arg::mptr(static_cast<void*>(&drop_count())));  // 0x00464030
                t2 = types(drop_item());
            }
            const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_units(kObjects)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(drop_item()))));  // 0x0049ea90
            u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(idx)));  // 0x00425d20
            joan::orig::as_map_object(o)->set_world(jo::self<jo::Order0b>(this)->world);             // 0x004994b0
            joan::orig::as_map_object(o)->set_position(vec_at(jo::self<jo::Order0b>(this)->pos));    // 0x0041ff50
            joan::orig::as_map_object(o)->set_yaw(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Order0b>(this)->yaw_)));             // 0x00499fd0
            jo::view<u16>(jo::as_map_object(o)->side) = jo::view<u16>(jo::self<jo::Order0b>(this)->side);
            if (jo::as_map_object(o)->actor != nullptr) {
                const float* const src = jo::as_map_object(o)->actor->def->model->sphere;
                float* const dst = jo::as_map_object(o)->actor->aim_sphere;
                jo::view<u32>(dst[0]) = jo::view<u32>(src[0]);
                jo::view<u32>(dst[1]) = jo::view<u32>(src[1]);
                jo::view<u32>(dst[2]) = jo::view<u32>(src[2]);
                const u32 v = jo::view<u32>(jo::as_map_object(o)->actor->def->model->sphere[3]);
                jo::view<u32>(jo::as_map_object(o)->actor->aim_sphere[3]) = v;
                jo::as_map_object(o)->actor->aim_mode = 1;
            }
            void* const mem = op_new(static_cast<u32>(sizeof(jo::WorldItem)));  // 0x52 on i686
            u8* const m = mem != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_world_item(mem)->construct()) : nullptr;  // 0x00456eb0
            joan::orig::as_item_stack(m)->init(joan::orig::arg::i16(static_cast<i32>(drop_item())), joan::orig::arg::i16(static_cast<i32>(drop_count())), joan::orig::arg::i16(static_cast<i32>(jo::view<i16>(jo::as_item_def(t2)->stats[3]))), 0u, 0u, 0u);  // 0x00456cd0
            jo::view<u16>(jo::as_world_item(m)->object_id) = jo::view<u16>(jo::as_map_object(o)->index);
            jo::as_world_item(m)->side = jo::view<u16>(jo::self<jo::Order0b>(this)->side);
            joan::orig::as_world_item(m)->set_state(jo::self<jo::Order0b>(this)->world);           // 0x00457120
            joan::orig::as_world_item(m)->set_pos(jo::view<u32>(jo::self<jo::Order0b>(this)->pos[0]), jo::view<u32>(jo::self<jo::Order0b>(this)->pos[1]), jo::view<u32>(jo::self<jo::Order0b>(this)->pos[2]));  // 0x00456fe0
            joan::orig::as_world_item(m)->set_heading(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Order0b>(this)->yaw_)));           // 0x004571b0
            jo::view<u8*>(jo::as_map_object(o)->world_item) = m;
        }
        if (jo::self<jo::Order0b>(this)->actor != nullptr) {
            jo::view<u32>(jo::self<jo::Order0b>(this)->actor->fade) = 0xbf000000u;  // -0.5f
            jo::self<jo::Order0b>(this)->actor->fade_38 = now();
            jo::view<u32>(jo::self<jo::Order0b>(this)->actor->alpha) = 0xff;
        } else {
            jo::self<jo::Order0b>(this)->active = 1;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("Obj489::~Obj489", 0x00489f40, &H::dtor_489f40);
B("Obj489::update", 0x0048ba70, &H::update_48ba70);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

u8* type_record(i16 type) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(type))); }  // 0x00425ce0

JOAN_COUNTER(c_is_idle_48b900, "Obj::is_idle_48b900");

struct H {
    // ------------------------------------------------------------------ 0x00489fb0
    u8* init_489fb0(u32 type) {
        joan::orig::as_map_object(this)->set_kind(joan::orig::arg::i16(type));  // 0x004178e0
        u8* const t = type_record(static_cast<i16>(type));
        jo::self<jo::Order0b>(this)->busy = jo::view<u32>(jo::as_item_def(t)->model_count);
        jo::self<jo::Order0b>(this)->state = jo::as_item_def(t)->need_key;
        jo::self<jo::Order0b>(this)->active = 1;
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(t)->open_with_interact)) != 0) jo::self<jo::Order0b>(this)->interact_trigger = 0x200;
        return t;
    }

    // ------------------------------------------------------------------ 0x0048b900
    i32 is_idle_48b900() {
        c_is_idle_48b900.hit();
        if (jo::self<jo::Order0b>(this)->active == 0 || jo::self<jo::Order0b>(this)->busy != 0) return 0;
        if (jo::view<u32>(jo::as_item_def(type_record(jo::self<jo::Order0b>(this)->type_id))->model_count) == 0) return 0;
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(type_record(jo::self<jo::Order0b>(this)->type_id))->model_kinds[0])) == 0 ||
            static_cast<i32>(jo::view<i8>(jo::as_item_def(type_record(jo::self<jo::Order0b>(this)->type_id))->model_kinds[0])) == 2) {
            const float v = joan::orig::arg::as<float>(joan::orig::as_unit(this)->anim_time_left_48a070());  // 0x0048a070
            // fcomp 0.0 / test ah,0x40 je: equal or unordered -> 1.
            if (!(v < 0.0f || v > 0.0f)) return 1;
            return 0;
        }
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(type_record(jo::self<jo::Order0b>(this)->type_id))->model_kinds[0])) == 1) return 1;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::init_489fb0", 0x00489fb0, &H::init_489fb0);
B("Obj::is_idle_48b900", 0x0048b900, &H::is_idle_48b900);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {

struct H {
    // ------------------------------------------------------------------ 0x0048a040 / 0x0048f8a0
    u32 base_4251b0() { return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->release_4251b0()); }  // 0x004251b0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::base_48a040", 0x0048a040, &H::base_4251b0);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02 {

std::uint32_t now() { return global<std::uint32_t>(d::g_now); }

JOAN_COUNTER(c_anim_left, "Unit::anim_time_left");
JOAN_COUNTER(c_set_group, "Member::set_group");

struct H {
    // 0x0048a070
    float anim_time_left() {
        c_anim_left.hit();
        if (jo::self<jo::Order0b>(this)->actor == nullptr) return global<float>(d::k_9cf5c4);
        void* const actor = jo::view<void*>(jo::self<jo::Order0b>(this)->actor);
        const std::uint32_t elapsed = now() - jo::as_actor(actor)->start_time;
        std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Order0b>(this)->type_id))));  // 0x00425ce0
        if (reinterpret_cast<std::uintptr_t>(def) + offsetof(jo::ItemDef, anim_names) == 0) return global<float>(d::k_9cf5c4);  // &def->anim_names == 0 (integer test, as the original)
        const std::int32_t k = jo::view<std::int32_t>(jo::self<jo::Order0b>(this)->busy);
        if (jo::as_item_def(def)->anim_names[k] == nullptr) return global<float>(d::k_9cf5c4);
        const std::int16_t idx = joan::orig::arg::as<std::int16_t>(joan::orig::as_anim_def(jo::self<jo::Order0b>(this)->actor->anim)->lookup_word(jo::as_item_def(def)->anim_names[jo::view<std::int32_t>(jo::self<jo::Order0b>(this)->busy)], static_cast<std::uint8_t>(joan::orig::arg::u32(1)), jo::self<jo::Order0b>(this)->actor));  // 0x005b4a00
        const std::int32_t i = idx;
        void* const a1 = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::self<jo::Order0b>(this)->actor->anim)->frame(joan::orig::arg::i32(i), 0u));  // 0x005b5dd0
        const float r1 = joan::orig::arg::as<float>(joan::orig::as_range(a1)->span());                                   // 0x0042d130
        if (!(static_cast<double>(elapsed) >= static_cast<double>(r1))) {
            void* const a2 = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::self<jo::Order0b>(this)->actor->anim)->frame(joan::orig::arg::i32(i), 0u));  // 0x005b5dd0
            const float r2 = joan::orig::arg::as<float>(joan::orig::as_range(a2)->span());                                   // 0x0042d130
            const joan::hook::x87::Fpu fpu{true};
            return static_cast<float>(
                fpu.sub(static_cast<double>(r2), static_cast<double>(static_cast<std::int32_t>(elapsed))));  // fisub
        }
        return global<float>(d::k_9cf5c4);
    }

    // 0x0048be30
    // Sets this TrackNode's search parent (named Member::set_group; its callers are the route search's): unlinks it
    // from the old parent's child table, appends it to the new one's.
    void set_group(void* g) {
        c_set_group.hit();
        jo::TrackNode* const n = jo::self<jo::TrackNode>(this);
        if (static_cast<void*>(n->parent) == g) return;
        if (n->parent != nullptr) {
            for (std::int32_t i = n->parent->child_count; i > 0; --i) {
                jo::TrackNodePtr* const arr = n->parent->children;
                if (arr[i - 1].p != n) continue;
                joan::orig::as_misc(obj(d::g_misc))->del_array_rec_591070(joan::orig::arg::str(static_cast<void*>(arr)), static_cast<std::int32_t>(n->parent->child_count), static_cast<std::int32_t>(sizeof(jo::TrackNodePtr)), joan::orig::arg::i32(i));  // 0x00591070 (4-byte records on x86)
                n->parent->child_count = static_cast<std::int16_t>(n->parent->child_count - 1);
                n->parent = nullptr;
                break;
            }
        }
        if (g != nullptr) {
            jo::TrackNode* const parent = jo::as_track_node(g);
            parent->children[parent->child_count].p = n;
            parent->child_count = static_cast<std::int16_t>(parent->child_count + 1);
            n->parent = parent;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Unit::anim_time_left", 0x0048a070, &H::anim_time_left);
B("Member::set_group", 0x0048be30, &H::set_group);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b109_13.cpp (game/b109_13)
namespace {
namespace game_b109_13 {

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

struct V3 {  // 12 bytes copied bit for bit, passed by value: float members (System V: XMM registers, as the callees)
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "sizes");

V3 load_v3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

constexpr img::Addr kUnits(d::g_units);

JOAN_COUNTER(c_event, "Obj::on_event_48a1f0");

// ------------------------------------------------------------------ 0x0048a1f0 helpers
// Advances the step script by the step kind byte (+0x96 + step); false for an unknown kind.
bool step_advance(u8* self, u8* rec);
void spawn_follower(u8* self);
void drop_and_count(u8* self, u8* rec) {
    if (jo::view<u32>(jo::as_item_def(rec)->sounds[0]) != 0) {
        joan::orig::as_sound_list(obj(d::g_sfx_player))->play_at(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(rec)->sounds[0])), load_v3(jo::self<jo::Order0b>(self)->pos));  // 0x004950a0
    }
    if (joan::crt::stricmp(joan::orig::arg::str(jo::as_map_object(self)->name), str(d::s_s7_food)) == 0) {
        global<i32>(d::g_get_a591be) = global<i32>(d::g_get_a591be) + 1;
    }
}
void step_anim(u8* self, u8* rec) {
    if (jo::view<i8>(jo::self<jo::Order0b>(self)->MapObject::active) != 0 && jo::self<jo::Order0b>(self)->actor != nullptr) {
        u8* const actor = jo::view<u8*>(jo::self<jo::Order0b>(self)->actor);
        joan::orig::as_actor(actor)->start_anim(joan::orig::arg::mptr(jo::as_actor(actor)->anim), jo::as_item_def(rec)->anim_names[jo::view<i32>(jo::self<jo::Order0b>(self)->busy)], joan::orig::arg::u32(global<u32>(d::g_now)), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x005abe30
    }
}
bool step_advance(u8* self, u8* rec) {
    const i32 kind0 = jo::view<i8>(jo::as_item_def(rec)->model_kinds[jo::view<i32>(jo::self<jo::Order0b>(self)->busy) - 1]);
    if (kind0 == 0) {
        virtual_call(self, 0x18, jo::view<i32>(jo::self<jo::Order0b>(self)->busy) - 1);
        step_anim(self, rec);
        return true;
    }
    if (jo::view<i8>(jo::as_item_def(rec)->model_kinds[jo::view<i32>(jo::self<jo::Order0b>(self)->busy) - 1]) == 1) {
        virtual_call(self, 0x18, jo::view<i32>(jo::self<jo::Order0b>(self)->busy) - 1);
        return true;
    }
    if (jo::view<i8>(jo::as_item_def(rec)->model_kinds[jo::view<i32>(jo::self<jo::Order0b>(self)->busy) - 1]) == 2) {
        virtual_call(self, 0x18, jo::view<i32>(jo::self<jo::Order0b>(self)->busy) - 1);
        step_anim(self, rec);
        return true;
    }
    return false;
}
i16 drop_item(const u8* self) { return jo::self<jo::Order0b>(self)->drop_item; }
i16 drop_count(const u8* self) { return jo::self<jo::Order0b>(self)->drop_count; }
void spawn_follower(u8* self) {
    if (drop_item(self) == 0) return;
    u8* const rec2 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(drop_item(self)))));  // 0x00425ce0
    if (jo::view<i8>(jo::as_item_def(rec2)->unique_item) != 0 &&
        joan::orig::arg::as<u8>(joan::orig::as_game(obj(d::g_game_state))->has_word(joan::orig::arg::i16(static_cast<i32>(drop_item(self))))) != 0) {  // 0x00472740
        return;
    }
    const i32 id = joan::orig::arg::as<i32>(joan::orig::as_units(obj(d::g_items))->create_49ea90(joan::orig::arg::u32(static_cast<i32>(drop_item(self)))));  // 0x0049ea90
    u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
    joan::orig::as_map_object(unit)->set_world(jo::self<jo::Order0b>(self)->world);                  // 0x004994b0
    joan::orig::as_map_object(unit)->set_position(load_v3(jo::self<jo::Order0b>(self)->pos));             // 0x0041ff50
    joan::orig::as_map_object(unit)->set_yaw(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Order0b>(self)->yaw_)));                  // 0x00499fd0
    jo::view<u16>(jo::as_map_object(unit)->side) = jo::view<u16>(jo::self<jo::Order0b>(self)->side);
    if (jo::as_map_object(unit)->actor != nullptr) {
        u8* const actor = jo::view<u8*>(jo::as_map_object(unit)->actor);
        const u8* const src = reinterpret_cast<const u8*>(jo::as_actor(actor)->def->model->sphere);
        std::memcpy(jo::as_actor(actor)->aim_sphere, src, 12);
        u8* const actor2 = jo::view<u8*>(jo::as_map_object(unit)->actor);
        jo::view<u32>(jo::as_actor(actor2)->aim_sphere[3]) = jo::view<u32>(jo::as_actor(actor2)->def->model->sphere[3]);
        jo::as_map_object(unit)->actor->aim_mode = 1;
    }
    void* const mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::WorldItem)));  // 0x52 on i686
    void* const o = mem != nullptr ? joan::orig::as_world_item(mem)->construct() : nullptr;  // 0x00456eb0
    joan::orig::as_item_stack(o)->init(joan::orig::arg::i16(static_cast<i32>(drop_item(self))), joan::orig::arg::i16(static_cast<i32>(drop_count(self))), joan::orig::arg::i16(static_cast<i32>(jo::view<i16>(jo::as_item_def(rec2)->stats[3]))), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x00456cd0
    jo::view<u16>(jo::as_world_item(o)->object_id) = jo::view<u16>(jo::as_map_object(unit)->index);
    jo::as_world_item(o)->side = jo::view<u16>(jo::self<jo::Order0b>(self)->side);
    joan::orig::as_world_item(o)->set_state(jo::self<jo::Order0b>(self)->world);       // 0x00457120
    joan::orig::as_world_item(o)->set_pos(jo::view<u32>(jo::self<jo::Order0b>(self)->pos[0]), jo::view<u32>(jo::self<jo::Order0b>(self)->pos[1]), jo::view<u32>(jo::self<jo::Order0b>(self)->pos[2]));  // 0x00456fe0
    joan::orig::as_world_item(o)->set_heading(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Order0b>(self)->yaw_)));       // 0x004571b0
    jo::view<void*>(jo::as_map_object(unit)->world_item) = o;
}
// Steps the script and, when it ended (+0x52 == 0), spawns the follower.
void advance_spawn(u8* self) {
    u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Order0b>(self)->type_id))));  // 0x00425ce0
    if (jo::self<jo::Order0b>(self)->busy == 0) return;
    if (!step_advance(self, rec)) return;
    if (jo::self<jo::Order0b>(self)->busy != 0) return;
    spawn_follower(self);
    if (jo::self<jo::Order0b>(self)->actor != nullptr) {
        jo::self<jo::Order0b>(self)->actor->aim_mode = 0;
        if (jo::view<u32>(jo::self<jo::Order0b>(self)->actor->record) != 0) {
            joan::orig::as_players(obj(d::g_physic_sys))->set_active(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order0b>(self)->actor->record)), joan::orig::arg::i16(i32{0}));  // 0x006ad980
        }
    }
    drop_and_count(self, rec);
}
// Event sender (+0xe) is a live unit of the player's side.
bool same_side(u8* ev) {
    if (jo::view<i16>(jo::as_save_entry(ev)->ref_id) == 0) return false;
    if (joan::orig::as_dynarray(img::obj(kUnits))->empty_at(static_cast<i32>(jo::view<i16>(jo::as_save_entry(ev)->ref_id))) != 0) return false;  // 0x00428b70
    const i32 a = jo::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(static_cast<i32>(jo::view<i16>(jo::as_save_entry(ev)->ref_id)))))->side;  // 0x0041b3c0
    const i32 b = jo::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)))))->side;
    return a == b;
}
// Event 0x20 / 0x200 step: returns false when the original leaves the function.
bool step_drop(u8* self) {
    u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Order0b>(self)->type_id))));  // 0x00425ce0
    if (jo::self<jo::Order0b>(self)->busy != 0) {
        jo::self<jo::Order0b>(self)->active = 0;
        if (!step_advance(self, rec)) return false;
    }
    drop_and_count(self, rec);
    return true;
}

struct H {
    // -------------------------------------------------------------- 0x0048a1f0
    bool on_event(i32 id_arg) {
        c_event.hit();
        u8* const self = reinterpret_cast<u8*>(this);
        u8* const ev = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_a5ccd0))->at_485f20(joan::orig::arg::u32(static_cast<i32>(static_cast<i16>(id_arg)))));  // 0x00485f20
        u8* const rec0 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Order0b>(self)->type_id))));
        const i8 busy = jo::view<i8>(jo::as_item_def(rec0)->open_with_interact);
        const i32 type = jo::as_save_entry(ev)->type;
        if (type == 0x40 || type == 2 || type == 0x2000) {
            if (busy != 0 || jo::view<i32>(jo::as_save_entry(ev)->ref_kind) != 1) return true;
            if (!same_side(ev)) return true;
            advance_spawn(self);
            return true;
        }
        if (type == 0x4000) {
            if (busy != 0 || jo::view<i32>(jo::as_save_entry(ev)->ref_kind) != 2) return true;
            advance_spawn(self);
            return true;
        }
        if (type == 1) {
            if (busy != 0 || jo::view<i32>(jo::as_save_entry(ev)->ref_kind) != 1) return true;
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(self)->can_reach_49a010(joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_save_entry(ev)->ref_id))), joan::orig::arg::u32(i32{0})))) == 0) {  // 0x0049a010
                return true;
            }
            if (!same_side(ev)) return true;
            advance_spawn(self);
            return true;
        }
        if (type != 0x20 && type != 0x200) return true;
        if (type == 0x20 && jo::view<i32>(jo::as_save_entry(ev)->ref_kind) == 1) {
            if (!step_drop(self)) return true;
        }
        // 0x200 (and the fall-through of 0x20)
        if (joan::orig::arg::as<u8>(joan::orig::as_map_object(self)->is_active()) != 0 && jo::view<i32>(jo::as_save_entry(ev)->ref_kind) == 1) {  // 0x0048ba00
            if (!step_drop(self)) return true;
        }
        if (jo::view<i32>(jo::as_save_entry(ev)->ref_kind) == 1 && jo::self<jo::Order0b>(self)->busy != 0) {
            u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(static_cast<i32>(jo::view<i16>(jo::as_save_entry(ev)->ref_id))));
            jo::view<u16>(jo::as_unit(unit)->interact_item) = jo::view<u16>(jo::self<jo::Order0b>(self)->index);
        }
        return true;
    }
};

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_13", name, addr, fn)
B("Obj::on_event_48a1f0", 0x0048a1f0, &H::on_event);
#undef B

}  // namespace game_b109_13
}  // namespace

// ---------------------------------------- from modules/game_b89_03.cpp (game/b89_03)
namespace {
namespace game_b89_03 {

using joan::call::import_call;

constexpr img::Addr kGameUnits(d::g_units);
constexpr img::Addr kMounts(d::g_physic_sys);
constexpr img::Addr kKinds(d::g_types);
constexpr img::Addr kPlayer(d::g_player_unit_recno);
std::int32_t now() { return global<std::int32_t>(d::g_now); }

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* mount_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(img::obj(kMounts))->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x006af310
std::uint8_t* kind_get(std::int32_t k) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(img::obj(kKinds))->get(joan::orig::arg::i16(k))); }  // 0x0042d0a0
std::int32_t target_of(const void* u) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(u)->mounted()); }               // 0x00430190
double v3_length(const void* v) { return import_call<double>(d::iat_v3d_87_length, v); }                       // length
std::uint32_t packed_colour(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t tmp = 0, packed = 0;
    void* const c = joan::orig::as_colour(&tmp)->construct(r, g, b, a);  // 0x0042a260
    joan::orig::as_colour(&packed)->from_rgba(joan::orig::arg::mptr(c));                         // 0x005e95b0
    return packed;
}

JOAN_COUNTER(c_filter, "Unit::filter_command");

struct H {
    // 0x0048b460
    void filter_command(std::int16_t* cmd) {
        c_filter.hit();
        if (*cmd == 10 && jo::self<jo::Unit>(this)->rider_id != 0 && jo::self<jo::Unit>(this)->actor != nullptr) {
            std::uint8_t* const m = mount_get(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record));
            if (v3_length(&jo::as_shot(m)->state) > static_cast<double>(global<float>(d::flt_5)) &&
                jo::view<float>(jo::as_shot(m)->i32_2a4) > global<float>(d::flt_5)) {
                *cmd = 0xb;
            }
        }
        if (*cmd == 1) {
            if (unit_missing(jo::self<jo::Unit>(this)->rider_id) == 0) {
                std::uint8_t* const u = unit_get(jo::self<jo::Unit>(this)->rider_id);
                if (jo::as_unit(u)->anim_kind != 0x43) joan::orig::as_unit(u)->play_idle(0, 1);  // 0x00508b90
            }
        }
        if (target_of(this) != 0 && *cmd == 0x43) {
            if (unit_missing(jo::self<jo::Unit>(this)->mount_id) == 0) {
                std::uint8_t* const u = unit_get(jo::self<jo::Unit>(this)->mount_id);
                if (jo::as_unit(u)->anim_kind == 1) joan::orig::as_unit(u)->play_idle(0, 1);  // 0x00508b90
            }
        }
        if (target_of(this) != 0 && *cmd == 0x23) *cmd = 0x4a;
        if (target_of(this) != 0 && *cmd == 0x26) *cmd = 0x4b;
        if (target_of(this) != 0 && *cmd == 0x27) *cmd = 0x4c;
        if (target_of(this) != 0 && *cmd == 0x28) *cmd = 0x4d;
        if (*cmd == 0x44 && jo::self<jo::Unit>(this)->mount_id != 0 && unit_missing(jo::self<jo::Unit>(this)->mount_id) == 0) {
            std::uint8_t* const u = unit_get(jo::self<jo::Unit>(this)->mount_id);
            if (jo::as_unit(u)->actor != nullptr) {
                std::uint8_t* const m = mount_get(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(u)->actor)->record));
                if (v3_length(&jo::as_shot(m)->state) > static_cast<double>(global<float>(d::flt_5)) &&
                    jo::self<jo::Unit>(this)->paired_anim_start_ms != 0) {
                    *cmd = 0x46;
                }
            }
        }
        if (*cmd != 0x4e) return;
        if (unit_missing(jo::self<jo::Unit>(this)->mount_id) == 0) {
            std::uint8_t* const u = unit_get(jo::self<jo::Unit>(this)->mount_id);
            joan::orig::as_slot(u)->select(0);                                // 0x00482930
            const float v = joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading());            // 0x004ff850
            joan::orig::as_unit(u)->set_heading(joan::orig::arg::f32(v / global<float>(d::flt_0_017453292)));    // 0x0050abd0
            jo::view<std::int32_t>(jo::as_actor(jo::as_unit(u)->actor)->aim_mode) = 0;
            joan::orig::as_actor(jo::as_unit(u)->actor)->set_faded(static_cast<char>(joan::orig::arg::i32(1)));              // 0x005ae590
            jo::view<std::int32_t>(jo::as_unit(u)->mount_pending_ms) = now() + 0xbb8;
            jo::as_unit(u)->mount_pending = 1;
            const std::uint32_t colour = packed_colour(0xff, 0xff, 0xff, 0xaa);
            joan::orig::as_plane(jo::as_actor(jo::as_unit(u)->actor)->plane)->set_fade(joan::orig::arg::u32(colour));  // 0x005d5e80
        }
        if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
            std::uint8_t* const m = mount_get(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record));
            joan::orig::as_anim_table(img::obj(kMounts))->restart_6af370(joan::orig::arg::u32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)), joan::orig::arg::u32(// 0x006af370
                     jo::view<std::int32_t>(jo::as_shot(m)->u32_27a)), joan::orig::arg::u32(static_cast<std::int32_t>(static_cast<std::int16_t>(jo::as_shot(m)->type))), 0u, 2u, 1u);
        }
        if (jo::self<jo::Unit>(this)->type_id != 0) {
            std::uint8_t* const k = kind_get(jo::self<jo::Unit>(this)->type_id);
            jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode) = jo::view<std::int32_t>(jo::as_unit_type(k)->shape_kind);
        }
        jo::self<jo::Unit>(this)->dismounted_flag = 1;
        if (jo::self<jo::Unit>(this)->model != nullptr) joan::orig::as_model_instance(jo::self<jo::Unit>(this)->model)->set_10a(1u);  // 0x0042cf20
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(this)->dead())) != 0) {  // 0x0042d0e0
            joan::orig::as_unit(this)->set_action(0x2a, 0, 1);                                    // 0x00501190
        }
        {
            const std::int32_t id = jo::self<jo::Unit>(this)->id;
            if (id == joan::orig::arg::as<std::int32_t>(joan::orig::as_player(img::obj(kPlayer))->unit_id())) joan::orig::as_player(img::obj(kPlayer))->set_byte_c4(0u);  // 0x004203c0 / 0x0051c4b0
        }
        jo::self<jo::Unit>(this)->mounted_flag = 0;
        jo::self<jo::Unit>(this)->mount_id = 0;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_03", name, addr, fn)
M("Unit::filter_command", 0x0048b460, &H::filter_command);
#undef M

}  // namespace game_b89_03
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

using joan::orig::helpers::u;

JOAN_COUNTER(c_is_active, "Obj::is_active");

struct H {
    // ------------------------------------------------------------------ 0x0048ba00
    std::uintptr_t is_active() {
        c_is_active.hit();
        // 0x00425ce0 reads its arg as a word (movsx).
        const u8* const rec = joan::orig::arg::as<const u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Order0b>(this)->type_id))));
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(rec)->open_with_interact)) == 0) return u(rec) & ~std::uintptr_t{0xffu};
        const std::uintptr_t hi = u(this) & ~std::uintptr_t{0xffu};
        if (!(jo::view<i32>(jo::self<jo::Order0b>(this)->busy) > 0)) return hi;
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Order0b>(this)->state)) != 0) return hi;
        return hi | 1u;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Obj::is_active_48ba00", 0x0048ba00, &H::is_active);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MapObject::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_03::H::base_4251b0>(this)); }  // 0x48a040
auto joan::orig::Unit::anim_time_left_48a070() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b92_02::H::anim_time_left>(this)); }  // 0x48a070
auto joan::orig::Unit::filter_command(void* a0) -> void { joan::bind::member<&::game_b89_03::H::filter_command>(this, a0); }  // 0x48b460
auto joan::orig::MapObject::is_active() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_02::H::is_active>(this)); }  // 0x48ba00
auto joan::orig::Member::set_group_48be30(void* a0) -> void { joan::bind::member<&::game_b92_02::H::set_group>(this, a0); }  // 0x48be30
