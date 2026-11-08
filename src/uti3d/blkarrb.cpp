// uti3d/blkarrb.cpp: functions of the original d:\ww\uti3d\blkarrb.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstddef>
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

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

using joan::img::obj;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::str;

using joan::call::thiscall;

}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
//
//      0x00637700 BlockArray::alloc(src) (ret 4)        - blkarrb.cpp, returns a 1-based id
//
//      0x006375b0 BlockArray::free_all()                - blkarrb.cpp
//
//      0x006376d0 BlockArray::reset()                   - blkarrb.cpp
namespace {
namespace game_b84_04 {

void* const kMemory = obj(d::g_mem);  // allocator (0x0058e7b0 alloc / 0x0058eb30 realloc / 0x0058ed70 free)
constexpr img::Addr kBlkarrbCpp(d::s_d_ww_uti3d_blkarrb_cpp);  // "d:\\ww\\uti3d\\blkarrb.cpp"

using joan::orig::helpers::line_base;

JOAN_COUNTER(c_blk_alloc, "BlockArray::alloc");
JOAN_COUNTER(c_blk_free, "BlockArray::free_all");

struct H {
    void blk_reset() {  // 0x006376d0
        jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->free_count) = 0;
    }

    // ---- uti3d/blkarrb.cpp ---------------------------------------------------------------
    // +0 block pointers, +4 capacity, +8 element size, +0xc per block, +0x10 used ids,
    // +0x14 blocks, +0x18 free ids, +0x1c free count, +0x20.
    void blk_free_all() {  // 0x006375b0
        c_blk_free.hit();
        for (std::int32_t i = jo::self<jo::BlockArray>(this)->block_num - 1; i >= 0; --i)
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void**>(jo::self<jo::BlockArray>(this)->blocks)[i]), joan::orig::arg::str(img::str(kBlkarrbCpp)), joan::orig::arg::i32(line_base(d::g_a197c4) + 3));  // 0x0058ed70
        jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->block_num) = 0;
        if (jo::view<void*>(jo::self<jo::BlockArray>(this)->blocks) != nullptr) joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::BlockArray>(this)->blocks)), joan::orig::arg::str(img::str(kBlkarrbCpp)), joan::orig::arg::i32(line_base(d::g_a197c4) + 8));
        jo::self<jo::BlockArray>(this)->blocks = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->block_cap) = 0;
        if (jo::view<void*>(jo::self<jo::BlockArray>(this)->free_ids) != nullptr) joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::BlockArray>(this)->free_ids)), joan::orig::arg::str(img::str(kBlkarrbCpp)), joan::orig::arg::i32(line_base(d::g_a197c4) + 0xe));
        jo::self<jo::BlockArray>(this)->free_ids = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->free_count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->free_cap) = 0;
    }
    std::uint32_t blk_alloc(const void* src) {  // 0x00637700
        c_blk_alloc.hit();
        std::uint32_t id = 0;
        if (jo::self<jo::BlockArray>(this)->free_count > 0) {
            id = jo::view<std::uint32_t*>(jo::self<jo::BlockArray>(this)->free_ids)[jo::self<jo::BlockArray>(this)->free_count - 1];
            jo::self<jo::BlockArray>(this)->free_count = jo::self<jo::BlockArray>(this)->free_count - 1;
        } else {
            jo::self<jo::BlockArray>(this)->count = jo::self<jo::BlockArray>(this)->count + 1;
            id = jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->count);
            if (jo::self<jo::BlockArray>(this)->count > static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::view<std::int32_t>(jo::self<jo::BlockArray>(this)->per_block)) *
                                                                  static_cast<std::uint32_t>(jo::self<jo::BlockArray>(this)->block_num))) {
                const std::int32_t old = jo::self<jo::BlockArray>(this)->block_num;
                jo::self<jo::BlockArray>(this)->block_num = (jo::self<jo::BlockArray>(this)->count + jo::view<std::int32_t>(jo::self<jo::BlockArray>(this)->per_block) - 1) / jo::view<std::int32_t>(jo::self<jo::BlockArray>(this)->per_block);
                if (jo::self<jo::BlockArray>(this)->block_num > jo::self<jo::BlockArray>(this)->block_cap) {
                    jo::self<jo::BlockArray>(this)->block_cap = jo::self<jo::BlockArray>(this)->block_num + 0xf;
                    jo::view<void*>(jo::self<jo::BlockArray>(this)->blocks) = joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::BlockArray>(this)->blocks)), joan::orig::arg::u32(static_cast<std::uint32_t>(jo::self<jo::BlockArray>(this)->block_cap) * static_cast<std::uint32_t>(sizeof(void*))), joan::orig::arg::str(img::str(kBlkarrbCpp)), joan::orig::arg::i32(line_base(d::g_a197c8) + 0x1a));  // 0x0058eb30
                }
                for (std::int32_t i = old; i < jo::self<jo::BlockArray>(this)->block_num; ++i) {
                    void* const block = joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(jo::self<jo::BlockArray>(this)->per_block * jo::self<jo::BlockArray>(this)->ele_size), joan::orig::arg::str(img::str(kBlkarrbCpp)), joan::orig::arg::i32(line_base(d::g_a197c8) + 0x20));  // 0x0058e7b0
                    jo::view<void**>(jo::self<jo::BlockArray>(this)->blocks)[i] = block;
                }
            }
        }
        const std::uint32_t per = jo::self<jo::BlockArray>(this)->per_block;
        std::uint8_t* const elem = jo::view<std::uint8_t**>(jo::self<jo::BlockArray>(this)->blocks)[(id - 1) / per] + ((id - 1) % per) * jo::self<jo::BlockArray>(this)->ele_size;
        if (src != nullptr)
            std::memcpy(static_cast<void*>(elem), joan::orig::arg::ptr(src), joan::orig::arg::u32(jo::self<jo::BlockArray>(this)->ele_size));  // memmove
        else
            std::memset(static_cast<void*>(elem), 0, joan::orig::arg::u32(jo::self<jo::BlockArray>(this)->ele_size));  // memset
        return id;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("BlockedArrayB::linkin", 0x00637700, &H::blk_alloc);
