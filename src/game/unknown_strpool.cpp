// game/unknown_strpool.cpp: functions whose original source file is unknown (formerly game/_unplaced/strpool.cpp;
// re/attribution.csv: between ww/ilib/sound3d.cpp and ww/ilib/txtres.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../platform/win32_types.hpp"
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdint>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../original/engine.hpp"
#include "../original/gameset.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::call::thiscall;

using u8 = std::uint8_t;

using joan::call::cdecl_call;

using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

struct H {
    // 0x005a1730
    H* clear_pair() {
        jo::self<jo::StringPool>(this)->head = nullptr;
        jo::self<jo::StringPool>(this)->tail = nullptr;
        return this;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("Pair::clear", 0x005a1730, &H::clear_pair);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b107_05.cpp (game/b107_05)
namespace {
namespace game_b107_05 {

struct H {
    // 0x005a1770
    std::uintptr_t forward_5a1770() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_list(this)->clear_5a17d0()); }  // 0x005a17d0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_05", name, addr, fn)
B("Obj::forward_5a1770", 0x005a1770, &H::forward_5a1770);
#undef B

}  // namespace game_b107_05
}  // namespace

// ---------------------------------------- from modules/game_b107_11.cpp (game/b107_11)
namespace {
namespace game_b107_11 {

struct H {
    // ------------------------------------------------------------------ 0x005a17a0
    std::uintptr_t forward_5a17a0() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_list(this)->clear_5a17d0()); }  // 0x005a17d0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_11", name, addr, fn)
B("Obj::forward_5a17a0", 0x005a17a0, &H::forward_5a17a0);
#undef B

}  // namespace game_b107_11
}  // namespace

