// Reimplementation of Mem, the engine's debug allocator (layout in
// original/mem.hpp): add, add_clear, resize, resize_keep, del, get_mem_size.
//
// Blocks still come from the game's own CRT heap (operator new / delete of the
// statically linked MSVC 6 runtime), so memory allocated by not-yet-replaced
// code and by this module can be mixed freely.
#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../original/engine.hpp"
#include "../original/mem.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../platform/win32_types.hpp"
#include <cstdarg>
#include "../original/gameset.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include <cmath>
#include <limits>
#include <vector>
#include "../original/misc.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

namespace {
using joan::hook::original;
using joan::orig::kMemMagic;
using joan::orig::kMemOverhead;
using joan::orig::MemHeader;

JOAN_COUNTER(c_add, "Mem::add");
JOAN_COUNTER(c_add_clear, "Mem::add_clear");
JOAN_COUNTER(c_resize, "Mem::resize");
JOAN_COUNTER(c_resize_keep, "Mem::resize_keep");
JOAN_COUNTER(c_del, "Mem::del");
JOAN_COUNTER(c_size, "Mem::get_mem_size");

// operator new (0x007b1d35, _nh_malloc(n, 1)) and operator delete (0x007b1d2a, free).
void* crt_new(std::uint32_t n) { return original<void*(JOAN_CDECL*)(std::uint32_t)>(fn::op_new)(n); }
void crt_delete(void* p) { original<void(JOAN_CDECL*)(void*)>(fn::op_delete)(p); }

// Error::mem_error (0x00586af0, thiscall on `err`): "Insufficient Memory,
// execution interrupt." message box, then exit(-2). Returns only if an error
// message is already being shown.
struct ErrObj {
    int unused;
};
void out_of_memory() {
    auto* err = &img::global<ErrObj>(joan::orig::kErr);
    (err->*original<void (ErrObj::*)()>(fn::Error_mem_error_body))();
}

const char* game_string(img::Addr address) { return &img::global<const char>(address); }
constexpr img::Addr kDelNullMsg = d::s_Mem_del_Free_value_not_found;   // "Mem::del - Free value not found, File name:%s, line no.:%d\n"
constexpr img::Addr kSizeNullMsg = d::s_Error_Mem_get_mem_size;  // "Error: Mem::get_mem_size()."

// Overwrites a released block (body, header, guard byte) with 0xff.
void wipe(MemHeader* h) {
    const std::uint32_t size = h->size;
    char* guard = h->body() + size;
    std::memset(h->body(), 0xff, size);
    std::memset(h, 0xff, sizeof(MemHeader));
    *guard = static_cast<char>(0xff);
}
}  // namespace

namespace joan::orig {

namespace {
// Allocates a block, fills the body and appends it to the list.
void* new_block(Mem& m, std::uint32_t size, int fill) {
    auto* h = static_cast<MemHeader*>(crt_new(size + kMemOverhead));
    if (!h) {
        out_of_memory();
        return nullptr;
    }
    h->size = size;
    h->magic = kMemMagic;
    std::memset(h->body(), fill, size);
    // (the guard byte is left as allocated: its setter is empty in this build)
    h->prev = m.tail;
    h->next = nullptr;
    if (!m.tail) m.head = h;
    else m.tail->next = h;
    m.tail = h;
    ++m.count;
    return h->body();
}
}  // namespace

// 0x0058e7b0
void* Mem::add_impl(std::uint32_t size, const char*, int) { return new_block(*this, size, 0xee); }

// 0x0058e8d0
void* Mem::add_clear(std::uint32_t size, const char*, int) { return new_block(*this, size, 0); }

// 0x0058eb30 - a new block takes the old one's place in the list.
void* Mem::resize_impl(void* p, std::uint32_t size, const char* file, int line) {
    if (size == 0 && !p) return nullptr;
    if (size == 0) {
        del(p, file, line);
        return nullptr;
    }
    if (!p) return add(size, file, line);
    MemHeader* old = MemHeader::of(p);
    if (old->size == size) return p;
    auto* h = static_cast<MemHeader*>(crt_new(size + kMemOverhead));
    if (!h) {
        out_of_memory();
        return nullptr;
    }
    h->size = size;
    h->magic = kMemMagic;
    std::memcpy(h->body(), p, std::min(size, old->size));  // the rest of the body is left uninitialised
    h->prev = old->prev;
    h->next = old->next;
    if (!h->prev) head = h;
    else h->prev->next = h;
    if (!h->next) tail = h;
    else h->next->prev = h;
    wipe(old);
    crt_delete(old);
    return h->body();
}

// 0x0058e9f0 - resize, explicitly preserving the first `keep` bytes through a
// temporary copy when growing (resize already copies them; never called).
void* Mem::resize_keep(void* p, std::uint32_t keep, std::uint32_t size, const char* file, int line) {
    if (size == 0 && !p) return nullptr;
    if (size == 0) {
        del(p, file, line);
        return nullptr;
    }
    if (!p) return add(size, file, line);
    if (size <= keep) return resize(p, size, file, line);
    void* tmp = crt_new(keep);
    std::memcpy(tmp, p, keep);
    void* r = resize(p, size, file, line);
    if (r && r != p) std::memcpy(r, tmp, keep);  // (the original would write to null after a failed resize)
    crt_delete(tmp);
    return r;
}

// 0x0058ed70
void Mem::del_impl(void* p, const char* file, int line) {
    if (!p) {
        err_run(game_string(kDelNullMsg), file, line);
        return;  // (the original carries on and faults; err.run normally exits first)
    }
    MemHeader* h = MemHeader::of(p);
    if (!h->prev) head = h->next;
    else h->prev->next = h->next;
    if (!h->next) tail = h->prev;
    else h->next->prev = h->prev;
    --count;
    wipe(h);
    crt_delete(h);
}

// 0x0058ee90
std::uint32_t Mem::get_mem_size(void* p) {
    if (!p) {
        err_run(game_string(kSizeNullMsg));
        return 0;  // (the original faults here)
    }
    return MemHeader::of(p)->size;
}

}  // namespace joan::orig