M("BlockedArrayB::~BlockedArrayB", 0x006375b0, &H::blk_free_all);
M("BlockedArrayB::zap", 0x006376d0, &H::blk_reset);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- from modules/game_ui7.cpp (game/ui7)
//
//      - BlockArray::at 0x00637920 (uti3d/blkarrb.cpp, 1-based index): block[(i-1) / per] +
//        ((i-1) % per) * stride, unsigned division. ret 4.
namespace {
namespace game_ui7 {

struct H {
    // 0x00637920 BlockArray::at (+0 blocks, +8 stride, +0xc per block). ret 4.
    void* block_at(std::uint32_t index) {
        const std::uint32_t k = index - 1u;
        const std::uint32_t per = jo::self<jo::BlockArray>(this)->per_block;
        const std::uint32_t block = k / per;
        const std::uint32_t rem = k % per;
        std::uint8_t* const elem = jo::view<std::uint8_t**>(jo::self<jo::BlockArray>(this)->blocks)[block] + rem * jo::self<jo::BlockArray>(this)->ele_size;
        return elem;
    }
};

#define U7(name, addr, fn) JOAN_REPLACE("game/ui7", name, addr, fn)
U7("BlockedArrayB::get", 0x00637920, &H::block_at);
#undef U7

}  // namespace game_ui7
}  // namespace

