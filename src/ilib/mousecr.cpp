// ilib/mousecr.cpp: functions of the original d:\ww\ilib\mousecr.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../interfac/mouse_options.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../platform/win32_types.hpp"
#include <cmath>
#include <limits>
#include <cstdarg>
#include <vector>
#include "../original/engine.hpp"
#include "../original/gameset.hpp"
#include "../original/misc.hpp"
#include "../canvas_api.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../platform/input.hpp"
#include "../interfac/joystick_option.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

// The Input's per-element members as arrays (the layout has one member per element: x_0_, x_1_, ...).
inline jo::Pad* joy_states(const void* in) { return &jo::self<jo::Input>(in)->joy_state_0_; }
inline jo::Joystick* joysticks(const void* in) { return &jo::self<jo::Input>(in)->joystick_0_; }  inline constexpr std::uint32_t kJoyClear = static_cast<std::uint32_t>(sizeof(jo::Joystick) + offsetof(jo::Joystick, axis_offset) + 5 * sizeof(std::uint32_t));  // the original's 0xe4-byte joystick clear (4 pads' size): joystick 0 and joystick 1 up to axis_offset[5]
inline jo::MouseClick* clicks(const void* in) { return &jo::self<jo::Input>(in)->clicks[0]; }
inline jo::InputEvent* events(const void* in) { return &jo::self<jo::Input>(in)->event_queue_0_; }

}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
//
//      0x004915f0 MouseCursor::load       (thiscall, ret 0; SEH frame only destroys the
//                 local resource on unwind - normal path kept)
namespace {
namespace game_b94_02 {

using u16 = std::uint16_t;

void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_cursor_load, "MouseCursor::load");

using joan::orig::helpers::memset_;  // memset
using joan::orig::helpers::memcpy_;  // memcpy

struct H {

    // ------------------------------------------------------------------------ 0x004915f0
    void cursor_load() {
        c_cursor_load.hit();
        alignas(8) u8 res[((sizeof(jo::Database) > 0x148 ? sizeof(jo::Database) : 0x148) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger Database
        joan::orig::as_database(res)->construct(str(d::s_RESOURCE_CURSOR_RES), i32{1}, i32{0});  // 0x00580730 'RESOURCE\CURSOR.RES'
        jo::self<jo::MouseCursor>(this)->i16_d2 = joan::orig::arg::as<i16>(joan::orig::as_database(res)->rec_count());                     // 0x00415280
        jo::view<void*>(jo::self<jo::MouseCursor>(this)->shape_table) = joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(static_cast<u32>(static_cast<i32>(jo::self<jo::MouseCursor>(this)->i16_d2)) * static_cast<u32>(sizeof(jo::CursorRec))), str(d::s_d_ww_ilib_mousecr_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a15914)) + 8));  // 0x0058e7b0 mousecr.cpp
        memset_(jo::self<jo::MouseCursor>(this)->shape_table, 0, static_cast<u32>(static_cast<i32>(jo::self<jo::MouseCursor>(this)->i16_d2)) * static_cast<u32>(sizeof(jo::CursorRec)));
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::MouseCursor>(this)->i16_d2); ++i) {
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_database(res)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            jo::CursorRec* const e = jo::self<jo::MouseCursor>(this)->shape_table + i;
            u32 key = 0;
            memcpy_(&key, rec + 0xe, 4);
            e->bitmap = joan::orig::arg::as<jo::Bitmap*>(joan::orig::as_resource_db(&jo::self<jo::MouseCursor>(this)->res_cursor)->read_imported(joan::orig::arg::i32(key)));  // 0x0059b830
            jo::view<u16>(e->hot_x) = joan::orig::arg::as<u16>(joan::orig::as_misc(obj(d::g_misc))->atoi_field_590270(joan::orig::arg::str(rec + 8), joan::orig::arg::u32(i32{3})));    // 0x00590270
            jo::view<u16>(e->hot_y) = joan::orig::arg::as<u16>(joan::orig::as_misc(obj(d::g_misc))->atoi_field_590270(joan::orig::arg::str(rec + 0xb), joan::orig::arg::u32(i32{3})));  // 0x00590270
        }
        joan::orig::as_database(res)->destruct();  // 0x00580810
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("MouseCursor::load", 0x004915f0, &H::cursor_load);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
//
//    - MouseCursor::release_4941b0 0x004941b0 (ret 0, ilib/mousecr.cpp) -> this: when byte +0 != 0:
//      device ShowCursor(0) (D3D8 vtable +0x30), balance the Win32 ShowCursor count at +0x200 to 0,
//      delete +0x40 (0x00596760, flags 1) and +0x1fc (0x00596b30, flags 1), free +0xd4 and the five
//      buffers +0x7c..+0x8c of each 0x16-byte entry (count = signed byte +1) through 0x0058ed70,
namespace {
namespace game_b105_05 {

using i8 = std::int8_t;

// KERNEL32 / USER32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;
// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    using Fn = R(JOAN_STDCALL*)(void*, A...);
    return reinterpret_cast<Fn>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}

JOAN_COUNTER(c_release, "MouseCursor::release_4941b0");

constexpr img::Addr kShowCursor = d::iat_USER32_ShowCursor;  // USER32!ShowCursor IAT slot
void* const kMemory = obj(d::g_mem);              // allocator (0x0058ed70 free)
const char* const kMousecrCpp = str(d::s_d_ww_ilib_mousecr_cpp);    // "d:\\ww\\ilib\\mousecr.cpp"

void mem_free(void* p, i32 line) {
    joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(kMousecrCpp), joan::orig::arg::i32(line));  // 0x0058ed70
}
i32 line_base() { return static_cast<i32>(global<i16>(d::g_a15910)); }

struct H {

    // ------------------------------------------------------------------ 0x004941b0
    H* release_4941b0() {
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::MouseCursor>(this)->init_flag)) == 0) return this;
        c_release.hit();
        com_call<i32>(global<void*>(d::g_game_device), 0x30, 0);  // IDirect3DDevice8::ShowCursor(FALSE)
        while (jo::self<jo::MouseCursor>(this)->show_count != 0) {
            if (jo::self<jo::MouseCursor>(this)->show_count > 0) {
                win_call<i32>(kShowCursor, 0);
                jo::self<jo::MouseCursor>(this)->show_count = jo::self<jo::MouseCursor>(this)->show_count - 1;
            } else {
                win_call<i32>(kShowCursor, 1);
                jo::self<jo::MouseCursor>(this)->show_count = jo::self<jo::MouseCursor>(this)->show_count + 1;
            }
        }
        void* const a = jo::view<void*>(jo::self<jo::MouseCursor>(this)->cursor_image);
        if (a != nullptr) joan::orig::as_image(a)->destroy(joan::orig::arg::u32(u32{1}));  // 0x00596760 (deleting dtor)
        jo::self<jo::MouseCursor>(this)->cursor_image = nullptr;
        mem_free(jo::view<void*>(jo::self<jo::MouseCursor>(this)->shape_table), line_base() + 0x22);
        if (jo::self<jo::MouseCursor>(this)->vga_buf != nullptr) {
            void* const b = jo::view<void*>(jo::self<jo::MouseCursor>(this)->vga_buf);
            if (b != nullptr) joan::orig::as_vga_buf(b)->destroy(joan::orig::arg::u32(u32{1}));  // 0x00596b30 (deleting dtor)
            jo::self<jo::MouseCursor>(this)->vga_buf = nullptr;
        }
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::MouseCursor>(this)->shape_count); ++i) {
            jo::CursorShape& e = jo::self<jo::MouseCursor>(this)->shape[i];
            static const i32 kLine[5] = {0x2d, 0x30, 0x33, 0x36, 0x39};
            for (i32 k = 0; k < 5; ++k) {
                if (e.buf[k] != nullptr) mem_free(e.buf[k], line_base() + kLine[k]);
                e.buf[k] = nullptr;
            }
        }
        joan::orig::as_resource_db(&jo::self<jo::MouseCursor>(this)->res_cursor)->deinit();  // 0x0059b440
        jo::view<u8>(jo::self<jo::MouseCursor>(this)->init_flag) = 0;
        jo::self<jo::MouseCursor>(this)->icon_bitmap = nullptr;
        jo::self<jo::MouseCursor>(this)->cur_shape = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("MouseCursor::release_4941b0", 0x004941b0, &H::release_4941b0);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::str;

using joan::call::stdcall_call;
template <class R = void, class... A>
R winapi_call(img::Addr slot, A... args) {  // user32 through its IAT slot
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}

JOAN_COUNTER(c_cursor, "MouseCursor::init_493eb0");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x00493eb0
    u32 cursor_init(u32 n) {
        c_cursor.hit();
        struct Frame {
            u8 pal[0x300];
            u8 file[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger File
            u8 path[0x194];
        } f;
        std::memset(&f, 0, sizeof(f));
        jo::view<u8>(jo::self<jo::MouseCursor>(this)->shape_count) = static_cast<u8>(n);
        joan::orig::as_string(f.path)->construct();           // 0x005a1170
        joan::orig::as_string(f.path)->assign(str(d::s_RESOURCE));           // 0x005a1440 "RESOURCE\\"
        joan::orig::as_string(f.path)->append(str(d::s_I_CURSOR_RES));               // 0x005a1550 "I_CURSOR.RES"
        const char* const name = joan::orig::as_string(f.path)->c_str();  // 0x00417d90
        joan::orig::as_resource_db(&jo::self<jo::MouseCursor>(this)->res_cursor)->init_imported(joan::orig::arg::str(name), static_cast<char>(i32{1}), i32{0});  // 0x0059b6d0
        joan::orig::as_file(f.file)->construct();                                 // 0x004179c0
        joan::orig::as_file(f.file)->file_open(str(d::s_RESOURCE_PAL_STD_RES), i32{1}, static_cast<char>(i32{0}));  // 0x00586da0
        joan::orig::vector(joan::orig::arg::mptr(static_cast<void*>(f.pal)), joan::orig::arg::i32(u32{3}), joan::orig::arg::i32(u32{0x100}), fn::code_ptr(fn::Obj_ctor_596bd0));  // 0x00449af0
        joan::orig::as_file(f.file)->file_seek(i32{8}, i32{-1});            // 0x00587ea0
        joan::orig::as_file(f.file)->file_read(joan::orig::arg::mptr(f.pal), u32{0x300});  // 0x005873a0
        void* m = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::VgaBuf)));  // 0x10 on x86
        void* o = m != nullptr ? joan::orig::as_vga_buf(m)->construct(joan::orig::arg::mptr(f.pal), joan::orig::arg::mptr(i32{0}), joan::orig::arg::mptr(i32{0}), joan::orig::arg::mptr(i32{0})) : nullptr;  // 0x00612600
        jo::view<void*>(jo::self<jo::MouseCursor>(this)->vga_buf) = o;
        joan::orig::as_vga_buf(jo::self<jo::MouseCursor>(this)->vga_buf)->expand_palette(i32{1});  // 0x00612dd0
        joan::orig::as_file(f.file)->file_close();                        // 0x00587260
        joan::orig::as_mouse_cursor(this)->load();                         // 0x004915f0
        m = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Image)));  // 0x34 on x86
        o = m != nullptr ? joan::orig::as_image(m)->construct() : nullptr;  // 0x0060fad0
        jo::view<void*>(jo::self<jo::MouseCursor>(this)->cursor_image) = o;
        joan::orig::as_image(jo::self<jo::MouseCursor>(this)->cursor_image)->create(joan::orig::arg::u32(i32{0x20}), joan::orig::arg::u32(i32{0x20}), joan::orig::arg::u32(i32{0}), i32{0xb});  // 0x0060fb60
        joan::orig::as_bit_writer(jo::self<jo::MouseCursor>(this)->cursor_image)->push_bit();                                          // 0x00610040
        joan::orig::as_image(jo::self<jo::MouseCursor>(this)->cursor_image)->fill_rect(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0x1f}), joan::orig::arg::u32(i32{0x1f}), joan::orig::arg::u32(i32{0}));  // 0x00612f30
        joan::orig::as_bit_writer(jo::self<jo::MouseCursor>(this)->cursor_image)->drop();                                          // 0x006100a0
        std::memset(joan::orig::arg::mptr(jo::self<jo::MouseCursor>(this)->shape), i32{0}, joan::orig::arg::u32(n * static_cast<u32>(sizeof(jo::CursorShape))));  // memset
        jo::self<jo::MouseCursor>(this)->show_count = 0;
        constexpr img::Addr kShowCursor = d::iat_USER32_ShowCursor;
        if ((global<u32>(d::g_a52488) & 1) != 0) {
            const i32 r = winapi_call<i32>(kShowCursor, i32{1});
            jo::self<jo::MouseCursor>(this)->show_count += 1;
            if (r < 0) {
                winapi_call<i32>(kShowCursor, i32{1});
                jo::self<jo::MouseCursor>(this)->show_count += 1;
            }
        } else {
            const i32 r = winapi_call<i32>(kShowCursor, i32{0});
            jo::self<jo::MouseCursor>(this)->show_count -= 1;
            if (r >= 0) {
                winapi_call<i32>(kShowCursor, i32{0});
                jo::self<jo::MouseCursor>(this)->show_count -= 1;
            }
        }
        jo::view<u8>(jo::self<jo::MouseCursor>(this)->init_flag) = 1;
        joan::orig::as_file(f.file)->destruct_5872b0();  // 0x005872b0
        return 0;  // ~File returns nothing; callers (0x0060392a, sysen.cpp) discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("MouseCursor::init_493eb0", 0x00493eb0, &H::cursor_init);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
namespace {
namespace game_b98_05 {

using joan::call::thiscall;

JOAN_COUNTER(c_try_slots, "Obj::try_slots");

struct H {

    // 0x005922d0
    void* ctor_5922d0() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::Input));  // 0xd69 on x86
        jo::self<jo::Input>(this)->u32_652 = 8;
        jo::self<jo::Input>(this)->u32_656 = 0x10;
        jo::self<jo::Input>(this)->key_repeat_delay = 0x1f4;
        jo::self<jo::Input>(this)->key_repeat_rate = 0x64;
        return this;
    }

    // 0x00594490
    std::int32_t try_slots(std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4,
                           std::uint32_t a5, std::uint32_t a6, std::int32_t which) {
        c_try_slots.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->has_mouse_event) == 0) return 0;
        if (which == 0 || which == 2) {
            jo::MouseClick* const slot = &jo::self<jo::Input>(this)->clicks[0];
            if (jo::self<jo::Input>(this)->mouse_event_type == 0 && slot->count == 2) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_triangle(this)->contains(joan::orig::arg::i32(a1), joan::orig::arg::i32(a2), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(jo::view<std::uint32_t>(slot->x)), joan::orig::arg::i32(jo::view<std::uint32_t>(slot->y)))) != 0) {  // 0x00596140
                    return 1;
                }
            }
        }
        if (which == 1 || which == 2) {
            jo::MouseClick* const slot = &jo::self<jo::Input>(this)->clicks[1];
            if (jo::self<jo::Input>(this)->mouse_event_type == 1 && slot->count == 2) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_triangle(this)->contains(joan::orig::arg::i32(a1), joan::orig::arg::i32(a2), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(jo::view<std::uint32_t>(slot->x)), joan::orig::arg::i32(jo::view<std::uint32_t>(slot->y)))) != 0) {  // 0x00596140
                    return 1;
                }
            }
        }
        return 0;
    }

    // 0x00598a90
    void call_598ac0() { joan::orig::as_input(this)->reset_keys(); }  // 0x00598ac0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("Obj::ctor_5922d0", 0x005922d0, &H::ctor_5922d0);
B("Obj::try_slots", 0x00594490, &H::try_slots);
B("Obj::call_598ac0", 0x00598a90, &H::call_598ac0);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
namespace {
namespace game_b96_05 {

using joan::call::thiscall;

JOAN_COUNTER(c_region_hit, "Region::hit");

struct H {

    // 0x00592350
    std::uint32_t call_592830() {
        joan::orig::as_input(this)->release();  // 0x00592830
        return 0;  // Input::release returns nothing; only called from the atexit stub 0x005647d0 (result unused)
    }