// ---------------------------------------------------------------------------
// Entry points
// ---------------------------------------------------------------------------
namespace {
using joan::orig::Mem;

struct Entry : Mem {
    void* add_counted(joan::hook::Counter& c, std::uint32_t size, const char* file, int line,
                      void* (Mem::*mine)(std::uint32_t, const char*, int)) {
        c.hit();
        return (this->*mine)(size, file, line);
    }
    void* add_entry(std::uint32_t size, const char* file, int line) {
        return add_counted(c_add, size, file, line, &Mem::add_impl);
    }
    void* add_clear_entry(std::uint32_t size, const char* file, int line) {
        return add_counted(c_add_clear, size, file, line, &Mem::add_clear);
    }

    void* resize_entry(void* p, std::uint32_t size, const char* file, int line) {
        c_resize.hit();
        return resize_impl(p, size, file, line);
    }

    void* resize_keep_entry(void* p, std::uint32_t keep, std::uint32_t size, const char* file, int line) {
        c_resize_keep.hit();
        return resize_keep(p, keep, size, file, line);
    }

    void del_entry(void* p, const char* file, int line) {
        c_del.hit();
        return del_impl(p, file, line);
    }

    std::uint32_t get_mem_size_entry(void* p) {
        c_size.hit();
        return get_mem_size(p);
    }
};
static_assert(sizeof(Entry) == sizeof(Mem));
}  // namespace

