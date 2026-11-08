// en2d/image2d.cpp: functions of the original d:\ww\en2d\image2d.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include <cmath>
#include <limits>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;


}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
//
//      0x00629830 Image2d::release()                            thiscall, returns 1
namespace {
namespace game_b92_05 {

using joan::img::obj;
using joan::img::str;

void* const kMem = obj(d::g_mem);

// Memory manager free / realloc with source file and line (0x0058ed70 / 0x0058eb30).
void mem_free(void* p, img::Addr file, std::int32_t line) {
    joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}
using joan::orig::helpers::line_base;

struct H {
    // 0x00629830
    std::int32_t image_release() {
        jo::self<jo::Image2D>(this)->sprite_set = nullptr;
        jo::view<std::uint8_t>(jo::self<jo::Image2D>(this)->sprite_name[0]) = 0;
        if (jo::self<jo::Image2D>(this)->text != nullptr) {
            mem_free(jo::self<jo::Image2D>(this)->text, d::s_d_ww_en2d_image2d_cpp, line_base(d::g_a19758) + 6);  // 'd:\ww\en2d\image2d.cpp'
            jo::self<jo::Image2D>(this)->text = nullptr;
        }
        jo::self<jo::Image2D>(this)->active = 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("Image2d::release", 0x00629830, &H::image_release);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
//
//    - Image2D::set_name (0x006298c0, en2d/image2d.cpp): reallocates and copies the name.
namespace {
namespace game_leaves80b {

constexpr img::Addr kMem(d::g_mem);

JOAN_COUNTER(c_set_name, "Image2D::set_name");

struct H {
    // 0x006298c0
    void image_set_name(const char* name) {
        c_set_name.hit();
        if (name == nullptr) return;
        if (joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(name))) == 0) return;  // strlen
        const char* const file = &img::global<const char>(d::s_d_ww_en2d_image2d_cpp);  // "d:\ww\en2d\image2d.cpp"
        if (jo::view<void*>(jo::self<jo::Image2D>(this)->text) != nullptr) {
            joan::orig::as_mem(img::obj(kMem))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Image2D>(this)->text)), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a1975c)) + 9));  // 0x0058ed70 free
            jo::view<void*>(jo::self<jo::Image2D>(this)->text) = nullptr;
        }
        const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a1975c)) + 0xd;
        const std::uint32_t size = joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(name))) + 1;
        jo::view<void*>(jo::self<jo::Image2D>(this)->text) = joan::orig::as_mem(img::obj(kMem))->add(joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058e7b0 alloc
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Image2D>(this)->text)), joan::orig::arg::str(name));  // strcpy
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("Image2D::set_name", 0x006298c0, &H::image_set_name);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
//
//      0x006294c0 Widget::~Widget       (thiscall, ret 0; SEH frame only guards the base
//                 destructor on unwind - normal path kept).
namespace {
namespace game_b93_03 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // eax: this (left by the base destructor 0x0061dfb0, whose 0x0061e000 tail is empty).
    void* widget_dtor() {  // 0x006294c0
        jo::view<std::uintptr_t>(jo::self<jo::Image2d>(this)->vtable) = img::addr(d::vt_TextDesc);
        joan::orig::as_image2d(this)->release();  // 0x00629830
        joan::orig::as_gadget(this)->destruct();  // 0x0061dfb0 (base destructor)
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Widget::~Widget", 0x006294c0, &H::widget_dtor);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b90_05.cpp (game/b90_05)
//
//    - Camera::set_ranges 0x00613ca0 (stores positive values to +0x44 / +0x4c / +0x50, then
//      0x00614170), Grid::to_cell 0x006152b0 (offsets from the origin, inside test),
//      Camera::screen_ray 0x006154d0 (two points through the view matrix),
//      Viewport::set_size 0x00616a00, Viewport::set_58 0x00616a60, CamParams::init 0x00616d50,
//      Rect9::clear 0x00620a20, Widget::~Widget 0x006211a0, Button::create 0x00621540,
//      Button::enabled 0x006226e0, Button::set_text 0x00623ce0, TextDesc::clear 0x00623dc0,
//      Image2D::create 0x00629530, Label::create 0x00629660, List::get 0x0062a430,
//      Scroll::set_pos 0x0062be50, clamp 0x0062c200 (cdecl), Pool::alloc 0x0063e450 (cdecl),
//      Pool::free 0x0063e4f0 (cdecl), Reader::Reader 0x0063e550, Reader::list_get 0x0063e9f0,
//      Script::error 0x0063eb80 (cdecl varargs), Reader::count 0x0063ecd0,
//      Reader::next 0x0063ee20, Obj::size_14 0x00641a40.
namespace {
namespace game_b90_05 {

using joan::call::thiscall;

JOAN_COUNTER(c_image_create, "Image2D::create");

struct H {
    // 0x00629530
    std::int32_t image_create(std::int32_t x, std::int32_t y, jo::Font2d* a3, const char* name, std::int32_t a5,
                              std::int32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10,
                              std::int32_t a11, std::int32_t a12, std::int32_t b13, std::int32_t b14,
                              std::int32_t b15) {
        c_image_create.hit();
        if (static_cast<std::int8_t>(jo::self<jo::Image2D>(this)->active) != 0) return 0;
        jo::self<jo::Image2D>(this)->active = 1;
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x) = static_cast<std::uint16_t>(x);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y) = static_cast<std::uint16_t>(y);
        jo::view<std::int32_t>(jo::self<jo::Image2D>(this)->colour) = a6;
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->text_9a) = static_cast<std::uint16_t>(a7);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b1) = static_cast<std::uint16_t>(a8);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b3) = static_cast<std::uint16_t>(a9);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b5) = static_cast<std::uint16_t>(a10);
        jo::view<std::int32_t>(jo::self<jo::Image2D>(this)->scale_x) = a11;
        jo::view<std::int32_t>(jo::self<jo::Image2D>(this)->scale_y) = a12;
        jo::view<std::uint8_t>(jo::self<jo::Image2D>(this)->fit) = static_cast<std::uint8_t>(b13);
        jo::view<std::uint8_t>(jo::self<jo::Image2D>(this)->enabled) = static_cast<std::uint8_t>(b14);
        jo::view<std::uint8_t>(jo::self<jo::Image2D>(this)->i8_8) = static_cast<std::uint8_t>(b15);
        jo::self<jo::Image2D>(this)->kind = 1;
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->text_mode) = static_cast<std::uint16_t>(a5);
        jo::self<jo::Image2D>(this)->font = a3;
        joan::orig::as_image2d(this)->set_name(joan::orig::arg::str(name));  // 0x006298c0
        return 1;
    }
};

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("Image2D::create", 0x00629530, &H::image_create);
#undef B9005

}  // namespace game_b90_05
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using u8 = std::uint8_t;
using u16 = std::uint16_t;


