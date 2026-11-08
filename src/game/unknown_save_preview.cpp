// game/unknown_save_preview.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_473140.cpp; re/attribution.csv: between ww/game.cpp and ww/gamefil2.cpp). They move to their
// original file when evidence names it (Phase 3).
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
#include "sound_slots.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::img::obj;

using i32 = std::int32_t;

using joan::call::cdecl_call;

using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

JOAN_COUNTER(c_slot473, "Obj::has_slot_473140");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x00473140
    u8 has_slot(const char* arg) {
        c_slot473.hit();
        const i32 key = joan::orig::arg::as<i16>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(arg));  // 0x004d12f0
        if (key == 0) return 0;
        for (i16 i = 1; i <= joan::sound_slots::count(); ++i) {
            jo::SoundSlotEntry* const e = joan::orig::as_sound_slot_entry(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(i)));  // 0x004cf690
            if (e->sound == nullptr) continue;
            if (joan::orig::arg::as<u32>(e->id()) != static_cast<u32>(key)) continue;  // 0x004d0440
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_sound3d(e->sound)->state())) == 4) continue;  // 0x0059fc90
            return 1;
        }
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Obj::has_slot_473140", 0x00473140, &H::has_slot);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

using joan::hook::global;

// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    using Fn = R(JOAN_STDCALL*)(void*, A...);
    return reinterpret_cast<Fn>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}

struct alignas(alignof(void*)) Image {  // 0x0060fad0 (memset 0x34 on x86; jo::Image holds pointers)
    std::uint8_t b[sizeof(jo::Image)];
};

void* const kFiles = obj(d::g_misc);
using joan::orig::helpers::device;

JOAN_COUNTER(c_preview, "GameFile::save_preview");

struct H {
    // ---- 0x004731e0 GameFile::save_preview(camera) -----------------------------------------------
    std::int32_t save_preview(void* cam) {
        c_preview.hit();
        if (joan::orig::as_misc(kFiles)->is_file_exist(str(d::s_no_save_preview_pict_sys)) != 0) return 1;  // 0x00590b80 "no_save_preview_pict.sys"
        void* surface = nullptr;
        com_call<std::int32_t>(global<void*>(d::g_a52743), 0x3c, 0u, static_cast<void*>(&surface));  // GetSurfaceLevel(0)
        const std::int32_t vx1 = jo::as_camera(cam)->view_x1, vy1 = jo::as_camera(cam)->view_y1;
        const std::int32_t vx2 = jo::as_camera(cam)->view_x2, vy2 = jo::as_camera(cam)->view_y2;
        const char b1 = jo::view<char>(jo::as_camera(cam)->clear_target), b2 = jo::view<char>(jo::as_camera(cam)->clear_depth);
        const std::uint32_t d = jo::view<std::uint32_t>(jo::as_camera(cam)->clear_colour);
        void* target = nullptr;
        void* depth = nullptr;
        com_call<std::int32_t>(device(), 0x80, static_cast<void*>(&target));  // GetRenderTarget
        com_call<std::int32_t>(device(), 0x84, static_cast<void*>(&depth));   // GetDepthStencilSurface
        com_call<std::int32_t>(device(), 0x7c, surface, depth);                // SetRenderTarget
        std::int32_t rect[4] = {0, 0, 0x140, 0xf0};
        std::int32_t point[2] = {0, 0};
        joan::orig::as_camera(cam)->set_cf(joan::orig::arg::u32(static_cast<char>(1)), joan::orig::arg::u32(static_cast<char>(1)), joan::orig::arg::u32(d));  // 0x00613d30
        joan::orig::as_camera(cam)->set_viewport(0u, 0u, joan::orig::arg::u32(rect[0]), joan::orig::arg::u32(rect[1]), joan::orig::arg::u32(rect[2] - 1), joan::orig::arg::u32(rect[3] - 1));  // 0x00613bf0
        const std::int32_t ok = joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(cam)->begin_frame());  // 0x00614630
        if (ok != 0) {
            joan::orig::as_camera(cam)->release_scene();  // 0x006146e0
            joan::orig::as_camera(cam)->render();  // 0x00614760
        }
        if (ok != 0) {
            Image img;
            joan::orig::as_image(&img)->construct();  // 0x0060fad0
            joan::orig::as_image(&img)->create(0x200u, 0x100u, 0u, joan::orig::arg::i32(global<std::uint32_t>(d::g_a5cc7c)));  // 0x0060fb60
            com_call<std::int32_t>(device(), 0x70, surface, static_cast<void*>(rect), 1u, jo::view<void*>(jo::as_image(&img)->surface),
                                   static_cast<void*>(point));  // CopyRects
            char path[0x108];  // ebp-0x194
            joan::orig::as_game_file(this)->preview_name(joan::orig::arg::str(static_cast<void*>(path)));  // 0x00473550
            if (joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(static_cast<const char*>(path))) != 0)  // 0x00590b80
                joan::orig::libc::remove(joan::orig::arg::str(static_cast<const char*>(path)));          // remove
            joan::orig::as_texture(&img)->save_bmp(joan::orig::arg::str(static_cast<void*>(path)));  // 0x00611e70
            joan::orig::as_image(&img)->release();                            // 0x0060fd20
            joan::orig::as_image(&img)->dtor();                            // 0x0060fb10 dtor
        }
        joan::orig::as_camera(cam)->set_cf(joan::orig::arg::u32(b1), joan::orig::arg::u32(b2), joan::orig::arg::u32(d));              // 0x00613d30
        joan::orig::as_camera(cam)->set_viewport(0u, 0u, joan::orig::arg::u32(vx1), joan::orig::arg::u32(vy1), joan::orig::arg::u32(vx2), joan::orig::arg::u32(vy2));  // 0x00613bf0
        com_call<std::int32_t>(device(), 0x7c, target, depth);  // SetRenderTarget
        com_call<std::int32_t>(target, 8);                      // Release
        com_call<std::int32_t>(depth, 8);
        com_call<std::int32_t>(surface, 8);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("GameFile::save_preview", 0x004731e0, &H::save_preview);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b85_01.cpp (game/b85_01)
