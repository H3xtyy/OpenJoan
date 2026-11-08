// ilib/vqueue.cpp: functions of the original d:\ww\ilib\vqueue.cpp (re/attribution.csv).
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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

void* const kMemory = obj(d::g_mem);  // allocator (0x0058eb30 realloc / 0x0058ed70 free)

}  // namespace

// ---------------------------------------- from modules/game_b102_10.cpp (game/b102_10)
//
//    - VQueue::ctor 0x005a8690 (size), ret 4: buffer from 0x0058e7b0 on 0xa48838 (line word 0xa16e24).
//
//    - VQueue::copy_ctor 0x005a8700 (src), ret 4 / VQueue::assign 0x005a8800 (src), ret 4.
//
//    - VQueue::consume 0x005a8920 (n), ret 4: memmove the tail down, returns the count removed.
//
//    - VQueue::swap 0x005a8a10 (other), ret 4 (eax = old this+0xc, kept).
namespace {
namespace game_b102_10 {

using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::cdecl_call;
using joan::orig::helpers::at;

using joan::orig::helpers::crt_memcpy;
void* crt_memmove(void* d, const void* s, u32 n) { return joan::orig::arg::as<void*>(joan::orig::libc::memcpy(joan::orig::arg::mptr(d), joan::orig::arg::mptr(s), joan::orig::arg::u32(n))); }

JOAN_COUNTER(c_vq_copy, "VQueue::copy_ctor");

struct H {

    // 0x005a8690 VQueue(size)
    H* vq_ctor(u32 size) {
        const i32 line = global<i16>(d::g_a16e24);
        jo::self<jo::VQueue>(this)->data = joan::orig::arg::as<u8*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size), str(d::s_d_ww_ilib_vqueue_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        jo::self<jo::VQueue>(this)->capacity = size;
        jo::view<u32>(jo::self<jo::VQueue>(this)->length) = 0;
        jo::self<jo::VQueue>(this)->cursor = nullptr;
        return this;
    }
    // 0x005a8700 VQueue(const VQueue&)
    H* vq_copy(const u8* src) {
        c_vq_copy.hit();
        const i32 line = global<i16>(d::g_a16e28);
        jo::self<jo::VQueue>(this)->data = joan::orig::arg::as<u8*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(jo::as_vqueue(src)->capacity), str(d::s_d_ww_ilib_vqueue_cpp), joan::orig::arg::i32(line)));
        jo::self<jo::VQueue>(this)->capacity = jo::as_vqueue(src)->capacity;
        crt_memcpy(jo::view<void*>(jo::self<jo::VQueue>(this)->data), jo::view<void*>(jo::as_vqueue(src)->data), jo::view<u32>(jo::as_vqueue(src)->length));
        jo::view<u32>(jo::self<jo::VQueue>(this)->length) = jo::view<u32>(jo::as_vqueue(src)->length);
        jo::self<jo::VQueue>(this)->cursor = jo::self<jo::VQueue>(this)->data + (jo::as_vqueue(src)->cursor - jo::as_vqueue(src)->data);
        return this;
    }
    // 0x005a8800 VQueue::operator=
    H* vq_assign(const u8* src) {
        joan::orig::as_vqueue(this)->reserve(joan::orig::arg::i32(jo::view<u32>(jo::as_vqueue(src)->length)));  // 0x005a8ae0 reserve
        jo::view<u32>(jo::self<jo::VQueue>(this)->length) = jo::view<u32>(jo::as_vqueue(src)->length);
        crt_memcpy(jo::view<void*>(jo::self<jo::VQueue>(this)->data), jo::view<void*>(jo::as_vqueue(src)->data), jo::view<u32>(jo::self<jo::VQueue>(this)->length));
        jo::self<jo::VQueue>(this)->cursor = jo::self<jo::VQueue>(this)->data + (jo::as_vqueue(src)->cursor - jo::as_vqueue(src)->data);
        return this;
    }
    // 0x005a8920 VQueue::consume(n)
    i32 vq_consume(i32 n) {
        if (n > jo::self<jo::VQueue>(this)->length) n = jo::self<jo::VQueue>(this)->length;
        if (n > 0) {
            crt_memmove(jo::view<void*>(jo::self<jo::VQueue>(this)->data), jo::self<jo::VQueue>(this)->data + n, static_cast<u32>(jo::self<jo::VQueue>(this)->length - n));
            jo::self<jo::VQueue>(this)->length = jo::self<jo::VQueue>(this)->length - n;
            jo::self<jo::VQueue>(this)->cursor = jo::self<jo::VQueue>(this)->cursor - n;
        } else {
            n = 0;
        }
        return n;
    }
    // 0x005a8a10 VQueue::swap
    std::uintptr_t vq_swap(u8* o) {
        u8* p = jo::self<jo::VQueue>(this)->data;
        jo::self<jo::VQueue>(this)->data = jo::as_vqueue(o)->data;
        jo::as_vqueue(o)->data = p;
        u32 t = jo::self<jo::VQueue>(this)->capacity;
        jo::self<jo::VQueue>(this)->capacity = jo::as_vqueue(o)->capacity;
        jo::as_vqueue(o)->capacity = t;
        t = jo::view<u32>(jo::self<jo::VQueue>(this)->length);
        jo::view<u32>(jo::self<jo::VQueue>(this)->length) = jo::view<u32>(jo::as_vqueue(o)->length);
        jo::view<u32>(jo::as_vqueue(o)->length) = t;
        p = jo::self<jo::VQueue>(this)->cursor;
        jo::self<jo::VQueue>(this)->cursor = jo::as_vqueue(o)->cursor;
        jo::as_vqueue(o)->cursor = p;
        return joan::orig::arg::as<std::uintptr_t>(p);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_10", name, addr, fn)
B("VQueue::ctor", 0x005a8690, &H::vq_ctor);
B("VQueue::copy_ctor", 0x005a8700, &H::vq_copy);
B("VQueue::assign", 0x005a8800, &H::vq_assign);
B("VQueue::consume", 0x005a8920, &H::vq_consume);
B("VQueue::swap", 0x005a8a10, &H::vq_swap);
#undef B

}  // namespace game_b102_10
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (game/b82_17)
//
//      0x005a87b0 VQueue::free()                 - 0x0058ed70 on +0 (vqueue.cpp)
//
//      0x005a88c0 VQueue::push(n) (ret 4)        - reserve (0x005a8ae0), returns +0 + old +8
namespace {
namespace en3d_b82_17 {

using joan::orig::helpers::at;

struct H {

