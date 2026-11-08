// game/unknown_partslot.cpp: functions whose original source file is unknown (formerly game/_unplaced/partslot.cpp;
// re/attribution.csv: between ww/enparticle/particle.cpp and ww/enparticle/particle.cpp). They move to their original
// file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::img::obj;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_fx5.cpp (game/fx6)
namespace {
namespace game_fx5 {

struct H {
    // 0x00662e20 Particle::reset: zero the 0x80 bytes but the first dword; life / +0x14 /
    // size 1, colour opaque white.
    void particle_reset() {
        const std::uint32_t first = jo::self<jo::Particle>(this)->id;
        std::memset(this, 0, sizeof(jo::Particle));
        jo::self<jo::Particle>(this)->id = first;
        jo::view<std::uint32_t>(jo::self<jo::Particle>(this)->lifetime) = 0x3f800000u;
        jo::view<std::uint32_t>(jo::self<jo::Particle>(this)->f32_14) = 0x3f800000u;
        jo::view<std::uint32_t>(jo::self<jo::Particle>(this)->scale) = 0x3f800000u;
        std::uint32_t colour = 0;
        jo::view<std::uint32_t>(jo::self<jo::Particle>(this)->colour) = *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(static_cast<void*>(&colour))->construct(0xffu, 0xffu, 0xffu, 0xffu));  // 0x0042a260 Color
        jo::self<jo::Particle>(this)->u32_38 = 0;
        jo::self<jo::Particle>(this)->node = nullptr;
    }
};

#define FX6(name, addr, fn) JOAN_REPLACE("game/fx6", name, addr, fn)
FX6("Particle::reset", 0x00662e20, &H::particle_reset);
#undef FX6

}  // namespace game_fx5
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

using joan::call::thiscall;

using joan::call::thiscall;

struct H {
    void slot_clear() {                                                                  // 0x00662ed0
        if (jo::view<void*>(jo::self<jo::Particle>(this)->node) == nullptr) return;
        if (jo::self<jo::Particle>(this)->node->scene != nullptr) joan::orig::as_scene(jo::self<jo::Particle>(this)->node->scene)->detach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Particle>(this)->node)));  // 0x005efdb0
        void* const obj = jo::view<void*>(jo::self<jo::Particle>(this)->node);
        if (obj != nullptr) joan::orig::as_scene_object(obj)->destroy(1u);  // 0x00420840
        jo::self<jo::Particle>(this)->node = nullptr;
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("PartSlot::clear", 0x00662ed0, &H::slot_clear);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x00663630
    void* elem_ctor() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(jo::Particle)), 0x64, 0, 0);  // 0x005856a0 (Particle records: 0x80 on x86, Particle::reset clears sizeof(Particle))
        jo::ParticlePool* const pool = jo::self<jo::ParticlePool>(this);
        std::memset(&pool->alive_, 0, sizeof pool->alive_ + sizeof pool->emitted + sizeof pool->last_emit + sizeof pool->carry);  // +0x2e..+0x3e
        return this;
    }

    // ------------------------------------------------------------------ 0x006637a0
    void* elem_reset() {
        joan::orig::as_particle_pool(this)->release_all();  // 0x00663740
        jo::self<jo::ParticlePool>(this)->alive_ = 0;
        jo::self<jo::ParticlePool>(this)->emitted = 0;
        jo::self<jo::ParticlePool>(this)->last_emit = 0;
        jo::view<i32>(jo::self<jo::ParticlePool>(this)->carry) = 0;
        return this;
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("Elem::Elem_663630", 0x00663630, &H::elem_ctor);
B103("Elem::reset_6637a0", 0x006637a0, &H::elem_reset);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04 {

using joan::call::thiscall;

struct H {
    // 0x00663680
    void dtor_663680() {
        joan::orig::as_particle_pool(this)->clear();  // 0x00663710
        joan::orig::dynarray_b(this)->destruct();  // 0x00585710
    }
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("Obj663680::destructor", 0x00663680, &H::dtor_663680);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

using joan::call::thiscall;

// Thunks.
constexpr fn::Code kListSlot = fn::List_slot;    // 0x00417200

struct H {
    std::int32_t pool_alive() { return joan::orig::arg::as<std::int32_t>(joan::orig::as_pool(this)->alive_count()); }  // 0x006638e0 -> 0x004aede0
    std::int32_t pool_free_at(std::int32_t i) {                    // 0x00663910
        void* const p = thiscall<void*>(kListSlot, this, i);
        return p == nullptr || *static_cast<std::uint32_t*>(p) == 0 ? 1 : 0;
    }
    void* pool_slot(std::int32_t i) { return thiscall<void*>(kListSlot, this, i); }  // 0x00663960
    void pool_release_all() {                                      // 0x00663740
        for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_particle_pool(this)->count()); i > 0; --i)  // 0x006638b0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_particle_pool(this)->free_at(joan::orig::arg::i32(i))) == 0) joan::orig::as_particle_pool(this)->release(joan::orig::arg::i32(i));  // 0x00663910 / 0x00663850
    }
    void slot_init() {}                                            // 0x006636f0
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("SlotPool::alive", 0x006638e0, &H::pool_alive);
FX1("SlotPool::free_at", 0x00663910, &H::pool_free_at);
FX1("SlotPool::slot", 0x00663960, &H::pool_slot);
FX1("SlotPool::release_all", 0x00663740, &H::pool_release_all);
FX1("PartSlot::init", 0x006636f0, &H::slot_init);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04 {

using joan::call::thiscall;

struct H {
    void wrap_663710() {
        joan::orig::as_particle_pool(this)->release_all();  // 0x00663740
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("Obj663710::wrap", 0x00663710, &H::wrap_663710);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Particle::reset() -> void { joan::bind::member<&::game_fx5::H::particle_reset>(this); }  // 0x662e20
auto joan::orig::Particle::clear_model() -> void { joan::bind::member<&::game_world::H::slot_clear>(this); }  // 0x662ed0
auto joan::orig::ParticlePool::destruct() -> void { joan::bind::member<&::game_b83_04::H::dtor_663680>(this); }  // 0x663680
auto joan::orig::ParticlePool::init() -> void { joan::bind::member<&::game_fx::H::slot_init>(this); }  // 0x6636f0
auto joan::orig::ParticlePool::clear() -> void { joan::bind::member<&::game_b84_04::H::wrap_663710>(this); }  // 0x663710
auto joan::orig::ParticlePool::release_all() -> void { joan::bind::member<&::game_fx::H::pool_release_all>(this); }  // 0x663740
auto joan::orig::ParticlePool::alive() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx::H::pool_alive>(this)); }  // 0x6638e0
auto joan::orig::ParticlePool::free_at(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx::H::pool_free_at>(this, a0)); }  // 0x663910
auto joan::orig::ParticlePool::slot(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::pool_slot>(this, a0)); }  // 0x663960