#define MEM(name, addr, fn) JOAN_REPLACE("ilib/mem.cpp", "Mem::" name, addr, fn)
MEM("add", 0x0058e7b0, &Entry::add_entry);
MEM("add_clear", 0x0058e8d0, &Entry::add_clear_entry);
MEM("resize_keep", 0x0058e9f0, &Entry::resize_keep_entry);
MEM("resize", 0x0058eb30, &Entry::resize_entry);
MEM("del", 0x0058ed70, &Entry::del_entry);
MEM("get_mem_size", 0x0058ee90, &Entry::get_mem_size_entry);
#undef MEM

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
//
//      0x0058e650 Pool::init            (thiscall(a, b, c), ret 0xc) - eax = a.
//
//      0x0058e740 Byte::set_ff          (thiscall, ret 0) - memset(this, 0xff, 1).
namespace {
namespace game_b93_03 {

using joan::orig::helpers::at;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {

    u32 pool_init(u32 a, u32, u32) {  // 0x0058e650
        jo::self<jo::MemHeader>(this)->next = nullptr;
        jo::self<jo::MemHeader>(this)->prev = nullptr;
        jo::self<jo::MemHeader>(this)->size = a;
        jo::self<jo::MemHeader>(this)->magic = 0x12345678u;
        return a;
    }
    void* set_ff() { return memset_(this, 0xff, 1); }  // 0x0058e740
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("MemInfo::init", 0x0058e650, &H::pool_init);
B93("Byte::set_ff", 0x0058e740, &H::set_ff);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
//
//    - Obj::init_58e6a0 0x0058e6a0 (a, b, c, d), ret 0x10: [0] = [4] = 0, +8 = b, +0xc =
//      0x12345678 (returns b in eax like the original).
namespace {
namespace game_b97_05 {

using joan::orig::helpers::at;

struct H {

    // 0x0058e6a0
    std::uint32_t init_58e6a0(std::uint32_t, std::uint32_t b, std::uint32_t, std::uint32_t) {
        jo::self<jo::MemHeader>(this)->next = nullptr;
        jo::self<jo::MemHeader>(this)->prev = nullptr;
        jo::self<jo::MemHeader>(this)->size = b;
        jo::self<jo::MemHeader>(this)->magic = 0x12345678;
        return b;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("Obj::init_58e6a0", 0x0058e6a0, &H::init_58e6a0);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
//
//    - Obj::fill_ff 0x0058e6f0: memset(this, 0xff, 0x10), returns memset's result. `this` is a Mem block header
//      (the callers 0x58ecc7 / 0x58ee27 are Mem::del / Mem::resize wiping a released block): sizeof(MemHeader).
namespace {
namespace game_b94_05 {

static_assert(sizeof(void*) != 4 || sizeof(joan::orig::MemHeader) == 0x10, "Mem block header");

struct H {
    // 0x0058e6f0
    void* fill_ff() { return std::memset(static_cast<void*>(this), 0xff, sizeof(joan::orig::MemHeader)); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("Obj::fill_ff", 0x0058e6f0, &H::fill_ff);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
//
//  * module game/b91_03:
//    TextCursor::advanced 0x0058c120, to_lower 0x0058e330, empty methods 0x0058e720 /
//    0x0059d1b0, Stream::forward 0x005939f0, Joystick::in_rect 0x00594360,
//    Joystick::pressed_kind 0x00594a10, Key::repeat_count 0x00595fb0, word getters
//    0x005979a0 / 0x005979c0, Pad::axis 0x00597c80, Pad::axis_range 0x00597e00,
//    same16 0x00598700, Pair::clear 0x005a1730, Bitmap::address 0x005a93b0,
//    MusicStream::release 0x005a98a0, MusicStream::read_header 0x005a9ad0,
//    MusicStream::reset 0x005a9b40, MusicStream::rewind 0x005aa040, Actor::set_byte_fd
//    0x005ad000, Table::get 0x005af330, Table::missing 0x005af360, AnimDef::lookup_word
//    0x005b4a00, Names::get 0x005ba910, Rect::set 0x005bed90 / 0x00612ee0, Array::at1
//    0x005ea410, RenderTarget::create_depth 0x006032d0, RenderTarget::release_depth
//    0x00603380, z_plane_hit 0x0060e6a0, BitWriter::put 0x0060ff70, BitWriter::drop
//    0x006100a0, Viewport::set_int 0x00614110, Sound3d::set 0x00616a90, getter 0x00616b60,
//    Dialog::reset_name 0x00616f80, base dtor bodies 0x00628ad0 / 0x0062c720,
//    TextBox::init 0x0062b8d0, Scroll::set 0x0062be10 / 0x0062cd40, Scroll::at_end
//    0x0062c040 / 0x0062cf70, Scroll::set_end 0x0062ced0, TextBox::cursor_x 0x00630770,
//    TextBox::line_y 0x00630ab0, getter 0x00631770, Table16::entry 0x00638150.
namespace {
namespace game_b91_03 {

struct H {
    void empty() {}  // 0x0058e720 / 0x0059d1b0
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("MemTrailer::init", 0x0058e720, &H::empty);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
namespace {
namespace game_b100_10 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {

    void* ctor_58e770() {  // 0x0058e770
        jo::view<u32>(jo::self<jo::Mem>(this)->count) = 0;
        jo::self<jo::Mem>(this)->tail = nullptr;
        jo::self<jo::Mem>(this)->head = nullptr;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Obj::ctor_58e770", 0x0058e770, &H::ctor_58e770);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b104_08.cpp (game/b104_08)
namespace {
namespace game_b104_08 {

struct H {
    void empty_dtor() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_08", name, addr, fn)
B("Obj::empty_dtor_58eee0", 0x0058eee0, &H::empty_dtor);
#undef B

}  // namespace game_b104_08
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
namespace {
namespace game_b103_10 {

// ---------------------------------------------------------------------------- cdecl functions
// 0x0058ef00 (dead code): `if (p) q = p - 0x10;` with q unused.
static void JOAN_CDECL free_stub(void* /*p*/) {}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("free_stub", 0x0058ef00, &free_stub);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Mem::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_10::H::ctor_58e770>(this)); }  // 0x58e770
