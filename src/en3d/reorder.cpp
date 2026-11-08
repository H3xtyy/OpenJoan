// en3d/reorder.cpp: functions of the original d:\ww\en3d\reorder.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/dynarray.hpp"
#include "../x87.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::str;

using i16 = std::int16_t;

void* const kMem = obj(d::g_mem);

using joan::orig::helpers::memset_;  // memset


}  // namespace

// ---------------------------------------- from modules/game_b89_04.cpp (game/b89_04)
//
//      0x005e61c0 Reorder::clear() - ret 0. Destroys every 0x28-byte element of each 0x10-byte
//                 bucket (+0x96, count +0x8a) through 0x005e8250 (backwards), frees the buckets'
//                 element arrays and the bucket array (0x0058ed70 on 0x00a48838, "reorder.cpp"),
//                 clears +0x8a / +0x8e / +0x92, calls 0x005e8b20 and empties the +0x5c / +0x2e / +0
//                 arrays (0x00417ad0 / 0x00417200 / 0x005d5360 / 0x005c5b50 / 0x00585d20).
//
//  - 0x004991e0 / 0x00459970 / 0x00451dc0 / 0x0044a950 / 0x00687500 / 0x005e61c0 return
//    nothing meaningful in eax (void).
namespace {
namespace game_b89_04 {

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }

void* const kMemory = obj(d::g_mem);
constexpr img::Addr kReorderCpp(d::s_d_ww_en3d_reorder_cpp);  // "d:\ww\en3d\reorder.cpp"

JOAN_COUNTER(c_reorder_clear, "Reorder::clear");

void free_(void* p, std::int32_t line) {
    joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(img::str(kReorderCpp)), joan::orig::arg::i32(line));  // 0x0058ed70
}

struct H {
    // ------------------------------------------------------------------------ 0x005e61c0
    void reorder_clear() {
        c_reorder_clear.hit();
        jo::self<jo::Reorder>(this)->current = nullptr;
        std::uint8_t* bucket = jo::view<std::uint8_t*>(jo::self<jo::Reorder>(this)->groups);  // ebp-0xc
        for (std::int32_t i = 0; i < jo::self<jo::Reorder>(this)->group_count; ++i, bucket += static_cast<std::int32_t>(sizeof(jo::ReorderGroup))) {
            for (std::int32_t j = jo::as_reorder_group(bucket)->count - 1; j >= 0; --j)
                joan::orig::as_plane(jo::view<std::uint8_t*>(jo::as_reorder_group(bucket)->batches) + j * static_cast<std::int32_t>(sizeof(jo::ReorderBatch)))->release_lights();  // 0x005e8250
            jo::view<std::uint32_t>(jo::as_reorder_group(bucket)->count) = 0;
            if (jo::as_reorder_group(bucket)->batches != nullptr) {
                free_(jo::view<void*>(jo::as_reorder_group(bucket)->batches), sx16(global<std::int16_t>(d::g_a17408)) + 0x12);
                jo::as_reorder_group(bucket)->batches = nullptr;
            }
            jo::view<std::uint32_t>(jo::as_reorder_group(bucket)->capacity) = 0;
        }
        jo::view<std::uint32_t>(jo::self<jo::Reorder>(this)->current_group) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Reorder>(this)->group_count) = 0;
        if (jo::self<jo::Reorder>(this)->groups != nullptr) {
            free_(jo::view<void*>(jo::self<jo::Reorder>(this)->groups), sx16(global<std::int16_t>(d::g_a17408)) + 0x1b);
            jo::self<jo::Reorder>(this)->groups = nullptr;
        }
        jo::view<std::uint32_t>(jo::self<jo::Reorder>(this)->group_capacity) = 0;
        joan::orig::reorder_release();  // 0x005e8b20
        for (std::int32_t i = joan::orig::as_dynarray(&jo::self<jo::Reorder>(this)->materials)->size(); i > 0; --i) {  // 0x00417ad0
            std::uint32_t* const e = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_dynarray(&jo::self<jo::Reorder>(this)->materials)->get(joan::orig::arg::i32(i)));  // 0x00417200
            if (e != nullptr && jo::view<std::uint32_t>(jo::as_material_slot(e)->slot) != 0) joan::orig::as_material(&jo::as_material_slot(e)->material)->dtor();  // 0x005d5360
        }
        joan::orig::as_sound(&jo::self<jo::Reorder>(this)->materials)->reset_585d20();  // 0x00585d20
        joan::orig::as_sound(&jo::self<jo::Reorder>(this)->sectors)->reset_585d20();
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {
            std::uint32_t* const e = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(i)));
            if (e != nullptr && jo::view<std::uint32_t>(jo::as_light_slot(e)->slot) != 0) joan::orig::as_light(&jo::as_light_slot(e)->light)->release();  // 0x005c5b50
        }
        joan::orig::as_sound(this)->reset_585d20();
    }
};