// ---------------------------------------- from modules/game_b100_12.cpp (game/b100_12)
//
//    - Pool::set 0x00637980 (src, n), ret 8: memcpy(src) into element n-1 of a paged pool.
namespace {
namespace game_b100_12 {

using joan::orig::helpers::crt_memcpy;

struct H {
    // ------------------------------------------------------------------ 0x00637980
    void* pool_set(const void* src, u32 n) {
        const u32 per = jo::self<jo::BlockArray>(this)->per_block;
        void* const page = jo::view<u8**>(jo::self<jo::BlockArray>(this)->blocks)[(n - 1u) / per];
        void* const dst = static_cast<u8*>(page) + ((n - 1u) % per) * jo::self<jo::BlockArray>(this)->ele_size;
        return crt_memcpy(dst, src, jo::self<jo::BlockArray>(this)->ele_size);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_12", name, addr, fn)
B("BlockedArrayB::update", 0x00637980, &H::pool_set);
#undef B

}  // namespace game_b100_12
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
//
//      0x006379f0 BlkArr::push(v)                   (thiscall, ret 4) - debug realloc growth.
namespace {
namespace game_b96_06 {

using i16 = std::int16_t;
using i32 = std::int32_t;

void* const kHeap = obj(d::g_mem);   // debug allocator

JOAN_COUNTER(c_blk_push, "BlkArr::push");

struct H {
    // ------------------------------------------------------------------ 0x006379f0
    void blk_push(u32 v) {
        c_blk_push.hit();
        if (!(jo::self<jo::BlkArr>(this)->free_count < jo::self<jo::BlkArr>(this)->free_cap)) {
            jo::self<jo::BlkArr>(this)->free_cap = jo::self<jo::BlkArr>(this)->free_count + jo::view<i32>(jo::self<jo::BlkArr>(this)->per_block) + 1;
            jo::view<void*>(jo::self<jo::BlkArr>(this)->free_ids) = joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::BlkArr>(this)->free_ids)), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::BlkArr>(this)->free_cap) << 2), str(d::s_d_ww_uti3d_blkarrb_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a197cc)) + 0x15));  // 0x0058eb30
        }
        jo::view<u32>(jo::self<jo::BlkArr>(this)->free_ids[jo::self<jo::BlkArr>(this)->free_count].value) = v;
        jo::self<jo::BlkArr>(this)->free_count = jo::self<jo::BlkArr>(this)->free_count + 1;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("BlockedArrayB::linkout", 0x006379f0, &H::blk_push);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b83_05.cpp (game/b83_05)
//
//      - BlkArr::BlkArr 0x00637aa0 (a, b) ret 8, BlkArr::~BlkArr 0x00637b00 ret 0,
//        BlkArr::reset 0x00637bb0 ret 0, BlkArr::add 0x00637cb0 (count) -> first index + 1, ret 4.
namespace {
namespace game_b83_05 {

constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kBlkArrFile(d::s_d_ww_uti3d_blkarrb_cpp);  // "d:\ww\uti3d\blkarrb.cpp"

JOAN_COUNTER(c_blk_add, "BlkArr::add");

struct H {
    // 0x00637aa0. ret 8.
    void* blk_ctor(std::uint32_t a, std::uint32_t b) {
        joan::orig::as_block_array(this)->construct(joan::orig::arg::u32(a), joan::orig::arg::u32(b));  // 0x00637520
        jo::self<jo::BlkArr>(this)->ids = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_cap) = 0;
        jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_count) = 0;
        return this;
    }

