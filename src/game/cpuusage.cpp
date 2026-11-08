// game/cpuusage.cpp: functions of the original d:\ww\cpuusage.cpp (re/attribution.csv).
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
#include "../registry.hpp"
#include "sound_slots.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
//
//    - CpuUsage::init_45c040 0x0045c040 (ret 4, cpuusage.cpp): tables (0x0058e7b0 on 0xa48838 with
//      line = [0x9fc9cc] + n), memsets, eh-vector-ctor'd 0x1c-byte array, graph object (0x0045ec60),
//      rdtsc / QueryPerformanceCounter calibration over Sleep(100):
//      +0x44 = (tsc_delta * qpf) / qpc_delta (_allmul / _aulldiv as u64), then 0x0045c990,
//      0x0045f0f0 (eax returned), [this] = 1. SEH frame: normal path only.
namespace {
namespace game_b108_15 {

using u32 = std::uint32_t;
using u64 = std::uint64_t;

using joan::call::stdcall_call;
template <class R = void, class... A>
R import_call(img::Addr slot, A... args) {
    return stdcall_call<R>(global<std::uintptr_t>(slot), args...);
}

JOAN_COUNTER(c_cpu_init, "CpuUsage::init_45c040");

struct H {
    // ------------------------------------------------------------------ 0x0045c040
    u32 cpu_init(i32 n) {
        c_cpu_init.hit();
        joan::orig::as_profiler(this)->release();  // 0x0045c5c0
        jo::view<i32>(jo::self<jo::CpuUsage>(this)->enabled) = -1;
        jo::self<jo::CpuUsage>(this)->func_count = n;
        auto alloc = [](u32 size, i32 line_add) {
            return joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size), str(d::s_d_ww_cpuusage_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fc9cc)) + line_add));  // 0x0058e7b0
        };
        jo::self<jo::CpuUsage>(this)->last = static_cast<jo::ArrU64*>(alloc(static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) << 3, 6));
        jo::self<jo::CpuUsage>(this)->total = static_cast<jo::ArrU64*>(alloc(static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) << 3, 7));
        jo::self<jo::CpuUsage>(this)->start = static_cast<jo::ArrU64*>(alloc(static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) << 3, 8));
        jo::self<jo::CpuUsage>(this)->depth = static_cast<jo::ArrU32*>(alloc(static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) << 2, 9));
        jo::self<jo::CpuUsage>(this)->average = static_cast<jo::ArrU64*>(alloc(static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) << 3, 0xb));
        jo::self<jo::CpuUsage>(this)->history = static_cast<jo::ArrU64*>(alloc((static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) * 0x3cu) << 3, 0xd));
        jo::self<jo::CpuUsage>(this)->history_hits = static_cast<jo::ArrU32*>(alloc((static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) * 0x3cu) << 2, 0xe));
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::CpuUsage>(this)->history)), 0, joan::orig::arg::u32((static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) * 0x3cu) << 3));
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::CpuUsage>(this)->history_hits)), 0, joan::orig::arg::u32((static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) * 0x3cu) << 2));
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::CpuUsage>(this)->total)), 0, joan::orig::arg::u32(static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) << 3));
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::CpuUsage>(this)->average)), 0, joan::orig::arg::u32(static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) << 3));
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::CpuUsage>(this)->depth)), 0, joan::orig::arg::u32(static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) << 2));
        const u32 cnt = static_cast<u32>(jo::self<jo::CpuUsage>(this)->func_count) + 1u;
        // new ProfFunc[cnt]: a 4-byte element-count cookie before the array (read back by 0x0045c520), on x64 too.
        u32* const block = joan::orig::arg::as<u32*>(joan::crt::operator_new(joan::orig::arg::u32(cnt * static_cast<u32>(sizeof(jo::ProfFunc)) + 4u)));  // operator new
        jo::ProfFunc* arr = nullptr;
        if (block != nullptr) {
            block[0] = cnt;
            joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(block + 1)), static_cast<u32>(sizeof(jo::ProfFunc)), joan::orig::arg::i32(cnt), fn::code_value(fn::Obj_clear_45e830),  // ctor 0x0045e830
                               fn::code_value(fn::Profiler_free_lists));  // dtor 0x0045e860
            arr = reinterpret_cast<jo::ProfFunc*>(block + 1);
        }
        jo::self<jo::CpuUsage>(this)->funcs = arr;
        for (i32 i = 0; i <= jo::self<jo::CpuUsage>(this)->func_count; ++i) {
            jo::view<i32>(jo::self<jo::CpuUsage>(this)->funcs[i].id) = i;
        }
        void* const mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::ProfNode)));  // 0x50 on x86
        jo::ProfNode* graph = nullptr;
        if (mem != nullptr) graph = static_cast<jo::ProfNode*>(joan::orig::as_prof_node(mem)->construct());  // 0x0045ec60
        jo::self<jo::CpuUsage>(this)->root = graph;
        joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::CpuUsage>(this)->root))->set_func(joan::orig::arg::mptr(jo::self<jo::CpuUsage>(this)->funcs));  // 0x0045ed30
        joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::CpuUsage>(this)->root))->set_parent(joan::orig::arg::mptr(0u));                  // 0x0045efb0
        joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::CpuUsage>(this)->root))->set_edge(joan::orig::arg::mptr(0u));                  // 0x0045ee70
        jo::self<jo::CpuUsage>(this)->current = jo::self<jo::CpuUsage>(this)->root;
        jo::self<jo::CpuUsage>(this)->cpu_hz[0] = 0;
        jo::self<jo::CpuUsage>(this)->cpu_hz[1] = 0;
        u64 tsc0 = 0, tsc1 = 0, qpc0 = 0, qpc1 = 0, freq = 0;
        if (joan::orig::arg::as<u32>(joan::orig::as_profiler(this)->read_tsc(&tsc0)) != 0) {  // 0x0045e750 (rdtsc, always writes)
            import_call<i32>(d::iat_KERNEL32_QueryPerformanceCounter, &qpc0);            // QueryPerformanceCounter
            import_call<void>(d::iat_KERNEL32_Sleep, 100u);            // Sleep
            joan::orig::as_profiler(this)->read_tsc(&tsc1);
            import_call<i32>(d::iat_KERNEL32_QueryPerformanceCounter, &qpc1);
            import_call<i32>(d::iat_KERNEL32_QueryPerformanceFrequency, &freq);  // QueryPerformanceFrequency
            const u64 prod = (tsc1 - tsc0) * freq;  // _allmul (low 64 bits)
            const u64 rate = prod / (qpc1 - qpc0);   // _aulldiv
            jo::self<jo::CpuUsage>(this)->cpu_hz[0] = static_cast<u32>(rate);
            jo::self<jo::CpuUsage>(this)->cpu_hz[1] = static_cast<u32>(rate >> 32);
        }
        joan::orig::as_profiler(this)->read_clock(jo::self<jo::CpuUsage>(this)->clock);  // 0x0045c990
        joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::CpuUsage>(this)->root))->start(jo::self<jo::CpuUsage>(this)->clock);  // 0x0045f0f0
        jo::self<jo::CpuUsage>(this)->enabled = 1;
        return 0;  // callers (0x00499960 and its replacement) discard the result; Ref::add returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("CpuUsage::init", 0x0045c040, &H::cpu_init);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