JOAN_COUNTER(c_widget629_ctor, "Widget629::ctor");

struct H {
    // ------------------------------------------------------------------------ 0x00629400
    void* widget629_ctor() {
        c_widget629_ctor.hit();
        joan::orig::as_gadget(this)->ctor();  // 0x0061de00
        jo::view<std::uintptr_t>(jo::self<jo::Image2D>(this)->vtable) = img::addr(d::vt_TextDesc);
        jo::self<jo::Image2D>(this)->active = 0;
        jo::view<u16>(jo::self<jo::Image2D>(this)->x) = 0xffff;
        jo::view<u16>(jo::self<jo::Image2D>(this)->y) = 0xffff;
        jo::view<u16>(jo::self<jo::Image2D>(this)->x2) = 0xffff;
        jo::view<u16>(jo::self<jo::Image2D>(this)->y2) = 0xffff;
        jo::self<jo::Image2D>(this)->kind = 0xff;
        jo::self<jo::Image2D>(this)->sprite_set = nullptr;
        jo::self<jo::Image2D>(this)->font = nullptr;
        jo::view<u8>(jo::self<jo::Image2D>(this)->sprite_name[0]) = 0;
        jo::self<jo::Image2D>(this)->text = nullptr;
        jo::view<u8>(jo::self<jo::Image2D>(this)->fit) = 0;
        jo::view<u8>(jo::self<jo::Image2D>(this)->enabled) = 0;
        jo::view<u8>(jo::self<jo::Image2D>(this)->i8_8) = 0;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Widget629::ctor", 0x00629400, &H::widget629_ctor);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b90_05_2.cpp (game/b90_05)
namespace {
namespace game_b90_05_2 {

using joan::call::thiscall;


std::int32_t sx16(std::int32_t v) { return static_cast<std::int16_t>(v); }

char* c_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_label_create, "Label::create");

struct H {
    // 0x00629660
    std::int32_t label_create(std::int32_t x, std::int32_t y, void* font, const char* text, std::int32_t a5,
                              std::int32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10,
                              std::int32_t a11, std::int32_t a12, std::int32_t a13, std::int32_t unused14,
                              std::int32_t w, std::int32_t h, std::int32_t b17, std::int32_t b18,
                              std::int32_t b19) {
        c_label_create.hit();
        (void)unused14;
        if (static_cast<std::int8_t>(jo::self<jo::Image2D>(this)->active) != 0) return 0;
        jo::self<jo::Image2D>(this)->active = 1;
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x) = static_cast<std::uint16_t>(x);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y) = static_cast<std::uint16_t>(y);
        jo::view<std::int32_t>(jo::self<jo::Image2D>(this)->colour) = a5;
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->sprite_86) = static_cast<std::uint16_t>(a6);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b1) = static_cast<std::uint16_t>(a7);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b3) = static_cast<std::uint16_t>(a8);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b5) = static_cast<std::uint16_t>(a9);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->src_rect[0]) = static_cast<std::uint16_t>(a10);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->src_rect[1]) = static_cast<std::uint16_t>(a11);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->src_rect[2]) = static_cast<std::uint16_t>(a12);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->src_rect[3]) = static_cast<std::uint16_t>(a13);
        if (static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x2)) == -1) {
            const std::int32_t base = sx16(x);
            std::uint8_t* const size = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_bmp_res(font)->find(joan::orig::arg::str(text)));  // 0x00620b40
            jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x2) = static_cast<std::uint16_t>(base + static_cast<std::int16_t>(jo::as_bmp_entry(size)->width) - 1);
        } else {
            jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x2) = static_cast<std::uint16_t>(w);
        }
        if (static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y2)) == -1) {
            const std::int32_t base = sx16(y);
            std::uint8_t* const size = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_bmp_res(font)->find(joan::orig::arg::str(text)));  // 0x00620b40
            jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y2) = static_cast<std::uint16_t>(base + static_cast<std::int16_t>(jo::as_bmp_entry(size)->height) - 1);
        } else {
            jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y2) = static_cast<std::uint16_t>(h);
        }
        jo::view<std::uint8_t>(jo::self<jo::Image2D>(this)->fit) = static_cast<std::uint8_t>(b17);
        jo::view<std::uint8_t>(jo::self<jo::Image2D>(this)->enabled) = static_cast<std::uint8_t>(b18);
        jo::view<std::uint8_t>(jo::self<jo::Image2D>(this)->i8_8) = static_cast<std::uint8_t>(b19);
        jo::self<jo::Image2D>(this)->kind = 0;
        jo::view<void*>(jo::self<jo::Image2D>(this)->sprite_set) = font;
        c_strcpy(jo::self<jo::Image2D>(this)->sprite_name, text);  // +0x76
        return 1;
    }
};

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("Label::create", 0x00629660, &H::label_create);
#undef B9005

}  // namespace game_b90_05_2
}  // namespace