    // 0x00594ab0
    std::int32_t region_hit(std::int32_t a, std::int32_t b, std::int32_t c, std::int32_t d, std::int32_t e,
                            std::int32_t g, std::int32_t mode) {
        c_region_hit.hit();
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Input>(this)->has_mouse_event)) == 0) return 0;
        if (mode == 0 || mode == 2) {
            jo::MouseClick* const p = &jo::self<jo::Input>(this)->clicks[0];
            if (jo::self<jo::Input>(this)->mouse_event_type == 4 &&
                joan::orig::arg::as<std::int32_t>(joan::orig::as_triangle(this)->contains(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(g), joan::orig::arg::i32(p->release_x), joan::orig::arg::i32(p->release_y))) != 0)  // 0x00596140
                return 1;
        }
        if (mode == 1 || mode == 2) {
            jo::MouseClick* const p = &jo::self<jo::Input>(this)->clicks[1];
            if (jo::self<jo::Input>(this)->mouse_event_type == 5 &&
                joan::orig::arg::as<std::int32_t>(joan::orig::as_triangle(this)->contains(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(g), joan::orig::arg::i32(p->release_x), joan::orig::arg::i32(p->release_y))) != 0)  // 0x00596140
                return 2;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("Obj::call_592830", 0x00592350, &H::call_592830);
B("Region::hit", 0x00594ab0, &H::region_hit);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

using joan::hook::global;

using joan::call::cdecl_call;

using i16 = std::int16_t;

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;
using joan::img::str;

using u16 = std::uint16_t;

using joan::call::stdcall_call;

using u32 = std::uint32_t;

using joan::call::thiscall;
// stdcall through an IAT slot.
template <class R = void, class... A>
R import_call(img::Addr slot, A... args) {
    return (*reinterpret_cast<R(JOAN_STDCALL* const*)(A...)>(img::addr(slot)))(args...);
}
// COM method (stdcall, interface pointer first) through the vtable byte offset.
template <class R = i32, class... A>
R com_call(void* iface, std::int32_t offset, A... args) {
    using Fn = R(JOAN_STDCALL*)(void*, A...);
    Fn fn;
    std::memcpy(&fn, &joan::call::vtable_of(iface)[joan::call::x86_vslot(offset)], sizeof fn);
    return fn(iface, args...);
}


JOAN_COUNTER(c_input, "Input::init_592380");
JOAN_COUNTER(c_cursor, "Cursor::set_596db0");

struct H {
    // -------------------------------------------------------------- 0x00592380
    // The original created DirectInput's mouse (exclusive, buffer 200 items) and keyboard (non-exclusive, 16) and
    // enumerated its joysticks; here the platform layer's devices (platform/input.hpp), same buffer sizes, and its
    // pads (poll_joysticks below). No DirectInput object or device interfaces (the `di` argument is not used).
    void* input_init(void* /*instance*/, void* /*window*/, void* /*di*/) {
        c_input.hit();
        struct {
            i32 x, y;
        } pt{0, 0};
        import_call<i32>(d::iat_USER32_GetCursorPos, &pt);  // GetCursorPos
        jo::self<jo::Input>(this)->cur_x = pt.x;
        jo::self<jo::Input>(this)->cur_y = pt.y;
        jo::self<jo::Input>(this)->cur_z = (global<i32>(d::g_a48878) + global<i32>(d::g_a4887c)) / 2;
        jo::view<void*>(jo::self<jo::Input>(this)->direct_input) = nullptr;
        jo::view<void*>(jo::self<jo::Input>(this)->di_mouse) = nullptr;
        jo::view<void*>(jo::self<jo::Input>(this)->di_keyboard) = nullptr;
        joan::platform::input::open(0xc8, 0x10);
        joan::platform::input::acquire(joan::platform::input::Device::Mouse);
        joan::platform::input::acquire(joan::platform::input::Device::Keyboard);
        joan::orig::as_input(this)->reset_keys_thunk();  // 0x00598a90
        std::memset(static_cast<void*>(joysticks(this)), i32{0}, joan::orig::arg::u32(kJoyClear));  // memset (0xe4 bytes on x86, the size of the 4 pads)
        std::memset(static_cast<void*>(joy_states(this)), i32{0}, joan::orig::arg::u32(static_cast<u32>(4 * sizeof(jo::Pad))));  // memset (0xe4 on x86)
        for (i32 i = 0; i < 4; ++i) {
            joysticks(this)[i].index = i;
            joy_states(this)[i].joystick = &joysticks(this)[i];
        }
        joan::orig::as_input(this)->reset_boundary();  // 0x00593f60
        jo::view<u32>(jo::self<jo::Input>(this)->saved_x) = 0;
        jo::view<u32>(jo::self<jo::Input>(this)->saved_y) = 0;
        jo::view<u32>(jo::self<jo::Input>(this)->queue_tail) = 0;
        jo::view<u32>(jo::self<jo::Input>(this)->queue_head) = 0;
        // Platform-layer pads: into their joystick slots now, as the original's EnumDevices did for DirectInput
        // joysticks (the options menu, built right after, offers the joystick control type only when slot 0 holds one).
        joan::orig::as_input(this)->poll_joysticks(i32{-1});  // 0x00598730
        jo::self<jo::Input>(this)->init_time = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050());  // 0x00591050
        jo::view<u8>(jo::self<jo::Input>(this)->inactive) = 0;
        jo::view<u8>(jo::self<jo::Input>(this)->init_flag) = 1;
        return this;
    }

    // -------------------------------------------------------------- 0x00596db0
    std::uintptr_t cursor_set(i32 shape) {  // returns a pointer-sized eax leftover (the facade returns std::uintptr_t)
        c_cursor.hit();
        if (jo::view<i8>(jo::self<jo::MouseCursor>(this)->init_flag) == 0) return reinterpret_cast<std::uintptr_t>(this);
        jo::Canvas* const ctx = static_cast<jo::Canvas*>(obj(d::g_vga_back));
        joan::orig::as_canvas(this)->callback_597350(ctx, joan::orig::arg::u32(i32{0}));  // 0x00597350
        joan::orig::as_canvas(this)->callback_597250(ctx, joan::orig::arg::u32(i32{0}));  // 0x00597250
        const i32 sx = jo::self<jo::MouseCursor>(this)->x1 + jo::self<jo::MouseCursor>(this)->hot_spot_x;
        const i32 sy = jo::self<jo::MouseCursor>(this)->y1 + jo::self<jo::MouseCursor>(this)->hot_spot_y;
        if (shape == -1) {
            jo::self<jo::MouseCursor>(this)->icon_bitmap = jo::self<jo::MouseCursor>(this)->default_bitmap;
            jo::view<u32>(jo::self<jo::MouseCursor>(this)->hot_spot_x) = jo::view<u32>(jo::self<jo::MouseCursor>(this)->default_hot_x);
            jo::view<u32>(jo::self<jo::MouseCursor>(this)->hot_spot_y) = jo::view<u32>(jo::self<jo::MouseCursor>(this)->default_hot_y);
        } else {
            const jo::CursorRec* const e = jo::self<jo::MouseCursor>(this)->shape_table + (shape - 1);
            jo::self<jo::MouseCursor>(this)->icon_bitmap = e->bitmap;
            jo::self<jo::MouseCursor>(this)->hot_spot_x = e->hot_x;
            jo::self<jo::MouseCursor>(this)->hot_spot_y = e->hot_y;
        }
        jo::self<jo::MouseCursor>(this)->width = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_bitmap(jo::self<jo::MouseCursor>(this)->icon_bitmap)->width()));  // 0x005979a0
        jo::self<jo::MouseCursor>(this)->height = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_bitmap(jo::self<jo::MouseCursor>(this)->icon_bitmap)->height()));  // 0x005979c0
        jo::view<u16>(jo::self<jo::MouseCursor>(this)->cur_shape) = static_cast<u16>(shape);
        jo::self<jo::MouseCursor>(this)->x1 = sx - jo::self<jo::MouseCursor>(this)->hot_spot_x;
        jo::self<jo::MouseCursor>(this)->y1 = sy - jo::self<jo::MouseCursor>(this)->hot_spot_y;
        jo::self<jo::MouseCursor>(this)->x2 = jo::self<jo::MouseCursor>(this)->x1 + jo::self<jo::MouseCursor>(this)->width - 1;
        jo::self<jo::MouseCursor>(this)->y2 = jo::self<jo::MouseCursor>(this)->y1 + jo::self<jo::MouseCursor>(this)->height - 1;
        joan::orig::as_bit_writer(jo::self<jo::MouseCursor>(this)->cursor_image)->push_bit();  // 0x00610040
        joan::orig::as_image(jo::self<jo::MouseCursor>(this)->cursor_image)->fill_rect(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0x1f}), joan::orig::arg::u32(i32{0x1f}), joan::orig::arg::u32(i32{0}));  // 0x00612f30
        // The VgaBuf (rgb, map8, pal16, pal32) by value, as the original pushed its four dwords; pointer-sized on x64
        // (map8 is the unused int argument).
        jo::VgaBuf r;
        std::memcpy(&r, jo::self<jo::MouseCursor>(this)->vga_buf, sizeof r);
        joan::orig::as_image(jo::self<jo::MouseCursor>(this)->cursor_image)->blit_sprite(i32{0}, i32{0}, joan::orig::arg::mptr(jo::self<jo::MouseCursor>(this)->icon_bitmap), joan::orig::arg::mptr(r.rgb), static_cast<i32>(reinterpret_cast<std::uintptr_t>(r.map8)), joan::orig::arg::mptr(r.pal16), joan::orig::arg::mptr(r.pal32));  // 0x00610e10
        joan::orig::as_bit_writer(jo::self<jo::MouseCursor>(this)->cursor_image)->drop();  // 0x006100a0
        void* const surface = jo::self<jo::MouseCursor>(this)->cursor_image->surface;
        com_call<i32>(global<void*>(d::g_game_device), 0x28, jo::view<u32>(jo::self<jo::MouseCursor>(this)->hot_spot_x), jo::view<u32>(jo::self<jo::MouseCursor>(this)->hot_spot_y), surface);  // SetCursorProperties
        bool redraw = false;
        if (jo::view<i8>(jo::self<jo::MouseCursor>(this)->hide_flag) == 0) {
            if (jo::view<i8>(jo::self<jo::MouseCursor>(this)->frozen) == 0) {
                redraw = true;
            } else if (joan::orig::arg::as<i32>(joan::orig::as_rect(this)->overlaps(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::MouseCursor>(this)->old_x1)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::MouseCursor>(this)->old_y1)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::MouseCursor>(this)->old_x2)), joan::orig::arg::i32(// 0x0045c7f0
                                     jo::view<u32>(jo::self<jo::MouseCursor>(this)->old_y2)))) == 0) {
                redraw = true;
            }
        }
        std::uintptr_t eax = static_cast<u32>(static_cast<i32>(jo::view<i8>(jo::self<jo::MouseCursor>(this)->clip_flag)));
        if (jo::view<i8>(jo::self<jo::MouseCursor>(this)->clip_flag) != 0) eax = thiscall<u32>(fn::Canvas_callback_5972e0, this, ctx, i32{0});  // 0x005972e0
        if (redraw) eax = thiscall<u32>(fn::Canvas_callback_597200, this, ctx, i32{0});                     // 0x00597200
        if (jo::view<i8>(jo::self<jo::MouseCursor>(this)->clip_flag) != 0) eax = thiscall<u32>(fn::Canvas_callback_597390, this, ctx);             // 0x00597390
        if (redraw) eax = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_mouse_cursor(this)->update_cursor(ctx));                               // 0x0045b4c0
        return eax;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("Mouse::init", 0x00592380, &H::input_init);