//
//      0x0045c430 call_508b90(p)                            (thiscall, ret 4, this unused)
namespace {
namespace game_b99_01 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0045c430
    u32 call_508b90(void* p) {
        joan::orig::as_unit(p)->play_idle(0, 1);  // 0x00508b90
        return 0;  // only caller: Action::dispatch 0x004bb660 (handler table, result discarded); play_idle returns nothing
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("call_508b90", 0x0045c430, &H::call_508b90);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b88_05.cpp (game/b88_05)
//
//    - Station::leave 0x0045c460 (short* id), ret 4: binary search (0x005911a0, comparator
//      0x004b5e10) in the +0x1541 short list (count word +0x153d), removes and decrements.
//      Returns 0 when not found, this otherwise (eax leftover).
//
//  - 0x0045c460: the key local is a 4-byte slot with only the low word written; 0x005911a0
//    only hands its address to the comparator 0x004b5e10, which reads a word. Upper half 0.
namespace {
namespace game_b88_05 {

std::uintptr_t u32(const void* p) { return reinterpret_cast<std::uintptr_t>(p); }

void* const kArrays = obj(d::g_misc);     // 0x00591070 remove, 0x005911a0 search, 0x00590660 random

void array_remove(void* data, std::int32_t count, std::int32_t size, std::int32_t index) {
    joan::orig::as_misc(kArrays)->del_array_rec_591070(joan::orig::arg::str(data), joan::orig::arg::i32(count), joan::orig::arg::i32(size), joan::orig::arg::i32(index));  // 0x00591070
}

JOAN_COUNTER(c_leave, "Station::leave");

struct H {
    // ------------------------------------------------------------ 0x0045c460 (ret 4)
    std::uintptr_t station_leave(const std::uint16_t* id) {
        c_leave.hit();
        std::uint32_t key = *id;  // ebp-0x8 (low word)
        void* const hit = joan::orig::arg::as<void*>(joan::orig::as_misc(kArrays)->bin_search_5911a0(joan::orig::arg::mptr(static_cast<void*>(&key)), joan::orig::arg::str(jo::view<void*>(jo::self<jo::Nation>(this)->stations)), joan::orig::arg::i32(// 0x005911a0
                                          static_cast<std::int32_t>(static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::self<jo::Nation>(this)->station_count)))), 2, joan::orig::arg::as<std::uint32_t>(fn::code_ptr(fn::compare_short_4b5e10)), joan::orig::arg::mptr(0)));
        if (hit == nullptr) return 0;
        array_remove(jo::view<void*>(jo::self<jo::Nation>(this)->stations), static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::self<jo::Nation>(this)->station_count)), 2, static_cast<std::int32_t>(u32(hit)));
        jo::view<std::uint16_t>(jo::self<jo::Nation>(this)->station_count) = static_cast<std::uint16_t>(jo::view<std::uint16_t>(jo::self<jo::Nation>(this)->station_count) - 1);
        return u32(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_05", name, addr, fn)
B("Station::leave", 0x0045c460, &H::station_leave);
#undef B

}  // namespace game_b88_05
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
//
//      0x0045c520 CpuUsage::vector_deleting_dtor  (thiscall, ret 4) - element 0x1c.
namespace {
namespace game_b100_02 {

using joan::orig::helpers::at;

using u32 = std::uint32_t;

u8* bp(const void* p) { return static_cast<u8*>(const_cast<void*>(p)); }

using joan::orig::helpers::game_delete;                // operator delete

struct H {
    u8* self() { return bp(this); }

    // The 4-byte array-new cookie (element count) before the array, written by init_45c040: not a member.
    static i32 array_cookie(const u8* array) {
        i32 n;
        std::memcpy(&n, array - 4, sizeof n);
        return n;
    }

    // ------------------------------------------------------------------ 0x0045c520
    void* vector_deleting_dtor(u32 flags) {
        if ((flags & 2) != 0) {
            using Iter = void(JOAN_STDCALL*)(void*, u32, i32, void*);
            reinterpret_cast<Iter>(static_cast<std::uintptr_t>(fn::code_value(fn::crt_eh_vector_destructor_iterator)))(this, static_cast<u32>(sizeof(jo::ProfFunc)), array_cookie(self()),  // the 4-byte array-new cookie (element count) before the array (written by init_45c040)
                                                                          fn::code_ptr(fn::Profiler_free_lists));  // 0x0045e860
            if ((flags & 1) != 0) game_delete(self() - 4);
            return self() - 4;
        }
        joan::orig::as_prof_func(this)->destruct();  // 0x0045e860
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("CpuUsage::vector_deleting_dtor", 0x0045c520, &H::vector_deleting_dtor);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
//
//    - CpuUsage::release 0x0045c5c0 (ret 0, cpuusage.cpp): frees members, returns this.
namespace {
namespace game_b108_03 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_cpu_release, "CpuUsage::release_45c5c0");

struct H {
    // ------------------------------------------------------------------ 0x0045c5c0
    void* cpu_release() {
        c_cpu_release.hit();
        if (jo::self<jo::Profiler>(this)->enabled == 0) return this;
        jo::self<jo::Profiler>(this)->current = nullptr;
        if (jo::view<void*>(jo::self<jo::Profiler>(this)->root) != nullptr) joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::Profiler>(this)->root))->destroy(1u);  // 0x0045c870
        jo::self<jo::Profiler>(this)->root = nullptr;
        if (jo::view<void*>(jo::self<jo::Profiler>(this)->funcs) != nullptr) joan::orig::as_prof_func(jo::view<void*>(jo::self<jo::Profiler>(this)->funcs))->vector_destroy(3u);  // 0x0045c520
        jo::self<jo::Profiler>(this)->funcs = nullptr;
        void* const heap = obj(d::g_mem);
        const i32 line = global<i16>(d::g_9fc9d0);
        const char* const file = str(d::s_d_ww_cpuusage_cpp);
        if (jo::self<jo::Profiler>(this)->last != nullptr) joan::orig::as_mem(heap)->del(joan::orig::arg::mptr(jo::self<jo::Profiler>(this)->last), joan::orig::arg::str(file), joan::orig::arg::i32(line + 0xd));  // 0x0058ed70
        jo::self<jo::Profiler>(this)->last = nullptr;
        if (jo::self<jo::Profiler>(this)->start != nullptr)
            joan::orig::as_mem(heap)->del(joan::orig::arg::mptr(jo::self<jo::Profiler>(this)->start), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fc9d0)) + 0x11));
        jo::self<jo::Profiler>(this)->start = nullptr;
        if (jo::self<jo::Profiler>(this)->total != nullptr)
            joan::orig::as_mem(heap)->del(joan::orig::arg::mptr(jo::self<jo::Profiler>(this)->total), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fc9d0)) + 0x15));
        jo::self<jo::Profiler>(this)->total = nullptr;
        if (jo::self<jo::Profiler>(this)->depth != nullptr)
            joan::orig::as_mem(heap)->del(joan::orig::arg::mptr(jo::self<jo::Profiler>(this)->depth), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fc9d0)) + 0x19));
        jo::self<jo::Profiler>(this)->depth = nullptr;
        if (jo::self<jo::Profiler>(this)->average != nullptr)
            joan::orig::as_mem(heap)->del(joan::orig::arg::mptr(jo::self<jo::Profiler>(this)->average), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fc9d0)) + 0x1d));
        jo::self<jo::Profiler>(this)->average = nullptr;
        if (jo::self<jo::Profiler>(this)->history != nullptr) {
            joan::orig::as_mem(heap)->del(joan::orig::arg::mptr(jo::self<jo::Profiler>(this)->history), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fc9d0)) + 0x22));
            jo::self<jo::Profiler>(this)->history = nullptr;
        }
        if (jo::self<jo::Profiler>(this)->history_hits != nullptr) {
            joan::orig::as_mem(heap)->del(joan::orig::arg::mptr(jo::self<jo::Profiler>(this)->history_hits), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fc9d0)) + 0x28));
            jo::self<jo::Profiler>(this)->history_hits = nullptr;
        }
        jo::view<u32>(jo::self<jo::Profiler>(this)->func_count) = 0;
        jo::self<jo::Profiler>(this)->enabled = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("CpuUsage::deinit", 0x0045c5c0, &H::cpu_release);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
//
//    - Obj::call_45c010 0x0045c010 (ret 0): this->0x0045c5c0().
namespace {
namespace game_b108_19 {

using u32 = std::uint32_t;

using joan::call::thiscall;

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x0045c010
    std::uintptr_t call_45c010() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_profiler(this)->release()); }  // 0x0045c5c0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Obj::call_45c010", 0x0045c010, &H::call_45c010);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