// ---------------------------------------- from modules/game_b82_11.cpp (game/b82_11)
namespace {
namespace game_b82_11 {

using joan::call::thiscall;
using joan::img::obj;

using u32 = std::uint32_t;

using joan::hook::x87::Fpu;

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite queue

JOAN_COUNTER(c_label, "Label::draw");

std::int32_t s16(std::uint32_t v) { return static_cast<std::int16_t>(v); }

// fild (byte) ; fmul float [+0x11] (PC24) ; _ftol ; low byte.
std::uint8_t scale_alpha(std::uint32_t a, float k) {
    return static_cast<std::uint8_t>(
        joan::hook::x87::ftol(Fpu{true}.mul(static_cast<double>(a), static_cast<double>(k))));
}

struct H {

    // ---------------------------------------------------------------- 0x00629bd0
    std::int32_t draw_label(std::uint32_t arg, std::uint32_t /*unused*/) {
        if (jo::view<std::int8_t>(jo::self<jo::Image2D>(this)->active) == 0) return 0;
        if (jo::self<jo::Image2D>(this)->i8_8 == 0) return 0;
        c_label.hit();
        // [ebp-0x8] = +0xad; [ebp-0x5] (its alpha byte) = ftol(byte +0xb0 * float +0x11).
        const std::uint8_t alpha = scale_alpha(static_cast<std::uint8_t>(jo::self<jo::Image2D>(this)->colour >> 24), jo::self<jo::Image2D>(this)->scale);
        const std::uint32_t colour = (jo::self<jo::Image2D>(this)->colour & 0x00ffffffu) | (static_cast<std::uint32_t>(alpha) << 24);
        const std::uint8_t kind = jo::self<jo::Image2D>(this)->kind;
        if (kind == 0) {
            joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x))), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y))), joan::orig::arg::u32(arg), joan::orig::arg::str(static_cast<const char*>(&jo::self<jo::Image2D>(this)->sprite_name[0])), joan::orig::arg::mptr(jo::self<jo::Image2D>(this)->sprite_set), joan::orig::arg::u32(colour), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->sprite_86))), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b1))), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b3))), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b5))), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->src_rect[0]))), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->src_rect[1]))), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->src_rect[2]))), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->src_rect[3]))), 0, joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x2))), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y2))));  // 0x00633500
        } else if (kind == 1) {
            void* const text = jo::view<void*>(jo::self<jo::Image2D>(this)->font);
            const char* const t = jo::self<jo::Image2D>(this)->text;
            switch (static_cast<std::uint32_t>(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->text_mode)))) {
            case 0:
                joan::orig::as_text(text)->draw_line(joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y))), joan::orig::arg::u32(arg), t, joan::orig::arg::u32(colour), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x2))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->text_9a))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b1))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b3))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b5))), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Image2D>(this)->scale_x)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Image2D>(this)->scale_y)));  // 0x006255d0
                break;
            case 1:
                joan::orig::as_text(text)->draw_3(joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x2))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y2))), joan::orig::arg::u32(arg), t, joan::orig::arg::u32(colour), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->text_9a))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b1))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b3))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b5))), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Image2D>(this)->scale_x)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Image2D>(this)->scale_y)));  // 0x00626130
                break;
            case 2:
                joan::orig::as_text(text)->draw_2(joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y))), joan::orig::arg::u32(arg), t, joan::orig::arg::u32(colour), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->text_9a))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b1))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b3))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b5))), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Image2D>(this)->scale_x)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Image2D>(this)->scale_y)));  // 0x00626080
                break;
            case 3:
                joan::orig::as_text(text)->draw(joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x2))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y2))), joan::orig::arg::u32(arg), t, joan::orig::arg::u32(colour), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->wrap_a4))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->wrap_a6))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->wrap_a8))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b1))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b3))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b5))), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Image2D>(this)->scale_x)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Image2D>(this)->scale_y)), joan::orig::arg::i32(static_cast<std::uint32_t>(jo::self<jo::Image2D>(this)->wrap_aa)), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->wrap_ab))));  // 0x00626320
                break;
            case 4:
                joan::orig::as_text_renderer(text)->draw_wrapped(joan::orig::arg::u32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x))), joan::orig::arg::u32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y))), joan::orig::arg::u32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x2))), joan::orig::arg::u32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y2))), joan::orig::arg::u32(arg), joan::orig::arg::str(t), joan::orig::arg::u32(colour), joan::orig::arg::u32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->wrap_a4))), joan::orig::arg::u32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->text_9a))), joan::orig::arg::u32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b1))), joan::orig::arg::u32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b3))), joan::orig::arg::u32(s16(jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->queue_b5))), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Image2D>(this)->scale_x)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Image2D>(this)->scale_y)), 2u);  // 0x00626e00
                break;
            default:
                break;
            }
        }
        return 1;
    }

    // ---------------------------------------------------------------- 0x006299b0
    void* set_layout(std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d, std::uint32_t e, std::uint32_t g,
                     std::uint32_t h) {
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->x2) = static_cast<std::uint16_t>(a);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->y2) = static_cast<std::uint16_t>(b);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->wrap_a4) = static_cast<std::uint16_t>(c);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->wrap_a6) = static_cast<std::uint16_t>(d);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->wrap_a8) = static_cast<std::uint16_t>(e);
        jo::self<jo::Image2D>(this)->wrap_aa = static_cast<std::uint8_t>(g);
        jo::view<std::uint16_t>(jo::self<jo::Image2D>(this)->wrap_ab) = static_cast<std::uint16_t>(h);
        return this;
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b82_11", name, addr, fn)
B11("Label::draw", 0x00629bd0, &H::draw_label);
B11("Label::set_layout", 0x006299b0, &H::set_layout);
#undef B11

}  // namespace game_b82_11
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
namespace {
namespace game_b102_12 {

using joan::call::thiscall;
using joan::img::obj;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;

JOAN_COUNTER(c_hit_test, "Widget::hit_test");

struct H {
    // ------------------------------------------------------------------ 0x00629a40
    u32 hit_test(u32 /*unused*/) {
        c_hit_test.hit();
        if (jo::view<i8>(jo::self<jo::Image2d>(this)->active) == 0) return 0;
        if (jo::self<jo::Image2d>(this)->i8_8 == 0) return 0;
        if (jo::self<jo::Image2d>(this)->enabled == 0) return 0;
        // dword stack slots holding words (the callee writes words through the pointers)
        u32 l = jo::view<u16>(jo::self<jo::Image2d>(this)->x), t = jo::view<u16>(jo::self<jo::Image2d>(this)->y), r = jo::view<u16>(jo::self<jo::Image2d>(this)->x2), b = jo::view<u16>(jo::self<jo::Image2d>(this)->y2);
        u32 sx = 0x3f800000u, sy = 0x3f800000u;
        if (jo::self<jo::Image2d>(this)->fit != 0) {
            joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::view<u16>(jo::self<jo::Image2d>(this)->x)), joan::orig::arg::i16(jo::view<u16>(jo::self<jo::Image2d>(this)->y)), joan::orig::arg::i16(jo::view<u16>(jo::self<jo::Image2d>(this)->x2)), joan::orig::arg::i16(jo::view<u16>(jo::self<jo::Image2d>(this)->y2)), joan::orig::arg::mptr(static_cast<void*>(&sx)), joan::orig::arg::mptr(static_cast<void*>(&sy)), joan::orig::arg::mptr(static_cast<void*>(&l)), joan::orig::arg::mptr(static_cast<void*>(&t)), joan::orig::arg::mptr(static_cast<void*>(&r)), joan::orig::arg::mptr(static_cast<void*>(&b)), joan::orig::arg::i16(0x280u), joan::orig::arg::i16(0x1e0u));  // 0x00633e60
        }
        const i32 L = static_cast<i16>(l), T = static_cast<i16>(t), R = static_cast<i16>(r), B = static_cast<i16>(b);
        if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(d::g_mouse))->rect_value(joan::orig::arg::i32(L), joan::orig::arg::i32(T), joan::orig::arg::i32(R), joan::orig::arg::i32(B), 0)) != 0) return 1;  // 0x005945d0
        if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(d::g_mouse))->rect_mode2(joan::orig::arg::i32(L), joan::orig::arg::i32(T), joan::orig::arg::i32(R), joan::orig::arg::i32(B), 0)) != 0) return 1;  // 0x00594900
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("Widget::hit_test", 0x00629a40, &H::hit_test);
#undef B

}  // namespace game_b102_12
}  // namespace