#define B89(name, addr, fn) JOAN_REPLACE("game/b89_04", name, addr, fn)
B89("RenderOrder::deinit", 0x005e61c0, &H::reorder_clear);
#undef B89

}  // namespace game_b89_04
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    std::uint8_t* light_state() { return reinterpret_cast<std::uint8_t*>(&jo::self<jo::Reorder>(this)->key); }                              // 0x005e7b80

    // --- Light manager / mirror ------------------------------------------------------------------------
    void light_lists_reset() {  // 0x005e8340
        for (std::uint8_t* l = jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->spans); l; l = jo::view<std::uint8_t*>(jo::as_span_node(l)->next)) {
            jo::view<std::uint32_t>(jo::as_span_node(l)->count) = 0;
            jo::as_span_node(l)->u32_10 = 0;
        }
        jo::self<jo::ReorderBatch>(this)->span_tail = nullptr;
        for (std::uint8_t* l = jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->strips); l; l = jo::view<std::uint8_t*>(jo::as_strip_node(l)->next)) {
            jo::view<std::uint32_t>(jo::as_strip_node(l)->vertex_range) = 0;
            jo::view<std::uint32_t>(jo::as_strip_node(l)->index_count) = 0;
            jo::as_strip_node(l)->primitive_count = 0;
        }
        jo::self<jo::ReorderBatch>(this)->strip_tail = nullptr;
    }
    void mirror_reset() {  // 0x005e6430: every group (+0x96, 0x10 each, count +0x8a) -> its lights (+0xc, 0x28 each)
        jo::self<jo::Reorder>(this)->current = nullptr;
        std::uint8_t* group = jo::view<std::uint8_t*>(jo::self<jo::Reorder>(this)->groups);
        for (std::int32_t i = 0; i < jo::self<jo::Reorder>(this)->group_count; ++i, group += static_cast<std::int32_t>(sizeof(jo::ReorderGroup))) {
            std::uint8_t* item = jo::view<std::uint8_t*>(jo::as_reorder_group(group)->batches);
            for (std::int32_t k = jo::as_reorder_group(group)->count; k > 0; --k, item += static_cast<std::int32_t>(sizeof(jo::ReorderBatch))) joan::orig::as_reorder_batch(item)->reset();  // 0x005e8340
        }
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("RenderOrder::get_cur_primitive_key", 0x005e7b80, &H::light_state);
HELPER6("RenderPrimitive::clear", 0x005e8340, &H::light_lists_reset);
HELPER6("RenderOrder::render_ordered_begin", 0x005e6430, &H::mirror_reset);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/en3d_helpers7.cpp (en3d/helpers7)
namespace {
namespace en3d_helpers7 {

std::uint32_t frame() { return global<std::uint32_t>(d::g_frame_a500ae); }

// Generic slot table (ilib): 0x00417200 get(i) -> entry or null, 0x00417ad0 count,
// 0x005857a0(0) prepare, 0x00417520 add -> new slot, 0x005858c0 remove(i).
std::uint8_t* table_get(void* table, std::int32_t i) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(table)->get(joan::orig::arg::i32(i))); }
std::int32_t table_count(void* table) { return joan::orig::as_dynarray(table)->size(); }

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- Light manager (scene.cpp area) ------------------------------------------------------------
    // 0x005e76f0: the slot (in the table at this) holding `light`'s state (0x005c6560 compare on
    // entry +8), trying `slot` first then from the end; else a new slot (0xd6-byte entry, state
    // 0x005c63c0). Stamps the frame (+4). Returns the slot or 0.
    std::int32_t light_slot(std::int32_t slot, void* light) {
        void* const table = this;
        std::uint8_t* e = table_get(table, slot);
        if (e && jo::view<std::uint32_t>(jo::as_light_slot(e)->slot) != 0 && joan::orig::arg::as<bool>(joan::orig::as_light(&jo::as_light_slot(e)->light)->equals(joan::orig::arg::mptr(light)))) {
            jo::as_light_slot(e)->frame = frame();
            return slot;
        }
        for (std::int32_t i = table_count(table); i > 0; --i) {
            e = table_get(table, i);
            if (e && jo::view<std::uint32_t>(jo::as_light_slot(e)->slot) != 0 && joan::orig::arg::as<bool>(joan::orig::as_light(&jo::as_light_slot(e)->light)->equals(joan::orig::arg::mptr(light)))) {
                jo::as_light_slot(e)->frame = frame();
                return i;
            }
        }
        joan::orig::dynarray_b(table)->linkin(nullptr);  // 0x005857a0
        slot = joan::orig::as_dynarray(table)->cursor();  // 0x00417520 add
        e = table_get(table, slot);
        if (e == nullptr) return 0;
        std::memset(e, 0, sizeof(jo::LightSlot));
        jo::as_light_slot(e)->slot = slot;
        joan::orig::as_light(&jo::as_light_slot(e)->light)->assign(joan::orig::arg::mptr(light));  // 0x005c63c0
        jo::as_light_slot(e)->frame = frame();
        return slot;
    }
    // 0x005e7870: the same for a sector's light set (table at +0x2e, 0x8d-byte entries,
    // compare 0x005e8100 / init 0x005e8040 on the entry itself).
    std::int32_t sector_slot(std::int32_t slot, void* sector) {
        void* const table = &jo::self<jo::Reorder>(this)->sectors;
        std::uint8_t* e = table_get(table, slot);
        if (e && jo::view<std::uint32_t>(jo::as_light_set(e)->slot) != 0 && joan::orig::arg::as<bool>(joan::orig::as_light_set(e)->same(joan::orig::arg::mptr(sector)))) {
            jo::as_light_set(e)->frame = frame();
            return slot;
        }
        for (std::int32_t i = table_count(table); i > 0; --i) {
            e = table_get(table, i);
            if (e && jo::view<std::uint32_t>(jo::as_light_set(e)->slot) != 0 && joan::orig::arg::as<bool>(joan::orig::as_light_set(e)->same(joan::orig::arg::mptr(sector)))) {
                jo::as_light_set(e)->frame = frame();
                return i;
            }
        }
        joan::orig::dynarray_b(table)->linkin(nullptr);
        slot = joan::orig::as_dynarray(table)->cursor();
        e = table_get(table, slot);
        if (e == nullptr) return 0;
        std::memset(e, 0, sizeof(jo::LightSet));  // 0x8d
        jo::as_light_set(e)->slot = slot;
        joan::orig::as_light_set(e)->build(joan::orig::arg::mptr(sector));  // 0x005e8040
        jo::as_light_set(e)->frame = frame();
        return slot;
    }
    // 0x005e64e0: drop entries not used for more than 3 frames - the groups' 0x28-byte items
    // (+0x96, count +0x8a; stale ones released by 0x005e8250, the rest compacted), the tables
    // at +0x5c (material dtor 0x005d5360), +0x2e and +0 (light state dtor 0x005c5b50).
    void purge() {
        std::uint8_t* group = jo::view<std::uint8_t*>(jo::self<jo::Reorder>(this)->groups);
        for (std::int32_t g = 0; g < jo::self<jo::Reorder>(this)->group_count; ++g, group += static_cast<std::int32_t>(sizeof(jo::ReorderGroup))) {
            std::int32_t kept = 0;
            std::uint8_t* item = jo::view<std::uint8_t*>(jo::as_reorder_group(group)->batches);
            std::uint8_t* write = item;
            for (std::int32_t k = 0; k < jo::as_reorder_group(group)->count; ++k, item += static_cast<std::int32_t>(sizeof(jo::ReorderBatch))) {
                if (frame() - jo::as_reorder_batch(item)->frame > 3) {
                    joan::orig::as_plane(item)->release_lights();  // 0x005e8250
                    continue;
                }
                if (item != write) std::memcpy(write, item, sizeof(jo::ReorderBatch));
                write += static_cast<std::int32_t>(sizeof(jo::ReorderBatch));
                ++kept;
            }
            jo::as_reorder_group(group)->count = kept;
        }
        void* const materials = &jo::self<jo::Reorder>(this)->materials;
        for (std::int32_t i = table_count(materials); i > 0; --i) {
            std::uint8_t* const e = table_get(materials, i);
            if (e && jo::view<std::uint32_t>(jo::as_material_slot(e)->slot) != 0 && frame() - jo::as_material_slot(e)->frame > 3) {
                jo::view<std::uint32_t>(jo::as_material_slot(e)->slot) = 0;
                joan::orig::as_material(&jo::as_material_slot(e)->material)->dtor();  // 0x005d5360
                joan::orig::dynarray_b(materials)->linkout(joan::orig::arg::i32(i));  // 0x005858c0 remove
            }
        }
        void* const sectors = &jo::self<jo::Reorder>(this)->sectors;
        for (std::int32_t i = table_count(sectors); i > 0; --i) {
            std::uint8_t* const e = table_get(sectors, i);
            if (e && jo::view<std::uint32_t>(jo::as_light_set(e)->slot) != 0 && frame() - jo::as_light_set(e)->frame > 3) {
                jo::view<std::uint32_t>(jo::as_light_set(e)->slot) = 0;
                joan::orig::dynarray_b(sectors)->linkout(joan::orig::arg::i32(i));
            }
        }
        void* const lights = this;
        for (std::int32_t i = table_count(lights); i > 0; --i) {
            std::uint8_t* const e = table_get(lights, i);
            if (e && jo::view<std::uint32_t>(jo::as_light_slot(e)->slot) != 0 && frame() - jo::as_light_slot(e)->frame > 3) {
                jo::view<std::uint32_t>(jo::as_light_slot(e)->slot) = 0;
                joan::orig::as_light(&jo::as_light_slot(e)->light)->release();  // 0x005c5b50
                joan::orig::dynarray_b(lights)->linkout(joan::orig::arg::i32(i));
            }
        }
        jo::self<jo::Reorder>(this)->current = nullptr;
    }
};

