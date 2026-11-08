// ilib/dynarrb.cpp: functions of the original d:\ww\ilib\dynarrb.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstring>
#include "../original/helpers.hpp"
#include <type_traits>

#include "../original/direct.hpp"
#include "../fn_names.hpp"
#include "../original/dynarray.hpp"
#include "../original/engine.hpp"
#include "../registry.hpp"
#include <cstdarg>
#include <cstdint>
#include "../data_names.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from ilib/dynarr.cpp (ilib/dynarrb.cpp)
namespace {
namespace ilib_dynarr {

using joan::orig::DynArrayB;

#define DAB(name, addr, fn) JOAN_REPLACE("ilib/dynarrb.cpp", "DynArrayB::" name, addr, fn)
DAB("~DynArrayB", 0x00585710, &DynArrayB::destruct);
DAB("linkin", 0x005857a0, &DynArrayB::linkin);
DAB("linkout", 0x005858c0, &DynArrayB::linkout);
DAB("write_empty_room", 0x00585b40, &DynArrayB::write_empty_room);
DAB("read_empty_room", 0x00585bb0, &DynArrayB::read_empty_room);
#undef DAB

}  // namespace ilib_dynarr
}  // namespace

// ---------------------------------------- from modules/game_b103_08.cpp (game/b103_08)
namespace {
namespace game_b103_08 {

using i32 = std::int32_t;

using joan::orig::helpers::at;

JOAN_COUNTER(c_count_below, "Keys::count_below");

struct H {

    // ------------------------------------------------------------------------ 0x00585cb0
    i32 count_below(i32 k) {
        c_count_below.hit();
        i32 n = k;
        for (i32 i = 0; i < jo::self<jo::DynArrayB>(this)->empty_room_count; ++i) {
            if (jo::self<jo::DynArrayB>(this)->empty_room_array[i].recno < k) --n;
        }
        return n;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("Keys::count_below", 0x00585cb0, &H::count_below);
#undef B

}  // namespace game_b103_08
}  // namespace

// ---------------------------------------- from modules/game_stream2.cpp (game/stream2)
namespace {
namespace game_stream2 {

using joan::orig::helpers::at;

struct H {

    void* reset_585d20() {  // 0x00585d20 (eax = this: `mov eax,[ebp-4]` for the +0x25 store)
        joan::orig::dynarray(this)->zap(1);  // 0x00584580
        jo::view<std::uint32_t>(jo::self<jo::DynArrayB>(this)->empty_room_count) = 0;
        return this;
    }
};

#define STREAM2(name, addr, fn) JOAN_REPLACE("game/stream2", name, addr, fn)
STREAM2("DynArrayB::zap", 0x00585d20, &H::reset_585d20);
#undef STREAM2

}  // namespace game_stream2
}  // namespace

// ---------------------------------------- from modules/game_b102_07.cpp (game/b102_07)
//
//      0x00585d50 List::save(dst, b)                  (thiscall, ret 8)
//
//      0x00585e20 List::load(src, b, factory)         (thiscall, ret 0xc)
//
//  - 0x00585e20 calls the factory pointer with no arguments (ecx is whatever 0x00587c20 left).
namespace {
namespace game_b102_07 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::orig::helpers::at;

JOAN_COUNTER(c_list_save, "List::save");
JOAN_COUNTER(c_list_load, "List::load");

struct H {

    // -------------------------------------------------------------- 0x00585d50
    i32 list_save(void* dst, u32 b) {
        c_list_save.hit();
        joan::orig::as_file(dst)->file_put_long(joan::orig::arg::i32(joan::orig::as_dynarray(this)->size()));  // 0x00587da0(count 0x00417ad0)
        for (i32 i = 1; i <= joan::orig::as_dynarray(this)->size(); ++i) {
            void* const e = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0 (the element: a pointer)
            if (e == nullptr) {
                joan::orig::as_file(dst)->file_put_short(joan::orig::arg::i16(0));  // 0x00587ba0
            } else {
                joan::orig::as_file(dst)->file_put_short(joan::orig::arg::i16(1));
                if (joan::orig::as_file(dst)->file_write(joan::orig::arg::ptr(e), joan::orig::arg::u32(b)) == 0) return 0;  // 0x005872e0
            }
        }
        joan::orig::dynarray_b(this)->write_empty_room(joan::orig::arg::file(dst));  // 0x00585b40
        return 1;
    }

    // -------------------------------------------------------------- 0x00585e20
    i32 list_load(void* src, u32 b, std::uintptr_t factory) {
        c_list_load.hit();
        const i32 n = joan::orig::as_file(src)->file_get_long();   // 0x00587e20
        for (i32 i = 1; i <= n; ++i) {
            if (joan::orig::as_file(src)->file_get_short() == 0) {                     // 0x00587c20 (ax)
                joan::orig::dynarray(this)->add_blank(1);                    // 0x00584bd0
            } else {
                void* const e = cdecl_call<void*>(factory);  // the new element (a pointer)
                if (joan::orig::as_file(src)->file_read(joan::orig::arg::mptr(e), joan::orig::arg::u32(b)) == 0) return 0;  // 0x005873a0
            }
        }
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            joan::orig::as_dynarray(this)->go(joan::orig::arg::i32(i));                             // 0x00585660
            if (joan::orig::as_table(this)->last() == nullptr)                  // 0x00585f20
                joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(-1));  // 0x005858c0
        }
        joan::orig::dynarray_b(this)->read_empty_room(joan::orig::arg::file(src));  // 0x00585bb0
        return 1;
    }
};

#define B102(name, addr, fn) JOAN_REPLACE("game/b102_07", name, addr, fn)
B102("DynArrayB::write_ptr_array", 0x00585d50, &H::list_save);
B102("List::load", 0x00585e20, &H::list_load);
#undef B102

}  // namespace game_b102_07
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

using joan::orig::helpers::at;

struct H {

    // 0x00585f20
    void* table_last() {  // the pointer in the cursor's record (the lists of pointers), or null
        if (jo::self<jo::DynArray>(this)->cur_pos == 0) return nullptr;
        const std::int32_t off = (jo::self<jo::DynArray>(this)->cur_pos - 1) * jo::self<jo::DynArray>(this)->ele_size;
        void* last;  // element size is a runtime value of the generic array: copy the pointer out
        std::memcpy(&last, static_cast<const std::uint8_t*>(jo::view<void*>(jo::self<jo::DynArray>(this)->body_buf)) + off, sizeof last);
        return last;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Table::last", 0x00585f20, &H::table_last);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Sound::reset_585d20() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_stream2::H::reset_585d20>(this)); }  // 0x585d20
auto joan::orig::Table::last() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_04::H::table_last>(this)); }  // 0x585f20