// ---------------------------------------- from modules/game_b106_03.cpp (game/b106_03)
namespace {
namespace game_b106_03 {

JOAN_COUNTER(c_clear_5a17d0, "List::clear_5a17d0");

struct H {
    // ---- 0x005a17d0 ------------------------------------------------------------------------
    void* clear() {
        c_clear_5a17d0.hit();
        void* eax = this;
        u8* node = jo::view<u8*>(jo::self<jo::StringPool>(this)->head);
        while (node != nullptr) {
            u8* const next = jo::view<u8*>(jo::as_string_block(node)->next);
            eax = joan::orig::arg::as<void*>(joan::orig::as_string_block(node)->destroy(1u));  // 0x005a1860 (deleting dtor)
            node = next;
        }
        jo::self<jo::StringPool>(this)->head = nullptr;
        jo::self<jo::StringPool>(this)->tail = nullptr;
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_03", name, addr, fn)
B("List::clear_5a17d0", 0x005a17d0, &H::clear);
#undef B

}  // namespace game_b106_03
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16 {

using joan::orig::helpers::op_delete;

struct H {
    // ---------------------------------------------------------------- 0x005a1860 / 0x005a2190
    void* scalar_dtor_5a1860(u32 flags) {
        joan::orig::as_string_block(this)->destruct();  // 0x005a18b0
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Obj::scalar_dtor_5a1860", 0x005a1860, &H::scalar_dtor_5a1860);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_b104_09.cpp (game/b104_09)
namespace {
namespace game_b104_09 {

struct H {
    void stub_void() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_09", name, addr, fn)
B("Obj::stub_5a18b0", 0x005a18b0, &H::stub_void);
#undef B

}  // namespace game_b104_09
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04.cpp (game/b86_04)
namespace {
namespace en3d_b86_04 {

JOAN_COUNTER(c_pool_add, "StringPool::add");

struct H {
    // 0x005a18d0
    void* pool_add(const char* s, std::int32_t len) {
        c_pool_add.hit();
        if (len < 0) len = joan::orig::arg::as<std::int32_t>(std::strlen(joan::orig::arg::str(s)));  // strlen
        void* r = nullptr;
        if (jo::view<void*>(jo::self<jo::StringPool>(this)->tail) != nullptr) {
            r = joan::orig::arg::as<void*>(joan::orig::as_string_block(jo::view<void*>(jo::self<jo::StringPool>(this)->tail))->add(joan::orig::arg::str(s), joan::orig::arg::i32(len)));  // 0x005a1a70
            if (r != nullptr) return r;
        }
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::StringBlock)));  // 0xfa8 on x86; operator new
        void* const block = mem != nullptr ? joan::orig::as_string_block(mem)->construct() : nullptr;  // 0x005a1a40
        if (jo::view<void*>(jo::self<jo::StringPool>(this)->head) == nullptr) jo::view<void*>(jo::self<jo::StringPool>(this)->head) = block;
        if (jo::view<void*>(jo::self<jo::StringPool>(this)->tail) != nullptr) jo::view<void*>(jo::self<jo::StringPool>(this)->tail->next) = block;
        jo::view<void*>(jo::self<jo::StringPool>(this)->tail) = block;
        jo::as_string_block(block)->next = nullptr;
        if (jo::view<void*>(jo::self<jo::StringPool>(this)->tail) != nullptr) {
            r = joan::orig::arg::as<void*>(joan::orig::as_string_block(jo::view<void*>(jo::self<jo::StringPool>(this)->tail))->add(joan::orig::arg::str(s), joan::orig::arg::i32(len)));
            if (r != nullptr) return r;
        }
        return nullptr;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b86_04", name, addr, fn)
GAME("StringPool::add", 0x005a18d0, &H::pool_add);
#undef GAME

}  // namespace en3d_b86_04
}  // namespace

// ---------------------------------------- from modules/en3d_b87_02.cpp (game/b87_02)
namespace {
namespace en3d_b87_02 {

std::int32_t add(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

void game_strcpy(void* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(d))), joan::orig::arg::str(s)); }

struct H {
    // 0x005a1a70
    void* block_add(const char* s, std::int32_t len) {
        if (add(add(jo::self<jo::StringBlock>(this)->used, len), 1) > 0xfa0) return nullptr;
        void* const p = &jo::self<jo::StringBlock>(this)->text[jo::self<jo::StringBlock>(this)->used];
        if (s != nullptr) game_strcpy(p, s);
        jo::self<jo::StringBlock>(this)->used = add(add(jo::self<jo::StringBlock>(this)->used, len), 1);
        return p;
    }

    // 0x005a1a40
    void* block_ctor() {
        jo::view<std::uint32_t>(jo::self<jo::StringBlock>(this)->used) = 0;
        return this;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b87_02", name, addr, fn)
GAME("StringBlock::add", 0x005a1a70, &H::block_add);
GAME("StringBlock::ctor", 0x005a1a40, &H::block_ctor);
#undef GAME

}  // namespace en3d_b87_02
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
namespace {
namespace game_b103_10 {

using i32 = std::int32_t;

struct H {
    // ------------------------------------------------------------------ 0x005a1b00 (dead code)
    i32 sum_4() {
        u32 sum = 0;
        u8* node = jo::view<u8*>(jo::self<jo::StringPool>(this)->head);
        while (node != nullptr) {
            u8* const next = jo::view<u8*>(jo::as_string_block(node)->next);
            sum += jo::view<u32>(jo::as_string_block(node)->used);
            node = next;
        }
        return static_cast<i32>(sum);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("List::sum_4", 0x005a1b00, &H::sum_4);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::StringPool::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_03::H::clear_pair>(this)); }  // 0x5a1730
auto joan::orig::StringPool::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_05::H::forward_5a1770>(this)); }  // 0x5a1770
auto joan::orig::StringPool::reset() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_11::H::forward_5a17a0>(this)); }  // 0x5a17a0
auto joan::orig::List::clear_5a17d0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b106_03::H::clear>(this)); }  // 0x5a17d0
auto joan::orig::StringBlock::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b105_16::H::scalar_dtor_5a1860>(this, a0)); }  // 0x5a1860
auto joan::orig::StringBlock::destruct() -> void { joan::bind::member<&::game_b104_09::H::stub_void>(this); }  // 0x5a18b0
auto joan::orig::StringPool::add(const char* a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b86_04::H::pool_add>(this, a0, a1)); }  // 0x5a18d0
auto joan::orig::StringBlock::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b87_02::H::block_ctor>(this)); }  // 0x5a1a40
auto joan::orig::StringBlock::add(const char* a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b87_02::H::block_add>(this, a0, a1)); }  // 0x5a1a70