B("Cursor::set_596db0", 0x00596db0, &H::cursor_set);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {

using i32 = std::int32_t;
using u8 = std::uint8_t;


using u32 = std::uint32_t;

JOAN_COUNTER(c_input_release, "Input::release");
JOAN_COUNTER(c_double_click, "Input::double_click");

struct H {

    // ------------------------------------------------------------------------ 0x00592830
    // The original unacquired / released the DirectInput joysticks, keyboard, mouse and DirectInput itself; a
    // platform-layer pad's slot holds no interface (1), the device pointers are null (Input::init).
    void input_release() {
        c_input_release.hit();
        if (static_cast<i32>(jo::view<std::int8_t>(jo::self<jo::Input>(this)->init_flag)) != 0) jo::view<u8>(jo::self<jo::Input>(this)->init_flag) = 0;
        for (i32 i = 0; i < 4; ++i) {
            if (joysticks(this)[i].device != nullptr) joysticks(this)[i].device = nullptr;
        }
        joan::platform::input::close();
    }

    // ------------------------------------------------------------------------ 0x00594d30
    i32 double_click(i32 n) {
        c_double_click.hit();
        if (static_cast<i32>(jo::view<std::int8_t>(jo::self<jo::Input>(this)->has_mouse_event)) == 0) return 0;
        if (n == 0 || n == 2) {
            jo::MouseClick* const p = &jo::self<jo::Input>(this)->clicks[0];
            if (jo::self<jo::Input>(this)->mouse_event_type == 4 && jo::as_mouse_click(p)->count == 1 &&
                jo::as_mouse_click(p)->release_time - jo::as_mouse_click(p)->time <= 0xc8u)
                return 1;
        }
        if (n == 1 || n == 2) {
            jo::MouseClick* const p = &jo::self<jo::Input>(this)->clicks[1];
            if (jo::self<jo::Input>(this)->mouse_event_type == 5 && jo::as_mouse_click(p)->count == 1 &&
                jo::as_mouse_click(p)->release_time - jo::as_mouse_click(p)->time <= 0xc8u)
                return 2;
        }
        return 0;
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Input::release", 0x00592830, &H::input_release);
B92("Input::double_click", 0x00594d30, &H::double_click);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
namespace {
namespace game_b100_10 {

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using u32 = std::uint32_t;

JOAN_COUNTER(c_unacquire, "Input::unacquire_all");

struct H {
    void* ctor_596310() {  // 0x00596310
        joan::orig::as_file(this)->construct();  // 0x004179c0
        jo::view<u8>(jo::self<jo::ResourceDb>(this)->init_flag) = 0;
        return this;
    }

    // ------------------------------------------------------------------------ 0x005929f0
    // eax: movsx [this+2] if already set, `this` if [this+0]==0, else the keyboard's unacquire result.
    std::uintptr_t unacquire_all() {
        c_unacquire.hit();
        if (jo::view<i8>(jo::self<jo::Input>(this)->inactive) != 0) return static_cast<std::uint32_t>(static_cast<i32>(jo::view<i8>(jo::self<jo::Input>(this)->inactive)));
        jo::view<u8>(jo::self<jo::Input>(this)->inactive) = 1;
        if (jo::view<i8>(jo::self<jo::Input>(this)->init_flag) == 0) return reinterpret_cast<std::uintptr_t>(this);
        // pads: nothing to release (read only while focused)
        joan::platform::input::unacquire(joan::platform::input::Device::Mouse);
        return static_cast<u32>(joan::platform::input::unacquire(joan::platform::input::Device::Keyboard));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Input::unacquire_all", 0x005929f0, &H::unacquire_all);
B("Obj::ctor_596310", 0x00596310, &H::ctor_596310);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
namespace {
namespace game_b97_05 {

JOAN_COUNTER(c_pause_all, "Sound::pause_all");

struct H {
    // 0x00592ae0
    void pause_all() {
        c_pause_all.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->inactive) == 0) return;
        jo::view<std::uint8_t>(jo::self<jo::Input>(this)->inactive) = 0;
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->init_flag) == 0) return;
        // pads: read while focused, no acquire
        joan::platform::input::acquire(joan::platform::input::Device::Mouse);
        joan::platform::input::acquire(joan::platform::input::Device::Keyboard);
        jo::view<std::uint8_t>(jo::self<jo::Input>(this)->skey_changed) = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("Sound::pause_all", 0x00592ae0, &H::pause_all);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- from modules/en3d_b83_08.cpp (game/b83_08)
namespace {
namespace en3d_b83_08 {

using joan::hook::global;

using joan::img::obj;

using joan::call::thiscall;

struct H {

    // 0x00596140 (this unused)
    std::int32_t triangle_contains(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1, std::int32_t x2,
                                   std::int32_t y2, std::int32_t px, std::int32_t py) {
        auto u = [](std::int32_t v) { return static_cast<std::uint32_t>(v); };
        std::int32_t c = static_cast<std::int32_t>(0u - ((u(y0) - u(py)) * (u(x1) - u(x0)) - (u(x0) - u(px)) * (u(y1) - u(y0))));
        if (c < 0) return 0;
        c = static_cast<std::int32_t>(0u - ((u(y1) - u(py)) * (u(x2) - u(x1)) - (u(x1) - u(px)) * (u(y2) - u(y1))));
        if (c < 0) return 0;
        c = static_cast<std::int32_t>(0u - ((u(y2) - u(py)) * (u(x0) - u(x2)) - (u(x2) - u(px)) * (u(y0) - u(y2))));
        if (c < 0) return 0;
        return 1;
    }

    // 0x00592bd0 (eax = 0x004918d0's)
    std::uint32_t cursor_set() {
        global<std::uint8_t>(d::g_a4860a) = 1;
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_mouse_cursor(obj(d::g_mouse_cursor))->move(joan::orig::arg::i32(jo::self<jo::Input>(this)->cur_x + jo::self<jo::Input>(this)->saved_x), joan::orig::arg::i32(jo::self<jo::Input>(this)->cur_y + jo::self<jo::Input>(this)->saved_y)));  // 0x004918d0
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b83_08", name, addr, fn)
GAME("Triangle::contains", 0x00596140, &H::triangle_contains);
GAME("Cursor::set_pos", 0x00592bd0, &H::cursor_set);
#undef GAME

}  // namespace en3d_b83_08
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (game/b82_17)
namespace {
namespace en3d_b82_17 {

using joan::hook::global;

using joan::img::obj;

using joan::call::thiscall;

struct H {

    // 0x00592c30
    std::uint32_t cursor_reset() {
        global<std::uint8_t>(d::g_a4860a) = 0;
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_mouse_cursor(obj(d::g_mouse_cursor))->move(joan::orig::arg::i32(jo::self<jo::Input>(this)->cur_x + jo::self<jo::Input>(this)->saved_x), joan::orig::arg::i32(jo::self<jo::Input>(this)->cur_y + jo::self<jo::Input>(this)->saved_y)));  // 0x004918d0
    }

    // 0x00593980
    std::int32_t inside(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1) {
        return joan::orig::as_point(this)->inside(x0, y0, x1, y1);
    }
};

#define B82_17G(name, addr, fn) JOAN_REPLACE("game/b82_17", name, addr, fn)
B82_17G("Cursor::reset_pos", 0x00592c30, &H::cursor_reset);
B82_17G("Point::inside", 0x00593980, &H::inside);
#undef B82_17G

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/game_b105_15.cpp (game/b105_15)
namespace {
namespace game_b105_15 {

using joan::img::obj;

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // 0x00592c90
    // 0x005973d0 is an empty stub: the original enters it with eax = a (`mov eax,[ebp+8]; push eax`)
    // and returns that eax. Return a explicitly so the result does not depend on whether the
    // b104_09 replacement of 0x005973d0 is installed (no caller reads it anyway).
    u32 forward_5973d0(u32 a, u32 b, u32 c, u32 d) {
        joan::orig::as_input(obj(d::g_mouse_cursor))->lock_area_nop(joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d));  // 0x005973d0
        return a;
    }
    // 0x005976b0
    void empty_ret1c(u32, u32, u32, u32, u32, u32, u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("Obj::forward_5973d0", 0x00592c90, &H::forward_5973d0);
B("Obj::empty_5976b0", 0x005976b0, &H::empty_ret1c);
#undef B

}  // namespace game_b105_15
}  // namespace

// ---------------------------------------- from modules/game_b111_17.cpp (game/b111_17)
namespace {
namespace game_b111_17 {

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::call::thiscall;
template <class R, class... A>
R import_stdcall(img::Addr slot, A... args) {
    return (*reinterpret_cast<R(JOAN_STDCALL* const*)(A...)>(img::addr(slot)))(args...);
}
using joan::orig::helpers::at;

JOAN_COUNTER(c_joy_poll, "Input::poll_joysticks");

// A game pad of the platform layer (platform/input.hpp) in joystick slot `cfg` (Input +0x31e +
// i * 0xc6), set up as the original's Joystick::attach 0x00597bd0 + EnumObjects callback 0x00598160 (deleted with
// DirectInput in cleanup C1) did for a DirectInput joystick with the objects X, Y, Z, Rz (0..65535), one POV hat and 12 buttons, read from a DIJOYSTATE2 laid out as
// below. cfg+0 (the device interface) holds 1: "a joystick is there" for the game (options menu, key remapping);
// nothing dereferences it on this path.
namespace pad {
constexpr u32 kAxisOffsets[4] = {0x0, 0x4, 0x8, 0x14};  // DIJOYSTATE2 lX, lY, lZ, lRz
constexpr u8 kAxisSlots[4] = {0, 1, 3, 2};  // Joystick::axis_map index (+0xb2) of X, Y, Z, Rz as enum_object 0x00598160 assigned them
constexpr u32 kPovOffset = 0x20;                       // rgdwPOV[0]
constexpr u32 kButtonOffset = 0x30;                    // rgbButtons
constexpr u8 kButtonSlots[12] = {0, 1, 2, 3, 4, 5, 8, 9, 7, 6, 10, 11};  // Joystick::button_map index (+0xb8)

void attach(u8* cfg) {
    jo::Joystick* const j = jo::as_joystick(cfg);
    j->device = reinterpret_cast<jo::IDirectInputDevice8*>(static_cast<std::uintptr_t>(1));  // x64: integer passed as pointer (native pad: 1 = present)
    j->u8_8 = 0;
    j->u8_9 = 0;
    std::memset(j->axis_map, 0, 6);
    std::memset(j->button_map, 0, 0xe);
    for (i32 a = 0; a < 4; ++a) {
        const i32 n = static_cast<i8>(j->u8_8);
        j->u8_8 = static_cast<u8>(n + 1);
        j->axis_offset[n] = kAxisOffsets[a];
        j->axis_min[n] = 0;
        j->axis_max[n] = 0xffff;
        j->axis_map[kAxisSlots[a]] = static_cast<i8>(n + 1);
    }
    {
        const i32 n = static_cast<i8>(j->u8_8);
        j->u8_8 = static_cast<u8>(n + 1);
        j->axis_offset[n] = kPovOffset;
        j->axis_min[n] = 0x7fffffff;
        j->axis_max[n] = 0x8ca0;
        j->axis_map[4] = static_cast<i8>(n + 1);
        j->axis_map[5] = static_cast<i8>(n + 1);
    }
    for (i32 m = 0; m < 12; ++m) {
        j->u8_9 = static_cast<u8>(m + 1);
        j->button_offset[m] = kButtonOffset + static_cast<u32>(m);
        j->button_map[kButtonSlots[m]] = static_cast<i8>(-m - 1);
    }
}

// The pad was unplugged: an empty slot again (its number +4 kept), its axes / buttons released.
void detach(u8* cfg, u8* st) {
    jo::as_joystick(cfg)->device = nullptr;
    std::memset(&jo::as_joystick(cfg)->u8_8, 0, sizeof(jo::Joystick) - offsetof(jo::Joystick, u8_8));
    std::memset(jo::as_pad(st)->axis_, 0, sizeof(jo::Pad) - offsetof(jo::Pad, axis_));
}

// js: a DIJOYSTATE2 (0x110 bytes, DirectInput's data format; the joystick slot holds its field offsets as data).
void fill(u8* js, const joan::platform::input::PadState& ps) {
    std::memset(js, 0, 0x110);
    for (i32 a = 0; a < 4; ++a) at<i32>(js, static_cast<i32>(kAxisOffsets[a])) = ps.axes[a];
    { const u32 pov = ps.pov; std::memcpy(js + kPovOffset, &pov, sizeof pov); }  // DIJOYSTATE2 byte buffer, fixed layout
    for (i32 m = 0; m < 12; ++m) js[kButtonOffset + static_cast<u32>(m)] = ps.buttons[m];
}
}  // namespace pad

struct H {

    // -------------------------------------------------------------- 0x00592cd0
    // The original only called the empty 0x005973f0 (Obj::nop_5973f0, its only caller: deleted with it) on the Input
    // object 0x00a48608, keeping eax / edx; no caller uses a result.
    void release_592cd0() {}

    // -------------------------------------------------------------- 0x00598730
    u32 poll_joysticks(i32 /*mode: only clamped into dead locals*/) {
        c_joy_poll.hit();
        u32 changed = 0;  // [ebp-0xc]: uninitialised in the original when nothing was pressed; eax unused
        const u32 connected = joan::platform::input::pads();
        for (i32 i = 0; i < 4; ++i) {
            u8* const cfg = reinterpret_cast<u8*>(&joysticks(this)[i]);
            u8* const st = reinterpret_cast<u8*>(&joy_states(this)[i]);
            alignas(4) u8 js[0x110];
            // (The original polled / acquired the DirectInput joystick here and read its DIJOYSTATE2.)
            const bool here = ((connected >> i) & 1u) != 0;
            if (here != (jo::as_joystick(cfg)->device != nullptr)) {
                if (here) pad::attach(cfg);
                else pad::detach(cfg, st);
                // Slot 0 is the game's joystick (options menu, controls): follow it there.
                if (i == 0) joan::optmenu::joystick_presence_changed(here);
            }
            if (!here) continue;
            joan::platform::input::PadState ps{};
            if (joan::platform::input::read_pad(i, &ps) != 0) continue;  // background: kept as it was
            pad::fill(js, ps);
            alignas(4) u8 prev[sizeof(jo::Pad)];  // 0x39 on i686 (a Pad: x64 is larger)
            std::memcpy(prev, st, sizeof(jo::Pad));
            const i32 axes = static_cast<i8>(jo::as_joystick(cfg)->u8_8);
            for (i32 k = 0; k < axes; ++k) {
                u32 v;  // the joystick slot holds the DIJOYSTATE2 field offsets as data: a byte buffer, not a class
                std::memcpy(&v, js + jo::view<i32>(jo::as_joystick(cfg)->axis_offset[k]), sizeof v);
                jo::view<u32>(jo::as_pad(st)->axis_[k]) = v;
            }
            const i32 buttons_raw = static_cast<i8>(jo::as_joystick(cfg)->u8_9);
            const i32 buttons = static_cast<u32>(buttons_raw) < 0x80u ? buttons_raw : 0x80;
            for (i32 k = 0; k < buttons; ++k) jo::as_pad(st)->button[k] = static_cast<u8>((js[jo::view<i32>(jo::as_joystick(cfg)->button_offset[k])] & 0x80) != 0 ? 1 : 0);
            for (i32 n = 0; n < 0xe; ++n) {
                const i32 now = static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_ai(st)->is_ready(joan::orig::arg::i32(n))));    // 0x00597ed0
                const i32 was = static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_ai(prev)->is_ready(joan::orig::arg::i32(n))));  // 0x00597ed0
                if (now != 0 && was == 0) {
                    const u32 t = import_stdcall<u32>(d::iat_WINMM_timeGetTime);  // timeGetTime
                    joan::orig::as_input(this)->push_event3(joan::orig::arg::u32(i), joan::orig::arg::u32(n), joan::orig::arg::u32(t));       // 0x00593040
                    changed = 1;
                }
            }
        }
        return changed;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_17", name, addr, fn)
B("Input::release_592cd0", 0x00592cd0, &H::release_592cd0);
B("Input::poll_joysticks", 0x00598730, &H::poll_joysticks);
#undef B

}  // namespace game_b111_17
}  // namespace

// ---------------------------------------- from modules/en3d_b87_02.cpp (game/b87_02)
namespace {
namespace en3d_b87_02 {

using joan::call::thiscall;
using joan::call::stdcall_import;

std::int32_t add(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

JOAN_COUNTER(c_key_event, "Input::key_event");
JOAN_COUNTER(c_reset_keys, "Input::reset_keys");

struct H {

    // 0x00598bd0
    std::int32_t key_event(std::uint32_t key, std::uint32_t data, std::uint32_t time) {
        c_key_event.hit();
        std::int32_t result = 0;
        std::uint16_t& mods = jo::self<jo::Input>(this)->skey_state;
        const bool down = (data & 0x80) != 0;
        auto hold = [&](std::uint16_t bit) {
            if (down) {
                mods = static_cast<std::uint16_t>(mods | bit);
            } else {
                mods = static_cast<std::uint16_t>(mods & static_cast<std::uint16_t>(~bit));
            }
        };
        auto toggle = [&](std::uint16_t bit) {
            if (down) mods = static_cast<std::uint16_t>(mods ^ bit);
        };
        switch (key) {
            case 0x2a: hold(0x1); break;    // LSHIFT
            case 0x36: hold(0x2); break;    // RSHIFT
            case 0x1d: hold(0x4); break;    // LCONTROL
            case 0x9d: hold(0x8); break;    // RCONTROL
            case 0x38: hold(0x10); break;   // LMENU
            case 0xb8: hold(0x20); break;   // RMENU
            case 0x3a: toggle(0x80); break;   // CAPITAL
            case 0x45: toggle(0x40); break;   // NUMLOCK
            case 0x46: toggle(0x100); break;  // SCROLL
            case 0xd2:                        // INSERT
                if (down) {
                    mods = static_cast<std::uint16_t>(mods ^ 0x200);
                    joan::orig::as_input(this)->push_event2(joan::orig::arg::i32(key), joan::orig::arg::i32(time));  // 0x00592dc0
                    result = 1;
                }
                break;
            default:
                if (down) {
                    joan::orig::as_input(this)->push_event2(joan::orig::arg::i32(key), joan::orig::arg::i32(time));  // 0x00592dc0
                } else {
                    joan::orig::as_input(this)->push_event6(joan::orig::arg::i32(key), joan::orig::arg::i32(time));  // 0x00592f00
                }
                result = 1;
                break;
        }
        std::uint16_t& arrows = jo::self<jo::Input>(this)->key_flags;
        auto arrow = [&](std::uint16_t bit) {
            if (down) {
                arrows = static_cast<std::uint16_t>(arrows | bit);
            } else {
                arrows = static_cast<std::uint16_t>(arrows & static_cast<std::uint16_t>(~bit));
            }
        };
        switch (key) {
            case 0xcb: arrow(0x1); break;  // LEFT
            case 0xcd: arrow(0x2); break;  // RIGHT
            case 0xc8: arrow(0x4); break;  // UP
            case 0xd0: arrow(0x8); break;  // DOWN
            default: break;
        }
        if (key < 0xff) jo::self<jo::Input>(this)->key_state[static_cast<std::int32_t>(key)] = static_cast<std::uint8_t>(data & 0x80);
        return result;
    }

    // 0x00598ac0
    void reset_keys() {
        c_reset_keys.hit();
        std::uint16_t& mods = jo::self<jo::Input>(this)->skey_state;
        mods = 0;
        if ((stdcall_import<std::int16_t>(d::iat_USER32_GetKeyState, 0x90) & 1) != 0) mods = static_cast<std::uint16_t>(mods | 0x40);   // VK_NUMLOCK
        if ((stdcall_import<std::int16_t>(d::iat_USER32_GetKeyState, 0x14) & 1) != 0) mods = static_cast<std::uint16_t>(mods | 0x80);   // VK_CAPITAL
        if ((stdcall_import<std::int16_t>(d::iat_USER32_GetKeyState, 0x91) & 1) != 0) mods = static_cast<std::uint16_t>(mods | 0x100);  // VK_SCROLL
        mods = static_cast<std::uint16_t>(mods | 0x200);
        jo::self<jo::Input>(this)->key_flags = 0;
        std::memset(static_cast<void*>(&jo::self<jo::Input>(this)->key_state[0]), 0, 0x100u);                        // memset
    }

    // 0x00592d00
    void push_event(const void* ev) {
        const std::int32_t w = jo::self<jo::Input>(this)->queue_head;
        if (w == add(jo::self<jo::Input>(this)->queue_tail, -1)) return;
        if (w == 0x13 && jo::self<jo::Input>(this)->queue_tail == 0) return;
        std::memcpy(&events(this)[w], ev, sizeof(jo::InputEvent));
        jo::self<jo::Input>(this)->queue_head = add(jo::self<jo::Input>(this)->queue_head, 1);
        if (jo::self<jo::Input>(this)->queue_head >= 0x14) jo::self<jo::Input>(this)->queue_head = 0;
    }

    // 0x00595a70
    std::int32_t scale_axis(std::int32_t v) { return v; }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b87_02", name, addr, fn)
GAME("Input::key_event", 0x00598bd0, &H::key_event);
GAME("Input::reset_keys", 0x00598ac0, &H::reset_keys);
GAME("Input::push_event", 0x00592d00, &H::push_event);
GAME("Input::scale_axis", 0x00595a70, &H::scale_axis);
#undef GAME

}  // namespace en3d_b87_02
}  // namespace

// ---------------------------------------- from modules/game_b88_04.cpp (game/b88_04)
namespace {
namespace game_b88_04 {

std::uintptr_t u32(const void* p) { return reinterpret_cast<std::uintptr_t>(p); }

JOAN_COUNTER(c_push6, "EventQueue::push6");
JOAN_COUNTER(c_push2, "EventQueue::push2");

struct H {

    // ------------------------------------------------------------ 0x00592f00 / 0x00592dc0 (ret 8)
    std::uintptr_t push_event(std::int32_t kind, std::int32_t a, std::int32_t b) {
        const std::int32_t wr = jo::self<jo::Input>(this)->queue_head;
        const std::int32_t rd = jo::self<jo::Input>(this)->queue_tail;
        if (static_cast<std::uint32_t>(wr) == static_cast<std::uint32_t>(rd) - 1u || (wr == 0x13 && rd == 0))
            return u32(this);
        std::uint8_t* const e = reinterpret_cast<std::uint8_t*>(&events(this)[wr]);  // ebp-0x8
        jo::view<std::int32_t>(jo::as_input_event(e)->type) = kind;
        jo::view<std::int32_t>(jo::as_input_event(e)->scan_code) = a;
        jo::view<std::int16_t>(jo::as_input_event(e)->skey_state) = jo::view<std::int16_t>(jo::self<jo::Input>(this)->skey_state);
        jo::view<std::int32_t>(jo::as_input_event(e)->time) = b;
        jo::as_input_event(e)->x = jo::self<jo::Input>(this)->cur_x;
        jo::as_input_event(e)->y = jo::self<jo::Input>(this)->cur_y;
        jo::as_input_event(e)->joy_index = 0;
        jo::as_input_event(e)->joy_arg = 0;
        std::memcpy(&jo::as_input_event(e)->joy_state, &joy_states(this)[0], sizeof(jo::Pad));
        jo::self<jo::Input>(this)->queue_head = jo::self<jo::Input>(this)->queue_head + 1;
        if (jo::self<jo::Input>(this)->queue_head >= 0x14) jo::self<jo::Input>(this)->queue_head = 0;
        return u32(this);
    }
    std::uintptr_t push6(std::int32_t a, std::int32_t b) {
        c_push6.hit();
        return push_event(6, a, b);
    }
    std::uintptr_t push2(std::int32_t a, std::int32_t b) {
        c_push2.hit();
        return push_event(2, a, b);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_04", name, addr, fn)
B("EventQueue::push6", 0x00592f00, &H::push6);
B("EventQueue::push2", 0x00592dc0, &H::push2);
#undef B

}  // namespace game_b88_04
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {


JOAN_COUNTER(c_log_push, "Log::push");

struct H {

    // 0x00593040
    void log_push(std::uint32_t a, std::uint32_t b, std::uint32_t c) {
        c_log_push.hit();
        if (jo::view<std::uint32_t>(jo::self<jo::Input>(this)->queue_head) == jo::view<std::uint32_t>(jo::self<jo::Input>(this)->queue_tail) - 1u) return;  // wrapping sub
        if (jo::self<jo::Input>(this)->queue_head == 0x13 && jo::self<jo::Input>(this)->queue_tail == 0) return;
        std::uint8_t* const rec = reinterpret_cast<std::uint8_t*>(&events(this)[jo::self<jo::Input>(this)->queue_head]);
        jo::as_input_event(rec)->type = 3;
        jo::as_input_event(rec)->scan_code = 0;
        jo::as_input_event(rec)->skey_state = jo::self<jo::Input>(this)->skey_state;
        jo::as_input_event(rec)->time = c;
        jo::view<std::uint32_t>(jo::as_input_event(rec)->x) = jo::view<std::uint32_t>(jo::self<jo::Input>(this)->cur_x);
        jo::view<std::uint32_t>(jo::as_input_event(rec)->y) = jo::view<std::uint32_t>(jo::self<jo::Input>(this)->cur_y);
        jo::as_input_event(rec)->joy_index = static_cast<std::uint8_t>(a);
        jo::as_input_event(rec)->joy_arg = static_cast<std::uint8_t>(b);
        // rep movsd (14 dwords) + movsb from +0x156 + (signed byte rec[0x16]) * 0x39.
        const std::uint8_t* s = reinterpret_cast<const std::uint8_t*>(&joy_states(this)[static_cast<std::int32_t>(jo::view<std::int8_t>(jo::as_input_event(rec)->joy_index))]);
        std::uint8_t* d = reinterpret_cast<std::uint8_t*>(&jo::as_input_event(rec)->joy_state);
        for (std::int32_t i = 0; i < 14; ++i) {
            std::uint32_t v;
            std::memcpy(&v, s, 4);
            std::memcpy(d, &v, 4);
            s += 4;
            d += 4;
        }
        *d = *s;
        jo::self<jo::Input>(this)->queue_head = jo::self<jo::Input>(this)->queue_head + 1;
        if (!(jo::self<jo::Input>(this)->queue_head < 0x14)) jo::self<jo::Input>(this)->queue_head = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("Log::push", 0x00593040, &H::log_push);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

using joan::call::cdecl_call;

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using u16 = std::uint16_t;

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::op_delete;

JOAN_COUNTER(c_queue_event, "Sound::queue_event");

struct H {

    // ------------------------------------------------------------------ 0x00593190
    void queue_event(u32 a, u32 b, u32 c) {
        c_queue_event.hit();
        if (jo::view<u32>(jo::self<jo::Input>(this)->queue_head) == jo::view<u32>(jo::self<jo::Input>(this)->queue_tail) - 1u) return;
        if (jo::self<jo::Input>(this)->queue_head == 0x13 && jo::self<jo::Input>(this)->queue_tail == 0) return;
        u8* const p = reinterpret_cast<u8*>(&events(this)[jo::self<jo::Input>(this)->queue_head]);
        jo::as_input_event(p)->type = 7;
        jo::as_input_event(p)->scan_code = 0;
        jo::as_input_event(p)->skey_state = jo::self<jo::Input>(this)->skey_state;
        jo::as_input_event(p)->time = c;
        jo::view<u32>(jo::as_input_event(p)->x) = jo::view<u32>(jo::self<jo::Input>(this)->cur_x);
        jo::view<u32>(jo::as_input_event(p)->y) = jo::view<u32>(jo::self<jo::Input>(this)->cur_y);
        jo::as_input_event(p)->joy_index = static_cast<u8>(a);
        jo::as_input_event(p)->joy_arg = static_cast<u8>(b);
        {
            const u8* src = reinterpret_cast<const u8*>(&joy_states(this)[static_cast<i32>(jo::view<i8>(jo::as_input_event(p)->joy_index))]);
            u8* dst = reinterpret_cast<u8*>(&jo::as_input_event(p)->joy_state);
            for (i32 i = 0; i < 0xe; ++i) {
                u32 v;
                std::memcpy(&v, src, 4);
                std::memcpy(dst, &v, 4);
                src += 4;
                dst += 4;
            }
            *dst = *src;
        }
        jo::self<jo::Input>(this)->queue_head = jo::self<jo::Input>(this)->queue_head + 1;
        if (jo::self<jo::Input>(this)->queue_head >= 0x14) jo::self<jo::Input>(this)->queue_head = 0;
    }

    // ------------------------------------------------------------------ 0x00596b30
    H* scalar_dtor_596b30(u32 flags) {
        joan::orig::as_vga_buf(this)->free_all();  // 0x00612a00
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Sound::queue_event", 0x00593190, &H::queue_event);
B("Obj::scalar_dtor_596b30", 0x00596b30, &H::scalar_dtor_596b30);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b109_18.cpp (game/b109_18)
namespace {
namespace game_b109_18 {

using joan::hook::global;

using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using u16 = std::uint16_t;

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::orig::helpers::at;

JOAN_COUNTER(c_pop, "InputQueue::pop_5932e0");

using joan::orig::helpers::crt_abs;

struct H {
    // ------------------------------------------------------------------ 0x005932e0
    void apply_entry_state(const void* e) {
        jo::self<jo::Input>(this)->event_joy_index = jo::as_input_event(e)->joy_index;
        jo::self<jo::Input>(this)->event_joy_arg = jo::as_input_event(e)->joy_arg;
        std::memcpy(&jo::self<jo::Input>(this)->event_joy_state, &jo::as_input_event(e)->joy_state, sizeof(jo::Pad));
    }
    i32 pop_event() {
        c_pop.hit();
        i32 flag = 0;
        std::memcpy(joan::orig::arg::mptr(&jo::self<jo::Input>(this)->prev_joy_state_0_), joan::orig::arg::ptr(joy_states(this)), static_cast<u32>(4 * sizeof(jo::Pad)));  // memmove: the 4 pads (0xe4 on x86)
        joan::orig::as_input(this)->poll_joysticks(i32{-1});                                     // 0x00598730
        for (;;) {
            if (jo::self<jo::Input>(this)->queue_head == jo::self<jo::Input>(this)->queue_tail) {
                jo::self<jo::Input>(this)->scan_code = 0;
                jo::self<jo::Input>(this)->key_code = 0;
                jo::view<u8>(jo::self<jo::Input>(this)->has_mouse_event) = 0;
                jo::view<u8>(jo::self<jo::Input>(this)->has_joy_event) = 0;
                return flag;
            }
            void* const e = &events(this)[jo::self<jo::Input>(this)->queue_tail];
            jo::self<jo::Input>(this)->event_skey_state = jo::as_input_event(e)->skey_state;
            jo::view<u32>(jo::self<jo::Input>(this)->mouse_event_type) = jo::as_input_event(e)->type;
            jo::self<jo::Input>(this)->event_time = jo::as_input_event(e)->time;
            const u32 kind = jo::as_input_event(e)->type;
            switch (kind) {
                case 0:
                case 1: {
                    const i32 k = static_cast<i32>(kind);
                    jo::view<u32>(clicks(this)[1 - k].count) = 0;
                    void* const p = &clicks(this)[k];
                    if (jo::as_input_event(e)->time - jo::as_mouse_click(p)->time < global<u32>(d::g_a1590c) &&
                        crt_abs(static_cast<i32>(jo::view<u32>(jo::as_input_event(e)->x) - jo::view<u32>(jo::as_mouse_click(p)->x))) < 5 &&
                        crt_abs(static_cast<i32>(jo::view<u32>(jo::as_input_event(e)->y) - jo::view<u32>(jo::as_mouse_click(p)->y))) < 5) {
                        jo::view<u32>(jo::as_mouse_click(p)->count) += 1;
                    } else {
                        jo::view<u32>(jo::as_mouse_click(p)->count) = 1;
                    }
                    jo::as_mouse_click(p)->time = jo::as_input_event(e)->time;
                    jo::view<u32>(jo::as_mouse_click(p)->x) = jo::view<u32>(jo::as_input_event(e)->x);
                    jo::view<u32>(jo::as_mouse_click(p)->y) = jo::view<u32>(jo::as_input_event(e)->y);
                    jo::self<jo::Input>(this)->scan_code = 0;
                    jo::self<jo::Input>(this)->key_code = 0;
                    jo::view<u8>(jo::self<jo::Input>(this)->has_mouse_event) = 1;
                    jo::view<u8>(jo::self<jo::Input>(this)->has_joy_event) = 0;
                    apply_entry_state(e);
                    jo::self<jo::Input>(this)->button_down[k] = 1;
                    jo::self<jo::Input>(this)->button_press_time[k] = jo::as_input_event(e)->time;
                    flag = 1;
                    break;
                }
                case 2:
                    jo::self<jo::Input>(this)->scan_code = jo::as_input_event(e)->scan_code;
                    jo::self<jo::Input>(this)->key_code = joan::orig::arg::as<u32>(joan::orig::keyboard::translate(joan::orig::arg::u32(jo::self<jo::Input>(this)->scan_code), joan::orig::arg::u32(static_cast<u32>(jo::self<jo::Input>(this)->event_skey_state)), joan::orig::arg::u32(u32{0}), joan::orig::arg::u32(u32{0xe200})));  // 0x00599240
                    jo::view<u8>(jo::self<jo::Input>(this)->has_mouse_event) = 0;
                    jo::view<u8>(jo::self<jo::Input>(this)->has_joy_event) = 0;
                    apply_entry_state(e);
                    jo::self<jo::Input>(this)->last_scan_code = jo::self<jo::Input>(this)->scan_code;
                    jo::self<jo::Input>(this)->last_key_code = jo::self<jo::Input>(this)->key_code;
                    jo::self<jo::Input>(this)->last_key_time = jo::as_input_event(e)->time;
                    flag = 1;
                    break;
                case 3:
                case 7:
                    jo::view<u8>(jo::self<jo::Input>(this)->has_mouse_event) = 0;
                    jo::self<jo::Input>(this)->scan_code = 0;
                    jo::self<jo::Input>(this)->key_code = 0;
                    jo::view<u8>(jo::self<jo::Input>(this)->has_joy_event) = 1;
                    apply_entry_state(e);
                    flag = 1;
                    break;
                case 4:
                case 5: {
                    const i32 k = static_cast<i32>(kind);
                    void* const p = &clicks(this)[k - 4];
                    jo::as_mouse_click(p)->release_time = jo::as_input_event(e)->time;
                    jo::view<u32>(jo::as_mouse_click(p)->release_x) = jo::view<u32>(jo::as_input_event(e)->x);
                    jo::view<u32>(jo::as_mouse_click(p)->release_y) = jo::view<u32>(jo::as_input_event(e)->y);
                    jo::self<jo::Input>(this)->scan_code = 0;
                    jo::self<jo::Input>(this)->key_code = 0;
                    jo::view<u8>(jo::self<jo::Input>(this)->has_mouse_event) = 1;
                    jo::view<u8>(jo::self<jo::Input>(this)->has_joy_event) = 0;
                    apply_entry_state(e);
                    jo::view<u8>(jo::self<jo::Input>(this)->button_down[k - 4]) = 0;
                    flag = 1;
                    break;
                }
                case 6:
                    jo::view<u8>(jo::self<jo::Input>(this)->has_joy_event) = 0;
                    apply_entry_state(e);
                    jo::self<jo::Input>(this)->last_scan_code = 0;
                    jo::self<jo::Input>(this)->last_key_code = 0;
                    break;
                default:
                    break;
            }
            jo::self<jo::Input>(this)->queue_tail += 1;
            if (jo::self<jo::Input>(this)->queue_tail >= 0x14) jo::self<jo::Input>(this)->queue_tail = 0;
            if (flag != 0) return flag;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_18", name, addr, fn)
B("InputQueue::pop_5932e0", 0x005932e0, &H::pop_event);
#undef B

}  // namespace game_b109_18
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

using joan::call::cdecl_call;

using joan::img::obj;

using joan::call::thiscall;

constexpr img::Addr kClock(d::g_misc);

JOAN_COUNTER(c_in_rect, "Joystick::in_rect");
JOAN_COUNTER(c_axis, "Pad::axis");

// 0x00598700 the 16-byte compare (CRT 0x7b2620) is equal.
std::int32_t JOAN_CDECL same16(const void* a, const void* b) {
    return joan::orig::arg::as<std::int32_t>(joan::orig::libc::memcmp(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), 0x10u)) == 0 ? 1 : 0;
}

struct H {

    // 0x005939f0
    std::uint32_t forward(std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d, std::uint32_t e,
                          std::uint32_t g) {
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_triangle(this)->contains(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(g), joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Input>(this)->cur_x)), joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Input>(this)->cur_y))));  // 0x00596140
    }

    // 0x00594360 drag rectangle (left +0x672 / right +0x68e) completed inside (x0, y0)-(x1, y1).
    std::int32_t in_rect(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1, std::int32_t which) {
        c_in_rect.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->has_mouse_event) == 0) return 0;
        if (which == 0 || which == 2) {
            jo::MouseClick* const s = &jo::self<jo::Input>(this)->clicks[0];
            if (jo::self<jo::Input>(this)->mouse_event_type == 0 && jo::as_mouse_click(s)->count == 2 && jo::as_mouse_click(s)->x >= x0 &&
                jo::as_mouse_click(s)->y >= y0 && jo::as_mouse_click(s)->x <= x1 && jo::as_mouse_click(s)->y <= y1)
                return 1;
        }
        if (which == 1 || which == 2) {
            jo::MouseClick* const s = &jo::self<jo::Input>(this)->clicks[1];
            if (jo::self<jo::Input>(this)->mouse_event_type == 1 && jo::as_mouse_click(s)->count == 2 && jo::as_mouse_click(s)->x >= x0 &&
                jo::as_mouse_click(s)->y >= y0 && jo::as_mouse_click(s)->x <= x1 && jo::as_mouse_click(s)->y <= y1)
                return 1;
        }
        return 0;
    }

    // 0x00594a10
    std::int32_t pressed_kind(std::int32_t which) {
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->has_mouse_event) == 0) return 0;
        if ((which == 0 || which == 2) && jo::self<jo::Input>(this)->mouse_event_type == 4) return 1;
        if ((which == 1 || which == 2) && jo::self<jo::Input>(this)->mouse_event_type == 5) return 2;
        return 0;
    }

    // 0x00595fb0 0 = up, 1 = held < delay, else 2 + repeats.
    std::uint32_t repeat_count() {
        if (jo::self<jo::Input>(this)->last_scan_code == 0) return 0;
        const std::uint32_t held = joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(img::obj(kClock))->get_time_591050()) - jo::self<jo::Input>(this)->last_key_time;  // 0x00591050
        if (held < jo::self<jo::Input>(this)->key_repeat_delay) return 1;
        return (held - jo::self<jo::Input>(this)->key_repeat_delay) / jo::self<jo::Input>(this)->key_repeat_rate + 2;
    }

    std::uint16_t word0() { return jo::view<std::uint16_t>(jo::self<jo::Bitmap>(this)->width_); }  // 0x005979a0
    std::uint16_t word2() { return jo::view<std::uint16_t>(jo::self<jo::Bitmap>(this)->height_); }  // 0x005979c0

    // 0x00597c80
    std::int32_t axis(std::int32_t index) {
        c_axis.hit();
        const std::int32_t k = jo::self<jo::Pad>(this)->joystick->axis_map[index];
        if (k > 0) {
            if (index == 4) {
                const std::int32_t v = jo::self<jo::Pad>(this)->axis_[k - 1];  // axis_map holds 1-based axis numbers
                if (v > 0x57e4) {
                    if (v == 0x6978 || v == 0x7b0c) return -127;
                    return 0;
                }
                if (v == 0x57e4) return -127;
                if (v == 0x1194 || v == 0x2328 || v == 0x34bc) return 127;
                return 0;
            }
            if (index == 5) {
                const std::int32_t v = jo::self<jo::Pad>(this)->axis_[k - 1];  // axis_map holds 1-based axis numbers
                if (v > 0x4650) {
                    if (v == 0x57e4) return 127;
                    if (v == 0x7b0c || v == 0x8ca0) return -127;
                    return 0;
                }
                if (v == 0x4650) return 127;
                if (v == 0 || v == 0x1194) return -127;
                if (v == 0x34bc) return 127;
                return 0;
            }
            return jo::self<jo::Pad>(this)->axis_[k - 1];
        }
        if (k < 0) return jo::view<std::int8_t>(jo::self<jo::Pad>(this)->button[-k - 1]);
        return 0;
    }

    // 0x00597e00
    std::int32_t axis_range(std::int32_t index, std::int32_t* lo, std::int32_t* hi) {
        const std::int32_t k = jo::self<jo::Pad>(this)->joystick->axis_map[index];
        if (k > 0) {
            if (index == 4 || index == 5) {
                *lo = -127;
                *hi = 127;
            } else {
                *lo = jo::self<jo::Pad>(this)->joystick->axis_min[k - 1];
                *hi = jo::self<jo::Pad>(this)->joystick->axis_max[k - 1];
            }
            return 1;
        }
        if (k < 0) {
            *lo = 0;
            *hi = 1;
            return 1;
        }
        return 0;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("Stream::forward", 0x005939f0, &H::forward);
B91("Joystick::in_rect", 0x00594360, &H::in_rect);
B91("Joystick::pressed_kind", 0x00594a10, &H::pressed_kind);
B91("Key::repeat_count", 0x00595fb0, &H::repeat_count);
B91("word0", 0x005979a0, &H::word0);
B91("word2", 0x005979c0, &H::word2);
B91("Pad::axis", 0x00597c80, &H::axis);
B91("Pad::axis_range", 0x00597e00, &H::axis_range);
B91("same16", 0x00598700, &same16);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b102_09.cpp (game/b102_09)
namespace {
namespace game_b102_09 {

using joan::hook::global;

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_hit_box_a, "Mouse::hit_box_a");
JOAN_COUNTER(c_hit_box_b, "Mouse::hit_box_b");
JOAN_COUNTER(c_move_to, "Mouse::move_to");
JOAN_COUNTER(c_move_back, "Mouse::move_back");
JOAN_COUNTER(c_shift_boxes, "Mouse::shift_boxes");


struct H {

    // ------------------------------------------------------------------------ 0x00593a50
    i32 zone_hit(i32 x0, i32 y0, i32 x1, i32 y1, i32 mode) {
        if (jo::self<jo::Input>(this)->cur_x < x0 || jo::self<jo::Input>(this)->cur_y < y0 || jo::self<jo::Input>(this)->cur_x > x1 || jo::self<jo::Input>(this)->cur_y > y1) return 0;
        if (jo::view<u32>(jo::self<jo::Input>(this)->left_down) != 0 && (mode == 0 || mode == 2)) return 1;
        if (jo::view<u32>(jo::self<jo::Input>(this)->right_down) != 0 && (mode == 1 || mode == 2)) return 2;
        return 0;
    }

    // ------------------------------------------------------------------------ 0x00594220
    // The press (release = false) or release position of `box`.
    i32 test_box(const jo::MouseClick* box, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, bool release) {
        const u32 px = jo::view<u32>(release ? box->release_x : box->x);
        const u32 py = jo::view<u32>(release ? box->release_y : box->y);
        return joan::orig::arg::as<i32>(joan::orig::as_triangle(this)->contains(joan::orig::arg::i32(a1), joan::orig::arg::i32(a2), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(// 0x00596140
                             px), joan::orig::arg::i32(py)));
    }
    i32 hit_box_a(u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, i32 mode) {
        c_hit_box_a.hit();
        if (static_cast<i8>(jo::view<u8>(jo::self<jo::Input>(this)->has_mouse_event)) == 0) return 0;
        if (mode == 0 || mode == 2) {
            jo::MouseClick* const box = &jo::self<jo::Input>(this)->clicks[0];
            if (jo::view<u32>(jo::self<jo::Input>(this)->mouse_event_type) == 0 && jo::view<u32>(box->count) == 1 && test_box(box, a1, a2, a3, a4, a5, a6, false) != 0)
                return 1;
        }
        if (mode == 1 || mode == 2) {
            jo::MouseClick* const box = &jo::self<jo::Input>(this)->clicks[1];
            if (jo::view<u32>(jo::self<jo::Input>(this)->mouse_event_type) == 1 && jo::view<u32>(box->count) == 1 && test_box(box, a1, a2, a3, a4, a5, a6, false) != 0)
                return 2;
        }
        return 0;
    }

    // ------------------------------------------------------------------------ 0x00594e20
    i32 hit_box_b(u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, i32 mode) {
        c_hit_box_b.hit();
        if (static_cast<i8>(jo::view<u8>(jo::self<jo::Input>(this)->has_mouse_event)) == 0) return 0;
        if (mode == 0 || mode == 2) {
            jo::MouseClick* const box = &jo::self<jo::Input>(this)->clicks[0];
            if (jo::view<u32>(jo::self<jo::Input>(this)->mouse_event_type) == 4 && test_box(box, a1, a2, a3, a4, a5, a6, true) != 0 &&
                jo::view<u32>(box->count) == 1 && box->release_time - box->time <= 0xc8u)
                return 1;
        }
        if (mode == 1 || mode == 2) {
            jo::MouseClick* const box = &jo::self<jo::Input>(this)->clicks[1];
            if (jo::view<u32>(jo::self<jo::Input>(this)->mouse_event_type) == 5 && test_box(box, a1, a2, a3, a4, a5, a6, true) != 0 &&
                jo::view<u32>(box->count) == 1 && box->release_time - box->time <= 0xc8u)
                return 2;
        }
        return 0;
    }

    // Boxes: the two MouseClick records, four coordinates each (x, y, release x, release y).
    void boxes_sub(u32 dx, u32 dy) {
        for (i32 i = 0; i < 2; ++i) {
            jo::MouseClick& c = clicks(this)[i];
            jo::view<u32>(c.x) = jo::view<u32>(c.x) - dx;
            jo::view<u32>(c.y) = jo::view<u32>(c.y) - dy;
            jo::view<u32>(c.release_x) = jo::view<u32>(c.release_x) - dx;
            jo::view<u32>(c.release_y) = jo::view<u32>(c.release_y) - dy;
        }
    }

    // ------------------------------------------------------------------------ 0x00595cc0
    void* move_to(u32 x, u32 y, u32* old) {
        c_move_to.hit();
        old[0] = jo::view<u32>(jo::self<jo::Input>(this)->saved_x);
        old[1] = jo::view<u32>(jo::self<jo::Input>(this)->saved_y);
        const u32 dx = x - jo::view<u32>(jo::self<jo::Input>(this)->saved_x);
        const u32 dy = y - jo::view<u32>(jo::self<jo::Input>(this)->saved_y);
        jo::view<u32>(jo::self<jo::Input>(this)->cur_x) = jo::view<u32>(jo::self<jo::Input>(this)->cur_x) - dx;
        jo::view<u32>(jo::self<jo::Input>(this)->cur_y) = jo::view<u32>(jo::self<jo::Input>(this)->cur_y) - dy;
        boxes_sub(dx, dy);
        jo::view<u32>(jo::self<jo::Input>(this)->saved_x) = x;
        jo::view<u32>(jo::self<jo::Input>(this)->saved_y) = y;
        return this;
    }

    // ------------------------------------------------------------------------ 0x00595e40
    void* move_back(const u32* p) {
        c_move_back.hit();
        const u32 dx = jo::view<u32>(jo::self<jo::Input>(this)->saved_x) - p[0];
        const u32 dy = jo::view<u32>(jo::self<jo::Input>(this)->saved_y) - p[1];
        jo::view<u32>(jo::self<jo::Input>(this)->cur_x) = jo::view<u32>(jo::self<jo::Input>(this)->cur_x) + dx;
        jo::view<u32>(jo::self<jo::Input>(this)->cur_y) = jo::view<u32>(jo::self<jo::Input>(this)->cur_y) + dy;
        for (i32 i = 0; i < 2; ++i) {
            jo::MouseClick& c = clicks(this)[i];
            jo::view<u32>(c.x) = jo::view<u32>(c.x) + dx;
            jo::view<u32>(c.y) = jo::view<u32>(c.y) + dy;
            jo::view<u32>(c.release_x) = jo::view<u32>(c.release_x) + dx;
            jo::view<u32>(c.release_y) = jo::view<u32>(c.release_y) + dy;
        }
        jo::view<u32>(jo::self<jo::Input>(this)->saved_x) = p[0];
        jo::view<u32>(jo::self<jo::Input>(this)->saved_y) = p[1];
        return this;
    }

    // ------------------------------------------------------------------------ 0x00596040
    u32 shift_boxes(u32 dx, u32 dy) {
        c_shift_boxes.hit();
        boxes_sub(dx, dy);
        return 2;  // eax = final loop counter
    }

    // ------------------------------------------------------------------------ 0x00597190
    void* set_rect_6a(u32 a, u32 b, u32 c, u32 d) {
        jo::view<u32>(jo::self<jo::MouseCursor>(this)->clip_x1) = a;
        jo::view<u32>(jo::self<jo::MouseCursor>(this)->clip_y1) = b;
        jo::view<u32>(jo::self<jo::MouseCursor>(this)->clip_x2) = c;
        jo::view<u32>(jo::self<jo::MouseCursor>(this)->clip_y2) = d;
        return this;
    }

    void nop_5976d0() {}        // 0x005976d0
    void nop_597890(u32) {}     // 0x00597890
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_09", name, addr, fn)
B("Zone::hit", 0x00593a50, &H::zone_hit);
B("Mouse::hit_box_a", 0x00594220, &H::hit_box_a);
B("Mouse::hit_box_b", 0x00594e20, &H::hit_box_b);
B("Mouse::move_to", 0x00595cc0, &H::move_to);
B("Mouse::move_back", 0x00595e40, &H::move_back);
B("Mouse::shift_boxes", 0x00596040, &H::shift_boxes);
B("X::set_rect_6a", 0x00597190, &H::set_rect_6a);
B("X::nop_5976d0", 0x005976d0, &H::nop_5976d0);
B("X::nop_597890", 0x00597890, &H::nop_597890);
#undef B

}  // namespace game_b102_09
}  // namespace

// ---------------------------------------- from modules/game_b116_14.cpp (game/b116_14)
namespace {
namespace game_b116_14 {

using joan::hook::global;

using i32 = std::int32_t;

using joan::img::obj;

using u32 = std::uint32_t;

// Not joan::call::thiscall: a __thiscall function pointer, not a pointer to member (a class-type R is returned
// as a free function returns it); the target is an fn:: name or a computed address (see src/call.hpp).
template <class R = void, class T, class... A>
R thiscall(T address, const void* self, A... args) {
    return reinterpret_cast<R(JOAN_THISCALL*)(void*, A...)>(joan::fn::target(address))(const_cast<void*>(self), args...);
}
using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;
i32 wmul(i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) * static_cast<u32>(b)); }
i32 wneg(i32 a) { return static_cast<i32>(0u - static_cast<u32>(a)); }

JOAN_COUNTER(c_bound, "Mouse::set_boundary");
JOAN_COUNTER(c_reset, "Mouse::reset_boundary");

struct H {
    // -----------------------------------------------------------------------------------------
    // Mouse: cursor +0x14 / +0x18 / +0x1c, bounds +0x636 x1, +0x63a y1, +0x63e x2, +0x642 y2,
    // +0x646 / +0x64a z range, +0x64e mode, +0x662 / +0x666 hot spot.
    jo::Input* in() { return jo::self<jo::Input>(this); }
    void clamp_cursor() {
        if (in()->cur_x < in()->bound_x1) in()->cur_x = in()->bound_x1;
        if (in()->cur_x > in()->bound_x2) in()->cur_x = in()->bound_x2;
        if (in()->cur_y < in()->bound_y1) in()->cur_y = in()->bound_y1;
        if (in()->cur_y > in()->bound_y2) in()->cur_y = in()->bound_y2;
        if (in()->cur_z < in()->bound_z1) in()->cur_z = in()->bound_z1;
        if (in()->cur_z > in()->bound_z2) in()->cur_z = in()->bound_z2;
    }

    // 0x00593af0 Mouse::set_boundary(x1, y1, x2, y2, mode)
    u32 set_boundary(i32 x1, i32 y1, i32 x2, i32 y2, i32 mode) {
        c_bound.hit();
        in()->bound_z1 = global<i32>(d::g_a48878);
        in()->bound_z2 = global<i32>(d::g_a4887c);
        if (x1 >= 0) in()->bound_x1 = x1 < global<i32>(d::g_mouse_limit_x1) ? global<i32>(d::g_mouse_limit_x1) : x1;
        if (y1 >= 0) in()->bound_y1 = y1 < global<i32>(d::g_mouse_limit_y1) ? global<i32>(d::g_mouse_limit_y1) : y1;
        if (x2 >= 0) in()->bound_x2 = x2 > global<i32>(d::g_mouse_limit_x2) ? global<i32>(d::g_mouse_limit_x2) : x2;
        if (y2 >= 0) in()->bound_y2 = y2 > global<i32>(d::g_mouse_limit_y2) ? global<i32>(d::g_mouse_limit_y2) : y2;
        if (mode >= 0) in()->bound_mode = mode;
        const i32 old_x = in()->cur_x;
        const i32 old_y = in()->cur_y;
        const i32 sel = in()->bound_mode;
        if (sel == 0) {
            clamp_cursor();
        } else if (sel == 1) {
            const i32 w = wsub(in()->bound_x2, in()->bound_x1);
            const i32 h = wsub(in()->bound_y2, in()->bound_y1);
            const i32 dx = wsub(wsub(wadd(in()->cur_x, in()->cur_x), in()->bound_x1), in()->bound_x2);
            const i32 dy = wsub(wsub(wadd(in()->cur_y, in()->cur_y), in()->bound_y1), in()->bound_y2);
            i32 s = wmul(dx, h);
            i32 d = wmul(dy, w);
            s = wadd(s, d);
            d = wadd(d, wsub(d, s));
            const i32 lim = wmul(w, h);
            if (s > lim) s = lim;
            if (s < wneg(lim)) s = wneg(lim);
            if (d > lim) d = lim;
            if (d < wneg(lim)) d = wneg(lim);
            const i32 cx = wadd(in()->bound_x1, in()->bound_x2);
            in()->cur_x = wadd(wsub(s, d) / h / 2, cx) / 2;
            const i32 cy = wadd(in()->bound_y1, in()->bound_y2);
            in()->cur_y = wadd(wadd(s, d) / w / 2, cy) / 2;
            in()->cur_z = wadd(in()->bound_z1, in()->bound_z2) / 2;
        }
        if (old_x == in()->cur_x && old_y == in()->cur_y) return static_cast<u32>(old_y);
        const i32 py = wadd(in()->cur_y, in()->saved_y);
        const i32 px = wadd(in()->cur_x, in()->saved_x);
        return joan::orig::arg::as<u32>(joan::orig::as_mouse_cursor(obj(d::g_mouse_cursor))->move(joan::orig::arg::i32(px), joan::orig::arg::i32(py)));  // 0x004918d0
    }

    // 0x00593f60 Mouse::reset_boundary
    u32 reset_boundary() {
        c_reset.hit();
        in()->bound_x1 = global<i32>(d::g_mouse_limit_x1);
        in()->bound_y1 = global<i32>(d::g_mouse_limit_y1);
        in()->bound_x2 = global<i32>(d::g_mouse_limit_x2);
        in()->bound_y2 = global<i32>(d::g_mouse_limit_y2);
        in()->bound_z1 = global<i32>(d::g_a48878);
        in()->bound_z2 = global<i32>(d::g_a4887c);
        in()->bound_mode = 0;
        clamp_cursor();
        return static_cast<u32>(in()->cur_z);
    }
};

JOAN_REPLACE("game/b116_14", "Mouse::set_boundary", 0x00593af0, &H::set_boundary);
JOAN_REPLACE("game/b116_14", "Mouse::reset_boundary", 0x00593f60, &H::reset_boundary);

}  // namespace game_b116_14
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {


JOAN_COUNTER(c_rect_mode, "Parser::rect_mode");
JOAN_COUNTER(c_rect_value, "Parser::rect_value");
JOAN_COUNTER(c_rect_mode2, "Parser::rect_mode2");

struct H {

    // The MouseClick of button 0 (mode 0) / 1 (mode 1): press x, y, release x, y, click count.
    static bool inside01(const jo::MouseClick* r, std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1) {
        return !(r->x < x0) && !(r->y < y0) && !(r->x > x1) &&
               !(r->y > y1);
    }
    static bool inside23(const jo::MouseClick* r, std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1) {
        return !(r->release_x < x0) && !(r->release_y < y0) && !(r->release_x > x1) &&
               !(r->release_y > y1);
    }

    // 0x005940f0
    std::int32_t rect_mode(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1, std::int32_t which) {
        c_rect_mode.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->has_mouse_event) == 0) return 0;
        if (which == 0 || which == 2) {
            jo::MouseClick* const r = &jo::self<jo::Input>(this)->clicks[0];
            if (jo::self<jo::Input>(this)->mouse_event_type == 0 && r->count == 1 && inside01(r, x0, y0, x1, y1)) return 1;
        }
        if (which == 1 || which == 2) {
            jo::MouseClick* const r = &jo::self<jo::Input>(this)->clicks[1];
            if (jo::self<jo::Input>(this)->mouse_event_type == 1 && r->count == 1 && inside01(r, x0, y0, x1, y1)) return 2;
        }
        return 0;
    }

    // 0x005945d0
    std::int32_t rect_value(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1, std::int32_t which) {
        c_rect_value.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->has_mouse_event) == 0) return 0;
        if (which == 0 || which == 2) {
            jo::MouseClick* const r = &jo::self<jo::Input>(this)->clicks[0];
            if (jo::self<jo::Input>(this)->mouse_event_type == 0 && !(r->count < 1) && inside01(r, x0, y0, x1, y1)) {
                return r->count;
            }
        }
        if (which == 1 || which == 2) {
            jo::MouseClick* const r = &jo::self<jo::Input>(this)->clicks[1];
            if (jo::self<jo::Input>(this)->mouse_event_type == 1 && !(r->count < 1) && inside01(r, x0, y0, x1, y1)) {
                return r->count;
            }
        }
        return 0;
    }

    // 0x00594700
    std::int32_t value(std::int32_t which) {
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->has_mouse_event) == 0) return 0;
        if (which == 0 || which == 2) {
            jo::MouseClick* const r = &jo::self<jo::Input>(this)->clicks[0];
            if (jo::self<jo::Input>(this)->mouse_event_type == 0 && !(r->count < 1)) return r->count;
        }
        if (which == 1 || which == 2) {
            jo::MouseClick* const r = &jo::self<jo::Input>(this)->clicks[1];
            if (jo::self<jo::Input>(this)->mouse_event_type == 1 && !(r->count < 1)) return r->count;
        }
        return 0;
    }

    // 0x00594900
    std::int32_t rect_mode2(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1, std::int32_t which) {
        c_rect_mode2.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->has_mouse_event) == 0) return 0;
        if (which == 0 || which == 2) {
            jo::MouseClick* const r = &jo::self<jo::Input>(this)->clicks[0];
            if (jo::self<jo::Input>(this)->mouse_event_type == 4 && inside23(r, x0, y0, x1, y1)) return 1;
        }
        if (which == 1 || which == 2) {
            jo::MouseClick* const r = &jo::self<jo::Input>(this)->clicks[1];
            if (jo::self<jo::Input>(this)->mouse_event_type == 5 && inside23(r, x0, y0, x1, y1)) return 2;
        }
        return 0;
    }

    // 0x00595a20
    // eax = this (two callers tail-return it).
    void* parser_reset() {
        jo::view<std::uint8_t>(jo::self<jo::Input>(this)->has_mouse_event) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Input>(this)->clicks[0].count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Input>(this)->clicks[1].count) = 0;
        return this;
    }

    // 0x00597ed0
    std::uint8_t is_ready(std::int32_t i) {
        const std::int32_t v = static_cast<std::int32_t>(jo::self<jo::Pad>(this)->joystick->button_map[i]);
        if (v > 0) {
            const std::int32_t a = static_cast<std::int32_t>(jo::view<std::uint32_t>(jo::self<jo::Pad>(this)->axis_[v - 1]) << 1);
            void* const d = jo::view<void*>(jo::self<jo::Pad>(this)->joystick);
            const std::int32_t b = static_cast<std::int32_t>(jo::view<std::uint32_t>(jo::as_joystick(d)->axis_min[v - 1]) + jo::view<std::uint32_t>(jo::as_joystick(d)->axis_max[v - 1]));
            return a > b ? 1 : 0;
        }
        if (v < 0) return jo::self<jo::Pad>(this)->button[-v - 1];
        return 0;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Parser::rect_mode", 0x005940f0, &H::rect_mode);
M("Parser::rect_value", 0x005945d0, &H::rect_value);
M("Parser::value", 0x00594700, &H::value);
M("Parser::rect_mode2", 0x00594900, &H::rect_mode2);
M("Parser::reset", 0x00595a20, &H::parser_reset);
M("Ai::is_ready", 0x00597ed0, &H::is_ready);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

using joan::call::thiscall;

JOAN_COUNTER(c_play_5947c0, "Obj::play_5947c0");

struct H {

    // 0x005947c0
    std::int32_t play_5947c0(std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4,
                             std::uint32_t a5, std::uint32_t a6, std::int32_t mode) {
        c_play_5947c0.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->has_mouse_event) == 0) return 0;
        if (mode == 0 || mode == 2) {
            jo::MouseClick* const p = &jo::self<jo::Input>(this)->clicks[0];
            if (jo::self<jo::Input>(this)->mouse_event_type == 0 && p->count >= 1) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_triangle(this)->contains(joan::orig::arg::i32(a1), joan::orig::arg::i32(a2), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(jo::view<std::uint32_t>(p->x)), joan::orig::arg::i32(jo::view<std::uint32_t>(p->y)))) != 0)  // 0x00596140
                    return p->count;
            }
        }
        if (mode == 1 || mode == 2) {
            jo::MouseClick* const p = &jo::self<jo::Input>(this)->clicks[1];
            if (jo::self<jo::Input>(this)->mouse_event_type == 1 && p->count >= 1) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_triangle(this)->contains(joan::orig::arg::i32(a1), joan::orig::arg::i32(a2), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(jo::view<std::uint32_t>(p->x)), joan::orig::arg::i32(jo::view<std::uint32_t>(p->y)))) != 0)
                    return p->count;
            }
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("Obj::play_5947c0", 0x005947c0, &H::play_5947c0);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b101_10.cpp (game/b101_10)
namespace {
namespace game_b101_10 {

using joan::hook::global;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::call::thiscall;
template <class R = void, class... A>
R winapi(img::Addr slot, A... args) {  // KERNEL32 / WINMM through the IAT slot
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}
using joan::orig::helpers::at;

using joan::orig::helpers::crt_memset;
u32 time_ms() { return winapi<u32>(d::iat_WINMM_timeGetTime); }  // timeGetTime

JOAN_COUNTER(c_clicked, "Mouse::clicked_in");
JOAN_COUNTER(c_held, "Mouse::held_in");

struct H {

    // ------------------------------------------------------------------ 0x00594bd0
    bool released_in(i32 k, i32 l, i32 t, i32 r, i32 b) {
        const jo::MouseClick* const s = &clicks(this)[k];
        return jo::as_mouse_click(s)->release_x >= l && jo::as_mouse_click(s)->release_y >= t && jo::as_mouse_click(s)->release_x <= r && jo::as_mouse_click(s)->release_y <= b &&
               jo::as_mouse_click(s)->count == 1 && static_cast<u32>(jo::as_mouse_click(s)->release_time - jo::as_mouse_click(s)->time) <= 200u;
    }
    i32 clicked_in(i32 l, i32 t, i32 r, i32 b, i32 which) {
        c_clicked.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->has_mouse_event) == 0) return 0;
        if (which == 0 || which == 2) {
            if (jo::self<jo::Input>(this)->mouse_event_type == 4 && released_in(0, l, t, r, b)) return 1;
        }
        if (which == 1 || which == 2) {
            if (jo::self<jo::Input>(this)->mouse_event_type == 5 && released_in(1, l, t, r, b)) return 2;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x00595b10
    bool held(i32 k, i32 l, i32 t, i32 r, i32 b, u32 ms) {
        const jo::MouseClick* const s = &clicks(this)[k];
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->button_down[k]) == 0) return false;
        if (jo::as_mouse_click(s)->count < 1) return false;
        if (jo::as_mouse_click(s)->x < l || jo::as_mouse_click(s)->y < t || jo::as_mouse_click(s)->x > r || jo::as_mouse_click(s)->y > b) return false;
        if (jo::self<jo::Input>(this)->cur_x < l || jo::self<jo::Input>(this)->cur_y < t || jo::self<jo::Input>(this)->cur_x > r || jo::self<jo::Input>(this)->cur_y > b) return false;
        return !(time_ms() - jo::as_mouse_click(s)->time < ms);
    }
    i32 held_in(i32 l, i32 t, i32 r, i32 b, i32 which, u32 ms) {
        c_held.hit();
        if (which == 0 || which == 2) {
            if (held(0, l, t, r, b, ms)) return 1;
        }
        if (which == 1 || which == 2) {
            if (held(1, l, t, r, b, ms)) return 2;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x00596210
    H* ctor_596210() {
        joan::orig::as_resource_db(&jo::self<jo::MouseCursor>(this)->res_cursor)->construct();  // 0x00596310
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::MouseCursor)));  // 0x204 on x86
        return this;
    }

    // ------------------------------------------------------------------ 0x00596bd0
    H* ctor_596bd0() { return this; }

};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_10", name, addr, fn)
B("Mouse::clicked_in", 0x00594bd0, &H::clicked_in);
B("Mouse::held_in", 0x00595b10, &H::held_in);
B("Input::ctor_596210", 0x00596210, &H::ctor_596210);
B("Obj::ctor_596bd0", 0x00596bd0, &H::ctor_596bd0);
#undef B

}  // namespace game_b101_10
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04.cpp (game/b86_04)
namespace {
namespace en3d_b86_04 {

using joan::call::cdecl_call;

using joan::img::obj;

using joan::call::thiscall;

std::int32_t add(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}
std::int32_t sub(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b));
}