#define HELPER7(name, addr, fn) JOAN_REPLACE("en3d/helpers7", name, addr, fn)
HELPER7("RenderOrder::get_light_setting_id", 0x005e76f0, &H::light_slot);
HELPER7("RenderOrder::get_sector_light_set_id", 0x005e7870, &H::sector_slot);
HELPER7("RenderOrder::render_ordered_end", 0x005e64e0, &H::purge);
#undef HELPER7

}  // namespace en3d_helpers7
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
//
//    - Reorder::slot_for 0x005e79f0 (key, arg), ret 8: list +0x5c; reuses the key slot or the
//      last matching slot (0x5d5500), else appends a cleared 0x5c-byte slot (0x5d53e0); stamps
//      +4 = [0xa500ae]; returns the slot index or 0.
//
//    - Reorder::add_span 0x005e88c0 (10 args, ret 0x28): next free node of the +0x20/+0x24
//      chain (0x5e8de0 allocates at the end), fills it, returns 0.
namespace {
namespace game_b98_06 {


using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_slot_for, "Reorder::slot_for");
JOAN_COUNTER(c_add_span, "Reorder::add_span");

struct H {
    // ------------------------------------------------------------------ 0x005e79f0
    i32 slot_for(i32 key, void* arg) {
        c_slot_for.hit();
        void* const list = &jo::self<jo::Reorder>(this)->materials;
        u8* e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(key)));  // 0x00417200
        if (e != nullptr && jo::as_material_slot(e)->slot != 0 && joan::orig::arg::as<u8>(joan::orig::as_material(&jo::as_material_slot(e)->material)->equals(arg)) != 0) {  // 0x005d5500
            jo::as_material_slot(e)->frame = global<u32>(d::g_frame_a500ae);
            return key;
        }
        for (i32 i = joan::orig::as_dynarray(list)->size(); i > 0; --i) {  // 0x00417ad0
            e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(i)));  // 0x00417200
            if (e != nullptr && jo::as_material_slot(e)->slot != 0 && joan::orig::arg::as<u8>(joan::orig::as_material(&jo::as_material_slot(e)->material)->equals(arg)) != 0) {
                jo::as_material_slot(e)->frame = global<u32>(d::g_frame_a500ae);
                return i;
            }
        }
        joan::orig::dynarray_b(list)->linkin(nullptr);             // 0x005857a0
        const i32 k = joan::orig::as_dynarray(list)->cursor();  // 0x00417520
        e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(k)));  // 0x00417200
        if (e == nullptr) return 0;
        crt_memset(e, 0, sizeof(jo::MaterialSlot));
        jo::as_material_slot(e)->slot = k;
        joan::orig::as_material(&jo::as_material_slot(e)->material)->assign(arg);  // 0x005d53e0
        jo::as_material_slot(e)->frame = global<u32>(d::g_frame_a500ae);
        return k;
    }

    // ------------------------------------------------------------------ 0x005e88c0
    i32 add_span(void* a1, u32 /*a2 unused*/, u32 a3, u32 a4, void* a5, u32 a6, u32 a7, u32 a8, jo::Bone* a9, u32 a10) {
        c_add_span.hit();
        u8* const desc = joan::orig::arg::as<u8*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(jo::view<i8>(jo::self<jo::ReorderBatch>(this)->vertex_format)))));  // 0x00600200 (movsx byte)
        const u32 v8 = jo::as_vertex_format(desc)->stride;
        const i32 dc = static_cast<i32>((a4 & 0xffff) - (a3 & 0xffff));
        const i32 dr = static_cast<i32>(a7 - a6);
        {
            u8* const prev = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strip_tail);
            if (jo::view<void*>(jo::self<jo::ReorderBatch>(this)->strip_tail) == nullptr)
                jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strip_tail) = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strips);
            else
                jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strip_tail) = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strip_tail->next);
            if (jo::view<void*>(jo::self<jo::ReorderBatch>(this)->strip_tail) == nullptr) {
                jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strip_tail) = static_cast<u8*>(joan::orig::reorder::alloc_30(dc, joan::orig::arg::i32(v8)));  // 0x005e8de0 (the third pushed value, dr, is never read)
                if (prev == nullptr)
                    jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strips) = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strip_tail);
                else
                    jo::view<u8*>(jo::as_strip_node(prev)->next) = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strip_tail);
            }
        }
        u8* const n = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strip_tail);
        jo::as_strip_node(n)->vertices = 0;
        jo::as_strip_node(n)->vertex_buffer = a1;
        jo::as_strip_node(n)->vertex_range = dc;
        jo::as_strip_node(n)->min_index = a3 & 0xffff;
        jo::as_strip_node(n)->indices = nullptr;
        jo::as_strip_node(n)->index_buffer = a5;
        jo::as_strip_node(n)->start_index = a6;
        jo::as_strip_node(n)->index_count = dr;
        jo::as_strip_node(n)->primitive_count = a8;
        jo::as_strip_node(n)->bones = a9;
        jo::as_strip_node(n)->bone_count = a10;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("RenderOrder::get_material_setting_id", 0x005e79f0, &H::slot_for);