// ---------------------------------------- from modules/game_b111_14.cpp (game/b111_14)
namespace {
namespace game_b111_14 {

struct H {
    // ------------------------------------------------------------ 0x0062a0f0
    // Slot 0 of vtable 0x009da70c: thiscall, no arguments, a bare `ret` (eax is not set; the only caller that
    // reads eax after a slot-0 call, Obj::update_61f3b0 in gadget.cpp, passes it on to callers that discard it).
    void slot0() {}
};

JOAN_REPLACE("game/b111_14", "TextDesc::slot0_62a0f0", 0x0062a0f0, &H::slot0);

}  // namespace game_b111_14
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Image2D::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_06::H::widget629_ctor>(this)); }  // 0x629400
auto joan::orig::Widget::dtor_6294c0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_03::H::widget_dtor>(this)); }  // 0x6294c0
auto joan::orig::Image2D::create(std::int32_t a0, std::int32_t a1, Font2d* a2, const char* a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10, std::int32_t a11, std::int32_t a12, std::int32_t a13, std::int32_t a14) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_05::H::image_create>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)); }  // 0x629530
auto joan::orig::Label::create(std::int32_t a0, std::int32_t a1, void* a2, const char* a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10, std::int32_t a11, std::int32_t a12, std::int32_t a13, std::int32_t a14, std::int32_t a15, std::int32_t a16, std::int32_t a17, std::int32_t a18) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_05_2::H::label_create>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18)); }  // 0x629660
auto joan::orig::Image2D::release() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_05::H::image_release>(this)); }  // 0x629830
auto joan::orig::Image2D::set_name(const char* a0) -> void { joan::bind::member<&::game_leaves80b::H::image_set_name>(this, a0); }  // 0x6298c0
auto joan::orig::Label::set_layout(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_11::H::set_layout>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x6299b0
auto joan::orig::Label::draw(std::uint32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_11::H::draw_label>(this, a0, a1)); }  // 0x629bd0