JOAN_COUNTER(c_input_poll, "Input::poll");

struct H {

    // 0x00594f90
    std::int32_t input_poll() {
        c_input_poll.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->init_flag) == 0 || jo::view<std::int8_t>(jo::self<jo::Input>(this)->inactive) != 0) return 0;
        std::uint8_t mouse_ok = 1;
        std::uint8_t keys_ok = 1;
        namespace pin = joan::platform::input;  // the platform layer's devices (the original: DirectInput's)
        std::int32_t hr = pin::acquire(pin::Device::Mouse);
        if (hr != 0 && hr != 1) mouse_ok = 0;
        hr = pin::acquire(pin::Device::Keyboard);
        if (hr != 0 && hr != 1) keys_ok = 0;
        if (jo::view<std::int8_t>(jo::self<jo::Input>(this)->skey_changed) != 0) {
            joan::orig::as_input(this)->reset_keys();  // 0x00598ac0
            jo::view<std::uint8_t>(jo::self<jo::Input>(this)->skey_changed) = 0;
            jo::self<jo::Input>(this)->prev_skey_state = jo::self<jo::Input>(this)->skey_state;
        }
        std::int32_t moved = 0;
        std::int32_t result = 0;
        jo::InputEvent ev;
        auto send_event = [&](std::int32_t type, const joan::platform::input::Item* rec) {
            jo::view<std::int32_t>(ev.type) = type;
            ev.x = jo::self<jo::Input>(this)->cur_x;
            ev.y = jo::self<jo::Input>(this)->cur_y;
            ev.time = rec->time;
            jo::view<std::int32_t>(ev.scan_code) = 0;
            ev.skey_state = jo::self<jo::Input>(this)->skey_state;
            ev.joy_index = 0;
            ev.joy_arg = 0;
            std::memcpy(&ev.joy_state, &jo::self<jo::Input>(this)->joy_state_0_, sizeof(jo::Pad));
            joan::orig::as_input(this)->push_event(joan::orig::arg::mptr(static_cast<void*>(&ev)));  // 0x00592d00
        };
        // Axis clamp: mode 0 to [lo, hi]; mode 1 inset by |other - (o_lo + o_hi) / 2|.
        auto clamp_axis = [&](std::int32_t& value, const std::int32_t& lo, const std::int32_t& hi, const std::int32_t& other,
                              const std::int32_t& olo, const std::int32_t& ohi) {
            const std::int32_t mode = jo::self<jo::Input>(this)->bound_mode;
            if (mode == 0) {
                if (value < lo) value = lo;
                if (value > hi) value = hi;
            } else if (mode == 1) {
                const std::int32_t mid = add(olo, ohi) / 2;
                const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::libc::abs(joan::orig::arg::i32(sub(other, mid))));  // abs
                if (value < add(lo, r)) value = add(lo, r);
                if (value > sub(hi, r)) value = sub(hi, r);
            }
        };
        for (std::int32_t round = 1; round >= 0; --round) {
            std::uint32_t n_mouse = 200;
            std::uint32_t n_keys = 16;
            if (mouse_ok != 0) {
                hr = pin::read(pin::Device::Mouse, static_cast<pin::Item*>(obj(d::g_a65378)), &n_mouse);
                if (hr != 0 && hr != 1) n_mouse = 0;
            } else {
                n_mouse = 0;
            }
            if (keys_ok != 0) {
                hr = pin::read(pin::Device::Keyboard, static_cast<pin::Item*>(obj(d::g_a66328)), &n_keys);
                if (hr != 0 && hr != 1) n_keys = 0;
            } else {
                n_keys = 0;
            }
            if (n_mouse == 0 && n_keys == 0) break;
            const pin::Item* m = static_cast<const pin::Item*>(obj(d::g_a65378));
            const pin::Item* k = static_cast<const pin::Item*>(obj(d::g_a66328));
            while (n_mouse > 0 || n_keys > 0) {
                std::int32_t source = 0;
                if (n_mouse > 0 && n_keys > 0) {
                    source = sub(static_cast<std::int32_t>(m->sequence), static_cast<std::int32_t>(k->sequence)) > 0 ? 2 : 1;
                } else if (n_mouse > 0) {
                    source = 1;
                } else if (n_keys > 0) {
                    source = 2;
                }
                if (source == 1) {
                    const std::uint32_t data = m->data;
                    switch (m->ofs) {
                        case 0:  // DIMOFS_X
                            jo::self<jo::Input>(this)->delta_x = joan::orig::arg::as<std::int32_t>(joan::orig::as_input(this)->scale_axis(joan::orig::arg::i32(data)));  // 0x00595a70
                            jo::self<jo::Input>(this)->acc_x = add(jo::self<jo::Input>(this)->acc_x, jo::self<jo::Input>(this)->delta_x);
                            jo::self<jo::Input>(this)->cur_x = add(jo::self<jo::Input>(this)->cur_x, joan::optmenu::mouse::cursor_delta(0, jo::self<jo::Input>(this)->delta_x));
                            clamp_axis(jo::self<jo::Input>(this)->cur_x, jo::self<jo::Input>(this)->bound_x1, jo::self<jo::Input>(this)->bound_x2, jo::self<jo::Input>(this)->cur_y, jo::self<jo::Input>(this)->bound_y1, jo::self<jo::Input>(this)->bound_y2);
                            moved = 1;
                            break;
                        case 4:  // DIMOFS_Y
                            jo::self<jo::Input>(this)->delta_y = joan::orig::arg::as<std::int32_t>(joan::orig::as_input(this)->scale_axis(joan::orig::arg::i32(data)));
                            jo::self<jo::Input>(this)->acc_y = add(jo::self<jo::Input>(this)->acc_y, jo::self<jo::Input>(this)->delta_y);
                            jo::self<jo::Input>(this)->cur_y = add(jo::self<jo::Input>(this)->cur_y, joan::optmenu::mouse::cursor_delta(1, jo::self<jo::Input>(this)->delta_y));
                            clamp_axis(jo::self<jo::Input>(this)->cur_y, jo::self<jo::Input>(this)->bound_y1, jo::self<jo::Input>(this)->bound_y2, jo::self<jo::Input>(this)->cur_x, jo::self<jo::Input>(this)->bound_x1, jo::self<jo::Input>(this)->bound_x2);
                            moved = 1;
                            break;
                        case 8:  // DIMOFS_Z
                            jo::self<jo::Input>(this)->delta_z = joan::orig::arg::as<std::int32_t>(joan::orig::as_input(this)->scale_axis(joan::orig::arg::i32(data)));
                            jo::self<jo::Input>(this)->acc_z = add(jo::self<jo::Input>(this)->acc_z, jo::self<jo::Input>(this)->delta_z);
                            jo::self<jo::Input>(this)->cur_z = add(jo::self<jo::Input>(this)->cur_z, jo::self<jo::Input>(this)->delta_z);
                            if (jo::self<jo::Input>(this)->cur_z < jo::self<jo::Input>(this)->bound_z1) jo::self<jo::Input>(this)->cur_z = jo::self<jo::Input>(this)->bound_z1;
                            if (jo::self<jo::Input>(this)->cur_z > jo::self<jo::Input>(this)->bound_z2) jo::self<jo::Input>(this)->cur_z = jo::self<jo::Input>(this)->bound_z2;
                            moved = 1;
                            break;
                        case 0xc:  // button 0
                            if ((data & 0x80) != 0) {
                                jo::self<jo::Input>(this)->left_down = 1;
                                send_event(0, m);
                            } else {
                                jo::self<jo::Input>(this)->left_down = 0;
                                send_event(4, m);
                                joan::orig::as_input(this)->reset_boundary();  // 0x00593f60
                            }
                            result = 1;
                            break;
                        case 0xd:  // button 1
                            if ((data & 0x80) != 0) {
                                jo::self<jo::Input>(this)->right_down = 2;
                                send_event(1, m);
                            } else {
                                jo::self<jo::Input>(this)->right_down = 0;
                                send_event(5, m);
                            }
                            result = 1;
                            break;
                        case 0xe:  // button 2
                            jo::self<jo::Input>(this)->middle_down = (data & 0x80) != 0 ? 4 : 0;
                            result = 1;
                            break;
                        default:
                            break;
                    }
                    --n_mouse;
                    ++m;
                } else if (source == 2) {
                    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_input(this)->key_event(joan::orig::arg::u32(k->ofs), joan::orig::arg::u32(k->data), joan::orig::arg::u32(k->time))) != 0) {  // 0x00598bd0
                        result = 1;
                    }
                    --n_keys;
                    ++k;
                }
            }
            jo::view<std::int32_t>(jo::self<jo::Input>(this)->init_time) = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050());  // 0x00591050
        }
        if (moved != 0) {
            joan::orig::as_mouse_cursor(obj(d::g_mouse_cursor))->move(joan::orig::arg::i32(add(jo::self<jo::Input>(this)->cur_x, jo::self<jo::Input>(this)->saved_x)), joan::orig::arg::i32(add(jo::self<jo::Input>(this)->cur_y, jo::self<jo::Input>(this)->saved_y)));  // 0x004918d0
        }
        return result;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b86_04", name, addr, fn)