B("RenderPrimitive::add_vertex_and_index_static_buffer", 0x005e88c0, &H::add_span);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
//
//      0x005e7bb0 Reorder::set_count(n)      (thiscall, ret 4) - grows the 16-byte entry array
//                 (+0x96, used +0x8a, capacity +0x8e) via Memory::realloc, zero-fills new entries.
namespace {
namespace game_b95_06 {

JOAN_COUNTER(c_reorder_set_count, "Reorder::set_count");

void* mem_realloc(void* p, u32 size, const char* file, i32 line) {
    return joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
}

struct H {
    // ------------------------------------------------------------------------ 0x005e7bb0
    void reorder_set_count(i32 n) {
        c_reorder_set_count.hit();
        if (n < 0) n = 0;
        if (n >= jo::self<jo::Reorder>(this)->group_count) {
            const i32 m = n + 1;
            if (m > jo::self<jo::Reorder>(this)->group_capacity) {
                jo::self<jo::Reorder>(this)->group_capacity = m + 0xf;
                const i32 line = static_cast<i32>(global<i16>(d::g_a1740c)) + 0x14;
                jo::view<void*>(jo::self<jo::Reorder>(this)->groups) = mem_realloc(jo::view<void*>(jo::self<jo::Reorder>(this)->groups), static_cast<u32>(jo::self<jo::Reorder>(this)->group_capacity) * static_cast<u32>(sizeof(jo::ReorderGroup)), str(d::s_d_ww_en3d_reorder_cpp), line);
            }
            memset_(jo::view<u8*>(jo::self<jo::Reorder>(this)->groups) + (static_cast<u32>(jo::self<jo::Reorder>(this)->group_count) * static_cast<u32>(sizeof(jo::ReorderGroup))), 0,
                    static_cast<u32>(m - jo::self<jo::Reorder>(this)->group_count) * static_cast<u32>(sizeof(jo::ReorderGroup)));
            jo::self<jo::Reorder>(this)->group_count = m;
        }
        jo::self<jo::Reorder>(this)->current_group = n;
        jo::self<jo::Reorder>(this)->current = nullptr;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("RenderOrder::set_cur_object_type", 0x005e7bb0, &H::reorder_set_count);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
//
//    - Reorder::find_or_insert 0x005e7ce0 (char), ret 4: binary search (0x5911a0) of the key
//      +0x9e in the +0x96[+0x92] table, insert (grow by 0x80, 0x58eb30 "reorder.cpp" /
//      0x5910f0 / 0x5e8210) if missing; +0x9a = entry, entry +0x14 = [0xa500ae].
//
//    - Reorder::push_span 0x005e8400 (7 args), ret 0x1c -> 0: takes / allocates (0x5e8c40) a
//      node from the +0x18 / +0x1c free list and fills it.
//
//  - 0x005e7ce0: with a non-zero argument the original skips the binary search and then reads
//    the uninitialised local index (ebp-0x10). All five callers (0x5d3c..0x5d3f..) push 0, so
//    that path is dead; the replacement uses 0 there (deterministic). With arg 0, 0x5911a0
//    always writes the index before returning 0 (checked).
//
//  - Return values: callers of 0x5acfa0, 0x5b8a50, 0x5c80d0, 0x5e7ce0, 0x601140, 0x616dd0,
//    0x623bd0 ignore eax (checked); 0x5d5fe0's callers use al only.
namespace {
namespace game_b100_11 {


void* const kHeap = obj(d::g_mem);
void* const kSearch = obj(d::g_misc);  // binary search / insert helpers (0x5911a0 / 0x5910f0)

i32 line_at(img::Addr a, i32 add_) { return static_cast<i32>(global<i16>(a)) + add_; }
void* heap_realloc(void* p, u32 n, const char* file, i32 line) {
    return joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(n), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
}
i32 bsearch_(const void* key, void* base, i32 count, i32 stride, const void* cmp, i32* out) {
    return joan::orig::arg::as<i32>(joan::orig::as_misc(kSearch)->bin_search_5911a0(joan::orig::arg::mptr(key), joan::orig::arg::str(base), joan::orig::arg::i32(count), joan::orig::arg::i32(stride), joan::orig::arg::as<std::uint32_t>(cmp), joan::orig::arg::mptr(out)));  // 0x005911a0
}

JOAN_COUNTER(c_reorder_find, "Reorder::find_or_insert");
JOAN_COUNTER(c_push_span, "Reorder::push_span");

struct H {
    // ------------------------------------------------------------------ 0x005e7ce0
    u8* reorder_find(i8 skip_search) {
        c_reorder_find.hit();
        u8* const t = jo::view<u8*>(jo::self<jo::Reorder>(this)->groups) + (jo::view<u32>(jo::self<jo::Reorder>(this)->current_group) * static_cast<u32>(sizeof(jo::ReorderGroup)));
        i32 found = 0;
        i32 pos = 0;  // uninitialised in the original when skip_search != 0 (dead: callers push 0)
        if (static_cast<i32>(skip_search) == 0)
            found = bsearch_(&jo::self<jo::Reorder>(this)->key, jo::view<void*>(jo::as_reorder_group(t)->batches), jo::as_reorder_group(t)->count, static_cast<std::int32_t>(sizeof(jo::ReorderBatch)), fn::code_ptr(fn::cmp20_5e8a80), &pos);  // cmp 0x5e8a80
        if (found == 0) {
            if (!(jo::as_reorder_group(t)->count < jo::as_reorder_group(t)->capacity)) {
                jo::as_reorder_group(t)->capacity = jo::as_reorder_group(t)->count + 0x80;
                jo::view<void*>(jo::as_reorder_group(t)->batches) = heap_realloc(jo::view<void*>(jo::as_reorder_group(t)->batches), static_cast<u32>(jo::as_reorder_group(t)->capacity * static_cast<std::int32_t>(sizeof(jo::ReorderBatch))),
                                                 str(d::s_d_ww_en3d_reorder_cpp), line_at(d::g_a17410, 0x14));  // "reorder.cpp"
            }
            void* const slot = joan::orig::arg::as<void*>(joan::orig::as_misc(kSearch)->insert_array_rec_5910f0(joan::orig::arg::str(jo::view<void*>(jo::as_reorder_group(t)->batches)), joan::orig::arg::i32(jo::as_reorder_group(t)->count), static_cast<std::int32_t>(sizeof(jo::ReorderBatch)), joan::orig::arg::i32(pos), joan::orig::arg::mptr(static_cast<void*>(nullptr))));  // 0x005910f0
            jo::as_reorder_group(t)->count = jo::as_reorder_group(t)->count + 1;
            joan::orig::as_reorder_batch(slot)->construct(joan::orig::arg::mptr(&jo::self<jo::Reorder>(this)->key));  // 0x005e8210
            found = pos;
        }
        jo::view<u8*>(jo::self<jo::Reorder>(this)->current) = jo::view<u8*>(jo::as_reorder_group(t)->batches) + found * static_cast<std::int32_t>(sizeof(jo::ReorderBatch)) - static_cast<std::int32_t>(sizeof(jo::ReorderBatch));
        jo::self<jo::Reorder>(this)->current->frame = global<u32>(d::g_frame_a500ae);
        return jo::view<u8*>(jo::self<jo::Reorder>(this)->current);
    }

    // ------------------------------------------------------------------ 0x005e8400
    i32 push_span(void* base, u32 /*unused*/, i32 from, i32 to, u32 v10, jo::Bone* v14, u32 v18) {
        c_push_span.hit();
        const i32 stride = jo::view<i32>(jo::as_vertex_format(joan::orig::arg::as<u8*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<i32>(jo::view<i8>(jo::self<jo::ReorderBatch>(this)->vertex_format)))))))->stride);  // 0x00600200
        const i32 n = to - from;
        u8* const prev = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->span_tail);
        if (jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->span_tail) == nullptr)
            jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->span_tail) = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->spans);
        else
            jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->span_tail) = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->span_tail->next);
        if (jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->span_tail) == nullptr) {
            jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->span_tail) = joan::orig::arg::as<u8*>(joan::orig::reorder::alloc_20(joan::orig::arg::i32(n), joan::orig::arg::i32(stride)));  // 0x005e8c40
            if (prev == nullptr)
                jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->spans) = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->span_tail);
            else
                jo::view<u8*>(jo::as_span_node(prev)->next) = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->span_tail);
        }
        u8* const node = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->span_tail);
        jo::as_span_node(node)->vertex_buffer = nullptr;
        jo::as_span_node(node)->vertices = static_cast<u8*>(base) + from * stride;
        jo::view<u32>(jo::as_span_node(node)->first) = 0;
        jo::as_span_node(node)->count = n;
        jo::as_span_node(node)->u32_10 = v10;
        jo::as_span_node(node)->bone = v14;
        jo::as_span_node(node)->u32_18 = v18;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("RenderOrder::set_cur_primitive", 0x005e7ce0, &H::reorder_find);
