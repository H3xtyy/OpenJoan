// The few unit fields the game reads through out-of-line accessors (refactor R6b, docs/RUNTIME.md). Only the bytes
// these accessors touch are laid out; the rest of the unit is opaque here.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "../call.hpp"
#include "../fn_names.hpp"
#include "gen/point_list.hpp"
#include "gen/task_slot.hpp"
#include "gen/unit_stats.hpp"
#include "gen/upgrade_list.hpp"

namespace joan::orig {

#pragma pack(push, 1)

struct Actor;
struct ActorDef;
struct AnimSet;
struct ArrI32;
struct Cell;
struct Group;
struct Modifiers;
struct Order;
struct Scene;
struct SceneObject;
struct ScriptBlock;
struct Trail;
struct UnitDraw;
struct World;

/// Layout view of the original Unit object (a unit of a mission): the fields at their
/// original offsets that the replaced functions use. Owned by the unit table; cleanup is Unit::cleanup (0x00461f10).
struct Unit {
    std::int16_t id;  ///< 0x0
    std::int16_t side;  ///< 0x2
    std::int16_t place_index;  ///< 0x4
    std::int16_t place_slot;  ///< 0x6
    std::int16_t zone;  ///< 0x8
    std::int32_t anim_entry;  ///< 0xa
    float position_[3];  ///< 0x0e  x, y, z
    float heading;  ///< 0x1a
    float heading_goal;  ///< 0x1e
    std::int32_t move_mode;  ///< 0x22
    std::uint32_t u32_26;  ///< 0x26
    std::uint32_t last_update_ms;  ///< 0x2a
    std::int8_t frozen;  ///< 0x2e
    std::int8_t speech_off;  ///< 0x2f
    std::int8_t excluded;  ///< 0x30
    std::int16_t group_id;  ///< 0x31
    Group* group;  ///< 0x33
    std::int16_t leader_id;  ///< 0x37
    std::uint16_t team_flag;  ///< 0x39
    std::int16_t type_id;  ///< 0x3b
    char name_[66];  ///< 0x3d
    char name_b[66];  ///< 0x7f
    char name_c[65];  ///< 0xc1
    std::int16_t layer;  ///< 0x102
    std::int32_t rank;  ///< 0x104
    std::int16_t kingdom_id;  ///< 0x108
    std::int16_t race_id;  ///< 0x10a
    std::int32_t formation_slot;  ///< 0x10c
    std::int32_t state_120_saved;  ///< 0x110
    std::int32_t markers;  ///< 0x114
    std::int32_t last_hp_regen_ms;  ///< 0x118
    std::int32_t lives_max;  ///< 0x11c
    std::int32_t state_120;  ///< 0x120 nonzero keeps a unit without hit points from counting as dead
    std::int16_t level;  ///< 0x124
    std::int32_t experience;  ///< 0x126
    std::int16_t stat_points;  ///< 0x12a
    std::int16_t attr_points;  ///< 0x12c
    std::int16_t i16_12e;  ///< 0x12e
    std::uint8_t levelup_flag;  ///< 0x130
    std::int32_t levelup_time;  ///< 0x131
    std::int32_t hit_points;  ///< 0x135
    // --- members from re/layout/layouts.json (gen_types.py) ---
    std::int32_t hp_shown;  ///< 0x139
    std::int32_t hit_points_base;  ///< 0x13d
    std::int32_t regen_hp;  ///< 0x141
    std::int32_t mana;  ///< 0x145
    std::int32_t base_max_ep;  ///< 0x149
    std::int32_t regen_ep;  ///< 0x14d
    std::int32_t last_ep_regen_ms;  ///< 0x151
    std::int16_t stats[9];  ///< 0x155
    std::int32_t idle_time;  ///< 0x167
    std::int32_t target_select;  ///< 0x16b
    UnitStats stats_block;  ///< 0x16f
    UpgradeList upgrades;  ///< 0x1aa
    std::uint16_t kill_points;  ///< 0x1ae
    std::int16_t kills;  ///< 0x1b0
    std::int16_t kills_high_rank;  ///< 0x1b2
    std::int8_t crit_ready;  ///< 0x1b4
    std::uint16_t crit_hits;  ///< 0x1b5
    std::uint16_t crit_kills;  ///< 0x1b7
    std::int16_t squad_losses;  ///< 0x1b9
    std::int16_t squad_kills;  ///< 0x1bb
    std::int16_t carried_a;  ///< 0x1bd
    std::int16_t carried_b;  ///< 0x1bf
    Modifiers* modifiers;  ///< 0x1c1
    std::int16_t drop_item;  ///< 0x1c5
    std::int32_t treasure_level;  ///< 0x1c7
    std::int16_t morale;  ///< 0x1cb
    std::int32_t morale_time;  ///< 0x1cd
    std::int32_t fell_timer;  ///< 0x1d1
    std::uint16_t u16_1d5;  ///< 0x1d5
    std::uint8_t gap_1d7[0x6];  ///< 0x1d7 bytes not used by this view
    std::int32_t hit_value;  ///< 0x1dd
    std::uint8_t downed_hit;  ///< 0x1e1
    std::int32_t mount_id;  ///< 0x1e2
    std::int32_t rider_id;  ///< 0x1e6
    std::uint8_t mounted_flag;  ///< 0x1ea
    std::uint8_t dismounted_flag;  ///< 0x1eb
    std::uint8_t mount_pending;  ///< 0x1ec
    std::uint32_t mount_pending_ms;  ///< 0x1ed
    std::int32_t paired_anim_start_ms;  ///< 0x1f1
    std::int32_t mount_ref;  ///< 0x1f5
    std::int32_t rider_ref;  ///< 0x1f9
    std::int32_t held_item_id;  ///< 0x1fd
    std::uint8_t u8_201;  ///< 0x201
    std::uint8_t u8_202;  ///< 0x202
    float height_point[3];  ///< 0x203
    std::uint32_t act_time;  ///< 0x20f
    std::int16_t action;  ///< 0x213
    std::int16_t anim_kind;  ///< 0x215
    std::int16_t anim_kind_alt;  ///< 0x217
    std::int16_t prev_action;  ///< 0x219
    std::int16_t anim_item_using;  ///< 0x21b
    std::int16_t anim_special;  ///< 0x21d
    std::int32_t anim_start_ms;  ///< 0x21f
    std::int32_t anim_loop;  ///< 0x223
    std::uint8_t armour_flags[12];  ///< 0x227
    std::int8_t combo_state;  ///< 0x233
    std::int8_t combo_pre_len;  ///< 0x234
    std::int16_t combo_queued;  ///< 0x235
    std::uint32_t combo_min_time;  ///< 0x237
    std::uint32_t combo_max_time;  ///< 0x23b
    std::uint32_t combo_time;  ///< 0x23f
    std::int32_t gravity_level;  ///< 0x243
    std::int16_t think_forced;  ///< 0x247 the message queue's count (Queue::push3 0x00414f50 / Obj::find_pair 0x00414fe0 wrote it through the Queue view)
    std::uint8_t gap_249[0x50];  ///< 0x249 its 10 QueuePair records {i32 key, i16 b, i16 c} (name kept: the save records generator refers to it)
    ScriptBlock* script;  ///< 0x299
    std::int32_t task_kind;  ///< 0x29d
    Order* orders;  ///< 0x2a1
    TaskSlot task;  ///< 0x2a5
    TaskSlot task_next;  ///< 0x2be
    std::int16_t target_building;  ///< 0x2d7
    std::int16_t target_object_id;  ///< 0x2d9
    std::int16_t look_a;  ///< 0x2db
    std::int16_t look_b;  ///< 0x2dd
    std::int16_t attack_chance;  ///< 0x2df
    std::int8_t stance;  ///< 0x2e1
    std::int8_t prisoner_pose;  ///< 0x2e2
    std::int16_t hit_by;  ///< 0x2e3
    std::int16_t interact_item;  ///< 0x2e5
    std::int16_t interact_stage;  ///< 0x2e7
    std::int16_t can_interact;  ///< 0x2e9
    std::int16_t bribe_state;  ///< 0x2eb
    std::uint8_t in_scene;  ///< 0x2ed
    std::uint8_t instantiated;  ///< 0x2ee
    std::uint8_t gap_2ef[0x2];  ///< 0x2ef bytes not used by this view
    AnimSet* anim_set;  ///< 0x2f1
    Actor* actor;  ///< 0x2f5
    Actor* actor_b;  ///< 0x2f9
    ActorDef* actor_def_b;  ///< 0x2fd
    AnimSet* anim_set_b;  ///< 0x301
    std::uint32_t alt_mesh_state;  ///< 0x305
    Actor* actor_c;  ///< 0x309
    ActorDef* actor_def_c;  ///< 0x30d
    AnimSet* anim_set_c;  ///< 0x311
    std::uint32_t u32_315;  ///< 0x315
    SceneObject* model;  ///< 0x319
    UnitDraw* draw;  ///< 0x31d
    SceneObject* model_b;  ///< 0x321
    SceneObject* weapon_model;  ///< 0x325
    std::uint32_t item_ref;  ///< 0x329
    Trail* trail_a;  ///< 0x32d
    Trail* trail_b;  ///< 0x331
    World* world;  ///< 0x335
    Cell* cell;  ///< 0x339
    Unit* cell_prev;  ///< 0x33d
    Unit* cell_next;  ///< 0x341
    Scene* scene;  ///< 0x345
    std::uint32_t u32_349;  ///< 0x349
    std::uint32_t u32_34d;  ///< 0x34d
    std::uint32_t view;  ///< 0x351
    std::uint8_t anim_done;  ///< 0x355
    std::uint8_t mount_busy;  ///< 0x356
    std::uint8_t use_action_alt;  ///< 0x357
    std::uint8_t prisoner_attackable;  ///< 0x358
    float throw_target[3];  ///< 0x359
    float look_dir[3];  ///< 0x365
    std::int16_t hit_counter;  ///< 0x371
    std::uint8_t u8_373;  ///< 0x373
    std::int32_t getting_weapon;  ///< 0x374
    ArrI32* mask_slots;  ///< 0x378
    ArrI32* mask_slots_b;  ///< 0x37c
    std::int32_t slot_index;  ///< 0x380
    std::int32_t weapon;  ///< 0x384
    std::int32_t weapon_slot;  ///< 0x388
    std::int16_t crew_slots_ready;  ///< 0x38c
    std::int16_t mount_stick;  ///< 0x38e
    std::int32_t in_water;  ///< 0x390
    float splash_pos[3];  ///< 0x394
    std::int32_t snd_last_ms_a;  ///< 0x3a0
    std::uint32_t last_spawn_time;  ///< 0x3a4
    std::uint32_t fx_last_ms;  ///< 0x3a8
    std::int32_t step_time;  ///< 0x3ac
    std::int32_t step_time_b;  ///< 0x3b0
    std::int32_t siege_tower_state;  ///< 0x3b4
    std::uint32_t scripted_move;  ///< 0x3b8
    std::int32_t upg_timer;  ///< 0x3bc
    std::uint8_t can_be_ridden;  ///< 0x3c0
    std::uint32_t vision_range;  ///< 0x3c1
    std::int32_t collision_id;  ///< 0x3c5
    std::uint32_t name_index;  ///< 0x3c9
    float move_target[3];  ///< 0x3cd
    float follow_target[3];  ///< 0x3d9
    std::uint8_t u8_3e5;  ///< 0x3e5
    std::int32_t path_index;  ///< 0x3e6
    std::int32_t i32_3ea;  ///< 0x3ea
    std::uint32_t sidestep_phase;  ///< 0x3ee
    std::int32_t i32_3f2;  ///< 0x3f2
    std::uint32_t wait_count;  ///< 0x3f6
    std::uint32_t wait_limit;  ///< 0x3fa
    std::uint32_t u32_3fe;  ///< 0x3fe
    std::uint32_t stuck_count;  ///< 0x402
    std::uint32_t stuck_ms;  ///< 0x406
    float aim_heading;  ///< 0x40a
    std::uint32_t path_remaining;  ///< 0x40e
    float formation_offset[3];  ///< 0x412
    std::uint32_t form_width;  ///< 0x41e
    float formation_grid_x;  ///< 0x422
    std::uint32_t form_slot_y;  ///< 0x426
    std::uint8_t gap_42a[0x4];  ///< 0x42a bytes not used by this view
    std::int32_t weapon_model_id;  ///< 0x42e
    PointList path;  ///< 0x432
    PointList path_b;  ///< 0x473

    /// 0x004fedb0 Unit::position(out): copies the 12 position bytes to out and returns out.
    void* position(void* out) const noexcept {
        std::memcpy(out, position_, sizeof position_);
        return out;
    }
    /// 0x0042d0e0 Unit::dead(): 1 when hit points <= 0 and +0x120 is 0 (the original returns al).
    [[nodiscard]] std::uint8_t dead() const noexcept { return hit_points <= 0 && state_120 == 0 ? 1 : 0; }

    // Every other Unit function the game code calls (typed, through the function's slot; refactor R9).
#include "gen/unit_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Unit, position_) == 0x0e);
static_assert(offsetof(Unit, state_120) == 0x120);
static_assert(offsetof(Unit, hit_points) == 0x135);
#endif

}  // namespace joan::orig