//
//    - CpuUsageItem::scalar_dtor 0x0045c870 (1 arg, ret 4): 0x0045ec90 + delete when bit 0.
namespace {
namespace game_b108_03_2 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

struct H {
    // ------------------------------------------------------------------ 0x0045c870
    void* cpu_item_scalar_dtor(u32 flags) {
        joan::orig::as_prof_node(this)->destruct();  // 0x0045ec90
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("CpuUsageItem::scalar_dtor", 0x0045c870, &H::cpu_item_scalar_dtor);
#undef B

}  // namespace game_b108_03_2
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
//
//    - setters 0x00422f70 (+0x66), 0x00422fc0 (+0x6a), 0x0042a1c0 (slot (i-1)*0xf4 + 0x5d),
//      0x0045bf80 (+0x83 = 1); getters 0x004322d0 (float +0xe1), 0x0044cf10 (word +8),
//      0x0044cf30 (byte +0xe), 0x0045cce0 (+0x38 ? *+0x38 : 0); forwards 0x004231c0
//      (0x00417ad0 on +0x57), 0x0045c990 (0x0045e750).
namespace {
namespace game_b92_01 {

using joan::call::thiscall;

struct H {
    void* forward_e750(void* a) { return joan::orig::as_profiler(this)->read_tsc(a); }  // 0x0045c990 -> 0x0045e750
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("CpuUsage::default_timer", 0x0045c990, &H::forward_e750);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

struct H {
    H* ctor_50() {  // 0x0045ec60
        std::memset(static_cast<void*>(this), 0, static_cast<std::uint32_t>(sizeof(jo::ProfNode)));                        // memset (0x50 on x86)
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("CpuUsageCallStackInfo::CpuUsageCallStackInfo", 0x0045ec60, &H::ctor_50);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
//
//      0x0045ed30 Link38::set_owner(owner)   node +0x38 / +0x3c / +0x40; owner +0x18 head, +0x14 tail; ret 4
//
//      0x0045ee70 Link44::set_owner(owner)   node +0x44 / +0x48 / +0x4c; owner +0x1c head, +0x18 tail; ret 4
//
//      0x0045efb0 Link24::set_owner(owner)   node +0x24 / +0x28 / +0x2c; owner +0x34 head, +0x30 tail; ret 4
//
//      0x0045f0f0 Ref::add(const qword*)     first reference copies 8 bytes to +0x10; ++count +0x18; ret 4
namespace {
namespace game_b92_02 {

// Intrusive list relink shared by the five Link*::set_owner functions: node N's owner link `o`, its prev / next
// links `p` / `n` in the owner's list, the owner O's list `head` / `tail`.
template <class N, class O>
void relink(N* self, O* owner, O* N::*o, N* N::*p, N* N::*n, N* O::*head, N* O::*tail) {
    O* const old = self->*o;
    if (owner == old) return;
    if (old != nullptr) {
        N* const prev = self->*p;
        N* const next = self->*n;
        if (prev != nullptr)
            prev->*n = next;
        else
            old->*head = next;
        if (next != nullptr)
            next->*p = prev;
        else
            old->*tail = prev;
        self->*p = nullptr;
        self->*n = nullptr;
        self->*o = nullptr;
    }
    if (owner != nullptr) {
        N* const first = owner->*head;
        owner->*head = self;  // (dead "insert after" branch: local always 0)
        if (first != nullptr)
            first->*p = self;
        else
            owner->*tail = self;
        self->*p = nullptr;
        self->*n = first;
        self->*o = owner;
    }
}

struct H {
    void link38(void* owner) { relink(jo::self<jo::ProfNode>(this), static_cast<jo::ProfFunc*>(owner), &jo::ProfNode::func, &jo::ProfNode::func_prev, &jo::ProfNode::func_next, &jo::ProfFunc::nodes_head, &jo::ProfFunc::nodes_tail); }
    void link44(void* owner) { relink(jo::self<jo::ProfNode>(this), static_cast<jo::ProfEdge*>(owner), &jo::ProfNode::edge, &jo::ProfNode::edge_prev, &jo::ProfNode::edge_next, &jo::ProfEdge::nodes_head, &jo::ProfEdge::nodes_tail); }
    void link24(void* owner) { relink(jo::self<jo::ProfNode>(this), static_cast<jo::ProfNode*>(owner), &jo::ProfNode::parent, &jo::ProfNode::sibling_prev, &jo::ProfNode::sibling_next, &jo::ProfNode::children_head, &jo::ProfNode::children_tail); }

    // 0x0045f0f0
    void ref_add(const std::uint32_t* v) {
        if (jo::self<jo::ProfNode>(this)->nesting == 0) {
            jo::self<jo::ProfNode>(this)->start_[0] = v[0];
            jo::self<jo::ProfNode>(this)->start_[1] = v[1];
        }
        jo::self<jo::ProfNode>(this)->nesting = jo::self<jo::ProfNode>(this)->nesting + 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Link38::set_owner", 0x0045ed30, &H::link38);
B("Link44::set_owner", 0x0045ee70, &H::link44);
B("Link24::set_owner", 0x0045efb0, &H::link24);
B("CpuUsageCallStackInfo::start_check_time", 0x0045f0f0, &H::ref_add);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
//
//      0x0045e750 read_tsc(uint64* out)                        thiscall, ret 4
namespace {
namespace game_b91_02 {

struct H {
    // 0x0045e750
    std::uint32_t* read_tsc(std::uint32_t* out) {
        const unsigned long long t = joan::replay::cpu_tsc();
        const unsigned long long v = joan::replay::tsc(t);  // recorded / replayed ([Debug] Record / Replay)
        out[0] = static_cast<std::uint32_t>(v);
        out[1] = static_cast<std::uint32_t>(v >> 32);
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("CpuUsage::precision_timer", 0x0045e750, &H::read_tsc);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

using joan::hook::global;

using joan::call::thiscall;

using joan::hook::x87::Fpu;

JOAN_COUNTER(c_overlaps, "Rect::overlaps");
JOAN_COUNTER(c_set_scale, "Party::set_scale");

struct H {
    // ---- 0x0045f6b0 Party::set_scale(int) -----------------------------------------------------
    void set_scale(std::int32_t v) {
        c_set_scale.hit();
        jo::self<jo::Panels>(this)->volume = v;
        for (std::int16_t i = 1; static_cast<std::int32_t>(i) <= joan::sound_slots::count(); i = static_cast<std::int16_t>(i + 1)) {
            void** const slot = joan::orig::arg::as<void**>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(i)));  // 0x004cf690
            if (*slot == nullptr) continue;
            const joan::hook::x87::Fpu fpu{true};
            const float s = static_cast<float>(fpu.mul(static_cast<double>(v), static_cast<double>(global<float>(d::flt_0_01))));
            joan::orig::as_sound3d(*slot)->set_gain(joan::orig::arg::bits(s));  // 0x0059f700
        }
    }

    // ---- 0x0045c7f0 Rect::overlaps(x1, y1, x2, y2) ---------------------------------------------
    std::int32_t overlaps(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2) {
        c_overlaps.hit();
        if (jo::self<jo::MouseCursor>(this)->y1 > y1 || jo::self<jo::MouseCursor>(this)->y2 < y1) {
            if (y1 > jo::self<jo::MouseCursor>(this)->y1) return 0;
            if (y2 < jo::self<jo::MouseCursor>(this)->y1) return 0;
        }
        if (jo::self<jo::MouseCursor>(this)->x1 > x1 || jo::self<jo::MouseCursor>(this)->x2 < x1) {
            if (x1 > jo::self<jo::MouseCursor>(this)->x1) return 0;
            if (x2 < jo::self<jo::MouseCursor>(this)->x1) return 0;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("Rect::overlaps", 0x0045c7f0, &H::overlaps);
B("Party::set_scale", 0x0045f6b0, &H::set_scale);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02_2 {

using joan::hook::global;

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

JOAN_COUNTER(c_cpu_measure, "CpuSpeed::measure");

struct H {
    // ------------------------------------------------------------------ 0x0045c8c0
    u32 cpu_measure() {
        c_cpu_measure.hit();
        jo::self<jo::Profiler>(this)->cpu_hz[0] = 0;
        jo::self<jo::Profiler>(this)->cpu_hz[1] = 0;
        using Qpc = i32(JOAN_STDCALL*)(u64*);
        using SleepFn = void(JOAN_STDCALL*)(u32);
        u64 t0 = 0, t1 = 0, q0 = 0, q1 = 0, fq = 0;
        if (joan::orig::arg::as<void*>(joan::orig::as_profiler(this)->read_tsc(joan::orig::arg::mptr(static_cast<void*>(&t0)))) == nullptr) return 0;  // 0x0045e750
        reinterpret_cast<Qpc>(global<std::uintptr_t>(d::iat_KERNEL32_QueryPerformanceCounter))(&q0);   // QueryPerformanceCounter
        reinterpret_cast<SleepFn>(global<std::uintptr_t>(d::iat_KERNEL32_Sleep))(100);  // Sleep
        joan::orig::as_profiler(this)->read_tsc(joan::orig::arg::mptr(static_cast<void*>(&t1)));          // 0x0045e750
        reinterpret_cast<Qpc>(global<std::uintptr_t>(d::iat_KERNEL32_QueryPerformanceCounter))(&q1);
        reinterpret_cast<Qpc>(global<std::uintptr_t>(d::iat_KERNEL32_QueryPerformanceFrequency))(&fq);   // QueryPerformanceFrequency
        const u64 prod = (t1 - t0) * fq;          // _allmul
        const u64 hz = prod / (q1 - q0);          // _aulldiv
        jo::self<jo::Profiler>(this)->cpu_hz[0] = static_cast<u32>(hz);
        jo::self<jo::Profiler>(this)->cpu_hz[1] = static_cast<u32>(hz >> 32);
        return static_cast<u32>(hz);
    }

    // ------------------------------------------------------------------ 0x0045d0f0
    i32 not_after(void* other) {
        const i32 r = joan::orig::arg::as<i32>(joan::orig::as_track_node(this)->compare_keys(joan::orig::arg::i32(jo::self<jo::TrackNode>(this)->cost_total), joan::orig::arg::i32(jo::self<jo::TrackNode>(this)->weight_sum), static_cast<i32>(jo::self<jo::TrackNode>(this)->weight_max), joan::orig::arg::i32(jo::as_track_node(other)->cost_total), joan::orig::arg::i32(jo::as_track_node(other)->weight_sum), static_cast<i32>(jo::as_track_node(other)->weight_max)));  // 0x004eea00
        return r <= 0 ? 1 : 0;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("CpuSpeed::measure", 0x0045c8c0, &H::cpu_measure);
B100("Obj::not_after", 0x0045d0f0, &H::not_after);
#undef B100

}  // namespace game_b100_02_2
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_prof_enter, "Profiler::enter");

using joan::orig::helpers::new_;  // operator new
using joan::orig::helpers::delete_;       // operator delete

struct H {
    // ------------------------------------------------------------------ scalar deleting dtors
    H* sdtor(std::uintptr_t dtor, u32 flags) {
        thiscall(dtor, this);
        if ((flags & 1) != 0) delete_(this);
        return this;
    }
    H* sdtor_45e930(u32 flags) { return sdtor(fn::code_value(fn::Obj_clear_both), flags); }  // 0x0045e9b0

    // ------------------------------------------------------------------ 0x0045c9c0
    u32 prof_enter(i32 id) {
        c_prof_enter.hit();
        if (jo::self<jo::Profiler>(this)->enabled == 0) return joan::call::register_low_bits(this);
        u8* n = jo::view<u8*>(jo::self<jo::Profiler>(this)->current->children_tail);  // ebp-0x14
        while (n != nullptr) {
            if (joan::orig::arg::as<i32>(joan::orig::as_prof_node(n)->func_id()) == id) break;  // 0x0045cce0
            n = jo::view<u8*>(jo::as_prof_node(n)->sibling_prev);
        }
        if (n == nullptr) {
            if (joan::orig::arg::as<i32>(joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::Profiler>(this)->current))->func_id()) == id) n = jo::view<u8*>(jo::self<jo::Profiler>(this)->current);
        }
        if (n == nullptr) {
            for (u8* q = jo::view<u8*>(jo::self<jo::Profiler>(this)->current); q != nullptr; q = jo::view<u8*>(jo::as_prof_node(q)->parent)) {  // ebp-0x18
                if (joan::orig::arg::as<i32>(joan::orig::as_prof_node(q)->func_id()) == id) {
                    joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Cannot_profile_recursive_fun));  // 0x00586cb0 "Cannot profile recursive function"
                }
            }
            u8* const parent = jo::view<u8*>(jo::self<jo::Profiler>(this)->current->func);  // ebp-0x1c
            u8* const rec = reinterpret_cast<u8*>(&jo::self<jo::Profiler>(this)->funcs[id]);        // ebp-0x20 (0x1c-byte records on x86)
            u8* c = nullptr;                                        // ebp-0x24
            if (parent != nullptr) {
                for (c = jo::view<u8*>(jo::as_prof_func(parent)->out_tail); c != nullptr; c = jo::view<u8*>(jo::as_prof_edge(c)->caller_prev)) {
                    if (joan::orig::arg::as<i32>(joan::orig::as_prof_edge(c)->func_id()) == id) break;  // 0x0045ccb0
                }
            }
            if (c == nullptr) {
                void* const mem = new_(static_cast<std::uint32_t>(sizeof(jo::ProfEdge)));  // 0x20 on x86
                c = mem != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_prof_edge(mem)->construct()) : nullptr;  // 0x0045e980
                joan::orig::as_prof_edge(c)->set_caller(joan::orig::arg::mptr(static_cast<void*>(parent)));          // 0x0045e9e0
                joan::orig::as_prof_edge(c)->set_callee(joan::orig::arg::mptr(static_cast<void*>(rec)));             // 0x0045eb20
            }
            void* const mem = new_(static_cast<std::uint32_t>(sizeof(jo::ProfNode)));  // 0x50 on x86
            n = mem != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_prof_node(mem)->construct()) : nullptr;  // 0x0045ec60
            joan::orig::as_prof_node(n)->set_func(joan::orig::arg::mptr(static_cast<void*>(rec)));              // 0x0045ed30
            joan::orig::as_prof_node(n)->set_parent(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Profiler>(this)->current)));                // 0x0045efb0
            joan::orig::as_prof_node(n)->set_edge(joan::orig::arg::mptr(static_cast<void*>(c)));                // 0x0045ee70
        }
        jo::view<u8*>(jo::self<jo::Profiler>(this)->current) = n;
        u32 t[2];                                                         // ebp-0x2c (rdtsc)
        joan::orig::as_profiler(this)->read_clock(static_cast<void*>(t));        // 0x0045c990
        joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::Profiler>(this)->current))->start(joan::orig::arg::mptr(static_cast<void*>(t)));  // 0x0045f0f0
        u8* const counts = jo::view<u8*>(jo::self<jo::Profiler>(this)->depth);
        if (jo::as_arr_u32(counts)[id - 1].value == 0) {
            jo::ArrU64* const starts = jo::self<jo::Profiler>(this)->start;
            starts[id - 1].value[0] = t[0];
            starts[id - 1].value[1] = t[1];
        }
        const u32 v = jo::self<jo::Profiler>(this)->depth[id - 1].value + 1u;
        jo::self<jo::Profiler>(this)->depth[id - 1].value = v;
        return v;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("CpuUsage::start_check_time", 0x0045c9c0, &H::prof_enter);
B98("Obj::scalar_dtor_45e930", 0x0045e930, &H::sdtor_45e930);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0045ccb0 ProfEdge::func_id
    u32 deref_c() { return jo::self<jo::ProfEdge>(this)->callee->id; }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::deref_c", 0x0045ccb0, &H::deref_c);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b92_01_2.cpp (game/b92_01)
namespace {
namespace game_b92_01_2 {

struct H {
    std::uint32_t first_38() { return jo::view<std::uint32_t*>(jo::self<jo::ProfNode>(this)->func) != nullptr ? *jo::view<std::uint32_t*>(jo::self<jo::ProfNode>(this)->func) : 0u; }  // 0x0045cce0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("CpuUsage::get_name", 0x0045cce0, &H::first_38);
#undef B

}  // namespace game_b92_01_2
}  // namespace

// ---------------------------------------- from modules/game_b99_01_2.cpp (game/b99_01)
namespace {
namespace game_b99_01_2 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

JOAN_COUNTER(c_prof_leave, "Profiler::leave");
JOAN_COUNTER(c_free_lists, "Profiler::free_lists");

struct H {
    // ------------------------------------------------------------------ 0x0045cd20
    std::uintptr_t prof_leave(i32 id) {
        c_prof_leave.hit();
        if (jo::self<jo::Profiler>(this)->enabled == 0) return joan::call::register_low_bits(this);
        u64 t;                                                         // ebp-0xc (rdtsc)
        joan::orig::as_profiler(this)->read_clock(static_cast<void*>(&t));    // 0x0045c990
        u32& cnt = jo::self<jo::Profiler>(this)->depth[id - 1].value;
        cnt = cnt - 1u;
        if (jo::self<jo::Profiler>(this)->depth[id - 1].value == 0) {
            const u64 d = t - jo::view<u64>(jo::self<jo::Profiler>(this)->start[id - 1].value);
            jo::view<u64>(jo::self<jo::Profiler>(this)->last[id - 1].value) = d;
            const u64 tot = jo::view<u64>(jo::self<jo::Profiler>(this)->total[id - 1].value) + jo::view<u64>(jo::self<jo::Profiler>(this)->last[id - 1].value);
            jo::view<u64>(jo::self<jo::Profiler>(this)->total[id - 1].value) = tot;
            const i32 k = (id - 1) * 0x3c + jo::self<jo::Profiler>(this)->frame_slot;
            const u64 s = jo::view<u64>(jo::self<jo::Profiler>(this)->history[k].value) + jo::view<u64>(jo::self<jo::Profiler>(this)->last[id - 1].value);
            jo::view<u64>(jo::self<jo::Profiler>(this)->history[k].value) = s;
            const i32 k2 = (id - 1) * 0x3c + jo::self<jo::Profiler>(this)->frame_slot;
            jo::self<jo::Profiler>(this)->history_hits[k2].value = jo::self<jo::Profiler>(this)->history_hits[k2].value + 1u;
        }
        jo::ProfNode* const n = joan::orig::arg::as<jo::ProfNode*>(joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::Profiler>(this)->current))->stop(joan::orig::arg::mptr(static_cast<void*>(&t))));  // 0x0045f140
        jo::self<jo::Profiler>(this)->current = n;
        return joan::orig::arg::as<std::uintptr_t>(n);
    }

    // ------------------------------------------------------------------ 0x0045e860
    void free_lists() {
        c_free_lists.hit();
        for (u8* n = jo::view<u8*>(jo::self<jo::ProfFunc>(this)->out_head); n != nullptr;) {
            u8* const next = jo::view<u8*>(jo::as_prof_edge(n)->caller_next);
            joan::orig::as_prof_edge(n)->destroy(1u);  // 0x0045e930 scalar dtor
            n = next;
        }
        for (u8* n = jo::view<u8*>(jo::self<jo::ProfFunc>(this)->in_head); n != nullptr;) {
            u8* const next = jo::view<u8*>(jo::as_prof_edge(n)->caller_next);
            joan::orig::as_prof_edge(n)->destroy(1u);
            n = next;
        }
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Profiler::leave", 0x0045cd20, &H::prof_leave);
B99("Profiler::free_lists", 0x0045e860, &H::free_lists);
#undef B99

}  // namespace game_b99_01_2
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02_2 {

using u32 = std::uint32_t;

// 0x0045cec0
const char* JOAN_CDECL score_name(std::int32_t i) {
    return reinterpret_cast<const char*>(img::addr(d::g_9fb2b8 + static_cast<std::uint32_t>(i) * 0x32u));
}

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("CpuUsageIDPool::id_to_usage_name", 0x0045cec0, &score_name);
#undef B

}  // namespace game_b91_02_2
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using joan::hook::global;

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using i64 = std::int64_t;

struct alignas(4) Path {  // 0x194 bytes in the frames (0x005a1170 writes byte 0)
    u8 bytes[0x194];
};
static_assert(sizeof(Path) == 0x194, "Path");

void* const kFmt = obj(d::g_misc);

using joan::orig::helpers::strlen_;
using joan::orig::helpers::strcpy_;
using joan::orig::helpers::memset_;

void path_ctor(Path& p) { joan::orig::as_string(&p)->construct(); }               // 0x005a1170
void path_assign(Path& p, const char* s) { joan::orig::as_string(&p)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void path_append(Path& p, const char* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }  // 0x005a1550
const char* path_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
const char* fmt_num(double v, i32 prec) { return joan::orig::arg::as<const char*>(joan::orig::as_misc(kFmt)->format_double(joan::orig::arg::f64(v), joan::orig::arg::i32(prec))); }  // 0x0058fe20

JOAN_COUNTER(c_prof_frame, "Profiler::frame");
JOAN_COUNTER(c_prof_report, "Profiler::report");

struct H {
    // ------------------------------------------------------------------------ 0x0045cef0
    u32 prof_frame(const u64* delta) {
        c_prof_frame.hit();
        if (jo::self<jo::Profiler>(this)->enabled == 0) return joan::call::register_low_bits(this);  // eax left = this; the only caller discards it (low half on x64)
        u64 stamp = 0;
        void* p = joan::orig::as_profiler(this)->read_clock(&stamp);  // 0x0045c990 (rdtsc)
        joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::Profiler>(this)->root))->stop(joan::orig::arg::mptr(p));        // 0x0045f140
        const u64 elapsed = static_cast<u64>(global<u32>(d::g_game_time_ms)) - reinterpret_cast<const u64&>(jo::self<jo::Profiler>(this)->last_report);  // last_report, u32_5c
        if (elapsed >= 0xbb8u) {
            if (jo::self<jo::Profiler>(this)->frames != 0) {
                // 0x007b2f30 is _aulldiv (unsigned); the divisor is the cdq-extended +8.
                for (i32 i = 1; i <= jo::self<jo::Profiler>(this)->func_count; ++i) {
                    const u64 n = static_cast<u64>(static_cast<i64>(jo::self<jo::Profiler>(this)->frames));
                    jo::view<u64*>(jo::self<jo::Profiler>(this)->average)[i - 1] = jo::view<u64*>(jo::self<jo::Profiler>(this)->total)[i - 1] / n;
                }
                jo::view<u64>(jo::self<jo::Profiler>(this)->frame_avg) = jo::view<u64>(jo::self<jo::Profiler>(this)->time_accum) / static_cast<u64>(static_cast<i64>(jo::self<jo::Profiler>(this)->frames));
                joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::Profiler>(this)->root))->rescale(joan::orig::arg::i32(jo::self<jo::Profiler>(this)->frames));  // 0x0045f1d0
                memset_(jo::view<void*>(jo::self<jo::Profiler>(this)->total), 0, static_cast<u32>(jo::self<jo::Profiler>(this)->func_count) << 3);
                jo::self<jo::Profiler>(this)->frames = 0;
                jo::self<jo::Profiler>(this)->time_accum[0] = 0;
                jo::self<jo::Profiler>(this)->time_accum[1] = 0;
            }
            jo::self<jo::Profiler>(this)->last_report = global<u32>(d::g_game_time_ms);
            jo::self<jo::Profiler>(this)->u32_5c = 0;
        }
        jo::self<jo::Profiler>(this)->frames = jo::self<jo::Profiler>(this)->frames + 1;
        jo::self<jo::Profiler>(this)->frame_slot = jo::self<jo::Profiler>(this)->frame_slot + 1;
        if (!(jo::self<jo::Profiler>(this)->frame_slot < 0x3c)) jo::self<jo::Profiler>(this)->frame_slot = 0;
        for (i32 i = 1; i <= jo::self<jo::Profiler>(this)->func_count; ++i) {
            const i32 k = (i - 1) * 0x3c + jo::self<jo::Profiler>(this)->frame_slot;
            jo::view<u64*>(jo::self<jo::Profiler>(this)->history)[k] = 0;
            jo::view<u32*>(jo::self<jo::Profiler>(this)->history_hits)[k] = 0;
        }
        jo::view<u64>(jo::self<jo::Profiler>(this)->time_accum) = jo::view<u64>(jo::self<jo::Profiler>(this)->time_accum) + *delta;
        p = joan::orig::as_profiler(this)->read_clock(&stamp);                // 0x0045c990
        joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::Profiler>(this)->root))->start(p);          // 0x0045f0f0
        return 0;  // callers (0x00499960 and its replacement) discard the result; Ref::add returns nothing
    }