GAME("Input::poll", 0x00594f90, &H::input_poll);
#undef GAME

}  // namespace en3d_b86_04
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (game/b85_04)
namespace {
namespace en3d_b85_04 {

using joan::call::thiscall;

struct H {

    // 0x005959c0
    H* parser_drain() {
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_input(this)->poll()) != 0) {  // 0x00594f90
        }
        jo::view<std::uint32_t>(jo::self<jo::Input>(this)->queue_head) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Input>(this)->queue_tail) = 0;
        return this;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b85_04", name, addr, fn)
GAME("Parser::drain", 0x005959c0, &H::parser_drain);
#undef GAME

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;


struct H {

    i32 buttons_state(i32 n) {  // 0x00595aa0
        if ((n == 0 || n == 2) && static_cast<i8>(jo::self<jo::Input>(this)->button_down[0]) != 0) return 1;
        if ((n == 1 || n == 2) && static_cast<i8>(jo::self<jo::Input>(this)->button_down[1]) != 0) return 2;
        return 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Buttons::state", 0x00595aa0, &H::buttons_state);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b104_08.cpp (game/b104_08)
namespace {
namespace game_b104_08 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // 0x00596760
    void* scalar_dtor(u32 flags) {
        joan::orig::as_image(this)->dtor();  // 0x0060fb10
        if ((flags & 1u) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_08", name, addr, fn)
B("Obj::scalar_dtor_596760", 0x00596760, &H::scalar_dtor);
#undef B

}  // namespace game_b104_08
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
namespace {
namespace game_b103_10 {

using i8 = std::int8_t;
using u8 = std::uint8_t;

using u32 = std::uint32_t;


struct H {

    // ------------------------------------------------------------------ 0x00597130 (dead code)
    H* set_50(i8 c, u32 /*unused*/) {
        if (jo::view<i8>(jo::self<jo::MouseCursor>(this)->clip_flag) != c) {
            jo::view<i8>(jo::self<jo::MouseCursor>(this)->clip_flag) = c;
            jo::view<u8>(jo::self<jo::MouseCursor>(this)->dirty_valid) = 0;
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("Obj::set_50", 0x00597130, &H::set_50);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (game/b84_01)
namespace {
namespace en3d_b84_01 {

struct H {
    // 0x00597200 / 0x00597250 / 0x005972e0 / 0x00597350 / 0x00597390
    void empty8(jo::Canvas*, std::uint32_t) {}
    void empty4(jo::Canvas*) {}
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b84_01", name, addr, fn)
GAME("Canvas::callback_597390", 0x00597390, &H::empty4);
GAME("Canvas::callback_597350", 0x00597350, &H::empty8);
GAME("Canvas::callback_5972e0", 0x005972e0, &H::empty8);
GAME("Canvas::callback_597250", 0x00597250, &H::empty8);
GAME("Canvas::callback_597200", 0x00597200, &H::empty8);
#undef GAME

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/game_b104_09.cpp (game/b104_09)
namespace {
namespace game_b104_09 {

using u32 = std::uint32_t;

struct H {
    // ---------------------------------------------------------------- empty stubs
    u32 stub_5973d0(u32 a, u32, u32, u32) { return a; }  // eax on entry = a at the only call
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_09", name, addr, fn)
B("Obj::stub_5973d0", 0x005973d0, &H::stub_5973d0);
#undef B

}  // namespace game_b104_09
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16 {

struct H {
    // ---------------------------------------------------------------- stubs / getters
    void stub0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Obj::stub_597870", 0x00597870, &H::stub0);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_b111_14.cpp (fix/uninit)
namespace {
namespace game_b111_14 {

using joan::call::cdecl_call;

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_gauge, "Gauge::compare_597f70");

struct H {
    // 0x00597f70 (fix/uninit)
    i32 gauge_compare(i32 axis) {
        c_gauge.hit();
        i32 lo = 0;  // [ebp-8]: FIX - unwritten by 0x00597e00 for an unmapped axis
        i32 hi = 0;  // [ebp-0xc]: same
        joan::orig::as_pad(this)->axis_range(joan::orig::arg::i32(axis), &lo, &hi);  // 0x00597e00
        const i32 mid = static_cast<i32>(static_cast<u32>(lo) + static_cast<u32>(hi)) / 2;
        const i32 t_lo = static_cast<i32>(
            static_cast<u32>(mid) +
            static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::mul_div(static_cast<i32>(static_cast<u32>(lo) - static_cast<u32>(mid)), i32{13}, i32{100}))));  // 0x00592170
        const i32 t_hi = static_cast<i32>(
            static_cast<u32>(mid) +
            static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::mul_div(static_cast<i32>(static_cast<u32>(hi) - static_cast<u32>(mid)), i32{13}, i32{100}))));
        if (joan::orig::arg::as<i32>(joan::orig::as_pad(this)->axis(joan::orig::arg::i32(axis))) < t_lo) return -1;  // 0x00597c80
        if (joan::orig::arg::as<i32>(joan::orig::as_pad(this)->axis(joan::orig::arg::i32(axis))) > t_hi) return 1;
        return 0;
    }
};

// fix/uninit: 0x00597e00 range outs unwritten for an unmapped axis (start at 0 -> centred).
JOAN_REPLACE("fix/uninit", "Gauge::compare", 0x00597f70, &H::gauge_compare);

}  // namespace game_b111_14
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using i32 = std::int32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_direction, "Input::direction_598040");

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x00598040
    i32 direction_598040() {
        c_direction.hit();
        i32 a = joan::orig::arg::as<i32>(joan::orig::as_pad(this)->axis_direction(0));  // 0x00597f70
        i32 b = joan::orig::arg::as<i32>(joan::orig::as_pad(this)->axis_direction(1));
        if (a == 0 && b == 0) {
            a = joan::orig::arg::as<i32>(joan::orig::as_pad(this)->axis_direction(4));
            b = joan::orig::arg::as<i32>(joan::orig::as_pad(this)->axis_direction(5));
        }
        if (a == -1) {
            if (b == -1) return 0xa;
            if (b == 1) return 0xc;
            return 0x1c;
        }
        if (a == 1) {
            if (b == -1) return 0xb;
            if (b == 1) return 0xd;
            return 0x22;
        }
        if (b == -1) return 0x25;
        if (b == 1) return 0x14;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Input::direction_598040", 0x00598040, &H::direction_598040);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b106_14.cpp (game/b106_14)
namespace {
namespace game_b106_14 {

using joan::call::cdecl_call;

using u8 = std::uint8_t;

using u16 = std::uint16_t;

using u32 = std::uint32_t;

JOAN_COUNTER(c_translate, "Keyboard::translate");

// ---------------------------------------------------------------- 0x00599240 table
struct Key {
    u16 lower, upper, alt;
    u8 numpad;
};
// Decoded from the switch at 0x599289 (index = scan code - 1): [ebp-4], [ebp-8], [ebp-0xc],
// [ebp-0x10]. Codes outside 1..0xd3 keep all four at 0.
constexpr Key kKeys[0xd3] = {
    {0x001b, 0x001b, 0x001b, 0},  // 1 (0x01)
    {0x0031, 0x0021, 0x0031, 0},  // 2 (0x02)
    {0x0032, 0x0040, 0x0032, 0},  // 3 (0x03)
    {0x0033, 0x0023, 0x0033, 0},  // 4 (0x04)
    {0x0034, 0x0024, 0x0034, 0},  // 5 (0x05)
    {0x0035, 0x0025, 0x0035, 0},  // 6 (0x06)
    {0x0036, 0x005e, 0x0036, 0},  // 7 (0x07)
    {0x0037, 0x0026, 0x0037, 0},  // 8 (0x08)
    {0x0038, 0x002a, 0x0038, 0},  // 9 (0x09)
    {0x0039, 0x0028, 0x0039, 0},  // 10 (0x0a)
    {0x0030, 0x0029, 0x0030, 0},  // 11 (0x0b)
    {0x002d, 0x005f, 0x002d, 0},  // 12 (0x0c)
    {0x003d, 0x002b, 0x003d, 0},  // 13 (0x0d)
    {0x0008, 0x0008, 0x0008, 0},  // 14 (0x0e)
    {0x0009, 0x0009, 0x0009, 0},  // 15 (0x0f)
    {0x0071, 0x0051, 0x0051, 0},  // 16 (0x10)
    {0x0077, 0x0057, 0x0057, 0},  // 17 (0x11)
    {0x0065, 0x0045, 0x0045, 0},  // 18 (0x12)
    {0x0072, 0x0052, 0x0052, 0},  // 19 (0x13)
    {0x0074, 0x0054, 0x0054, 0},  // 20 (0x14)
    {0x0079, 0x0059, 0x0059, 0},  // 21 (0x15)
    {0x0075, 0x0055, 0x0055, 0},  // 22 (0x16)
    {0x0069, 0x0049, 0x0049, 0},  // 23 (0x17)
    {0x006f, 0x004f, 0x004f, 0},  // 24 (0x18)
    {0x0070, 0x0050, 0x0050, 0},  // 25 (0x19)
    {0x005b, 0x007b, 0x005b, 0},  // 26 (0x1a)
    {0x005d, 0x007d, 0x005d, 0},  // 27 (0x1b)
    {0x000d, 0x000d, 0x000d, 0},  // 28 (0x1c)
    {0x0000, 0x0000, 0x0000, 0},  // 29 (0x1d)
    {0x0061, 0x0041, 0x0041, 0},  // 30 (0x1e)
    {0x0073, 0x0053, 0x0053, 0},  // 31 (0x1f)
    {0x0064, 0x0044, 0x0044, 0},  // 32 (0x20)
    {0x0066, 0x0046, 0x0046, 0},  // 33 (0x21)
    {0x0067, 0x0047, 0x0047, 0},  // 34 (0x22)
    {0x0068, 0x0048, 0x0048, 0},  // 35 (0x23)
    {0x006a, 0x004a, 0x004a, 0},  // 36 (0x24)
    {0x006b, 0x004b, 0x004b, 0},  // 37 (0x25)
    {0x006c, 0x004c, 0x004c, 0},  // 38 (0x26)
    {0x003b, 0x003a, 0x003b, 0},  // 39 (0x27)
    {0x0027, 0x0022, 0x0027, 0},  // 40 (0x28)
    {0x0060, 0x007e, 0x0060, 0},  // 41 (0x29)
    {0x0000, 0x0000, 0x0000, 0},  // 42 (0x2a)
    {0x005c, 0x007c, 0x005c, 0},  // 43 (0x2b)
    {0x007a, 0x005a, 0x005a, 0},  // 44 (0x2c)
    {0x0078, 0x0058, 0x0058, 0},  // 45 (0x2d)
    {0x0063, 0x0043, 0x0043, 0},  // 46 (0x2e)
    {0x0076, 0x0056, 0x0056, 0},  // 47 (0x2f)
    {0x0062, 0x0042, 0x0042, 0},  // 48 (0x30)
    {0x006e, 0x004e, 0x004e, 0},  // 49 (0x31)
    {0x006d, 0x004d, 0x004d, 0},  // 50 (0x32)
    {0x002c, 0x003c, 0x002c, 0},  // 51 (0x33)
    {0x002e, 0x003e, 0x002e, 0},  // 52 (0x34)
    {0x002f, 0x003f, 0x002f, 0},  // 53 (0x35)
    {0x0000, 0x0000, 0x0000, 0},  // 54 (0x36)
    {0x002a, 0x002a, 0x002a, 1},  // 55 (0x37)
    {0x0000, 0x0000, 0x0000, 0},  // 56 (0x38)
    {0x0020, 0x0020, 0x0020, 0},  // 57 (0x39)
    {0x0000, 0x0000, 0x0000, 0},  // 58 (0x3a)
    {0x3b00, 0x3b00, 0x3b00, 0},  // 59 (0x3b)
    {0x3c00, 0x3c00, 0x3c00, 0},  // 60 (0x3c)
    {0x3d00, 0x3d00, 0x3d00, 0},  // 61 (0x3d)
    {0x3e00, 0x3e00, 0x3e00, 0},  // 62 (0x3e)
    {0x3f00, 0x3f00, 0x3f00, 0},  // 63 (0x3f)
    {0x4000, 0x4000, 0x4000, 0},  // 64 (0x40)
    {0x4100, 0x4100, 0x4100, 0},  // 65 (0x41)
    {0x4200, 0x4200, 0x4200, 0},  // 66 (0x42)
    {0x4300, 0x4300, 0x4300, 0},  // 67 (0x43)
    {0x4400, 0x4400, 0x4400, 0},  // 68 (0x44)
    {0x0000, 0x0000, 0x0000, 0},  // 69 (0x45)
    {0x0000, 0x0000, 0x0000, 0},  // 70 (0x46)
    {0x0037, 0x0037, 0x0037, 1},  // 71 (0x47)
    {0x0038, 0x0038, 0x0038, 1},  // 72 (0x48)
    {0x0039, 0x0039, 0x0039, 1},  // 73 (0x49)
    {0x002d, 0x002d, 0x002d, 1},  // 74 (0x4a)
    {0x0034, 0x0034, 0x0034, 1},  // 75 (0x4b)
    {0x0035, 0x0035, 0x0035, 1},  // 76 (0x4c)
    {0x0036, 0x0036, 0x0036, 1},  // 77 (0x4d)
    {0x002b, 0x002b, 0x002b, 1},  // 78 (0x4e)
    {0x0031, 0x0031, 0x0031, 1},  // 79 (0x4f)
    {0x0032, 0x0032, 0x0032, 1},  // 80 (0x50)
    {0x0033, 0x0033, 0x0033, 1},  // 81 (0x51)
    {0x0030, 0x0030, 0x0030, 1},  // 82 (0x52)
    {0x002e, 0x002e, 0x002e, 1},  // 83 (0x53)
    {0x0000, 0x0000, 0x0000, 0},  // 84 (0x54)
    {0x0000, 0x0000, 0x0000, 0},  // 85 (0x55)
    {0x0000, 0x0000, 0x0000, 0},  // 86 (0x56)
    {0x5700, 0x5700, 0x5700, 0},  // 87 (0x57)
    {0x5800, 0x5800, 0x5800, 0},  // 88 (0x58)
    {0x0000, 0x0000, 0x0000, 0},  // 89 (0x59)
    {0x0000, 0x0000, 0x0000, 0},  // 90 (0x5a)
    {0x0000, 0x0000, 0x0000, 0},  // 91 (0x5b)
    {0x0000, 0x0000, 0x0000, 0},  // 92 (0x5c)
    {0x0000, 0x0000, 0x0000, 0},  // 93 (0x5d)
    {0x0000, 0x0000, 0x0000, 0},  // 94 (0x5e)
    {0x0000, 0x0000, 0x0000, 0},  // 95 (0x5f)
    {0x0000, 0x0000, 0x0000, 0},  // 96 (0x60)
    {0x0000, 0x0000, 0x0000, 0},  // 97 (0x61)
    {0x0000, 0x0000, 0x0000, 0},  // 98 (0x62)
    {0x0000, 0x0000, 0x0000, 0},  // 99 (0x63)
    {0x0000, 0x0000, 0x0000, 0},  // 100 (0x64)
    {0x0000, 0x0000, 0x0000, 0},  // 101 (0x65)
    {0x0000, 0x0000, 0x0000, 0},  // 102 (0x66)
    {0x0000, 0x0000, 0x0000, 0},  // 103 (0x67)
    {0x0000, 0x0000, 0x0000, 0},  // 104 (0x68)
    {0x0000, 0x0000, 0x0000, 0},  // 105 (0x69)
    {0x0000, 0x0000, 0x0000, 0},  // 106 (0x6a)
    {0x0000, 0x0000, 0x0000, 0},  // 107 (0x6b)
    {0x0000, 0x0000, 0x0000, 0},  // 108 (0x6c)
    {0x0000, 0x0000, 0x0000, 0},  // 109 (0x6d)
    {0x0000, 0x0000, 0x0000, 0},  // 110 (0x6e)
    {0x0000, 0x0000, 0x0000, 0},  // 111 (0x6f)
    {0x0000, 0x0000, 0x0000, 0},  // 112 (0x70)
    {0x0000, 0x0000, 0x0000, 0},  // 113 (0x71)
    {0x0000, 0x0000, 0x0000, 0},  // 114 (0x72)
    {0x0000, 0x0000, 0x0000, 0},  // 115 (0x73)
    {0x0000, 0x0000, 0x0000, 0},  // 116 (0x74)
    {0x0000, 0x0000, 0x0000, 0},  // 117 (0x75)
    {0x0000, 0x0000, 0x0000, 0},  // 118 (0x76)
    {0x0000, 0x0000, 0x0000, 0},  // 119 (0x77)
    {0x0000, 0x0000, 0x0000, 0},  // 120 (0x78)
    {0x0000, 0x0000, 0x0000, 0},  // 121 (0x79)
    {0x0000, 0x0000, 0x0000, 0},  // 122 (0x7a)
    {0x0000, 0x0000, 0x0000, 0},  // 123 (0x7b)
    {0x0000, 0x0000, 0x0000, 0},  // 124 (0x7c)
    {0x0000, 0x0000, 0x0000, 0},  // 125 (0x7d)
    {0x0000, 0x0000, 0x0000, 0},  // 126 (0x7e)
    {0x0000, 0x0000, 0x0000, 0},  // 127 (0x7f)
    {0x0000, 0x0000, 0x0000, 0},  // 128 (0x80)
    {0x0000, 0x0000, 0x0000, 0},  // 129 (0x81)
    {0x0000, 0x0000, 0x0000, 0},  // 130 (0x82)
    {0x0000, 0x0000, 0x0000, 0},  // 131 (0x83)
    {0x0000, 0x0000, 0x0000, 0},  // 132 (0x84)
    {0x0000, 0x0000, 0x0000, 0},  // 133 (0x85)
    {0x0000, 0x0000, 0x0000, 0},  // 134 (0x86)
    {0x0000, 0x0000, 0x0000, 0},  // 135 (0x87)
    {0x0000, 0x0000, 0x0000, 0},  // 136 (0x88)
    {0x0000, 0x0000, 0x0000, 0},  // 137 (0x89)
    {0x0000, 0x0000, 0x0000, 0},  // 138 (0x8a)
    {0x0000, 0x0000, 0x0000, 0},  // 139 (0x8b)
    {0x0000, 0x0000, 0x0000, 0},  // 140 (0x8c)
    {0x003d, 0x003d, 0x003d, 0},  // 141 (0x8d)
    {0x0000, 0x0000, 0x0000, 0},  // 142 (0x8e)
    {0x0000, 0x0000, 0x0000, 0},  // 143 (0x8f)
    {0x0000, 0x0000, 0x0000, 0},  // 144 (0x90)
    {0x0040, 0x0040, 0x0040, 0},  // 145 (0x91)
    {0x003a, 0x003a, 0x003a, 0},  // 146 (0x92)
    {0x005f, 0x005f, 0x005f, 0},  // 147 (0x93)
    {0x0000, 0x0000, 0x0000, 0},  // 148 (0x94)
    {0x0000, 0x0000, 0x0000, 0},  // 149 (0x95)
    {0x0000, 0x0000, 0x0000, 0},  // 150 (0x96)
    {0x0000, 0x0000, 0x0000, 0},  // 151 (0x97)
    {0x0000, 0x0000, 0x0000, 0},  // 152 (0x98)
    {0x0000, 0x0000, 0x0000, 0},  // 153 (0x99)
    {0x0000, 0x0000, 0x0000, 0},  // 154 (0x9a)
    {0x0000, 0x0000, 0x0000, 0},  // 155 (0x9b)
    {0x000d, 0x000d, 0x000d, 1},  // 156 (0x9c)
    {0x0000, 0x0000, 0x0000, 0},  // 157 (0x9d)
    {0x0000, 0x0000, 0x0000, 0},  // 158 (0x9e)
    {0x0000, 0x0000, 0x0000, 0},  // 159 (0x9f)
    {0x0000, 0x0000, 0x0000, 0},  // 160 (0xa0)
    {0x0000, 0x0000, 0x0000, 0},  // 161 (0xa1)
    {0x0000, 0x0000, 0x0000, 0},  // 162 (0xa2)
    {0x0000, 0x0000, 0x0000, 0},  // 163 (0xa3)
    {0x0000, 0x0000, 0x0000, 0},  // 164 (0xa4)
    {0x0000, 0x0000, 0x0000, 0},  // 165 (0xa5)
    {0x0000, 0x0000, 0x0000, 0},  // 166 (0xa6)
    {0x0000, 0x0000, 0x0000, 0},  // 167 (0xa7)
    {0x0000, 0x0000, 0x0000, 0},  // 168 (0xa8)
    {0x0000, 0x0000, 0x0000, 0},  // 169 (0xa9)
    {0x0000, 0x0000, 0x0000, 0},  // 170 (0xaa)
    {0x0000, 0x0000, 0x0000, 0},  // 171 (0xab)
    {0x0000, 0x0000, 0x0000, 0},  // 172 (0xac)
    {0x0000, 0x0000, 0x0000, 0},  // 173 (0xad)
    {0x0000, 0x0000, 0x0000, 0},  // 174 (0xae)
    {0x0000, 0x0000, 0x0000, 0},  // 175 (0xaf)
    {0x0000, 0x0000, 0x0000, 0},  // 176 (0xb0)
    {0x0000, 0x0000, 0x0000, 0},  // 177 (0xb1)
    {0x0000, 0x0000, 0x0000, 0},  // 178 (0xb2)
    {0x002c, 0x002c, 0x002c, 0},  // 179 (0xb3)
    {0x0000, 0x0000, 0x0000, 0},  // 180 (0xb4)
    {0x002f, 0x002f, 0x002f, 1},  // 181 (0xb5)
    {0x0000, 0x0000, 0x0000, 0},  // 182 (0xb6)
    {0x0000, 0x0000, 0x0000, 0},  // 183 (0xb7)
    {0x0000, 0x0000, 0x0000, 0},  // 184 (0xb8)
    {0x0000, 0x0000, 0x0000, 0},  // 185 (0xb9)
    {0x0000, 0x0000, 0x0000, 0},  // 186 (0xba)
    {0x0000, 0x0000, 0x0000, 0},  // 187 (0xbb)
    {0x0000, 0x0000, 0x0000, 0},  // 188 (0xbc)
    {0x0000, 0x0000, 0x0000, 0},  // 189 (0xbd)
    {0x0000, 0x0000, 0x0000, 0},  // 190 (0xbe)
    {0x0000, 0x0000, 0x0000, 0},  // 191 (0xbf)
    {0x0000, 0x0000, 0x0000, 0},  // 192 (0xc0)
    {0x0000, 0x0000, 0x0000, 0},  // 193 (0xc1)
    {0x0000, 0x0000, 0x0000, 0},  // 194 (0xc2)
    {0x0000, 0x0000, 0x0000, 0},  // 195 (0xc3)
    {0x0000, 0x0000, 0x0000, 0},  // 196 (0xc4)
    {0x0000, 0x0000, 0x0000, 0},  // 197 (0xc5)
    {0x0000, 0x0000, 0x0000, 0},  // 198 (0xc6)
    {0x4700, 0x4700, 0x4700, 0},  // 199 (0xc7)
    {0x4800, 0x4800, 0x4800, 0},  // 200 (0xc8)
    {0x4900, 0x4900, 0x4900, 0},  // 201 (0xc9)
    {0x0000, 0x0000, 0x0000, 0},  // 202 (0xca)
    {0x4b00, 0x4b00, 0x4b00, 0},  // 203 (0xcb)
    {0x0000, 0x0000, 0x0000, 0},  // 204 (0xcc)
    {0x4d00, 0x4d00, 0x4d00, 0},  // 205 (0xcd)
    {0x0000, 0x0000, 0x0000, 0},  // 206 (0xce)
    {0x4f00, 0x4f00, 0x4f00, 0},  // 207 (0xcf)
    {0x5000, 0x5000, 0x5000, 0},  // 208 (0xd0)
    {0x5100, 0x5100, 0x5100, 0},  // 209 (0xd1)
    {0x5200, 0x5200, 0x5200, 0},  // 210 (0xd2)
    {0x5300, 0x5300, 0x5300, 0},  // 211 (0xd3)
};

// 0x00599240
u32 JOAN_CDECL keyboard_translate(u32 code, u32 mods, u32 wanted, u32 flags) {
    c_translate.hit();
    Key k{0, 0, 0, 0};
    if (code - 1u <= 0xd2u) k = kKeys[code - 1u];
    const u32 m = mods & 0xffffu;
    bool ok = true;
    if (!(flags & 0x200)) {
        if (flags & 2) {
            if (!(m & 3)) ok = false;
        } else if (m & 3) {
            ok = false;
        }
    }
    if (!(flags & 0x400)) {
        if (flags & 4) {
            if (!(m & 0xc)) ok = false;
        } else if (m & 0xc) {
            ok = false;
        }
    }
    if (!(flags & 0x800)) {
        if (flags & 8) {
            if (!(m & 0x30)) ok = false;
        } else if (m & 0x30) {
            ok = false;
        }
    }
    if (!(flags & 0x4000)) {
        if (flags & 0x10) {
            if (k.numpad == 0) ok = false;
        } else if (k.numpad != 0) {
            ok = false;
        }
    }
    u32 r = k.lower;
    if (flags & 0x8000) {
        if (k.lower != k.alt) {
            if (m & 0x80)
                r = (m & 3) ? k.lower : k.alt;
            else
                r = (m & 3) ? k.upper : k.lower;
        } else {
            r = (m & 3) ? k.upper : k.lower;
        }
    }
    if (!ok) return 0;
    const u32 w = wanted & 0xffffu;
    bool match = false;
    if (w == 0 || r == w) {
        match = true;
    } else if ((flags & 0x200) && k.upper == w) {
        match = true;
    } else if ((flags & 0x2000) && k.alt == w) {
        match = true;
    } else if ((flags & 0x100) && r == (joan::orig::arg::as<u32>(joan::orig::libc::tolower(joan::orig::arg::i32(w))) & 0xffffu)) {
        match = true;
    }
    return match ? r : 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b106_14", name, addr, fn)
B("Keyboard::translate", 0x00599240, &keyboard_translate);
#undef B

}  // namespace game_b106_14
}  // namespace

// ---------------------------------------- from modules/game_b107_19.cpp (game/b107_19)
namespace {
namespace game_b107_19 {

using joan::call::cdecl_call;

using i16 = std::int16_t;

using i8 = std::int8_t;
using i32 = std::int32_t;

using joan::img::str;

using u16 = std::uint16_t;

using u32 = std::uint32_t;

JOAN_COUNTER(c_key_name, "Input::key_name_matches_59a540");

// ---- 0x0059a540 ----
u32 JOAN_CDECL key_name_matches_59a540(u32 key, u32 mods, const char* name, u32 flags) {
    c_key_name.hit();
    const u32 len = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(name)));  // strlen
    if (len == 0) return 0;
    if (len == 1) {
        const u32 ch = static_cast<u16>(static_cast<i16>(static_cast<i8>(name[0])));
        const u32 m = (flags & 0xffff0000u) | (mods & 0xffffu);
        return joan::orig::arg::as<u32>(joan::orig::keyboard::translate(joan::orig::arg::u32(key), joan::orig::arg::u32(m), joan::orig::arg::u32(ch), joan::orig::arg::u32(flags)));  // 0x00599240
    }
    std::uintptr_t a = 0;  // [ebp-0x8]
    std::uintptr_t b = 0;  // [ebp-0xc]
    i32 special = 0;       // [ebp-0x10]
    switch (key) {
        case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44:  // F1..F10
            a = b = img::addr(d::s_F1 - 4 * (key - 0x3b));
            break;
        case 0x57: a = b = img::addr(d::s_F11); break;  // F11
        case 0x58: a = b = img::addr(d::s_F12); break;  // F12
        case 0x47: a = img::addr(d::s_HOME); b = img::addr(d::s_7); special = 1; break;
        case 0x48: a = img::addr(d::s_UP); b = img::addr(d::s_8); special = 1; break;
        case 0x49: a = img::addr(d::s_PAGE_UP); b = img::addr(d::s_9); special = 1; break;
        case 0x4b: a = img::addr(d::s_LEFT); b = img::addr(d::s_4); special = 1; break;
        case 0x4c: a = img::addr(d::s_CENTER); b = img::addr(d::s_5); special = 1; break;
        case 0x4d: a = img::addr(d::s_RIGHT); b = img::addr(d::s_6); special = 1; break;
        case 0x4f: a = img::addr(d::s_END); b = img::addr(d::s_1_9d8de4); special = 1; break;
        case 0x50: a = img::addr(d::s_DOWN); b = img::addr(d::s_2_9d8de0); special = 1; break;
        case 0x51: a = img::addr(d::s_PAGE_DOWN); b = img::addr(d::s_3_9d8dd0); special = 1; break;
        case 0x52: a = img::addr(d::s_INSERT); b = img::addr(d::s_0); special = 1; break;
        case 0x53: a = img::addr(d::s_DELETE); b = img::addr(d::s_dot_9d36e8); special = 1; break;
        case 0xc7: a = b = img::addr(d::s_HOME); break;  // HOME
        case 0xc8: a = b = img::addr(d::s_UP); break;  // UP
        case 0xc9: a = b = img::addr(d::s_PAGE_UP); break;  // PAGE UP
        case 0xcb: a = b = img::addr(d::s_LEFT); break;  // LEFT
        case 0xcd: a = b = img::addr(d::s_RIGHT); break;  // RIGHT
        case 0xcf: a = b = img::addr(d::s_END); break;  // END
        case 0xd0: a = b = img::addr(d::s_DOWN); break;  // DOWN
        case 0xd1: a = b = img::addr(d::s_PAGE_DOWN); break;  // PAGE DOWN
        case 0xd2: a = b = img::addr(d::s_INSERT); break;  // INSERT
        case 0xd3: a = b = img::addr(d::s_DELETE); break;  // DELETE
        case 0xdb: a = b = img::addr(d::s_LEFT_WINDOW); break;  // LEFT WINDOW
        case 0xdc: a = b = img::addr(d::s_RIGHT_WINDOW); break;  // RIGHT WINDOW
        case 0xdd: a = b = img::addr(d::s_APP_MENU); break;  // APP MENU
        default: break;
    }
    const u32 m = mods & 0xffffu;
    i32 ok = 1;  // [ebp-0x14]
    if ((flags & 0x200) == 0) {
        if (flags & 0x2) {
            if ((m & 0x3) == 0) ok = 0;
        } else if ((m & 0x3) != 0) {
            ok = 0;
        }
    }
    if ((flags & 0x400) == 0) {
        if (flags & 0x4) {
            if ((m & 0xc) == 0) ok = 0;
        } else if ((m & 0xc) != 0) {
            ok = 0;
        }
    }
    if ((flags & 0x800) == 0) {
        if (flags & 0x8) {
            if ((m & 0x30) == 0) ok = 0;
        } else if ((m & 0x30) != 0) {
            ok = 0;
        }
    }
    if ((flags & 0x4000) == 0) {
        if (flags & 0x10) {
            if (special == 0) ok = 0;
        } else if (special != 0) {
            ok = 0;
        }
    }
    const std::uintptr_t s = (m & 0x40) ? b : a;
    i32 match = 0;
    if (s != 0) match = std::strcmp(joan::orig::arg::str(str(s)), joan::orig::arg::str(name)) == 0 ? 1 : 0;  // strcmp
    return (ok != 0 && match != 0) ? 1u : 0u;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_19", name, addr, fn)
B("Input::key_name_matches_59a540", 0x0059a540, &key_name_matches_59a540);
#undef B

}  // namespace game_b107_19
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MouseCursor::load() -> void { joan::bind::member<&::game_b94_02::H::cursor_load>(this); }  // 0x4915f0
auto joan::orig::MouseCursor::init_493eb0(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_19::H::cursor_init>(this, a0)); }  // 0x493eb0
auto joan::orig::MouseCursor::release_4941b0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b105_05::H::release_4941b0>(this)); }  // 0x4941b0
auto joan::orig::Input::init_592380(void* a0, void* a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b108_01::H::input_init>(this, a0, a1, a2)); }  // 0x592380
auto joan::orig::Input::release() -> void { joan::bind::member<&::game_b92_04::H::input_release>(this); }  // 0x592830
auto joan::orig::Input::unacquire_all() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_10::H::unacquire_all>(this)); }  // 0x5929f0
auto joan::orig::Sound::pause_all() -> void { joan::bind::member<&::game_b97_05::H::pause_all>(this); }  // 0x592ae0
auto joan::orig::Cursor::set_pos() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b83_08::H::cursor_set>(this)); }  // 0x592bd0
auto joan::orig::Cursor::reset_pos() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b82_17::H::cursor_reset>(this)); }  // 0x592c30
auto joan::orig::Input::lock_area(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_15::H::forward_5973d0>(this, a0, a1, a2, a3)); }  // 0x592c90
auto joan::orig::Input::push_event(void* a0) -> void { joan::bind::member<&::en3d_b87_02::H::push_event>(this, a0); }  // 0x592d00
auto joan::orig::Input::push_event2(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b88_04::H::push2>(this, a0, a1)); }  // 0x592dc0
auto joan::orig::Input::push_event6(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b88_04::H::push6>(this, a0, a1)); }  // 0x592f00
auto joan::orig::Input::push_event3(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b94_05::H::log_push>(this, a0, a1, a2); }  // 0x593040
auto joan::orig::Input::cursor_in_triangle(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_03::H::forward>(this, a0, a1, a2, a3, a4, a5)); }  // 0x5939f0
auto joan::orig::Parser::rect_mode(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::rect_mode>(this, a0, a1, a2, a3, a4)); }  // 0x5940f0
auto joan::orig::Joystick::in_rect(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_03::H::in_rect>(this, a0, a1, a2, a3, a4)); }  // 0x594360
auto joan::orig::Input::double_click_in(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::int32_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_05::H::try_slots>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x594490
auto joan::orig::Parser::rect_value(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::rect_value>(this, a0, a1, a2, a3, a4)); }  // 0x5945d0
auto joan::orig::Parser::value(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::value>(this, a0)); }  // 0x594700
auto joan::orig::Input::click_count_in(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::int32_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_05::H::play_5947c0>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x5947c0
auto joan::orig::Parser::rect_mode2(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::rect_mode2>(this, a0, a1, a2, a3, a4)); }  // 0x594900
auto joan::orig::Joystick::pressed_kind(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_03::H::pressed_kind>(this, a0)); }  // 0x594a10
auto joan::orig::Input::region_hit(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_05::H::region_hit>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x594ab0
auto joan::orig::Input::double_click(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_04::H::double_click>(this, a0)); }  // 0x594d30
auto joan::orig::Input::poll() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b86_04::H::input_poll>(this)); }  // 0x594f90
auto joan::orig::Parser::drain() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b85_04::H::parser_drain>(this)); }  // 0x5959c0
auto joan::orig::Parser::reset() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_04::H::parser_reset>(this)); }  // 0x595a20
auto joan::orig::Input::scale_axis(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b87_02::H::scale_axis>(this, a0)); }  // 0x595a70
auto joan::orig::Input::button_down_state(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_03::H::buttons_state>(this, a0)); }  // 0x595aa0
auto joan::orig::Key::repeat_count() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_03::H::repeat_count>(this)); }  // 0x595fb0
auto joan::orig::Triangle::contains(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b83_08::H::triangle_contains>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x596140
auto joan::orig::ResourceDb::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_10::H::ctor_596310>(this)); }  // 0x596310
auto joan::orig::Image::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b104_08::H::scalar_dtor>(this, a0)); }  // 0x596760
auto joan::orig::VgaBuf::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_07::H::scalar_dtor_596b30>(this, a0)); }  // 0x596b30
auto joan::orig::Cursor::set_596db0(std::int32_t a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b108_01::H::cursor_set>(this, a0)); }  // 0x596db0
auto joan::orig::Canvas::callback_597200(Canvas* a0, std::uint32_t a1) -> void { joan::bind::member<&::en3d_b84_01::H::empty8>(this, a0, a1); }  // 0x597200
auto joan::orig::Canvas::callback_597250(Canvas* a0, std::uint32_t a1) -> void { joan::bind::member<&::en3d_b84_01::H::empty8>(this, a0, a1); }  // 0x597250
auto joan::orig::Canvas::callback_5972e0(Canvas* a0, std::uint32_t a1) -> void { joan::bind::member<&::en3d_b84_01::H::empty8>(this, a0, a1); }  // 0x5972e0
auto joan::orig::Canvas::callback_597350(Canvas* a0, std::uint32_t a1) -> void { joan::bind::member<&::en3d_b84_01::H::empty8>(this, a0, a1); }  // 0x597350
auto joan::orig::Canvas::callback_597390(Canvas* a0) -> void { joan::bind::member<&::en3d_b84_01::H::empty4>(this, a0); }  // 0x597390
auto joan::orig::Input::lock_area_nop(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_09::H::stub_5973d0>(this, a0, a1, a2, a3)); }  // 0x5973d0
auto joan::orig::Bitmap::width() -> std::uint16_t { return joan::bind::conv<std::uint16_t>(joan::bind::member<&::game_b91_03::H::word0>(this)); }  // 0x5979a0
auto joan::orig::Bitmap::height() -> std::uint16_t { return joan::bind::conv<std::uint16_t>(joan::bind::member<&::game_b91_03::H::word2>(this)); }  // 0x5979c0
auto joan::orig::Pad::axis(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_03::H::axis>(this, a0)); }  // 0x597c80
auto joan::orig::Pad::axis_range(std::int32_t a0, void* a1, void* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_03::H::axis_range>(this, a0, a1, a2)); }  // 0x597e00
auto joan::orig::Ai::is_ready(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b90_04::H::is_ready>(this, a0)); }  // 0x597ed0
auto joan::orig::Pad::axis_direction(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b111_14::H::gauge_compare>(this, a0)); }  // 0x597f70
auto joan::orig::Input::direction_598040() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_08::H::direction_598040>(this)); }  // 0x598040
auto joan::orig::same16(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_03::same16>(a0, a1)); }  // 0x598700
auto joan::orig::Input::poll_joysticks(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_17::H::poll_joysticks>(this, a0)); }  // 0x598730
auto joan::orig::Input::reset_keys_thunk() -> void { joan::bind::member<&::game_b98_05::H::call_598ac0>(this); }  // 0x598a90
auto joan::orig::Input::reset_keys() -> void { joan::bind::member<&::en3d_b87_02::H::reset_keys>(this); }  // 0x598ac0
auto joan::orig::Input::key_event(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b87_02::H::key_event>(this, a0, a1, a2)); }  // 0x598bd0
auto joan::orig::keyboard::translate(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b106_14::keyboard_translate>(a0, a1, a2, a3)); }  // 0x599240