    // 0x00637b00 (SEH frame only for the base destructor on unwinding). ret 0.
    void blk_dtor() {
        if (jo::self<jo::BlkArr>(this)->ids != nullptr) {
            joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::BlkArr>(this)->ids)), joan::orig::arg::str(img::str(kBlkArrFile)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a197d0)) + 2));  // 0x0058ed70
        }
        jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_cap) = 0;
        jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_count) = 0;
        joan::orig::as_block_array(this)->free_all();  // 0x006375b0
    }

    // 0x00637bb0. ret 0 (the original kept the eax of 0x006376d0; every caller discards it).
    std::uint32_t blk_reset() {
        jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_count) = 0;
        joan::orig::as_block_array(this)->reset();  // 0x006376d0
        return 0;  // void callee; callers discard the result
    }

    // 0x00637cb0. ret 4.
    std::int32_t blk_add(std::int32_t n) {
        c_blk_add.hit();
        if (static_cast<std::int32_t>(jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_count) + static_cast<std::uint32_t>(n)) > jo::self<jo::BlkArr>(this)->id_cap) {
            const std::uint32_t grow = jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_count) + static_cast<std::uint32_t>(n) + jo::self<jo::BlkArr>(this)->per_block;
            jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_cap) = jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_cap) + grow;
            jo::view<void*>(jo::self<jo::BlkArr>(this)->ids) = joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::BlkArr>(this)->ids)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_cap) << 2), joan::orig::arg::str(img::str(kBlkArrFile)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a197d8)) + 4));  // 0x0058eb30
        }
        const std::int32_t first = jo::self<jo::BlkArr>(this)->id_count;
        for (; n > 0; --n) {
            const std::uint32_t item = joan::orig::arg::as<std::uint32_t>(joan::orig::as_block_array(this)->alloc(joan::orig::arg::mptr(0)));  // 0x00637700
            jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->ids[jo::self<jo::BlkArr>(this)->id_count].value) = item;
            jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_count) = jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_count) + 1;
        }
        return first + 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_05", name, addr, fn)