    // ------------------------------------------------------------------------ 0x0045e140
    // Emits one line: 1 = no lines left, 2 = does not fit, 0 = copied (buf / lines advanced).
    static i32 emit(char*& buf, u32 size, i32& lines, const char* s) {
        if (!(lines > 0)) return 1;
        if (!(strlen_(s) + 1 < size)) return 2;
        strcpy_(buf, s);
        buf += size;
        lines = lines - 1;
        return 0;
    }
    static i32 emit_path(char*& buf, u32 size, i32& lines, Path& p) {
        if (!(lines > 0)) return 1;
        if (!(strlen_(path_str(p)) + 1 < size)) return 2;
        strcpy_(buf, path_str(p));
        buf += size;
        lines = lines - 1;
        return 0;
    }
    i32 prof_report(char* buf, u32 size, i32 lines) {
        c_prof_report.hit();
        i32 r = 0;
        if (jo::self<jo::Profiler>(this)->enabled == 0) return emit(buf, size, lines, str(d::s_Not_Inited));  // "Not Inited"
        if (jo::self<jo::Profiler>(this)->root != nullptr) {
            r = joan::orig::arg::as<i32>(joan::orig::as_prof_node(jo::view<void*>(jo::self<jo::Profiler>(this)->root))->dump_text(&buf, joan::orig::arg::u32(size), &lines, 0));  // 0x0045f4c0
            if (r != 0) return r;
        }
        if ((r = emit(buf, size, lines, str(d::s_sp_9d05f0))) != 0) return r;
        if ((r = emit(buf, size, lines, str(d::s_dash40))) != 0) return r;  // "-----..."
        if ((r = emit(buf, size, lines, str(d::s_sp_9d05f0))) != 0) return r;
        if ((r = emit(buf, size, lines, str(d::s_sp_9d05f0))) != 0) return r;
        Path p;
        path_ctor(p);
        for (i32 i = 1; i <= jo::self<jo::Profiler>(this)->func_count; ++i) {
            if (jo::view<u64>(jo::self<jo::Profiler>(this)->frame_avg) == 0) {
                path_assign(p, fmt_num(0.0, 3));
            } else {
                const double v = joan::orig::arg::as<double>(joan::orig::as_profiler(this)->percent(joan::orig::arg::i32(i)));  // 0x0045d1f0
                path_assign(p, fmt_num(v, 3));
            }
            path_append(p, str(d::blob_9d0588));  // "\t["
            const double ms = joan::orig::arg::as<double>(joan::orig::as_timer(this)->elapsed_ms(&jo::view<i64*>(jo::self<jo::Profiler>(this)->average)[i - 1]));  // 0x0045e7f0
            path_append(p, fmt_num(ms, 1));
            path_append(p, str(d::s_ms_9d0580));                                      // " ms]\t"
            path_append(p, joan::orig::arg::as<const char*>(joan::orig::score_name(joan::orig::arg::i32(i))));               // 0x0045cec0
            if ((r = emit_path(buf, size, lines, p)) != 0) return r;
        }
        if ((r = emit(buf, size, lines, str(d::s_sp_9d05f0))) != 0) return r;
        const float avg = joan::orig::arg::as<float>(joan::orig::as_timer(this)->elapsed_ms(joan::orig::arg::mptr(jo::self<jo::Profiler>(this)->frame_avg)));  // 0x0045e7f0
        path_assign(p, str(d::s_Average_frame_time));  // "Average frame time : "
        path_append(p, fmt_num(static_cast<double>(avg), 1));
        path_append(p, str(d::s_ms_9d0544));  // " ms"
        if (avg >= global<float>(d::flt_1)) {  // fcomp + test ah,1: skipped when < or NaN
            path_append(p, str(d::s_sp_lparen));    // " ("
            const float fps = global<float>(d::flt_1000) / avg;
            path_append(p, fmt_num(static_cast<double>(fps), 1));
            path_append(p, str(d::s_fps));  // " fps)"
        }
        if ((r = emit_path(buf, size, lines, p)) != 0) return r;
        return 0;
    }
    void* this_at(i32 off) { return reinterpret_cast<u8*>(this) + off; }