B("RenderPrimitive::add_vertex", 0x005e8400, &H::push_span);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b101_11.cpp (game/b101_11)
//
//    - Mesh::push_span_fwd 0x005e7e70 (7 args), ret 0x1c: forwards to 0x005e8400 on [+0x9a].
namespace {
namespace game_b101_11 {

struct H {
    // ------------------------------------------------------------------ 0x005e7e70
    u32 push_span_fwd(void* a0, u32 a1, u32 a2, u32 a3, u32 a4, jo::Bone* a5, u32 a6) {
        return joan::orig::arg::as<u32>(joan::orig::as_reorder(jo::view<void*>(jo::self<jo::Reorder>(this)->current))->push_span(a0, joan::orig::arg::u32(a1), joan::orig::arg::i32(a2), joan::orig::arg::i32(a3), joan::orig::arg::u32(a4), a5, joan::orig::arg::u32(a6)));  // 0x005e8400
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_11", name, addr, fn)
B("RenderOrder::add_vertex", 0x005e7e70, &H::push_span_fwd);
#undef B

}  // namespace game_b101_11
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
//
//    - Reorder::forward 0x005e7ec0 (10 args), ret 0x28: (+0x9a)->0x5e8540(...).
//
//    - Obj::set_28 0x005e8210 (src), ret 4: memset(this, 0, 0x28), copy 5 dwords.
namespace {
namespace game_b97_06 {

void* crt_memset(void* d, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {

    // 0x005e7ec0
    std::uint32_t reorder_forward(void* a1, std::uint32_t a2, std::uint16_t a3, std::uint16_t a4, void* a5,
                                  std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, jo::Bone* a9, std::uint32_t a10) {
        // The words are pushed as dwords; the callee masks them (and 0xffff).
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_reorder(jo::view<void*>(jo::self<jo::Reorder>(this)->current))->add_batch(a1, joan::orig::arg::u32(a2), joan::orig::arg::u32(static_cast<std::uint32_t>(a3)), joan::orig::arg::u32(static_cast<std::uint32_t>(a4)), a5, joan::orig::arg::i32(a6), joan::orig::arg::i32(a7), joan::orig::arg::u32(a8), a9, joan::orig::arg::u32(a10)));  // 0x005e8540
    }

    // 0x005e8210
    void* set_28(const std::uint32_t* src) {
        crt_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::ReorderBatch)));
        const jo::BatchKey* const key = reinterpret_cast<const jo::BatchKey*>(src);  // the batch starts with its key
        jo::self<jo::ReorderBatch>(this)->key = key->key;
        std::memcpy(&jo::self<jo::ReorderBatch>(this)->vertex_format, &key->vertex_format, 4);  // format + padding as one dword
        jo::self<jo::ReorderBatch>(this)->states = key->states;
        jo::view<std::uint32_t>(jo::self<jo::ReorderBatch>(this)->material) = jo::view<std::uint32_t>(key->material);
        jo::view<std::uint32_t>(jo::self<jo::ReorderBatch>(this)->prim) = jo::view<std::uint32_t>(key->prim);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("RenderOrder::add_vertex_and_index", 0x005e7ec0, &H::reorder_forward);
B("RenderPrimitive::init", 0x005e8210, &H::set_28);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
//
//      0x005e7f20 Obj::forward_9a(7 args)         (thiscall, ret 0x1c) - 0x005e8780 on [+0x9a].
namespace {
namespace game_b103_11 {

// 0x005e8a50 / 0x005e8a80
i32 JOAN_CDECL cmp20(const void* a, const void* b) { return joan::orig::arg::as<i32>(joan::orig::libc::memcmp(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), 0x14u)); }

struct H {
    // 0x005e7f20
    u32 forward_9a(u32 a, u32 b, u32 c, u32 d, u32 e, u32 g, u32 h) {
        return joan::orig::arg::as<u32>(joan::orig::as_reorder_batch(jo::view<void*>(jo::self<jo::Reorder>(this)->current))->alloc(joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::u32(e), joan::orig::arg::u32(g), joan::orig::arg::u32(h)));  // 0x005e8780
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("RenderOrder::add_vertex_static_buffer", 0x005e7f20, &H::forward_9a);
B103("cmp20_5e8a50", 0x005e8a50, &cmp20);
B103("cmp20_5e8a80", 0x005e8a80, &cmp20);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - Reorder::add_span_fwd 0x005e7f70 (10 args), ret 0x28: 0x5e88c0 on +0x9a.
//
//    - span_begin 0x005e8ab0 (cdecl): resets 4 globals when [0xa69a84] == 0, ++[0xa69a84].
namespace {
namespace game_b99_07 {

struct H {
    // ------------------------------------------------------------------ 0x005e7f70
    i32 add_span_fwd(void* a1, u32 a2, u32 a3, u32 a4, void* a5, u32 a6, u32 a7, u32 a8, jo::Bone* a9, u32 a10) {
        return joan::orig::arg::as<i32>(joan::orig::as_reorder(jo::view<void*>(jo::self<jo::Reorder>(this)->current))->add_span(a1, joan::orig::arg::u32(a2), joan::orig::arg::u32(a3), joan::orig::arg::u32(a4), a5, joan::orig::arg::u32(a6), joan::orig::arg::u32(a7), joan::orig::arg::u32(a8), a9, joan::orig::arg::u32(a10)));  // 0x005e88c0
    }
};

// ---------------------------------------------------------------------------- 0x005e8ab0
static i32 JOAN_CDECL span_begin() {
    if (global<i32>(d::g_reorder_pool_refcount) == 0) {
        global<jo::SpanNode*>(d::g_reorder_span_free_list) = nullptr;
        global<u32>(d::g_reorder_span_free_count) = 0;
        global<jo::StripNode*>(d::g_reorder_strip_free_list) = nullptr;
        global<u32>(d::g_reorder_strip_free_count) = 0;
    }
    global<i32>(d::g_reorder_pool_refcount) = global<i32>(d::g_reorder_pool_refcount) + 1;
    return global<i32>(d::g_reorder_pool_refcount);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("RenderOrder::add_vertex_and_index_static_buffer", 0x005e7f70, &H::add_span_fwd);
B("RenderPrimitive::cache_add_ref", 0x005e8ab0, &span_begin);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b102_10.cpp (game/b102_10)
//
//    - Reorder::init 0x005e7fd0 (6 args), ret 0x18.
namespace {
namespace game_b102_10 {

void* crt_memset(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // 0x005e7fd0 (the third argument is unused)
    H* reorder_init(u32 a, u8 b, u32 /*unused*/, u32 d, u32 e, u32 g) {
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::BatchKey)));  // 0x14
        jo::view<u32>(jo::self<jo::BatchKey>(this)->key) = a;
        jo::self<jo::BatchKey>(this)->vertex_format = b;
        jo::self<jo::BatchKey>(this)->states = d;
        jo::view<u32>(jo::self<jo::BatchKey>(this)->material) = e;
        jo::view<u32>(jo::self<jo::BatchKey>(this)->prim) = g;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_10", name, addr, fn)
B("Reorder::init", 0x005e7fd0, &H::reorder_init);
#undef B

}  // namespace game_b102_10
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {

JOAN_COUNTER(c_light_set_build, "LightSet::build");
JOAN_COUNTER(c_light_set_same, "LightSet::same");

struct H {
    // 0x005e8040 LightSet::build(list): ids (+0xca) of up to 32 lights, plus the list's flag.
    H* light_set_build(void* list) {
        c_light_set_build.hit();
        jo::self<jo::LightSet>(this)->light_count = 0;
        for (std::int32_t i = 0;; ++i) {
            const std::uint8_t* const light = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene_s(list)->light_at(joan::orig::arg::i32(i)));  // 0x005f2730
            if (light == nullptr) break;
            if (jo::self<jo::LightSet>(this)->light_count < 0x20) {
                jo::self<jo::LightSet>(this)->light_ids[jo::self<jo::LightSet>(this)->light_count] = jo::as_light(light)->u32_ca;
                jo::self<jo::LightSet>(this)->light_count = jo::self<jo::LightSet>(this)->light_count + 1;
            }
        }
        jo::self<jo::LightSet>(this)->lit = joan::orig::arg::as<std::uint8_t>(joan::orig::as_scene_s(list)->lit());  // 0x005e8f90
        return this;
    }
    // 0x005e8100 LightSet::same(list).
    bool light_set_same(void* list) {
        c_light_set_same.hit();
        std::int32_t count = 0;
        while (joan::orig::arg::as<void*>(joan::orig::as_scene_s(list)->light_at(joan::orig::arg::i32(count))) != nullptr) ++count;
        if (count >= 0x20) count = 0x20;
        if (jo::self<jo::LightSet>(this)->light_count != count) return false;
        for (std::int32_t i = 0;; ++i) {
            const std::uint8_t* const light = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene_s(list)->light_at(joan::orig::arg::i32(i)));
            if (light == nullptr) break;
            if (jo::self<jo::LightSet>(this)->light_ids[i] != jo::as_light(light)->u32_ca) return false;
        }
        return jo::self<jo::LightSet>(this)->lit == joan::orig::arg::as<std::uint8_t>(joan::orig::as_scene_s(list)->lit());
    }

    // --- en3d/helpers17 ---------------------------------------------------------------------
    // 0x005e8250 Plane::release_lights: both node lists returned to their pools.
    void plane_release_lights() {
        // The pool word read here was pushed as a second argument the free functions never read.
        static_cast<void>(jo::as_vertex_format(joan::orig::arg::as<void*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::ReorderBatch>(this)->vertex_format)))))))->stride);
        for (std::uint8_t* node = jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->strips); node != nullptr;) {
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_strip_node(node)->next);
            joan::orig::light_node_free_b(node);  // 0x005e8f10
            node = next;
        }
        jo::self<jo::ReorderBatch>(this)->strip_tail = nullptr;
        jo::self<jo::ReorderBatch>(this)->strips = nullptr;
        for (std::uint8_t* node = jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->spans); node != nullptr;) {
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_span_node(node)->next);
            joan::orig::light_node_free_a(node);  // 0x005e8d60
            node = next;
        }
        jo::self<jo::ReorderBatch>(this)->span_tail = nullptr;
        jo::self<jo::ReorderBatch>(this)->spans = nullptr;
    }
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
#define HELPER17(name, addr, fn) JOAN_REPLACE("en3d/helpers17", name, addr, fn)
HELPER16("SectorLightSet::operator=", 0x005e8040, &H::light_set_build);
HELPER16("SectorLightSet::operator==", 0x005e8100, &H::light_set_same);
HELPER17("RenderPrimitive::deinit", 0x005e8250, &H::plane_release_lights);
#undef HELPER16
#undef HELPER17

}  // namespace en3d_helpers16
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
//
//      0x005e8540 Reorder::add_batch(10 args)       (thiscall, ret 0x28) - 0.
namespace {
namespace game_b96_06 {

using u16 = std::uint16_t;

// The index-buffer address + a 32-bit byte offset (address-sized arithmetic, as the original's).
u8* add(const void* p, u32 n) { return reinterpret_cast<u8*>(reinterpret_cast<std::uintptr_t>(p) + n); }

JOAN_COUNTER(c_add_batch, "Reorder::add_batch");

struct H {
    // ------------------------------------------------------------------ 0x005e8540
    i32 add_batch(void* a1, u32 a2, u32 a3, u32 a4, u16* idx, i32 first, i32 last, u32 a8, jo::Bone* a9, u32 a10) {
        c_add_batch.hit();
        u16 lo = static_cast<u16>(a3);
        u16 hi = static_cast<u16>(a4);
        if (static_cast<i32>(hi) < static_cast<i32>(lo)) {
            lo = static_cast<u16>(a2);
            hi = 0;
            for (i32 i = first; i < last; ++i) {
                const u16 v = *reinterpret_cast<u16*>(add(idx, static_cast<u32>(i) * 2));
                if (static_cast<i32>(v) < static_cast<i32>(lo)) lo = v;
                if (static_cast<i32>(v) >= static_cast<i32>(hi)) hi = static_cast<u16>(static_cast<i32>(v) + 1);
            }
        }
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(jo::self<jo::ReorderBatch>(this)->vertex_format))));  // 0x00600200
        const u32 pv = jo::as_vertex_format(p)->stride;
        const i32 range = static_cast<i32>(hi) - static_cast<i32>(lo);
        const i32 count = last - first;
        {
            u8* const prev = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strip_tail);
            if (jo::self<jo::ReorderBatch>(this)->strip_tail == nullptr) {
                jo::self<jo::ReorderBatch>(this)->strip_tail = jo::self<jo::ReorderBatch>(this)->strips;
            } else {
                jo::self<jo::ReorderBatch>(this)->strip_tail = jo::self<jo::ReorderBatch>(this)->strip_tail->next;
            }
            if (jo::self<jo::ReorderBatch>(this)->strip_tail == nullptr) {
                jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strip_tail) = static_cast<u8*>(joan::orig::reorder::alloc_30(range, joan::orig::arg::i32(pv)));  // 0x005e8de0 (the third pushed value, count, is never read)
                if (prev == nullptr) {
                    jo::self<jo::ReorderBatch>(this)->strips = jo::self<jo::ReorderBatch>(this)->strip_tail;
                } else {
                    jo::as_strip_node(prev)->next = jo::self<jo::ReorderBatch>(this)->strip_tail;
                }
            }
        }
        u8* const n = jo::view<u8*>(jo::self<jo::ReorderBatch>(this)->strip_tail);
        jo::as_strip_node(n)->vertex_buffer = 0;
        jo::as_strip_node(n)->vertices = a1;
        jo::as_strip_node(n)->vertex_range = range;
        jo::as_strip_node(n)->min_index = static_cast<u32>(lo);
        jo::as_strip_node(n)->index_buffer = 0;
        jo::view<u8*>(jo::as_strip_node(n)->indices) = add(idx, static_cast<u32>(first) * 2);
        jo::as_strip_node(n)->start_index = 0;
        jo::as_strip_node(n)->index_count = count;
        jo::as_strip_node(n)->primitive_count = a8;
        jo::as_strip_node(n)->bones = a9;
        jo::as_strip_node(n)->bone_count = a10;
        return 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("RenderPrimitive::add_vertex_and_index", 0x005e8540, &H::add_batch);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