namespace {
namespace game_b85_01 {

using joan::orig::helpers::strcpy_;
char* strcat_(char* d, const char* s) { return std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
char* strrchr_(const char* s, std::int32_t c) { return joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(s), joan::orig::arg::i32(c))); }

JOAN_COUNTER(c_clear_list, "List::clear_4734a0");
JOAN_COUNTER(c_preview_name, "GameFile::preview_name");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // ---- 0x004734a0 List::clear_4734a0 ----------------------------------------------------
    void clear_list() {
        c_clear_list.hit();
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i >= 1; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;              // 0x004af300
            void* p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));                              // 0x004af2d0
            if (p != nullptr) joan::orig::as_message(p)->destroy(1u);                  // 0x004ae160
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));                      // 0x005858c0
        }
        joan::orig::as_sound(this)->reset_585d20();                         // 0x00585d20
        joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(jo::self<jo::MessageList>(this)->message_texture));  // 0x0060a550 (a texture)
        jo::self<jo::MessageList>(this)->message_texture = nullptr;
    }

    // ---- 0x00473550 GameFile::preview_name(buf) ret 4 -----------------------------------------
    char* preview_name(char* buf) {
        c_preview_name.hit();
        strcpy_(buf, reinterpret_cast<const char*>(self() + 3));  // x64: needs GameFile+0x3 char[] (inside GameFile::flags +0x2; no pointer before it, so the offset holds on x64)
        char* dot = strrchr_(buf, 0x2e);
        if (dot != nullptr) *dot = 0;
        strcat_(buf, str(d::s_BMP));  // ".BMP"
        return buf;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_01", name, addr, fn)
B("List::clear_4734a0", 0x004734a0, &H::clear_list);
B("GameFile::preview_name", 0x00473550, &H::preview_name);
#undef B

}  // namespace game_b85_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Sound::is_playing(const char* a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_19::H::has_slot>(this, a0)); }  // 0x473140
auto joan::orig::GameFile::save_preview(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::save_preview>(this, a0)); }  // 0x4731e0
auto joan::orig::List::clear_4734a0() -> void { joan::bind::member<&::game_b85_01::H::clear_list>(this); }  // 0x4734a0
auto joan::orig::GameFile::preview_name(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b85_01::H::preview_name>(this, a0)); }  // 0x473550