    // ------------------------------------------------------------------------ 0x0045ec90
    u32 prof_clear() {
        u8* node = jo::view<u8*>(jo::self<jo::ProfNode>(this)->children_head);
        while (node != nullptr) {
            u8* const next = jo::view<u8*>(jo::as_prof_node(node)->sibling_next);
            joan::orig::as_prof_node(node)->destroy(1u);  // 0x0045c870 (deleting dtor)
            node = next;
        }
        joan::orig::as_prof_node(this)->set_parent(joan::orig::arg::mptr(0));         // 0x0045efb0
        joan::orig::as_prof_node(this)->set_edge(joan::orig::arg::mptr(0));         // 0x0045ee70
        joan::orig::as_prof_node(this)->set_func(nullptr);  // 0x0045ed30
        return 0;  // callers (0x0045c870 and its replacement) discard the result; Link38::set_owner returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("CpuUsage::next_frame", 0x0045cef0, &H::prof_frame);
B("CpuUsage::dump_45e140", 0x0045e140, &H::prof_report);
B("Profiler::clear", 0x0045ec90, &H::prof_clear);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i64 = std::int64_t;

using joan::hook::x87::Fpu;

// Counter i (1-based, as the original indexes it: base + i * 8 - 8) of a Profiler counter array: ArrU64 is one 64-bit
// counter kept as two dwords.
i64 counter(const jo::ArrU64* arr, i32 i) { return *reinterpret_cast<const i64*>(arr + (i - 1)); }

constexpr Fpu kFpu{true};

struct H {
    // ---------------------------------------------------------------- 0x0045d170 / 0x0045d2b0
    static bool exact53(i64 v) { return v <= (i64{1} << 53) && v >= -(i64{1} << 53); }
    float ratio_45d170(i32 i, const i64* total) {
        if (total == nullptr || jo::self<jo::Profiler>(this)->frames == 0) return 0.0f;
        const i64 a = counter(jo::self<jo::Profiler>(this)->total, i);
        const i64 t = *total;
        if (!exact53(a) || !exact53(t)) return thiscall<float>(fn::Stats_ratio_45d170_body, this, i, total);
        double r = kFpu.mul(static_cast<double>(a), 100.0);
        r = kFpu.div(r, static_cast<double>(jo::self<jo::Profiler>(this)->frames));
        r = kFpu.div(r, static_cast<double>(t));
        return static_cast<float>(r);
    }
    float ratio_45d2b0(i32 i, u32 unused, i32 j) {
        const i64 b = counter(jo::self<jo::Profiler>(this)->average, j);
        if (b == 0) return 0.0f;
        const i64 a = counter(jo::self<jo::Profiler>(this)->average, i);
        if (!exact53(a) || !exact53(b)) return thiscall<float>(fn::Stats_ratio_45d2b0_body, this, i, unused, j);
        const double r = kFpu.mul(static_cast<double>(a), 100.0);
        return static_cast<float>(kFpu.div(r, static_cast<double>(b)));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Stats::ratio_45d170", 0x0045d170, &H::ratio_45d170);
B("Stats::ratio_45d2b0", 0x0045d2b0, &H::ratio_45d2b0);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i64 = std::int64_t;

using joan::hook::x87::Fpu;

struct H {
    // ------------------------------------------------------------------------ 0x0045d1f0
    double percent(i32 i) {
        if ((jo::self<jo::Profiler>(this)->frame_avg[0] | jo::self<jo::Profiler>(this)->frame_avg[1]) == 0) return global<float>(d::k_9cf5c4);
        using joan::hook::x87::Precision;
        const i64* const arr = jo::view<i64*>(jo::self<jo::Profiler>(this)->average);
        const i64 a = arr[i - 1], b = jo::view<i64>(jo::self<jo::Profiler>(this)->frame_avg);  // tick sums, far below 2^53
        const Fpu fpu{joan::hook::x87::current_precision() == Precision::Single};
        const double v = fpu.mul(static_cast<double>(a), global<double>(d::dbl_100));  // 100.0
        return fpu.div(v, static_cast<double>(b));
    }

    // ------------------------------------------------------------------------ 0x0045f140
    void* timer_stop(const i64* now) {
        jo::self<jo::ProfNode>(this)->nesting = jo::self<jo::ProfNode>(this)->nesting - 1;
        if (jo::self<jo::ProfNode>(this)->nesting != 0) return this;
        jo::view<i64>(jo::self<jo::ProfNode>(this)->last) = static_cast<i64>(static_cast<std::uint64_t>(*now) - static_cast<std::uint64_t>(jo::view<i64>(jo::self<jo::ProfNode>(this)->start_)));
        jo::view<i64>(jo::self<jo::ProfNode>(this)->total) = static_cast<i64>(static_cast<std::uint64_t>(jo::view<i64>(jo::self<jo::ProfNode>(this)->total)) + static_cast<std::uint64_t>(jo::view<i64>(jo::self<jo::ProfNode>(this)->last)));
        return jo::view<void*>(jo::self<jo::ProfNode>(this)->parent);
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("CpuUsage::calculate_average_percentage", 0x0045d1f0, &H::percent);
B93("ProfTimer::stop", 0x0045f140, &H::timer_stop);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b111_16.cpp (game/b111_16)
namespace {
namespace game_b111_16 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i64 = std::int64_t;

using joan::hook::x87::Fpu;

struct H {
    // -------------------------------------------------------------- 0x0045d260
    double percent(i32 idx, const i64* p) {
        if (p == nullptr) return global<float>(d::k_9cf5c4);  // 0.0f
        const i64 a = jo::view<i64>(jo::self<jo::Profiler>(this)->average[idx - 1].value);
        const i64 b = *p;
        constexpr i64 kExact = i64{1} << 53;
        if (a > kExact || a < -kExact || b > kExact || b < -kExact) {
            using Fn = double (H::*)(i32, const i64*);
            return (this->*joan::hook::original<Fn>(fn::Stats_percent_45d260_body))(idx, p);  // untouched body
        }
        const Fpu fpu{true};
        return fpu.div(fpu.mul(static_cast<double>(a), global<double>(d::dbl_100)), static_cast<double>(b));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_16", name, addr, fn)
B("Stats::percent_45d260", 0x0045d260, &H::percent);
#undef B

}  // namespace game_b111_16
}  // namespace

// ---------------------------------------- from modules/game_b109_01.cpp (game/b109_01)
namespace {
namespace game_b109_01 {

using joan::hook::global;

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using joan::hook::x87::Fpu;

// stdcall through an IAT slot.
template <class R = void, class... A>
R import_call(img::Addr slot, A... args) {
    return (*reinterpret_cast<R(JOAN_STDCALL* const*)(A...)>(img::addr(slot)))(args...);
}
u8* bp(const void* p) { return static_cast<u8*>(const_cast<void*>(p)); }

JOAN_COUNTER(c_perf, "PerfStats::dump_45d330");

// ---------------------------------------------------------------- 0x0045d330 helpers
void* const kNum = obj(d::g_misc);
u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
void file_lit(void* file, img::Addr s) {
    const u32 n = crt_strlen(str(s));
    joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(str(s)), joan::orig::arg::u32(n));  // 0x005872e0
}
void sb_append(void* sb, const char* s) { joan::orig::as_string(sb)->append(joan::orig::arg::str(s)); }  // 0x005a1550
void sb_assign(void* sb, const char* s) { joan::orig::as_string(sb)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void sb_flush(void* file, void* sb) {
    const u32 len = joan::orig::arg::as<u32>(joan::orig::as_name(sb)->length());                    // 0x0045e110
    const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(sb)->c_str());  // 0x00417d90
    joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(s), joan::orig::arg::u32(len));  // 0x005872e0
}
const char* num_text(double v, i32 digits) { return joan::orig::arg::as<const char*>(joan::orig::as_misc(kNum)->format_double(joan::orig::arg::f64(v), joan::orig::arg::i32(digits))); }  // 0x0058fe20

struct H {
    // -------------------------------------------------------------- 0x0045d330
    // Original frame [ebp-0x150]..[ebp-0x10]: the 0x18-byte time buffer is followed by the
    // locals it can overrun (tod, date, local / system FILETIME - dead after the format call)
    // and then the live file object, exactly as in the original.
    struct PerfFrame {
        char tbuf[0x18];  // [ebp-0x150]
        float tod;        // [ebp-0x138]
        u32 date;         // [ebp-0x134]
        u32 lft[2];       // [ebp-0x130]
        u32 ft[2];        // [ebp-0x128]
        jo::File file;    // [ebp-0x120] (a 0x110-byte slot; a real File: x86 0x10e, x64 0x112 bytes)
    };
    static_assert(offsetof(PerfFrame, file) == 0x30, "PerfFrame: no padding between the frame slots");
    u32 perf_dump(const char* path) {
        c_perf.hit();
        alignas(4) PerfFrame fr;
        void* const file = &fr.file;
        alignas(4) u8 sb[0x194];
        joan::orig::as_file(file)->construct();               // 0x004179c0
        joan::orig::as_file(file)->file_create(joan::orig::arg::str(path), 1, static_cast<char>(0));  // 0x00586f00
        if (jo::self<jo::PerfStats>(this)->enabled == 0) {
            file_lit(file, d::s_Not_Inited);  // "Not Inited"
            file_lit(file, d::blob_9d05dc);
            joan::orig::as_file(file)->destruct_5872b0();  // 0x005872b0
            return 0;  // callers (0x0052f930 and its replacement) discard the result; ~File returns nothing
        }
        fr.ft[0] = fr.ft[1] = fr.lft[0] = fr.lft[1] = 0;
        import_call<void>(d::iat_KERNEL32_GetSystemTimeAsFileTime, static_cast<void*>(fr.ft));                                    // GetSystemTimeAsFileTime
        import_call<i32>(d::iat_KERNEL32_FileTimeToLocalFileTime, static_cast<const void*>(fr.ft), static_cast<void*>(fr.lft));  // FileTimeToLocalFileTime
        fr.date = joan::orig::arg::as<u32>(joan::orig::as_date(obj(d::g_date))->file_time_to_julian(joan::orig::arg::mptr(static_cast<void*>(fr.ft))));    // 0x00580350
        fr.tod = joan::orig::arg::as<float>(joan::orig::as_date(obj(d::g_date))->file_time_to_minutes(joan::orig::arg::mptr(static_cast<void*>(fr.lft))));  // 0x005803c0
        joan::orig::as_string(sb)->construct(str(d::s_Date));      // 0x005a11a0 "Date: "
        sb_append(sb, joan::orig::arg::as<const char*>(joan::orig::as_date(obj(d::g_date))->date_str(joan::orig::arg::i32(fr.date), 1, 0)));  // 0x0057fc30
        sb_append(sb, str(d::s_Time));  // "  Time: "
        sb_append(sb, joan::orig::arg::as<const char*>(joan::orig::as_date(obj(d::g_date))->time_str(joan::orig::arg::str(static_cast<char*>(fr.tbuf)), joan::orig::arg::f32(fr.tod), static_cast<char>(joan::orig::arg::i32(0)), static_cast<char>(joan::orig::arg::i32(1)))));  // 0x00580510
        sb_append(sb, str(d::blob_9d05c0));  // "\r\n\r\n"
        sb_flush(file, sb);
        if (jo::self<jo::PerfStats>(this)->root != nullptr) joan::orig::as_prof_node(jo::view<u8*>(jo::self<jo::PerfStats>(this)->root))->dump_file(joan::orig::arg::mptr(static_cast<void*>(file)), 0);  // 0x0045f270
        file_lit(file, d::blob_9d05dc);
        file_lit(file, d::s_dash40);
        file_lit(file, d::blob_9d05dc);
        file_lit(file, d::blob_9d05dc);
        for (i32 i = 1; i <= jo::self<jo::PerfStats>(this)->func_count; ++i) {
            if ((jo::self<jo::PerfStats>(this)->frame_avg[0] | jo::self<jo::PerfStats>(this)->frame_avg[1]) == 0) {
                sb_assign(sb, num_text(0.0, 3));
            } else {
                const double avg = joan::orig::arg::as<double>(joan::orig::as_profiler(this)->percent(joan::orig::arg::i32(i)));  // 0x0045d1f0
                sb_assign(sb, num_text(avg, 3));
            }
            sb_append(sb, str(d::blob_9d0588));
            {
                u8* const rec = bp(&jo::self<jo::PerfStats>(this)->average[i - 1]);
                const double v = joan::orig::arg::as<double>(joan::orig::as_timer(this)->elapsed_ms(joan::orig::arg::mptr(rec)));  // 0x0045e7f0
                sb_append(sb, num_text(v, 1));
            }
            sb_append(sb, str(d::s_ms_9d0580));  // " ms]\t"
            sb_append(sb, joan::orig::arg::as<const char*>(joan::orig::score_name(joan::orig::arg::i32(i))));  // 0x0045cec0
            sb_append(sb, str(d::blob_9d05dc));
            sb_flush(file, sb);
            sb_assign(sb, str(d::s_last_records));  // "last records\n"
            sb_flush(file, sb);
            for (i32 j = 0; j < 0x3c; ++j) {
                sb_assign(sb, joan::orig::arg::as<const char*>(joan::orig::as_misc(kNum)->format_int_58fca0(joan::orig::arg::i32(j + 1), 1)));  // 0x0058fca0
                sb_append(sb, str(d::blob_9d0588));
                const u32 k = static_cast<u32>(i - 1) * 0x3cu + static_cast<u32>(j);
                const double v = joan::orig::arg::as<double>(joan::orig::as_timer(this)->elapsed_ms(joan::orig::arg::mptr(bp(&jo::self<jo::PerfStats>(this)->history[k]))));  // 0x0045e7f0
                sb_append(sb, num_text(v, 1));
                sb_append(sb, str(d::s_ms_9d0568));  // "ms]\t"
                joan::orig::as_string(sb)->append(joan::orig::arg::i32(jo::view<i32>(jo::self<jo::PerfStats>(this)->history_hits[k].value)));  // 0x005a15d0
                sb_append(sb, str(d::kNl));
                sb_flush(file, sb);
            }
        }
        file_lit(file, d::blob_9d05dc);
        const float avg = joan::orig::arg::as<float>(joan::orig::as_timer(this)->elapsed_ms(joan::orig::arg::mptr(jo::self<jo::PerfStats>(this)->frame_avg)));  // 0x0045e7f0
        sb_assign(sb, str(d::s_Average_frame_time));  // "Average frame time : "
        sb_append(sb, num_text(static_cast<double>(avg), 1));
        sb_append(sb, str(d::s_ms_9d0544));  // " ms"
        if (avg >= 1.0f) {  // fcomp [0x9cf784], test ah,1: less or unordered skips
            sb_append(sb, str(d::s_sp_lparen));
            const double fps = Fpu{true}.div(1000.0, static_cast<double>(avg));  // [0x9cf778] / avg
            sb_append(sb, num_text(fps, 1));
            sb_append(sb, str(d::s_fps));  // " fps)"
        }
        sb_append(sb, str(d::blob_9d05dc));
        sb_flush(file, sb);
        file_lit(file, d::blob_9d05dc);
        file_lit(file, d::s_dash40);
        file_lit(file, d::blob_9d05dc);
        file_lit(file, d::blob_9d05dc);
        static const img::Addr kStats[][2] = {
            {d::s_Triangle_per_frame, d::g_a526bf}, {d::s_DrawPrimitive_calls_per_fram, d::g_a526c7}, {d::s_State_change_calls_per_frame, d::g_a526c3},
            {d::s_Switch_vertex_buffer_per_fra, d::g_a526cb}, {d::s_Material_texture_changes_per, d::g_a526cf}, {d::s_Grouped_DrawPrimitive, d::g_a526d3},
            {d::s_Total_Scene_Sector_Count, d::g_a526d7}, {d::s_Scene_Sector_Tested, d::g_a526db}, {d::s_Scene_Sector_Rendered, d::g_a526df},
            {d::s_Scene_Actor_Tested, d::g_a526e3}, {d::s_Scene_Actor_Rendered, d::g_a526e7},
        };
        alignas(4) char nbuf[0x18];
        for (const auto& s : kStats) {
            sb_assign(sb, str(s[0]));
            sb_append(sb, joan::orig::arg::as<const char*>(joan::orig::libc::_itoa(joan::orig::arg::i32(global<i32>(s[1])), joan::orig::arg::str(static_cast<char*>(nbuf)), 10)));  // _itoa
            sb_append(sb, str(d::blob_9d05dc));
            sb_flush(file, sb);
        }
        file_lit(file, d::blob_9d05dc);
        file_lit(file, d::s_dash40);
        file_lit(file, d::blob_9d05dc);
        joan::orig::as_file(file)->file_close();           // 0x00587260
        joan::orig::as_file(file)->destruct_5872b0();   // 0x005872b0
        return 0;  // callers (0x0052f930 and its replacement) discard the result; ~File returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_01", name, addr, fn)
B("CpuUsage::dump_45d330", 0x0045d330, &H::perf_dump);
#undef B

}  // namespace game_b109_01
}  // namespace

// ---------------------------------------- from modules/game_b89_03.cpp (game/b89_03)
namespace {
namespace game_b89_03 {

using joan::hook::global;

using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::cdecl_call;
using joan::img::obj;

using joan::hook::x87::Fpu;

bool equal_or_nan(float a, float b) { return !(a < b || a > b); }

constexpr img::Addr kGameUnits(d::g_units);
constexpr img::Addr kKinds(d::g_types);

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* kind_get(std::int32_t k) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(img::obj(kKinds))->get(joan::orig::arg::i16(k))); }  // 0x0042d0a0

JOAN_COUNTER(c_nearest, "Group::nearest_free_member");

struct H {
    // 0x0045de50
    std::int32_t nearest_free_member(const float* pos, const float* target, const float* offset, std::uint8_t* used,
                                     std::int32_t mode, std::int32_t /*unused*/, std::int32_t tolerance) {
        c_nearest.hit();
        float best;
        {
            const std::uint32_t bits = 0x501502f9u;  // 1e10
            std::memcpy(&best, &bits, 4);
        }
        std::int32_t found = 0;
        std::int32_t same = 0;
        float same_dist = 0.0f;  // only read when same != 0, which also sets it
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::Group>(this)->count); ++i) {
            if (static_cast<std::int8_t>(used[i]) != 0) continue;
            if (unit_missing(jo::self<jo::Group>(this)->members[i]) != 0) continue;
            std::uint8_t* const u = unit_get(jo::self<jo::Group>(this)->members[i]);
            if (mode == 1) {
                if (jo::as_unit_type(kind_get(jo::as_unit(u)->type_id))->unit_class != 1 &&
                    jo::as_unit_type(kind_get(jo::as_unit(u)->type_id))->unit_class != 4)
                    continue;
            } else if (mode == 2) {
                if (jo::as_unit_type(kind_get(jo::as_unit(u)->type_id))->unit_class != 5 &&
                    jo::as_unit_type(kind_get(jo::as_unit(u)->type_id))->unit_class != 6 &&
                    jo::as_unit_type(kind_get(jo::as_unit(u)->type_id))->unit_class != 8)
                    continue;
            } else if (mode == 3) {
                if (jo::as_unit_type(kind_get(jo::as_unit(u)->type_id))->unit_class != 3 &&
                    jo::as_unit_type(kind_get(jo::as_unit(u)->type_id))->unit_class != 2)
                    continue;
            }
            const float dx = reinterpret_cast<const float*>(jo::as_unit(u)->position_)[0] - jo::self<jo::Group>(this)->front_x;
            const float dy = reinterpret_cast<const float*>(jo::as_unit(u)->position_)[1] - jo::self<jo::Group>(this)->front_y;
            const float d2 = dx * dx + dy * dy;
            float tx, ty;
            if (static_cast<double>(global<std::int32_t>(d::g_a3cc74)) > static_cast<double>(d2)) {
                tx = reinterpret_cast<const float*>(jo::as_unit(u)->position_)[0] + offset[0];
                ty = reinterpret_cast<const float*>(jo::as_unit(u)->position_)[1] + offset[1];
            } else {
                tx = reinterpret_cast<const float*>(jo::as_unit(u)->position_)[0];
                ty = reinterpret_cast<const float*>(jo::as_unit(u)->position_)[1];
            }
            const float ex = pos[0] - tx;
            const float ey = pos[1] - ty;
            const float sum = ex * ex + ey * ey;
            const float dist = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sum)))));  // sqrt
            if (!(dist > best)) {
                best = dist;
                found = i + 1;
            }
            if (equal_or_nan(target[0], jo::as_unit(u)->formation_grid_x) && equal_or_nan(target[1], jo::view<float>(jo::as_unit(u)->form_slot_y))) {
                same = i + 1;
                same_dist = dist;
            }
        }
        if (same != 0 && same != found) {
            const double limit = joan::hook::x87::Fpu{true}.add(static_cast<double>(tolerance), static_cast<double>(best));
            if (limit > static_cast<double>(same_dist)) found = same;
        }
        used[found - 1] = 1;
        return found;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_03", name, addr, fn)
M("Group::nearest_free_member", 0x0045de50, &H::nearest_free_member);
#undef M

}  // namespace game_b89_03
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::img::str;

std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }

struct H {
    std::uint32_t name_length() { return strlen_(reinterpret_cast<const char*>(this)); }  // 0x0045e110
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Name::length", 0x0045e110, &H::name_length);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::hook::global;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::obj;

using joan::call::import_call;
struct RV3 {  // 12-byte vector copied as raw dwords
    u32 x, y, z;
};
static_assert(sizeof(RV3) == 12, "RV3");

void* const kUnitTable = obj(d::g_units);

JOAN_COUNTER(c_slot_of, "Squad::slot_of");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
RV3 vsub(const void* a, const void* b) {  // 0x0060cc90 (a - b), result copied out of the temp
    RV3 tmp;
    return *joan::orig::arg::as<RV3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
}

struct H {
    // ------------------------------------------------------------------ 0x0045e620
    i32 slot_of(i32 id) {
        c_slot_of.hit();
        float best = 0.0f;    // ebp-0x8
        float result = 0.0f;  // ebp-0xc
        if (unit_missing(id) != 0) return 0;
        u8* const unit = unit_get(id);
        for (i32 i = 1; i <= jo::self<jo::Place>(this)->slot_count; ++i) {
            if (jo::self<jo::Place>(this)->slot_unit[i] != 0) {
                if (jo::self<jo::Place>(this)->slot_unit[i] == id) return i;
                continue;
            }
            RV3 d = vsub(&jo::self<jo::Place>(this)->slot_pos_[(i - 1) * 3], jo::as_unit(unit)->position_);  // +0xc6 + (i - 1) * 0xc, unit +0xe
            const float ls = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<void*>(&d)));  // length_sq
            const float zero = global<float>(d::k_9cf5c4);
            if (!(best < zero || best > zero) || best > ls) {  // best == 0 (or NaN) || best > ls
                best = ls;
                result = static_cast<float>(i);
            }
        }
        return joan::hook::x87::ftol(static_cast<double>(result));
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Squad::slot_of", 0x0045e620, &H::slot_of);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::obj;

using joan::orig::helpers::at;

using joan::orig::helpers::memset_;

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // ------------------------------------------------------------------ 0x0045e780
    u32 get_44_45e780(u32* out) {
        out[0] = jo::self<jo::Profiler>(this)->cpu_hz[0];
        out[1] = jo::self<jo::Profiler>(this)->cpu_hz[1];
        // mov eax, [this+0x48] ... mov al, flag: upper 24 bits of eax come from dword +0x48.
        const u32 flag = (jo::self<jo::Profiler>(this)->cpu_hz[0] | jo::self<jo::Profiler>(this)->cpu_hz[1]) != 0 ? 1u : 0u;
        return (jo::self<jo::Profiler>(this)->cpu_hz[1] & 0xffffff00u) | flag;
    }
    // ------------------------------------------------------------------ 0x0045e830
    void* clear_45e830() {
        memset_(this, 0, static_cast<u32>(sizeof(jo::ProfFunc)));  // ProfFunc ctor (0x1c bytes on x86)
        return this;
    }
    // ------------------------------------------------------------------ 0x0045f730
    // Profiler scope guard (class unknown, no C++ caller): one 4-byte id at +0, an integer on both widths.
    struct ScopeId {
        u32 id;
    };
    void* set_45f730(u32 v) {
        ScopeId* const s = static_cast<ScopeId*>(static_cast<void*>(this));
        s->id = v;
        joan::orig::as_profiler(obj(d::g_cpu_usage))->enter(joan::orig::arg::i32(s->id));  // 0x0045c9c0
        return this;
    }
    // ------------------------------------------------------------------ 0x0045f770
    u32 call_45cd20_45f770() {
        return joan::orig::arg::as<u32>(joan::orig::as_profiler(obj(d::g_cpu_usage))->leave(joan::orig::arg::i32(static_cast<ScopeId*>(static_cast<void*>(this))->id)));  // 0x0045cd20
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::get_44_45e780", 0x0045e780, &H::get_44_45e780);
B("Obj::clear_45e830", 0x0045e830, &H::clear_45e830);
B("Obj::set_45f730", 0x0045f730, &H::set_45f730);
B("Obj::call_45cd20_45f770", 0x0045f770, &H::call_45cd20_45f770);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

using joan::hook::global;

using joan::hook::x87::Fpu;

JOAN_COUNTER(c_elapsed, "Timer::elapsed_ms");

struct H {
    // 0x0045e7f0 Timer::elapsed_ms(const int64* ticks): ticks * 1000.0 / +0x44 (st0).
    double elapsed_ms(const std::int64_t* ticks) {
        using joan::hook::x87::Precision;
        const std::int64_t a = *ticks, b = jo::view<std::int64_t>(jo::self<jo::Profiler>(this)->cpu_hz);  // tick delta, rate: far below 2^53
        c_elapsed.hit();
        const joan::hook::x87::Fpu fpu{joan::hook::x87::current_precision() == Precision::Single};
        const double v = fpu.mul(static_cast<double>(a), global<double>(d::dbl_1000));  // 1000.0
        return fpu.div(v, static_cast<double>(b));
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("CpuUsage::precision_timer_to_ms", 0x0045e7f0, &H::elapsed_ms);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

void* memset_(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    void* clear_20() {
        memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::ProfEdge)));  // 0x20 on x86
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::clear_20", 0x0045e980, &H::clear_20);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b94_01_2.cpp (game/b94_01)
namespace {
namespace game_b94_01_2 {

using joan::call::thiscall;

struct H {
    // 0x0045e9b0
    void clear_both() {
        joan::orig::as_prof_edge(this)->set_caller(joan::orig::arg::mptr(0));  // 0x0045e9e0
        joan::orig::as_prof_edge(this)->set_callee(joan::orig::arg::mptr(0));  // 0x0045eb20
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::clear_both", 0x0045e9b0, &H::clear_both);
#undef B94

}  // namespace game_b94_01_2
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02_2 {

// Intrusive list relink shared by the five Link*::set_owner functions: node N's owner link `o`, its prev / next
// links `p` / `n` in the owner's list, the owner O's list `head` / `tail`.
template <class N, class O>
void relink(N* self, O* owner, O* N::*o, N* N::*p, N* N::*n, N* O::*head, N* O::*tail) {
    O* const old = self->*o;
    if (owner == old) return;
    if (old != nullptr) {
        N* const prev = self->*p;
        N* const next = self->*n;
        if (prev != nullptr)
            prev->*n = next;
        else
            old->*head = next;
        if (next != nullptr)
            next->*p = prev;
        else
            old->*tail = prev;
        self->*p = nullptr;
        self->*n = nullptr;
        self->*o = nullptr;
    }
    if (owner != nullptr) {
        N* const first = owner->*head;
        owner->*head = self;  // (dead "insert after" branch: local always 0)
        if (first != nullptr)
            first->*p = self;
        else
            owner->*tail = self;
        self->*p = nullptr;
        self->*n = first;
        self->*o = owner;
    }
}

struct H {
    // 0x0045e9e0 .. 0x0045efb0
    void link00(void* owner) { relink(jo::self<jo::ProfEdge>(this), static_cast<jo::ProfFunc*>(owner), &jo::ProfEdge::caller, &jo::ProfEdge::caller_prev, &jo::ProfEdge::caller_next, &jo::ProfFunc::out_head, &jo::ProfFunc::out_tail); }
    void link0c(void* owner) { relink(jo::self<jo::ProfEdge>(this), static_cast<jo::ProfFunc*>(owner), &jo::ProfEdge::callee, &jo::ProfEdge::callee_prev, &jo::ProfEdge::callee_next, &jo::ProfFunc::in_head, &jo::ProfFunc::in_tail); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Link00::set_owner", 0x0045e9e0, &H::link00);
B("Link0c::set_owner", 0x0045eb20, &H::link0c);
#undef B

}  // namespace game_b92_02_2
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

struct H {
    // ------------------------------------------------------------------ 0x0045f1d0
    void rescale(i32 div) {
        if (div == 0) return;
        const u64 q = jo::view<u64>(jo::self<jo::ProfNode>(this)->total) / static_cast<u64>(static_cast<std::int64_t>(div));  // __aulldiv
        jo::self<jo::ProfNode>(this)->average[0] = static_cast<u32>(q);
        jo::self<jo::ProfNode>(this)->average[1] = static_cast<u32>(q >> 32);
        jo::view<i32>(jo::self<jo::ProfNode>(this)->total[0]) = 0;
        jo::view<i32>(jo::self<jo::ProfNode>(this)->total[1]) = 0;
        for (void* child = jo::view<void*>(jo::self<jo::ProfNode>(this)->children_tail); child != nullptr; child = jo::view<void*>(jo::as_prof_node(child)->sibling_prev)) {
            joan::orig::as_prof_node(child)->rescale(joan::orig::arg::i32(div));  // 0x0045f1d0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Clock::rescale", 0x0045f1d0, &H::rescale);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using joan::hook::global;

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using i64 = std::int64_t;

using joan::hook::x87::Fpu;

constexpr Fpu kFpu{true};


void* const kUtil = obj(d::g_misc);

JOAN_COUNTER(c_prof_dump, "Profiler::dump");

struct H {
    // ------------------------------------------------------------------ 0x0045f270
    void profiler_dump(void* out, i32 depth) {
        c_prof_dump.hit();
        for (i32 n = depth * 3; n > 0;) {
            const i32 k = n >= 0x15 ? 0x15 : n;
            joan::orig::as_file(out)->file_write(joan::orig::arg::ptr(str(d::s_colon_sp_sp_colon_sp_sp_colo)), joan::orig::arg::u32(k));  // 0x005872e0
            n -= k;
        }
        alignas(4) char sb[0x194];               // [ebp-0x1a4] fixed string (0x190 + NUL)
        joan::orig::as_string(sb)->construct();  // 0x005a1170
        if (jo::self<jo::ProfNode>(this)->parent != nullptr) {
            void* const parent = jo::view<void*>(jo::self<jo::ProfNode>(this)->parent);
            const char* pct;
            if ((jo::as_prof_node(parent)->average[0] | jo::as_prof_node(parent)->average[1]) == 0) {
                pct = joan::orig::arg::as<char*>(joan::orig::as_misc(kUtil)->format_double(joan::orig::arg::f64(0.0), 3));  // 0x0058fe20
            } else {
                const double v = kFpu.div(kFpu.mul(static_cast<double>(jo::view<i64>(jo::self<jo::ProfNode>(this)->average)), global<double>(d::dbl_100)),
                                          static_cast<double>(jo::view<i64>(jo::self<jo::ProfNode>(this)->parent->average)));
                pct = joan::orig::arg::as<char*>(joan::orig::as_misc(kUtil)->format_double(joan::orig::arg::f64(v), 3));  // 0x0058fe20
            }
            joan::orig::as_string(sb)->assign(joan::orig::arg::str(pct));  // 0x005a1440 assign
            joan::orig::as_string(sb)->append(str(d::s_sp_lbrack));                // 0x005a1550 append " ["
            const double ms = joan::orig::arg::as<double>(joan::orig::as_timer(obj(d::g_cpu_usage))->elapsed_ms(joan::orig::arg::mptr(jo::self<jo::ProfNode>(this)->average)));  // 0x0045e7f0
            joan::orig::as_string(sb)->append(joan::orig::arg::str(joan::orig::arg::as<char*>(joan::orig::as_misc(kUtil)->format_double(joan::orig::arg::f64(ms), 1))));
            joan::orig::as_string(sb)->append(str(d::s_ms_9d05f4));                // " ms] "
            const i32 id = joan::orig::arg::as<i32>(joan::orig::as_prof_node(this)->func_id());     // 0x0045cce0
            joan::orig::as_string(sb)->append(joan::orig::arg::str(joan::orig::arg::as<char*>(joan::orig::score_name(joan::orig::arg::i32(id)))));  // 0x0045cec0
            joan::orig::as_string(sb)->append(str(d::blob_9d05dc));                // "\r\n"
            const i32 len = joan::orig::arg::as<i32>(joan::orig::as_name(sb)->length());       // 0x0045e110
            char* const text = joan::orig::as_string(sb)->c_str();  // 0x00417d90
            joan::orig::as_file(out)->file_write(joan::orig::arg::ptr(text), joan::orig::arg::u32(len));  // 0x005872e0
        }
        for (void* child = jo::view<void*>(jo::self<jo::ProfNode>(this)->children_tail); child != nullptr; child = jo::view<void*>(jo::as_prof_node(child)->sibling_prev))
            joan::orig::as_prof_node(child)->dump_file(joan::orig::arg::mptr(out), joan::orig::arg::i32(depth + 1));  // 0x0045f270
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("CpuUsageCallStackInfo::dump_45f270", 0x0045f270, &H::profiler_dump);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0045f450
    std::int16_t first_free() {
        for (std::int16_t i = 1; i <= joan::sound_slots::count(); i = static_cast<std::int16_t>(i + 1)) {
            void** const p = joan::orig::arg::as<void**>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(i)));  // 0x004cf690
            if (*p == nullptr) continue;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sound3d(*p)->queued_59fd60()) > 0) continue;  // 0x0059fd60
            return i;
        }
        if constexpr (!joan::kReleaseBuild) {  // every slot busy: the caller drops its sound (game/sound_slots.hpp)
            static std::uint32_t full = 0;
            const std::uint32_t now = ++full;
            if ((now & (now - 1)) == 0)
                joan::hook::log_line("[Sound] all %d sound slots busy: a sound was not played (%u times)", joan::sound_slots::count(), full);
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Party::first_free", 0x0045f450, &H::first_free);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
namespace {
namespace game_b108_05 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using i64 = std::int64_t;

using joan::hook::x87::Fpu;


JOAN_COUNTER(c_prof_dump, "ProfNode::dump_45f4c0");

struct H {
    // ------------------------------------------------------------------ 0x0045f4c0
    i32 prof_dump(char** out, u32 cap, i32* lines, i32 depth) {
        c_prof_dump.hit();
        alignas(4) char text[0x194];
        void* const s = text;
        joan::orig::as_string(s)->construct();  // 0x005a1170
        if (depth > 0) {
            joan::orig::as_string(s)->assign(str(d::s_colon_sp_sp));            // 0x005a1440 ":  "
            joan::orig::as_string(s)->repeat(joan::orig::arg::i32(depth));  // 0x005a1640
        }
        if (jo::self<jo::ProfNode>(this)->parent != nullptr) {
            void* const parent = jo::view<void*>(jo::self<jo::ProfNode>(this)->parent);
            if ((jo::as_prof_node(parent)->average[0] | jo::as_prof_node(parent)->average[1]) == 0) {
                const char* const r = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_double(joan::orig::arg::f64(0.0), i32{3}));  // 0x0058fe20
                joan::orig::as_string(s)->append(joan::orig::arg::str(r));  // 0x005a1550
            } else {
                const Fpu fpu{true};
                const double num = fpu.mul(static_cast<double>(jo::view<i64>(jo::self<jo::ProfNode>(this)->average)), 100.0);  // [0x9d03c0]
                const double pct = fpu.div(num, static_cast<double>(jo::view<i64>(jo::as_prof_node(parent)->average)));
                const char* const r = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_double(joan::orig::arg::f64(pct), i32{3}));  // 0x0058fe20
                joan::orig::as_string(s)->append(joan::orig::arg::str(r));  // 0x005a1550
            }
            joan::orig::as_string(s)->append(str(d::s_sp_lbrack));                // 0x005a1550
            const double ms = joan::orig::arg::as<double>(joan::orig::as_timer(obj(d::g_cpu_usage))->elapsed_ms(joan::orig::arg::mptr(jo::self<jo::ProfNode>(this)->average)));  // 0x0045e7f0
            const char* const r2 = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_double(joan::orig::arg::f64(ms), i32{1}));  // 0x0058fe20
            joan::orig::as_string(s)->append(joan::orig::arg::str(r2));       // 0x005a1550
            joan::orig::as_string(s)->append(str(d::s_ms_9d05f4));                // 0x005a1550 " ms] "
            const u32 k = joan::orig::arg::as<u32>(joan::orig::as_prof_node(this)->func_id());    // 0x0045cce0
            const char* const kname = joan::orig::arg::as<const char*>(joan::orig::score_name(joan::orig::arg::i32(k)));  // 0x0045cec0
            joan::orig::as_string(s)->append(joan::orig::arg::str(kname));  // 0x005a1550
            if (!(*lines > 0)) return 1;
            const u32 n = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_string(s)->c_str()))));  // strlen(0x00417d90)
            if (!(n + 1u < cap)) return 2;
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(*out)), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_string(s)->c_str())));  // strcpy
            *out = *out + cap;
            *lines = *lines - 1;
        }
        for (void* child = jo::view<void*>(jo::self<jo::ProfNode>(this)->children_tail); child != nullptr; child = jo::view<void*>(jo::as_prof_node(child)->sibling_prev)) {
            joan::orig::as_prof_node(child)->dump_text(joan::orig::arg::mptr(out), joan::orig::arg::u32(cap), joan::orig::arg::mptr(lines), joan::orig::arg::i32(depth + 1));  // 0x0045f4c0
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("CpuUsageCallStackInfo::dump_45f4c0", 0x0045f4c0, &H::prof_dump);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Profiler::shutdown() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_19::H::call_45c010>(this)); }  // 0x45c010
auto joan::orig::Profiler::init(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_15::H::cpu_init>(this, a0)); }  // 0x45c040
auto joan::orig::Nation::remove_member(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b88_05::H::station_leave>(this, a0)); }  // 0x45c460
auto joan::orig::ProfFunc::vector_destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_02::H::vector_deleting_dtor>(this, a0)); }  // 0x45c520
auto joan::orig::Profiler::release() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b108_03::H::cpu_release>(this)); }  // 0x45c5c0
auto joan::orig::Rect::overlaps(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::overlaps>(this, a0, a1, a2, a3)); }  // 0x45c7f0
auto joan::orig::ProfNode::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b108_03_2::H::cpu_item_scalar_dtor>(this, a0)); }  // 0x45c870
auto joan::orig::Profiler::measure_speed() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_02_2::H::cpu_measure>(this)); }  // 0x45c8c0
auto joan::orig::Profiler::read_clock(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_01::H::forward_e750>(this, a0)); }  // 0x45c990
auto joan::orig::Profiler::enter(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_01::H::prof_enter>(this, a0)); }  // 0x45c9c0
auto joan::orig::ProfEdge::func_id() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_01::H::deref_c>(this)); }  // 0x45ccb0
auto joan::orig::ProfNode::func_id() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_01_2::H::first_38>(this)); }  // 0x45cce0
auto joan::orig::Profiler::leave(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_01_2::H::prof_leave>(this, a0)); }  // 0x45cd20
auto joan::orig::score_name(std::int32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::game_b91_02_2::score_name>(a0)); }  // 0x45cec0
auto joan::orig::Profiler::frame(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_17::H::prof_frame>(this, a0)); }  // 0x45cef0
auto joan::orig::TrackNode::sorts_before(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_02_2::H::not_after>(this, a0)); }  // 0x45d0f0
auto joan::orig::Profiler::percent(std::int32_t a0) -> double { return joan::bind::conv<double>(joan::bind::member<&::game_b93_01::H::percent>(this, a0)); }  // 0x45d1f0
auto joan::orig::Profiler::dump_log(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_01::H::perf_dump>(this, a0)); }  // 0x45d330
auto joan::orig::Name::length() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b85_06::H::name_length>(this)); }  // 0x45e110
auto joan::orig::Squad::slot_of(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_01::H::slot_of>(this, a0)); }  // 0x45e620
auto joan::orig::Profiler::read_tsc(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::read_tsc>(this, a0)); }  // 0x45e750
auto joan::orig::Timer::elapsed_ms(void* a0) -> double { return joan::bind::conv<double>(joan::bind::member<&::game_leaves82b::H::elapsed_ms>(this, a0)); }  // 0x45e7f0
auto joan::orig::ProfFunc::destruct() -> void { joan::bind::member<&::game_b99_01_2::H::free_lists>(this); }  // 0x45e860
auto joan::orig::ProfEdge::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_01::H::sdtor_45e930>(this, a0)); }  // 0x45e930
auto joan::orig::ProfEdge::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_01::H::clear_20>(this)); }  // 0x45e980
auto joan::orig::ProfEdge::set_caller(void* a0) -> void { joan::bind::member<&::game_b92_02_2::H::link00>(this, a0); }  // 0x45e9e0
auto joan::orig::ProfEdge::set_callee(void* a0) -> void { joan::bind::member<&::game_b92_02_2::H::link0c>(this, a0); }  // 0x45eb20
auto joan::orig::ProfNode::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_01::H::ctor_50>(this)); }  // 0x45ec60
auto joan::orig::ProfNode::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_17::H::prof_clear>(this)); }  // 0x45ec90
auto joan::orig::ProfNode::set_func(void* a0) -> void { joan::bind::member<&::game_b92_02::H::link38>(this, a0); }  // 0x45ed30
auto joan::orig::ProfNode::set_edge(void* a0) -> void { joan::bind::member<&::game_b92_02::H::link44>(this, a0); }  // 0x45ee70
auto joan::orig::ProfNode::set_parent(void* a0) -> void { joan::bind::member<&::game_b92_02::H::link24>(this, a0); }  // 0x45efb0
auto joan::orig::ProfNode::start(void* a0) -> void { joan::bind::member<&::game_b92_02::H::ref_add>(this, a0); }  // 0x45f0f0
auto joan::orig::ProfNode::stop(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_01::H::timer_stop>(this, a0)); }  // 0x45f140
auto joan::orig::ProfNode::rescale(std::int32_t a0) -> void { joan::bind::member<&::game_b108_18::H::rescale>(this, a0); }  // 0x45f1d0
auto joan::orig::ProfNode::dump_file(void* a0, std::int32_t a1) -> void { joan::bind::member<&::game_b108_13::H::profiler_dump>(this, a0, a1); }  // 0x45f270
auto joan::orig::Party::first_free() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b83_07::H::first_free>(this)); }  // 0x45f450
auto joan::orig::ProfNode::dump_text(void* a0, std::uint32_t a1, void* a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_05::H::prof_dump>(this, a0, a1, a2, a3)); }  // 0x45f4c0
auto joan::orig::Party::set_scale(std::int32_t a0) -> void { joan::bind::member<&::game_b84_05::H::set_scale>(this, a0); }  // 0x45f6b0