    // 0x005a87b0 (eax: this, or 0 after the 0x0058ed70 free - every caller discards it)
    std::uintptr_t vqueue_free() {  // returns a pointer-sized eax leftover (the facade returns std::uintptr_t)
        if (jo::view<void*>(jo::self<jo::VQueue>(this)->data) == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        joan::orig::as_mem(kMemory)->del_58ed70(jo::view<void*>(jo::self<jo::VQueue>(this)->data), str(d::s_d_ww_ilib_vqueue_cpp),
                                                static_cast<std::int32_t>(global<std::int16_t>(d::g_a16e2c)) + 2);  // 0x0058ed70
        return 0;  // callers (0x005da3d0, 0x005dde90, unwind funclets, en3d/plane.cpp) discard the result; del returns nothing
    }

    // 0x005a88c0
    void* vqueue_push(std::uint32_t n) {
        joan::orig::as_vqueue(this)->reserve(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::VQueue>(this)->length) + n));  // 0x005a8ae0
        void* const r = jo::self<jo::VQueue>(this)->data + jo::view<std::uint32_t>(jo::self<jo::VQueue>(this)->length);
        jo::view<std::uint32_t>(jo::self<jo::VQueue>(this)->length) = jo::view<std::uint32_t>(jo::self<jo::VQueue>(this)->length) + n;
        return r;
    }
};

#define B82_17G(name, addr, fn) JOAN_REPLACE("game/b82_17", name, addr, fn)
B82_17G("VQueue::free", 0x005a87b0, &H::vqueue_free);
B82_17G("VQueue::push", 0x005a88c0, &H::vqueue_push);
#undef B82_17G

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
//
//    - Buf::rewind 0x005a8880 (dead code): +8 = 0, +0xc = [this]; eax = [this].
//
//    - Buf::copy_from 0x005a89c0 (src), ret 4 (dead code): memcpy(0x5a88c0(len), [src], len); src is a VQueue
//      (data +0, length +8: read through the typed members).
namespace {
namespace game_b103_10 {

using joan::orig::helpers::at;

struct H {