//
//  * Obj::flag59 0x005c6790, VbAlloc::reset 0x005e4430, VbAlloc::alloc 0x005e8780 (node
//    from the free list or 0x005e8c40), Obj::get4c 0x005eeba0, get24 0x005f0020 (cdecl).
namespace {
namespace game_b94_06 {

JOAN_COUNTER(c_vb_alloc, "VbAlloc::alloc");

struct H {
    // 0x005e8780 VbAlloc::alloc(owner, unused, start, end, a5, a6, a7)
    std::int32_t vb_alloc(std::uint32_t owner, std::uint32_t, std::int32_t start, std::int32_t end, std::uint32_t a5,
                          std::uint32_t a6, std::uint32_t a7) {
        c_vb_alloc.hit();
        const std::uint8_t* const desc = joan::orig::arg::as<std::uint8_t*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<std::int8_t>(jo::view<std::int8_t>(jo::self<jo::ReorderBatch>(this)->vertex_format))))));  // 0x00600200
        const std::uint32_t stride = jo::as_vertex_format(desc)->stride;
        const std::int32_t len = static_cast<std::int32_t>(static_cast<std::uint32_t>(end) - static_cast<std::uint32_t>(start));
        std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->span_tail);
        if (jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->span_tail) == nullptr)
            jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->span_tail) = jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->spans);
        else
            jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->span_tail) = jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->span_tail->next);
        if (jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->span_tail) == nullptr) {
            jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->span_tail) = joan::orig::arg::as<std::uint8_t*>(joan::orig::reorder::alloc_20(joan::orig::arg::i32(len), joan::orig::arg::i32(stride)));  // 0x005e8c40
            if (prev == nullptr)
                jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->spans) = jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->span_tail);
            else
                jo::view<std::uint8_t*>(jo::as_span_node(prev)->next) = jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->span_tail);
        }
        std::uint8_t* const node = jo::view<std::uint8_t*>(jo::self<jo::ReorderBatch>(this)->span_tail);
        jo::as_span_node(node)->vertices = nullptr;
        jo::as_span_node(node)->vertex_buffer = joan::orig::arg::as<void*>(owner);
        jo::as_span_node(node)->first = start;
        jo::as_span_node(node)->count = len;
        jo::as_span_node(node)->u32_10 = a5;
        jo::as_span_node(node)->bone = joan::orig::arg::as<jo::Bone*>(a6);
        jo::as_span_node(node)->u32_18 = a7;
        return 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("RenderPrimitive::add_vertex_static_buffer", 0x005e8780, &H::vb_alloc);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