B("BlockedArray::add_blank", 0x00637cb0, &H::blk_add);
B("BlockedArray::~BlockedArray", 0x00637b00, &H::blk_dtor);
B("BlockedArray::BlockedArray", 0x00637aa0, &H::blk_ctor);
B("BlockedArray::zap", 0x00637bb0, &H::blk_reset);
#undef B

}  // namespace game_b83_05
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
//
//      0x00637be0 BlockArray::add(int)                          thiscall, ret 4
namespace {
namespace game_b92_05 {

void* const kMem = obj(d::g_mem);

void* mem_realloc(void* p, std::int32_t size, img::Addr file, std::int32_t line) {
    return joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058eb30
}
using joan::orig::helpers::line_base;

JOAN_COUNTER(c_block_add, "BlockArray::add");

struct H {
    // 0x00637be0
    std::int32_t block_add(void* v) {
        c_block_add.hit();
        if (!(jo::self<jo::BlkArr>(this)->id_count < jo::self<jo::BlkArr>(this)->id_cap)) {
            jo::self<jo::BlkArr>(this)->id_cap = jo::self<jo::BlkArr>(this)->id_cap + (jo::self<jo::BlkArr>(this)->id_count + jo::view<std::int32_t>(jo::self<jo::BlkArr>(this)->per_block));
            jo::view<void*>(jo::self<jo::BlkArr>(this)->ids) = mem_realloc(jo::view<void*>(jo::self<jo::BlkArr>(this)->ids), static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_cap) << 2),
                                         d::s_d_ww_uti3d_blkarrb_cpp, line_base(d::g_a197d4) + 4);  // 'd:\ww\uti3d\blkarrb.cpp'
        }
        const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::as_block_array(this)->alloc(v));  // 0x00637700
        jo::self<jo::BlkArr>(this)->ids[jo::self<jo::BlkArr>(this)->id_count].value = r;
        jo::self<jo::BlkArr>(this)->id_count = jo::self<jo::BlkArr>(this)->id_count + 1;
        return jo::self<jo::BlkArr>(this)->id_count;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("BlockedArray::linkin", 0x00637be0, &H::block_add);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
//
//      0x00637520 BlockArray::BlockArray(size, per) (ret 8)
namespace {
namespace game_b84_04_2 {

struct H {
    void* blk_ctor(std::uint32_t size, std::uint32_t per_block) {  // 0x00637520
        jo::self<jo::BlockArray>(this)->blocks = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->block_cap) = 0;
        jo::self<jo::BlockArray>(this)->ele_size = size;
        jo::self<jo::BlockArray>(this)->per_block = per_block;
        jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->block_num) = 0;
        jo::self<jo::BlockArray>(this)->free_ids = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->free_count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::BlockArray>(this)->free_cap) = 0;
        return this;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("BlockArray::BlockArray", 0x00637520, &H::blk_ctor);
#undef M

}  // namespace game_b84_04_2
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
//
//    - List::remove_at 0x00637e20 (i), ret 4 -> this.
namespace {
namespace game_b97_06 {

using joan::call::thiscall;
using joan::img::obj;

struct H {
    // 0x00637e20
    void* remove_at(std::int32_t i) {
        joan::orig::as_blk_arr(this)->push(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->ids[i - 1].value)));  // 0x006379f0
        joan::orig::as_misc(obj(d::g_misc))->del_array_rec_591070(joan::orig::arg::str(jo::self<jo::BlkArr>(this)->ids), joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::BlkArr>(this)->id_count)), 4, joan::orig::arg::i32(i));  // 0x00591070
        jo::self<jo::BlkArr>(this)->id_count = jo::self<jo::BlkArr>(this)->id_count - 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("BlockedArray::linkout", 0x00637e20, &H::remove_at);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_ui2.cpp (game/ui2)
namespace {
namespace game_ui2 {

#define U2(name, addr, fn) JOAN_REPLACE("game/ui2", name, addr, fn)
U2("BlockedArray::get", 0x00637da0, &joan::orig::Billboard::item);
#undef U2

}  // namespace game_ui2
}  // namespace

// ---------------------------------------- from modules/game_b103_13.cpp (game/b103_13)
namespace {
namespace game_b103_13 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
struct H {
    void* copy_637de0(void* a, u32 i) {  // 0x00637de0
        const u32 v = jo::view<u32>(jo::self<jo::BlkArr>(this)->ids[static_cast<i32>(i - 1u)].value);
        return joan::orig::as_pool(this)->set(a, joan::orig::arg::u32(v));  // 0x00637980
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_13", name, addr, fn)
B("BlockedArray::update", 0x00637de0, &H::copy_637de0);
#undef B

}  // namespace game_b103_13
}  // namespace

// Out-of-line definition (declared in original/en3d.hpp).
namespace {
using joan::call::thiscall;
}  // namespace

// 0x00637da0. ret 4.
void* joan::orig::Billboard::item(std::int32_t index) {
    return joan::orig::as_block_array(this)->at(joan::orig::arg::u32(items[static_cast<std::ptrdiff_t>(index) - 1]));  // 0x00637920
}

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::BlockArray::construct(std::uint32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_04_2::H::blk_ctor>(this, a0, a1)); }  // 0x637520
auto joan::orig::BlockArray::free_all() -> void { joan::bind::member<&::game_b84_04::H::blk_free_all>(this); }  // 0x6375b0
auto joan::orig::BlockArray::reset() -> void { joan::bind::member<&::game_b84_04::H::blk_reset>(this); }  // 0x6376d0
auto joan::orig::BlockArray::alloc(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_04::H::blk_alloc>(this, a0)); }  // 0x637700
auto joan::orig::BlockArray::at(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_ui7::H::block_at>(this, a0)); }  // 0x637920
auto joan::orig::Pool::set(void* a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_12::H::pool_set>(this, a0, a1)); }  // 0x637980
auto joan::orig::BlkArr::push(std::uint32_t a0) -> void { joan::bind::member<&::game_b96_06::H::blk_push>(this, a0); }  // 0x6379f0
auto joan::orig::BlkArr::construct(std::uint32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_05::H::blk_ctor>(this, a0, a1)); }  // 0x637aa0
auto joan::orig::BlkArr::dtor() -> void { joan::bind::member<&::game_b83_05::H::blk_dtor>(this); }  // 0x637b00
auto joan::orig::BlkArr::reset() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b83_05::H::blk_reset>(this)); }  // 0x637bb0
auto joan::orig::BlockArray::add(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_05::H::block_add>(this, a0)); }  // 0x637be0
auto joan::orig::BlkArr::add(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_05::H::blk_add>(this, a0)); }  // 0x637cb0
auto joan::orig::List::remove_at(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_06::H::remove_at>(this, a0)); }  // 0x637e20