    // ------------------------------------------------------------------ 0x005a8880 (dead code)
    std::uintptr_t buf_rewind() {
        jo::view<u32>(jo::self<jo::VQueue>(this)->length) = 0;
        jo::self<jo::VQueue>(this)->cursor = jo::self<jo::VQueue>(this)->data;
        return joan::orig::arg::as<std::uintptr_t>(jo::self<jo::VQueue>(this)->data);
    }

    // ------------------------------------------------------------------ 0x005a89c0 (dead code)
    std::uintptr_t buf_copy_from(const void* src) {  // src: a VQueue ([src] data, [src+8] length)
        const jo::VQueue* const q = jo::self<jo::VQueue>(src);
        const u32 len = jo::view<u32>(q->length);
        const void* const data = q->data;
        void* const dst = joan::orig::arg::as<void*>(joan::orig::as_vqueue(this)->push(joan::orig::arg::u32(jo::view<u32>(q->length))));  // 0x005a88c0
        return joan::orig::arg::as<std::uintptr_t>(std::memcpy(joan::orig::arg::mptr(dst), joan::orig::arg::ptr(data), joan::orig::arg::u32(len)));  // memcpy
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("Buf::rewind", 0x005a8880, &H::buf_rewind);
B("Buf::copy_from", 0x005a89c0, &H::buf_copy_from);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16 {

using joan::orig::helpers::at;

struct H {

    u32 get_08_5a8ac0() { return jo::view<u32>(jo::self<jo::VQueue>(this)->length); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Obj::get_08_5a8ac0", 0x005a8ac0, &H::get_08_5a8ac0);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/en3d_b83_08.cpp (game/b83_08)
//
//      0x005a8ae0 VQueue::reserve(n) (ret 4)           - grows +4 to n rounded to 0x1000 (signed %),
//                 realloc (0x0058eb30, vqueue.cpp), +0xc rebased
namespace {
namespace en3d_b83_08 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_reserve, "VQueue::reserve");

struct H {

    // 0x005a8ae0 (eax = this)
    void* reserve(std::int32_t n) {
        c_reserve.hit();
        if (n > jo::view<std::int32_t>(jo::self<jo::VQueue>(this)->capacity)) {
            std::uint8_t* const old = jo::self<jo::VQueue>(this)->data;
            const std::int32_t rem = n % 0x1000;
            jo::self<jo::VQueue>(this)->capacity = static_cast<std::uint32_t>(n) + (0x1000u - static_cast<std::uint32_t>(rem));
            const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a16e30)) + 6;
            jo::self<jo::VQueue>(this)->data = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(jo::self<jo::VQueue>(this)->data), joan::orig::arg::u32(jo::self<jo::VQueue>(this)->capacity), str(d::s_d_ww_ilib_vqueue_cpp), joan::orig::arg::i32(line)));  // 0x0058eb30 "d:\ww\ilib\vqueue.cpp"
            jo::self<jo::VQueue>(this)->cursor = jo::self<jo::VQueue>(this)->data + (jo::self<jo::VQueue>(this)->cursor - old);
        }
        return this;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b83_08", name, addr, fn)
GAME("VQueue::reserve", 0x005a8ae0, &H::reserve);
#undef GAME

}  // namespace en3d_b83_08
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (game/b82_17)
//
//      0x005a8640 VQueue::VQueue()               - 4 dwords = 0
namespace {
namespace en3d_b82_17_2 {

using joan::orig::helpers::at;

struct H {

    // 0x005a8640
    void* vqueue_ctor() {
        jo::self<jo::VQueue>(this)->data = nullptr;
        jo::self<jo::VQueue>(this)->capacity = 0;
        jo::view<std::uint32_t>(jo::self<jo::VQueue>(this)->length) = 0;
        jo::self<jo::VQueue>(this)->cursor = nullptr;
        return this;
    }
};

#define B82_17G(name, addr, fn) JOAN_REPLACE("game/b82_17", name, addr, fn)
B82_17G("VQueue::VQueue", 0x005a8640, &H::vqueue_ctor);
#undef B82_17G

}  // namespace en3d_b82_17_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::VQueue::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17_2::H::vqueue_ctor>(this)); }  // 0x5a8640
auto joan::orig::VQueue::free() -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::en3d_b82_17::H::vqueue_free>(this)); }  // 0x5a87b0
auto joan::orig::VQueue::push(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17::H::vqueue_push>(this, a0)); }  // 0x5a88c0
auto joan::orig::VQueue::reserve(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_08::H::reserve>(this, a0)); }  // 0x5a8ae0