//
//      0x005e8b20 reorder_release()                           cdecl (en3d/reorder.cpp)
namespace {
namespace game_b90_02 {

JOAN_COUNTER(c_reorder, "reorder_release");

// 0x005e8b20
void JOAN_CDECL reorder_release() {
    c_reorder.hit();
    global<std::int32_t>(d::g_reorder_pool_refcount) = global<std::int32_t>(d::g_reorder_pool_refcount) - 1;
    if (global<std::int32_t>(d::g_reorder_pool_refcount) != 0) return;
    for (void* p = global<void*>(d::g_reorder_strip_free_list); p != nullptr;) {
        void* const next = jo::as_strip_node(p)->next;
        joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(p), str(d::s_d_ww_en3d_reorder_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a17414)) + 0x11));  // 0x0058ed70
        global<std::int32_t>(d::g_reorder_strip_free_count) = global<std::int32_t>(d::g_reorder_strip_free_count) - 1;
        p = next;
    }
    global<jo::StripNode*>(d::g_reorder_strip_free_list) = nullptr;
    for (void* p = global<void*>(d::g_reorder_span_free_list); p != nullptr;) {
        void* const next = jo::as_span_node(p)->next;
        joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(p), str(d::s_d_ww_en3d_reorder_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a17414)) + 0x1e));  // 0x0058ed70
        global<std::int32_t>(d::g_reorder_span_free_count) = global<std::int32_t>(d::g_reorder_span_free_count) - 1;
        p = next;
    }
    global<jo::SpanNode*>(d::g_reorder_span_free_list) = nullptr;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("RenderPrimitive::cache_release", 0x005e8b20, &reorder_release);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
//
//      0x005e8c40 Reorder::alloc_20    (cdecl(a, b)) - free list 0xa69a74, count 0xa69a80.
//
//      0x005e8de0 Reorder::alloc_30    (cdecl(a, b)) - free list 0xa69a78, count 0xa69a7c.
//
//  - 0x005e8c40 / 0x005e8de0: the a * b product of the original is dead (no side effect) and
//    omitted; the "previous node" local is always 0, its branch is kept anyway.
namespace {
namespace game_b92_04 {

JOAN_COUNTER(c_alloc20, "Reorder::alloc_20");
JOAN_COUNTER(c_alloc30, "Reorder::alloc_30");

// Pool allocation shared by 0x005e8c40 / 0x005e8de0.
// The fields a recycled node gets cleared (+0, +0x10 and, for strip nodes, +0x20).
void clear_node(jo::SpanNode* n) {
    jo::view<u32>(n->count) = 0;
    n->u32_10 = 0;
}
void clear_node(jo::StripNode* n) {
    jo::view<u32>(n->vertex_range) = 0;
    jo::view<u32>(n->index_count) = 0;
    n->primitive_count = 0;
}
template <class Node>
void* pool_alloc(img::Addr head, img::Addr count, img::Addr line_global, i32 line_add) {
    constexpr i32 size = static_cast<i32>(sizeof(Node));
    Node* prev = nullptr;
    Node* p;
    if (global<i32>(d::g_a17404) == 0) {
        p = nullptr;
    } else {
        p = global<Node*>(head);
        prev = nullptr;
    }
    if (p == nullptr) {
        p = global<Node*>(head);
        prev = nullptr;
    }
    if (p == nullptr) {
        void* const fresh = joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(size), str(d::s_d_ww_en3d_reorder_cpp), joan::orig::arg::i32(// 0x0058e7b0 "d:\ww\en3d\reorder.cpp"
                            static_cast<i32>(global<i16>(line_global)) + line_add));
        memset_(fresh, 0, static_cast<u32>(size));
        return fresh;
    }
    if (prev == nullptr)
        global<Node*>(head) = p->next;
    else
        prev->next = p->next;
    global<i32>(count) = global<i32>(count) - 1;
    clear_node(p);
    p->next = nullptr;
    return p;
}

// ---------------------------------------------------------------------------- 0x005e8c40
static void* JOAN_CDECL reorder_alloc20(i32, i32) {
    c_alloc20.hit();
    void* const p = pool_alloc<jo::SpanNode>(d::g_reorder_span_free_list, d::g_reorder_span_free_count, d::g_a17418, 0x22);
    return p;
}
// ---------------------------------------------------------------------------- 0x005e8de0
static void* JOAN_CDECL reorder_alloc30(i32, i32) {
    c_alloc30.hit();
    void* const p = pool_alloc<jo::StripNode>(d::g_reorder_strip_free_list, d::g_reorder_strip_free_count, d::g_a17420, 0x23);
    return p;
}

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("RenderPrimitive::alloc_vertex_group", 0x005e8c40, &reorder_alloc20);
B92("RenderPrimitive::alloc_indexed_group", 0x005e8de0, &reorder_alloc30);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers20)
namespace {
namespace en3d_helpers20 {

constexpr img::Addr kMemory(d::g_mem);
std::int32_t line_base(img::Addr address) { return static_cast<std::int32_t>(global<std::int16_t>(address)); }
void mem_free(void* p, const char* file, std::int32_t line) { joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

JOAN_COUNTER(c_node_free, "light node free");

// --- free functions (helpers20) -------------------------------------------------------------
// 0x005e8d60 / 0x005e8f10: return a light node to its pool (at most 40 kept) or free it.
void JOAN_CDECL light_node_free_a(std::uint8_t* node) {
    c_node_free.hit();
    if (global<std::int32_t>(d::g_a17404) == 0 || !(global<std::int32_t>(d::g_reorder_span_free_count) < 0x28)) {
        mem_free(node, &img::global<const char>(d::s_d_ww_en3d_reorder_cpp), line_base(d::g_a1741c) + 0xb);  // reorder.cpp
        return;
    }
    jo::as_span_node(node)->next = global<jo::SpanNode*>(d::g_reorder_span_free_list);
    global<std::uint8_t*>(d::g_reorder_span_free_list) = node;
    global<std::int32_t>(d::g_reorder_span_free_count) = global<std::int32_t>(d::g_reorder_span_free_count) + 1;
}
void JOAN_CDECL light_node_free_b(std::uint8_t* node) {
    c_node_free.hit();
    if (global<std::int32_t>(d::g_a17404) == 0 || !(global<std::int32_t>(d::g_reorder_strip_free_count) < 0x28)) {
        mem_free(node, &img::global<const char>(d::s_d_ww_en3d_reorder_cpp), line_base(d::g_a17424) + 0xf);
        return;
    }
    jo::as_strip_node(node)->next = global<jo::StripNode*>(d::g_reorder_strip_free_list);
    global<std::uint8_t*>(d::g_reorder_strip_free_list) = node;
    global<std::int32_t>(d::g_reorder_strip_free_count) = global<std::int32_t>(d::g_reorder_strip_free_count) + 1;
}

#define HELPER20(name, addr, fn) JOAN_REPLACE("en3d/helpers20", name, addr, fn)
HELPER20("RenderPrimitive::free_vertex_group", 0x005e8d60, &light_node_free_a);
HELPER20("RenderPrimitive::free_indexed_group", 0x005e8f10, &light_node_free_b);
#undef HELPER20

}  // namespace en3d_helpers20
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - Reorder::ctor 0x005e6000 (ret 0): three 0x5856a0 arrays, fields, memset(+0x9e, 0x14).
//
//  - SEH frames (0x5e6000, 0x5f0040, 0x6035c0, 0x605d80, 0x609620) only matter on exceptions;
//    normal paths reproduced.
namespace {
namespace game_b99_07_2 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_reorder_ctor, "Reorder::ctor");

struct H {
    // ------------------------------------------------------------------ 0x005e6000
    H* reorder_ctor() {
        c_reorder_ctor.hit();
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(jo::LightSlot)), 0x1e, 0, 0);      // 0x005856a0 (0xd6 on x86)
        joan::orig::dynarray_b(&jo::self<jo::Reorder>(this)->sectors)->construct(static_cast<std::int32_t>(sizeof(jo::LightSet)), 0x1e, 0, 0);  // 0x005856a0 (0x8d on x86)
        joan::orig::dynarray_b(&jo::self<jo::Reorder>(this)->materials)->construct(static_cast<std::int32_t>(sizeof(jo::MaterialSlot)), 0x1e, 0, 0);  // 0x005856a0 (0x5c on x86)
        jo::view<u32>(jo::self<jo::Reorder>(this)->group_count) = 0;
        jo::view<u32>(jo::self<jo::Reorder>(this)->group_capacity) = 0;
        jo::view<u32>(jo::self<jo::Reorder>(this)->current_group) = 0;
        jo::self<jo::Reorder>(this)->groups = nullptr;
        jo::self<jo::Reorder>(this)->current = nullptr;
        crt_memset(&jo::self<jo::Reorder>(this)->key, 0, static_cast<u32>(sizeof(jo::BatchKey)));
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("RenderOrder::RenderOrder", 0x005e6000, &H::reorder_ctor);
#undef B

}  // namespace game_b99_07_2
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
//
//      0x005e6110 Obj5e::~Obj5e()                 - 0x005e61c0 body, then 0x00585710 on +0x5c,
//                 +0x2e, +0 (members in reverse order)
namespace {
namespace game_b88_06 {

using joan::call::thiscall;

JOAN_COUNTER(c_dtor5e, "Obj5e::~Obj5e");

struct H {
    // 0x005e6110
    void dtor_5e6110() {
        c_dtor5e.hit();
        joan::orig::as_reorder(this)->clear();  // 0x005e61c0
        joan::orig::dynarray_b(&jo::self<jo::Reorder>(this)->materials)->destruct();  // 0x00585710 (+0x5c on x86)
        joan::orig::dynarray_b(&jo::self<jo::Reorder>(this)->sectors)->destruct();  // 0x00585710 (+0x2e on x86)
        joan::orig::dynarray_b(this)->destruct();   // 0x00585710
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("RenderOrder::~RenderOrder", 0x005e6110, &H::dtor_5e6110);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b104_10.cpp (game/b104_10)
namespace {
namespace game_b104_10 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    u32 call_5e8ab0() { return joan::orig::arg::as<u32>(joan::orig::as_reorder(this)->span_begin()); }  // 0x005e8ab0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("RenderOrder::init", 0x005e61a0, &H::call_5e8ab0);
#undef B

}  // namespace game_b104_10
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {


// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint8_t sector_lit() { return jo::self<jo::SceneS>(this)->lit_; }    // 0x005e8f90
    std::uint8_t scene_lit() { return jo::self<jo::Scene>(this)->lit_; }     // 0x005e8fc0
    H* scene_set_lit(std::uint32_t lit) {                           // 0x005e8fe0
        jo::self<jo::Scene>(this)->lit_ = static_cast<std::uint8_t>(lit);
        return this;
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Sector::lit", 0x005e8f90, &H::sector_lit);
HELPER("Scene::lit", 0x005e8fc0, &H::scene_lit);
HELPER("Scene::set_lit", 0x005e8fe0, &H::scene_set_lit);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Reorder::ctor() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_07_2::H::reorder_ctor>(this)); }  // 0x5e6000
auto joan::orig::Reorder::destruct() -> void { joan::bind::member<&::game_b88_06::H::dtor_5e6110>(this); }  // 0x5e6110
auto joan::orig::Reorder::begin_frame() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_10::H::call_5e8ab0>(this)); }  // 0x5e61a0
auto joan::orig::Reorder::clear() -> void { joan::bind::member<&::game_b89_04::H::reorder_clear>(this); }  // 0x5e61c0
auto joan::orig::Reorder::reset_batches() -> void { joan::bind::member<&::en3d_helpers6::H::mirror_reset>(this); }  // 0x5e6430
auto joan::orig::Reorder::purge() -> void { joan::bind::member<&::en3d_helpers7::H::purge>(this); }  // 0x5e64e0
auto joan::orig::Reorder::light_slot(std::int32_t a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers7::H::light_slot>(this, a0, a1)); }  // 0x5e76f0
auto joan::orig::Reorder::sector_slot(std::int32_t a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers7::H::sector_slot>(this, a0, a1)); }  // 0x5e7870
auto joan::orig::Reorder::slot_for(std::int32_t a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_06::H::slot_for>(this, a0, a1)); }  // 0x5e79f0
auto joan::orig::Reorder::state() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers6::H::light_state>(this)); }  // 0x5e7b80
auto joan::orig::Reorder::set_count(std::int32_t a0) -> void { joan::bind::member<&::game_b95_06::H::reorder_set_count>(this, a0); }  // 0x5e7bb0
auto joan::orig::Reorder::find_or_insert(std::int8_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_11::H::reorder_find>(this, a0)); }  // 0x5e7ce0
auto joan::orig::Mesh::push_span_fwd(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, Bone* a5, std::uint32_t a6) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_11::H::push_span_fwd>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x5e7e70
auto joan::orig::Reorder::forward(void* a0, std::uint32_t a1, std::uint16_t a2, std::uint16_t a3, void* a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, Bone* a8, std::uint32_t a9) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_06::H::reorder_forward>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x5e7ec0
auto joan::orig::Reorder::add_span_fwd(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, Bone* a8, std::uint32_t a9) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_07::H::add_span_fwd>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x5e7f70
auto joan::orig::LightSet::build(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::light_set_build>(this, a0)); }  // 0x5e8040
auto joan::orig::LightSet::same(void* a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::en3d_helpers16::H::light_set_same>(this, a0)); }  // 0x5e8100
auto joan::orig::ReorderBatch::construct(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_06::H::set_28>(this, a0)); }  // 0x5e8210
auto joan::orig::Plane::release_lights() -> void { joan::bind::member<&::en3d_helpers16::H::plane_release_lights>(this); }  // 0x5e8250
auto joan::orig::ReorderBatch::reset() -> void { joan::bind::member<&::en3d_helpers6::H::light_lists_reset>(this); }  // 0x5e8340
auto joan::orig::Reorder::push_span(void* a0, std::uint32_t a1, std::int32_t a2, std::int32_t a3, std::uint32_t a4, Bone* a5, std::uint32_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_11::H::push_span>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x5e8400
auto joan::orig::Reorder::add_batch(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, std::int32_t a5, std::int32_t a6, std::uint32_t a7, Bone* a8, std::uint32_t a9) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_06::H::add_batch>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x5e8540
auto joan::orig::ReorderBatch::alloc(std::uint32_t a0, std::uint32_t a1, std::int32_t a2, std::int32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_06::H::vb_alloc>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x5e8780
auto joan::orig::Reorder::add_span(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, Bone* a8, std::uint32_t a9) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_06::H::add_span>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x5e88c0
auto joan::orig::reorder_release() -> void { joan::bind::free<&::game_b90_02::reorder_release>(); }  // 0x5e8b20
auto joan::orig::reorder::alloc_20(std::int32_t a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b92_04::reorder_alloc20>(a0, a1)); }  // 0x5e8c40
auto joan::orig::light_node_free_a(void* a0) -> void { joan::bind::free<&::en3d_helpers20::light_node_free_a>(a0); }  // 0x5e8d60
auto joan::orig::reorder::alloc_30(std::int32_t a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b92_04::reorder_alloc30>(a0, a1)); }  // 0x5e8de0
auto joan::orig::light_node_free_b(void* a0) -> void { joan::bind::free<&::en3d_helpers20::light_node_free_b>(a0); }  // 0x5e8f10
auto joan::orig::SceneS::lit() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::en3d_helpers::H::sector_lit>(this)); }  // 0x5e8f90
auto joan::orig::Scene::lit() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::en3d_helpers::H::scene_lit>(this)); }  // 0x5e8fc0
auto joan::orig::Scene::set_lit(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::scene_set_lit>(this, a0)); }  // 0x5e8fe0
