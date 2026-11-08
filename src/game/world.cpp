// game/world.cpp: functions of the original d:\ww\world.cpp (re/attribution.csv).
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
#include "../runtime/hitch.hpp"

#include "../original/dynarray.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include <cstdarg>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../en3d/scene_lists.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::orig::helpers::line_base;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
//
//    - Obj::nop 0x004e3790, Track::is_type1 0x00428aa0, Screen::call_30 0x0042d530 (a, b) ret 8,
//      Track::restart 0x004289e0, World::leave_mode 0x0042df50 (flag) ret 4, World::add_entry
//      0x0042ce10 (world.cpp).
namespace {
namespace game_b84_03 {

JOAN_COUNTER(c_world_add, "World::add_entry");

constexpr img::Addr kMemory = d::g_mem;

void* mem_realloc(void* p, std::int32_t n, img::Addr file, std::int32_t line) {
    return joan::orig::as_mem(obj(kMemory))->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(n), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058eb30
}

struct H {

    // ------------------------------------------------------------------ 0x0042ce10
    std::int32_t world_add() {
        c_world_add.hit();
        if (!(jo::self<jo::World>(this)->entry_count < jo::self<jo::World>(this)->entry_cap)) {
            jo::self<jo::World>(this)->entry_cap += 0xa;
            jo::view<void*>(jo::self<jo::World>(this)->entries) = mem_realloc(jo::view<void*>(jo::self<jo::World>(this)->entries), jo::self<jo::World>(this)->entry_cap * static_cast<std::int32_t>(sizeof(jo::SceneSPtr)), d::s_d_ww_world_cpp,
                                         line_base(d::g_a0b274) + 5);  // cap << 2 on x86
        }
        const std::int32_t n = jo::self<jo::World>(this)->entry_count + 1;
        jo::self<jo::World>(this)->entry_count = jo::self<jo::World>(this)->entry_count + 1;
        jo::SceneS* const v = joan::orig::arg::as<jo::SceneS*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::World>(this)->scene))->add_node());  // 0x005ea460
        jo::self<jo::World>(this)->entries[n - 1].p = v;
        return n;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("World::add_entry", 0x0042ce10, &H::world_add);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b87_05.cpp (game/b87_05)
//
//    - World::cleanup 0x00440320 (world.cpp): 0x004426a0, removes entries +0x72 .. 1
//      (0x005482c0), deletes +0x7e (0x00427930, flag 3), frees +0x7a / +0x82 / +0x8e with
//      lines 0xa0b268 + 0xf / 0x14 / 0x1f, clears +0x76 / +0x86 / +0x8a / +0 / +0x60.
//
//  - Dead locals kept as calls: 0x0042b6e0 0x0042d0a0 result, 0x00440320 deleting-destructor
//    result.
namespace {
namespace game_b87_05 {

void* const kMemory = obj(d::g_mem);  // 0x0058ed70 free
const char* const kWorldCpp = str(d::s_d_ww_world_cpp);  // "d:\ww\world.cpp"

JOAN_COUNTER(c_cleanup, "World::cleanup");

struct H {

    // ------------------------------------------------------------ 0x00440320
    void* cleanup() {
        c_cleanup.hit();
        joan::orig::as_world(this)->unload();  // 0x004426a0
        for (std::int32_t i = jo::self<jo::World>(this)->cell_count; i > 0; --i) joan::orig::as_world(this)->remove_entry(joan::orig::arg::i32(i));  // 0x005482c0
        void* const list = jo::view<void*>(jo::self<jo::World>(this)->box_tree);
        if (list != nullptr) joan::orig::as_box_tree(list)->vector_delete(3u);  // 0x00427930
        jo::view<void*>(jo::self<jo::World>(this)->box_tree) = nullptr;
        if (jo::view<void*>(jo::self<jo::World>(this)->cells) != nullptr) {
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::World>(this)->cells)), joan::orig::arg::str(kWorldCpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a0b268)) + 0xf));  // 0x0058ed70
            jo::view<void*>(jo::self<jo::World>(this)->cells) = nullptr;
        }
        if (jo::self<jo::World>(this)->cell_query != nullptr) {
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::self<jo::World>(this)->cell_query), joan::orig::arg::str(kWorldCpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a0b268)) + 0x14));
            jo::self<jo::World>(this)->cell_query = nullptr;
        }
        jo::self<jo::World>(this)->cell_capacity = 0;
        if (jo::view<void*>(jo::self<jo::World>(this)->entries) != nullptr) {
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::World>(this)->entries)), joan::orig::arg::str(kWorldCpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a0b268)) + 0x1f));
            jo::view<void*>(jo::self<jo::World>(this)->entries) = nullptr;
        }
        jo::self<jo::World>(this)->entry_count = 0;
        jo::self<jo::World>(this)->entry_cap = 0;
        jo::self<jo::World>(this)->active = 0;
        jo::self<jo::World>(this)->u8_60 = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_05", name, addr, fn)
B("World::cleanup", 0x00440320, &H::cleanup);
#undef B

}  // namespace game_b87_05
}  // namespace

// ---------------------------------------- from modules/game_world4.cpp (game/world4)
//
//    - World::build_tree 0x0044a260 (copies the cell pointers, allocates 2n-1 tree nodes
//      of 0x38 bytes with new[] and builds the cell tree 0x00548880).
namespace {
namespace game_world4 {

using joan::call::stdcall_call;

constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kWorldCpp(d::s_d_ww_world_cpp);  // "d:\ww\world.cpp"

JOAN_COUNTER(c_build, "World::build_tree");

struct H {

    // 0x0044a260 World::build_tree: a temporary copy of the cell pointers (0xa3ce04), new[]
    // 2n-1 nodes (0x38 bytes, ctor 0x0054aa90, dtor 0x0054ac60) at +0x7e (also 0xa3ce0c
    // while 0x00548880 builds). The original's SEH frame only guards the vector constructor.
    void build_tree() {
        if (jo::view<void*>(jo::self<jo::World>(this)->box_tree) != nullptr || !(jo::self<jo::World>(this)->cell_count > 0)) return;
        c_build.hit();
        global<void*>(d::g_bvh_block_ptrs) = joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(static_cast<std::uint32_t>(jo::self<jo::World>(this)->cell_count) * static_cast<std::uint32_t>(sizeof(void*))), joan::orig::arg::str(img::str(kWorldCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a0b270)) + 5));  // 0x0058e7b0 alloc
        for (std::int32_t i = 0; i < jo::self<jo::World>(this)->cell_count; ++i) global<void**>(d::g_bvh_block_ptrs)[i] = jo::view<void**>(jo::self<jo::World>(this)->cells)[i];
        const std::int32_t nodes = static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::self<jo::World>(this)->cell_count) * 2u - 1u);
        std::uint8_t* const block =
            joan::orig::arg::as<std::uint8_t*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(nodes) * static_cast<std::uint32_t>(sizeof(jo::BoxTree)) + 4u)));  // operator new
        void* tree = nullptr;
        if (block != nullptr) {
            *reinterpret_cast<std::int32_t*>(block) = nodes;  // the new[] element count cookie (x64: the MSVC cookie is a size_t)
            joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(block + 4)), static_cast<std::uint32_t>(sizeof(jo::BoxTree)), joan::orig::arg::i32(nodes), fn::code_value(fn::Obj_ctor_54aa90), fn::code_value(fn::fn_0x0054ac60_clear_34_4));  // eh vector constructor iterator
            tree = block + 4;
        }
        jo::view<void*>(jo::self<jo::World>(this)->box_tree) = tree;
        global<void*>(d::g_a3ce0c) = jo::view<void*>(jo::self<jo::World>(this)->box_tree);
        joan::orig::as_map(this)->build_tree(joan::orig::arg::mptr(0), 0, joan::orig::arg::i32(jo::self<jo::World>(this)->cell_count));  // 0x00548880
        global<void*>(d::g_a3ce0c) = nullptr;
        joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(global<void*>(d::g_bvh_block_ptrs)), joan::orig::arg::str(img::str(kWorldCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a0b270)) + 0x14));  // 0x0058ed70 free
        global<void*>(d::g_bvh_block_ptrs) = nullptr;
    }
};

#define W4(name, addr, fn) JOAN_REPLACE("game/world4", name, addr, fn)
W4("World::build_tree", 0x0044a260, &H::build_tree);
#undef W4

}  // namespace game_world4
}  // namespace

// ---------------------------------------- from modules/game_b84_02.cpp (game/b84_02)
//
//    - NodeList::add 0x00547bf0 (n): grows +0x7a / +0x82 (world.cpp), news n 0xac-byte nodes
//      (0x0041b5c0), returns the first new 1-based index.
//
//  - 0x00560120 / 0x0055fa40 / 0x00547bf0: the SEH frames only guard operator new + ctor
//    (normal path kept). 0x00560120 / 0x0055fa40 eax return is garbage and unused (only
//    caller 0x004f3df0 ignores it) - void here; 0x00547bf0 returns the first new index.
namespace {
namespace game_b84_02 {

void* const kMemory = obj(d::g_mem);  // allocator (0x0058eb30 realloc / 0x0058ed70 free)
const char* const kWorldCpp = str(d::s_d_ww_world_cpp);   // "d:\ww\world.cpp"

using joan::orig::helpers::game_new;  // operator new
void* mem_realloc(void* p, std::int32_t size, const char* file, std::int32_t line) {
    return joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
}

JOAN_COUNTER(c_node_add, "NodeList::add");

struct H {

    // 0x00547bf0 NodeList::add(n) -> first new 1-based index, ret 4. `this` is the World (+0x72 cell_count, +0x76
    // cell_capacity, +0x7a cells, +0x82 cell_query on x86; the Map facade's names for them sit at other x64 offsets).
    std::int32_t node_add(std::int32_t n) {
        c_node_add.hit();
        joan::orig::as_map(this)->release_tree();  // 0x0054a040
        jo::World* const w = jo::self<jo::World>(this);
        if (w->cell_count + n > w->cell_capacity) {
            w->cell_capacity = w->cell_count + n + 0x10;
            jo::view<void*>(w->cells) = mem_realloc(jo::view<void*>(w->cells), w->cell_capacity * static_cast<std::int32_t>(sizeof(void*)), kWorldCpp, line_base(d::g_a0b26c) + 7);
            w->cell_query = mem_realloc(w->cell_query, w->cell_capacity * static_cast<std::int32_t>(sizeof(void*)), kWorldCpp, line_base(d::g_a0b26c) + 8);
        }
        std::int32_t idx = w->cell_count + 1;  // ebp-0x18
        const std::int32_t first = idx;                // ebp-0x14
        for (std::int32_t k = n; k > 0; --k, ++idx) {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Cell)));  // 0xac on x86: the node is a Cell (MapNode is its untyped view)
            void* const node = mem != nullptr ? joan::orig::as_map_node(mem)->construct() : nullptr;  // 0x0041b5c0
            jo::as_map_node(node)->map = jo::self<jo::Map>(this);
            jo::view<void**>(w->cells)[idx - 1] = node;
        }
        w->cell_count = w->cell_count + n;
        return first;
    }
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("NodeList::add", 0x00547bf0, &H::node_add);
#undef B84

}  // namespace game_b84_02
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
//
//    - unit command resets 0x0043c500, 0x0042d1d0, 0x0042a770; Scenario::trigger_slot
//      0x004cecd0; Scenario::clear 0x004cddc0; message box init 0x0046c360; water init
//      0x00546b50; ref add 0x00437af0; release +0x7e 0x0054a040.
namespace {
namespace game_b85_06 {

using joan::call::thiscall;

struct H {

    void* release_7e() {  // 0x0054a040
        if (jo::view<void*>(jo::self<jo::World>(this)->box_tree) != nullptr) joan::orig::as_box_tree(jo::view<void*>(jo::self<jo::World>(this)->box_tree))->vector_delete(3u);  // 0x00427930
        jo::self<jo::World>(this)->box_tree = nullptr;
        return this;
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj54a040::release_7e", 0x0054a040, &H::release_7e);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using joan::call::cdecl_call;

using i32 = std::int32_t;

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_territory_547590, "Territory::create_all_547590");

struct H {
    // -------------------------------------------------------------- 0x00547590
    u32 create_territories(void* reader) {
        c_territory_547590.hit();
        for (;;) {
            const char* const line = joan::orig::arg::as<const char*>(joan::orig::as_file_txt(reader)->get_token(i32{1}));  // 0x00589c70
            if (line == nullptr) break;
            if (joan::crt::stricmp(joan::orig::arg::str(line), str(d::s_CreateTerritory)) == 0)  // "CreateTerritory"
                joan::orig::as_world(this)->load_settlement(joan::orig::arg::mptr(reader));                     // 0x004f9330
        }
        joan::orig::as_list(obj(d::g_territory_array))->init_index(fn::code_value(fn::cmp_name_4));  // 0x004e5960(0x004e59d0 thunk)
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Territory::create_all_547590", 0x00547590, &H::create_territories);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world2)
namespace {
namespace game_world {


using joan::call::thiscall;

using joan::img::obj;

using joan::hook::global;

JOAN_COUNTER(c_stream, "World::stream_cells");
JOAN_COUNTER(c_stream_check, "World::cells_pending");

struct H {

    // World (0xa45f3e): +0x6e scene, +0x72 cell count, +0x7a cells. Cell: +8 box (min, max),
    // +0x43 state (0 unloaded, 1 loaded, 2 to load, 3 keep).
    std::uint8_t* cell(std::int32_t i) { return jo::view<std::uint8_t**>(jo::self<jo::World>(this)->cells)[i]; }
    // 0x00547630 World::stream_cells(near boxes, far boxes, count): load the cells in the near
    // boxes, free the loaded ones outside the far boxes.
    void stream_cells(std::uint8_t* near, std::uint8_t* far, std::int32_t count) {
        joan::hitch::Span hitch_span("stream_cells");  // [Debug] HitchLog
        c_stream.hit();
        std::int32_t to_load = 0, kept = 0, freed = 0;
        for (std::int32_t i = 0; i < count; ++i) {
            std::int32_t n = 0;
            std::uint8_t** cells = joan::orig::arg::as<std::uint8_t**>(joan::orig::as_world(this)->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(near + i * 0x18)), &n));  // 0x00429090
            for (; n > 0; --n, ++cells)
                if (static_cast<std::int8_t>(jo::as_cell(*cells)->loaded) == 0) {
                    jo::as_cell(*cells)->loaded = 2;
                    ++to_load;
                }
            cells = joan::orig::arg::as<std::uint8_t**>(joan::orig::as_world(this)->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(far + i * 0x18)), &n));
            for (; n > 0; --n, ++cells)
                if (static_cast<std::int8_t>(jo::as_cell(*cells)->loaded) == 1) jo::as_cell(*cells)->loaded = 3;
        }
        global<std::int32_t>(d::g_a50265) = to_load;
        global<std::int32_t>(d::g_a50269) = 0;
        for (std::int32_t i = 0; i < jo::self<jo::World>(this)->cell_count; ++i)
            if (static_cast<std::int8_t>(jo::as_cell(cell(i))->loaded) == 1) {
                joan::orig::as_cell(cell(i))->unload();  // 0x0042ac40 free
                ++freed;
            }
        for (std::int32_t i = 0; i < jo::self<jo::World>(this)->cell_count; ++i) {
            std::uint8_t* const c = cell(i);
            if (static_cast<std::int8_t>(jo::as_cell(c)->loaded) == 2) {
                jo::as_cell(c)->loaded = 0;
                global<std::int32_t>(d::g_a50269) = global<std::int32_t>(d::g_a50269) + 1;
                global<std::int32_t>(d::g_a5026d) = 0;
                void* const progress = img::obj(d::g_progress);
                joan::orig::as_progress(progress)->update();  // 0x0075f3c0
                joan::orig::as_progress(progress)->redraw();  // 0x0075f4f0 (draws the loading screen)
                joan::orig::as_cell(c)->load(joan::orig::arg::mptr(static_cast<void*>(near)), joan::orig::arg::i32(count));  // 0x004329e0 load
            }
            if (static_cast<std::int8_t>(jo::as_cell(c)->loaded) == 3) {
                jo::as_cell(c)->loaded = 1;
                ++kept;
            }
        }
        (void)kept;
        (void)freed;
    }
    // 0x005479a0 World::cells_pending(near, far, count): a near cell is unloaded, or a loaded
    // cell is outside the far boxes. (The cell loop reuses the box index, as in the original.)
    std::int32_t cells_pending(std::uint8_t* near, std::uint8_t* far, std::int32_t count) {
        c_stream_check.hit();
        for (std::int32_t i = 0; i < count; ++i) {
            std::int32_t n = 0;
            std::uint8_t** cells = joan::orig::arg::as<std::uint8_t**>(joan::orig::as_world(this)->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(near + i * 0x18)), &n));
            for (; n > 0; --n, ++cells)
                if (static_cast<std::int8_t>(jo::as_cell(*cells)->loaded) == 0) return 1;
            std::uint8_t** const far_cells = joan::orig::arg::as<std::uint8_t**>(joan::orig::as_world(this)->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(far + i * 0x18)), &n));
            for (i = 0; i < jo::self<jo::World>(this)->cell_count; ++i) {
                std::uint8_t* const c = cell(i);
                bool outside = true;
                for (std::int32_t k = 0; k < n; ++k)
                    if (c == far_cells[k]) {
                        outside = false;
                        break;
                    }
                if (outside && static_cast<std::int8_t>(jo::as_cell(c)->loaded) == 1) return 1;
            }
        }
        return 0;
    }
};

#define W2(name, addr, fn) JOAN_REPLACE("game/world2", name, addr, fn)
W2("World::stream_cells", 0x00547630, &H::stream_cells);
W2("World::cells_pending", 0x005479a0, &H::cells_pending);
#undef W2

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;                      // operator delete

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00548430
    void* scalar_delete(std::uint32_t flags) {
        joan::orig::as_world_cell(self())->destruct();  // 0x0054e080
        if ((flags & 1u) != 0) game_delete(self());
        return self();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Obj::scalar_delete", 0x00548430, &H::scalar_delete);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b84_02_2.cpp (game/b84_02)
namespace {
namespace game_b84_02_2 {


float JOAN_CDECL lerp_54a8a0(float a, float b, float t) {
    if (b > a) return (b - a) * t + a;
    return (1.0f - t) * (a - b) + b;
}

struct H {

    // 0x0054aa00 (1-based, +0x86 count, +0x8e array), ret 4.
    void* get_54aa00(std::int32_t i) {
        if (i <= 0 || i > jo::self<jo::World>(this)->entry_count) return nullptr;
        return jo::view<void**>(jo::self<jo::World>(this)->entries)[i - 1];
    }
    // 0x00548600 (1-based, +0x72 count, +0x7a array), ret 4.
    void* get_548600(std::int32_t i) {
        if (i <= 0 || i > jo::self<jo::World>(this)->cell_count) return nullptr;
        return jo::view<void**>(jo::self<jo::World>(this)->cells)[i - 1];
    }
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("lerp_54a8a0", 0x0054a8a0, &lerp_54a8a0);
B84("Sectors::get", 0x0054aa00, &H::get_54aa00);
B84("NodeList::get", 0x00548600, &H::get_548600);
#undef B84

}  // namespace game_b84_02_2
}  // namespace

// ---------------------------------------- from modules/game_units5.cpp (game/units5)
namespace {
namespace game_units5 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;


struct V3 {
    float x, y, z;
};
struct B24 {
    std::uint32_t d[6];
};
static_assert(sizeof(V3) == 12 && sizeof(B24) == 24, "by-value sizes");

JOAN_COUNTER(c_find_cell, "Map::find_cell");
JOAN_COUNTER(c_build_tree, "Map::build_tree");

std::uint8_t box_contains(void* box, void* pos) { return joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(box)->contains(joan::orig::arg::ptr(pos))); }  // 0x0060d5f0

struct H {
    // 0x0054a2c0 (ret 0xc)
    std::int32_t find_cell(void** cell_io, void** scene_io, void* pos) {
        c_find_cell.hit();
        B24 box;  // -0x1c
        joan::orig::as_aabb(&box)->set_point(joan::orig::arg::ptr(pos));  // 0x0060d430
        if (*cell_io != nullptr && *scene_io != nullptr && box_contains(jo::as_scene_s(*scene_io)->inner_box, pos) != 0) return 1;
        void* fallback = nullptr;  // -0x20
        std::int32_t count = 0;    // -0x24
        void** const cells = joan::orig::arg::as<void**>(joan::orig::as_world(this)->cells_in_box(&box, &count));  // 0x00429090
        std::int32_t start = 0;
        if (*cell_io != nullptr && box_contains(jo::as_cell(*cell_io)->box, pos) != 0) start = -1;  // bounds: 6 floats from +0x8
        if (fallback == nullptr) {
            if (start == -1) {
                fallback = *cell_io;
            } else if (count > 0) {
                fallback = cells[0];
            }
        }
        for (std::int32_t i = start; i < count; ++i) {
            void* const cell = i == -1 ? *cell_io : cells[i];
            for (std::int32_t j = 0; j < jo::as_cell(cell)->sector_count; ++j) {
                void* const scene = jo::as_cell(cell)->sectors[j].item;
                if (box_contains(jo::as_scene_s(scene)->inner_box, pos) != 0) {
                    *cell_io = cell;
                    *scene_io = scene;
                    return 2;
                }
            }
        }
        *cell_io = fallback;
        return 0;
    }

    // 0x00548880 (ret 0xc): node layout +0 parent, +4 / +8 children, +0xc split axis,
    // +0x10 bounds, +0x28 centre, +0x34 item (leaves).
    std::uint8_t* build_tree(void* parent, std::int32_t first, std::int32_t last) {
        (void)parent;
        c_build_tree.hit();
        if (!(last > first)) return nullptr;
        std::int32_t axis = 3;  // -0x44
        std::uint8_t** const items = global<std::uint8_t**>(d::g_bvh_block_ptrs);
        B24 bounds;  // -0x1c
        std::memcpy(&bounds, jo::as_cell(items[first])->box, sizeof bounds);
        V3 t6c;
        B24 centres;  // -0x34
        joan::orig::as_aabb(&centres)->set_point(joan::orig::arg::ptr(joan::orig::arg::as<V3*>(joan::orig::as_box(jo::as_cell(items[first])->box)->centre(&t6c))));  // 0x0060d430(0x0060d3a0)
        for (std::int32_t i = first + 1; i < last; ++i) {
            joan::orig::as_box(&bounds)->merge(joan::orig::arg::mptr(jo::as_cell(global<std::uint8_t**>(d::g_bvh_block_ptrs)[i])->box));  // 0x0060d030
            V3 t78;
            joan::orig::as_aabb(&centres)->extend(joan::orig::arg::ptr(joan::orig::arg::as<V3*>(joan::orig::as_box(jo::as_cell(global<std::uint8_t**>(d::g_bvh_block_ptrs)[i])->box)->centre(&t78))));  // 0x0060d490(0x0060d3a0)
        }
        V3 t84;
        const V3 centre = *joan::orig::arg::as<V3*>(joan::orig::as_box(&centres)->centre(&t84));  // -0x40, 0x0060d3a0
        float c[6];
        std::memcpy(c, &centres, sizeof c);
        const float dx = c[3] - c[0];
        const float dy = c[4] - c[1];
        const float dz = c[5] - c[2];
        if (dx >= dy && dx >= dz) axis = 0;
        else if (dy >= dz) axis = 1;
        else axis = 2;
        std::uint8_t* const node = global<std::uint8_t*>(d::g_a3ce0c);
        global<std::uint8_t*>(d::g_a3ce0c) = reinterpret_cast<std::uint8_t*>(jo::as_box_tree(node) + 1);
        jo::view<std::int32_t>(jo::as_box_tree(node)->u32_c) = axis;
        jo::view<void*>(jo::as_box_tree(node)->left) = nullptr;
        jo::view<void*>(jo::as_box_tree(node)->right) = nullptr;
        std::memcpy(jo::as_box_tree(node)->box, &bounds, sizeof bounds);
        jo::view<V3>(jo::as_box_tree(node)->mid) = centre;
        if (last - first == 1) {
            jo::view<std::uint8_t*>(jo::as_box_tree(node)->item) = global<std::uint8_t**>(d::g_bvh_block_ptrs)[first];
            jo::view<std::uint8_t*>(jo::as_cell(global<std::uint8_t**>(d::g_bvh_block_ptrs)[first])->ptr_4) = node;
            return node;
        }
        V3 key = centre;  // the callee gets the address of -0x40
        void* local60 = nullptr;  // -0x60: the callee's swap buffer for one element (a Cell*)
        std::int32_t mid = 0;
        const auto zero_or_nan = [](float v) { return !(v < 0.0f || v > 0.0f); };
        if (zero_or_nan(dx) && zero_or_nan(dy) && zero_or_nan(dz)) {
            mid = (first + last) / 2;
        } else {
            std::uintptr_t compare = fn::code_value(fn::cmp_center_z);
            if (axis == 0) compare = fn::code_value(fn::cmp_center_x);
            else if (axis == 1) compare = fn::code_value(fn::cmp_center_y);
            mid = first + joan::orig::arg::as<std::int32_t>(joan::orig::remove_matching(joan::orig::arg::mptr(static_cast<void*>(global<std::uint8_t**>(d::g_bvh_block_ptrs) + first)), joan::orig::arg::i32(last - first), static_cast<std::int32_t>(sizeof(void*)), joan::orig::arg::mptr(static_cast<void*>(&key)), compare, joan::orig::arg::mptr(static_cast<void*>(&local60))));
        }
        std::uint8_t* const left = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_map(this)->build_tree(joan::orig::arg::mptr(node), joan::orig::arg::i32(first), joan::orig::arg::i32(mid)));  // 0x00548880
        jo::view<std::uint8_t*>(jo::as_box_tree(node)->left) = left;
        if (jo::as_box_tree(node)->left != nullptr) jo::as_box_tree(node)->left->u32_0 = jo::as_box_tree(node);
        std::uint8_t* const right = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_map(this)->build_tree(joan::orig::arg::mptr(node), joan::orig::arg::i32(mid), joan::orig::arg::i32(last)));  // 0x00548880
        jo::view<std::uint8_t*>(jo::as_box_tree(node)->right) = right;
        if (jo::as_box_tree(node)->right != nullptr) jo::as_box_tree(node)->right->u32_0 = jo::as_box_tree(node);
        return node;
    }
};

#define U5(name, addr, fn) JOAN_REPLACE("game/units5", name, addr, fn)
U5("Map::find_cell", 0x0054a2c0, &H::find_cell);
U5("Map::build_tree", 0x00548880, &H::build_tree);
#undef U5

}  // namespace game_units5
}  // namespace

// ---------------------------------------- from modules/game_b82_10.cpp (game/b82_10)
namespace {
namespace game_b82_10 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_remove_matching, "remove_matching");

// 0x005499b0 remove_matching(base, count, size, key, cmp, tmp) - cdecl.
using Cmp = std::int32_t(JOAN_CDECL*)(void*, void*);
std::int32_t JOAN_CDECL remove_matching(std::uint8_t* base, std::int32_t count, std::int32_t size, void* key, Cmp cmp,
                                     void* tmp) {
    c_remove_matching.hit();
    std::int32_t i = 0;
    while (i < count) {
        std::uint8_t* const elem = base + static_cast<std::uint32_t>(i) * static_cast<std::uint32_t>(size);
        if (cmp(elem, key) >= 0) {
            std::uint8_t* const last =
                base + static_cast<std::uint32_t>(count - 1) * static_cast<std::uint32_t>(size);
            if (last != elem) {
                std::memcpy(joan::orig::arg::mptr(tmp), static_cast<void*>(elem), joan::orig::arg::u32(size));  // memcpy
                std::memcpy(static_cast<void*>(elem), static_cast<void*>(last), joan::orig::arg::u32(size));
                std::memcpy(static_cast<void*>(last), joan::orig::arg::ptr(tmp), joan::orig::arg::u32(size));
            }
            --count;
        } else {
            ++i;
        }
    }
    return count;
}

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_10", name, addr, fn)
B82("remove_matching", 0x005499b0, &remove_matching);
#undef B82

}  // namespace game_b82_10
}  // namespace

// ---------------------------------------- from modules/game_b102_06.cpp (game/b102_06)
namespace {
namespace game_b102_06 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

// ---- comparators (cdecl) ----
const float* box_center(void* const* a, float* tmp) {
    return joan::orig::arg::as<const float*>(joan::orig::as_box(jo::as_cell(*a)->box)->centre(joan::orig::arg::mptr(tmp)));  // 0x0060d3a0
}
int compare_sign(float d) {
    const float zero = global<float>(d::k_9cf5c4);
    if (d > zero) return 1;
    if (!(d >= zero)) return -1;  // less or unordered
    return 0;
}
int JOAN_CDECL cmp_center_x(void* const* a, const float* b) {
    float tmp[3];
    const float* c = box_center(a, tmp);
    const float d = c[0] - b[0];
    return compare_sign(d);
}
int JOAN_CDECL cmp_center_z(void* const* a, const float* b) {
    float tmp[3];
    const float* c = box_center(a, tmp);
    const float d = c[2] - b[2];
    return compare_sign(d);
}

// ---- static initialisers / atexit destructors ----
void JOAN_CDECL init_54d910() { joan::orig::div_a3ce10(); }
void JOAN_CDECL init_54dab0() { joan::orig::init_41bbc0(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b102_06", name, addr, fn)
B("cmp_center_x", 0x00549c30, &cmp_center_x);
B("cmp_center_z", 0x00549f90, &cmp_center_z);
B("init_54d910", 0x0054d910, &init_54d910);
B("init_54dab0", 0x0054dab0, &init_54dab0);
#undef B

}  // namespace game_b102_06
}  // namespace

// ---------------------------------------- from modules/game_b101_08.cpp (game/b101_08)
namespace {
namespace game_b101_08 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i32 = std::int32_t;


struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_find_name, "Table::find_name_from");

struct H {

    // ------------------------------------------------------------------ 0x0054a0e0
    void* find_name_from(const char* name, i32 start) {
        c_find_name.hit();
        for (i32 i = static_cast<i32>(static_cast<u32>(start) + 1u); i <= jo::self<jo::World>(this)->cell_count;
             i = static_cast<i32>(static_cast<u32>(i) + 1u)) {
            if (joan::crt::stricmp(joan::orig::arg::str(name), joan::orig::arg::str(jo::self<jo::World>(this)->cells[i - 1].item->name)) == 0)  // stricmp
                return jo::self<jo::World>(this)->cells[i - 1].item;
        }
        return nullptr;
    }

    // ------------------------------------------------------------------ 0x0054aa90
    void* ctor_54aa90() {
        jo::self<jo::BoxTree>(this)->u32_0 = nullptr;
        jo::self<jo::BoxTree>(this)->u32_c = 3;
        jo::self<jo::BoxTree>(this)->right = nullptr;
        jo::self<jo::BoxTree>(this)->left = nullptr;
        jo::self<jo::BoxTree>(this)->item = nullptr;
        return this;
    }
};

// ---------------------------------------------------------------------- 0x00549dc0
static i32 JOAN_CDECL cmp_center_y(void** elem, const float* key) {
    Vec3 tmp;
    const Vec3* const c = joan::orig::arg::as<const Vec3*>(joan::orig::as_box(jo::as_cell(*elem)->box)->centre(&tmp));  // 0x0060d3a0
    const float d = c->y - key[1];
    const float zero = global<float>(d::k_9cf5c4);
    if (d > zero) return 1;
    if (!(d >= zero)) return -1;  // less or unordered
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b101_08", name, addr, fn)
B("cmp_center_y", 0x00549dc0, &cmp_center_y);
B("Table::find_name_from", 0x0054a0e0, &H::find_name_from);
B("Obj::ctor_54aa90", 0x0054aa90, &H::ctor_54aa90);
#undef B

}  // namespace game_b101_08
}  // namespace

// ---------------------------------------- from modules/game_b87_06.cpp (game/b87_06)
namespace {
namespace game_b87_06 {

using joan::call::thiscall;


struct H {

    // ------------------------------------------------------------ 0x0054ac60 (ret 0)
    void clear_34_4() {
        if (jo::self<jo::BoxTree>(this)->item != nullptr) jo::as_cell(jo::self<jo::BoxTree>(this)->item)->ptr_4 = nullptr;
    }
    // ------------------------------------------------------------ 0x0054b460 (ret 0)
    std::uint32_t forward_620ab0() { joan::orig::as_bmp_res(this)->free(); return 0; }  // 0x00620ab0 (returns nothing; every caller chain ends in atexit or a discard)
};

#define B06(name, addr, fn) JOAN_REPLACE("game/b87_06", name, addr, fn)
B06("0x0054ac60 clear_34_4", 0x0054ac60, &H::clear_34_4);
B06("0x0054b460 forward_620ab0", 0x0054b460, &H::forward_620ab0);
#undef B06

}  // namespace game_b87_06
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
namespace {
namespace game_leaves80b {

using joan::call::thiscall;


JOAN_COUNTER(c_collect, "BoxTree::collect");

struct H {

    // 0x0054ad90
    void tree_collect(void* box, std::int32_t* count, void** out) {
        c_collect.hit();
        if ((joan::orig::arg::as<std::uint8_t>(joan::orig::as_box(box)->overlaps(joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::BoxTree>(this)->box)))) & 0xff) == 0) return;  // 0x0060d6b0 overlaps
        if (jo::self<jo::BoxTree>(this)->item != nullptr) {
            out[*count] = jo::self<jo::BoxTree>(this)->item;
            *count = *count + 1;
        }
        if (jo::view<void*>(jo::self<jo::BoxTree>(this)->left) != nullptr) joan::orig::as_box_tree(jo::view<void*>(jo::self<jo::BoxTree>(this)->left))->collect_54ad90(joan::orig::arg::mptr(box), joan::orig::arg::mptr(count), joan::orig::arg::mptr(out));  // 0x0054ad90
        if (jo::view<void*>(jo::self<jo::BoxTree>(this)->right) != nullptr) joan::orig::as_box_tree(jo::view<void*>(jo::self<jo::BoxTree>(this)->right))->collect_54ad90(joan::orig::arg::mptr(box), joan::orig::arg::mptr(count), joan::orig::arg::mptr(out));
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("BoxTree::collect", 0x0054ad90, &H::tree_collect);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/game_b106_03.cpp (game/b106_03)
namespace {
namespace game_b106_03 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i32 = std::int32_t;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;
using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::call::import_call;
using joan::img::obj;
struct Vec3 {  // a 12-byte float vector, by value (floats: the System V ABI passes them in XMM registers, as the callee 0x00560d80 declares)
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "by-value 12-byte argument");

constexpr Fpu kFpu{true};

i32 sq(i32 v) { return static_cast<i32>(static_cast<u32>(v) * static_cast<u32>(v)); }  // imul

// ---- shared game helpers (through the thunks the originals call) -------------------------
i32 unit_invalid(i32 id) { return joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* type_of(i16 kind) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(kind))); }  // 0x0042d0a0
u8* leader_of(const void* unit) { return joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->leader()); }           // 0x004216d0
i8 unit_dead(const void* unit) { return joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->dead()); }             // 0x0042d0e0
using joan::orig::helpers::random;  // 0x00590540
Vec3* vsub(Vec3* out, const void* a, const void* b) {                                 // 0x0060cc90
    return joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(joan::orig::arg::mptr(out), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
}
double length_sq(const Vec3* v) { return import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(v)); }
using joan::orig::helpers::crt_abs;

// Same-army test of 0x0054b480 (both blocks): -1 when the chain of checks is not complete,
// else 1 when both armies (0x00637da0 records +0x70) match, 0 when they differ.
i32 same_army(u8* self, u8* other, i8 special) {
    if (special != 0) return -1;
    if (jo::as_unit(self)->actor == nullptr) return -1;
    if (jo::as_unit(other)->actor == nullptr) return -1;
    if (jo::view<u32>(jo::as_unit(self)->actor->record) == 0) return -1;
    if (jo::view<u32>(jo::as_unit(other)->actor->record) == 0) return -1;
    if (jo::as_unit(self)->layer == 0) return -1;
    jo::Shot* const a = joan::orig::arg::as<jo::Shot*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(self)->actor->record))));  // 0x006af310
    jo::Shot* const b = joan::orig::arg::as<jo::Shot*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(other)->actor->record))));  // 0x006af310
    // a, b: Players::list records (Shot): BodyState::sub (+0x212 on x86, the PathSpace) and ::zone (+0x20e). The Body
    // view put the two behind one pointer fewer and read other words on x64 (a crash in the item lookup below).
    if (jo::view<void*>(a->state.sub) == nullptr) return -1;
    if (jo::view<u32>(a->state.zone) == 0) return -1;
    if (jo::view<void*>(b->state.sub) == nullptr) return -1;
    if (jo::view<u32>(b->state.zone) == 0) return -1;
    if (jo::view<void*>(a->state.sub) != jo::view<void*>(b->state.sub)) return -1;
    // the PathSpace::zones items are PfdZone records (+0x70 group)
    const jo::PfdZone* const pa = jo::as_pfd_zone(joan::orig::as_billboard(jo::as_path_space(jo::view<void*>(a->state.sub))->zones)->item(joan::orig::arg::i32(jo::view<u32>(a->state.zone))));  // 0x00637da0
    const jo::PfdZone* const pb = jo::as_pfd_zone(joan::orig::as_billboard(jo::as_path_space(jo::view<void*>(b->state.sub))->zones)->item(joan::orig::arg::i32(jo::view<u32>(b->state.zone))));  // 0x00637da0
    return jo::view<u32>(pa->group) == jo::view<u32>(pb->group) ? 1 : 0;
}

// Unit carried by / attached to +0x384 whose type +0x118 == 0xb with slot state 2.
bool carrier_blocks(u8* unit) {
    if (unit_invalid(jo::as_unit(unit)->weapon) != 0) return false;
    u8* const e = unit_get(jo::as_unit(unit)->weapon);
    u8* const ety = type_of(jo::as_unit(e)->type_id);
    if (jo::as_unit_type(ety)->kind != 0xb) return false;
    return static_cast<i8>(jo::view<u8*>(jo::as_unit_type(ety)->buffers[2])[jo::as_unit(unit)->weapon_slot]) == 2;
}

// Line test 0x00560d80 on +0x339 (nonzero = blocked); type +0x109 * 0.8f range.
bool line_blocked(u8* self, u8* other, u8* type) {
    const i32 r = ftol(kFpu.mul(static_cast<double>(jo::as_unit_type(type)->height), static_cast<double>(global<float>(d::flt_0_8))));
    const u8 res = joan::orig::arg::as<u8>(joan::orig::as_world_cell(jo::as_unit(self)->cell)->occluders_hit(jo::view<Vec3>(jo::as_unit(self)->position_), jo::view<Vec3>(jo::as_unit(other)->position_), joan::orig::arg::i32(r)));
    return res != 0;
}

bool zone_ok(i32 a14, u8* other) {  // 0x00456c70 record +0x3a contains other's position (0x0060d5f0)
    u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(joan::orig::arg::i32(a14)));  // 0x00456c70
    return joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::as_place(rec)->box)->contains(static_cast<void*>(&jo::as_unit(other)->position_))) != 0;
}

JOAN_COUNTER(c_find_54b480, "Unit::find_target_54b480");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ---- 0x0054b480 ------------------------------------------------------------------------
    // Node filter by mode[0] (0x0054bf3b .. 0x0054c3a3): true = go on to the range checks.
    bool mode_filter(const char* mode, u8* o, u8* type, i32 a14) {
        const i32 c = static_cast<i8>(mode[0]);
        if (c == 'P') return static_cast<i32>(jo::as_unit(o)->id) == global<i32>(d::g_player_unit_recno);
        if (c == 'E') {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hostile_to(joan::orig::arg::i16(jo::as_unit(o)->id))) == 0) return false;  // 0x005094d0
            if (jo::self<jo::Unit>(this)->cell == nullptr || line_blocked(self(), o, type)) return false;
            u8* const ty = type_of(jo::as_unit(o)->type_id);
            if (joan::orig::arg::as<i8>(joan::orig::as_game(o)->byte_2e1()) == 2) return false;  // 0x00432b90
            if ((jo::as_unit_type(ty)->trigger_flags & 1u) == 0) return false;
            if (jo::as_unit_type(ty)->unit_class == 8) return false;
            if (carrier_blocks(o)) return false;
            if (static_cast<i8>(mode[1]) == 'P') {
                if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_place_array))->is_missing_ca0(joan::orig::arg::i32(a14))) == 0) {  // 0x00456ca0
                    if (!zone_ok(a14, o)) return false;
                }
            }
            return true;
        }
        if (c == 'S') {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hostile_to(joan::orig::arg::i16(jo::as_unit(o)->id))) == 0) return false;  // 0x005094d0
            u8* const ty = type_of(jo::as_unit(o)->type_id);
            if (jo::as_unit_type(ty)->unit_class != 8) return false;
            if ((jo::as_unit_type(ty)->trigger_flags & 0x40u) == 0) return false;
            if (jo::self<jo::Unit>(this)->cell == nullptr || line_blocked(self(), o, ty)) return false;
            return true;
        }
        if (c == 'F') return joan::orig::arg::as<i8>(joan::orig::as_unit(this)->is_allied_with(joan::orig::arg::i16(jo::as_unit(o)->id))) != 0;  // 0x005095e0
        if (c == 'L') {
            if (leader_of(this) == nullptr) return false;
            const i32 oid = jo::as_unit(o)->id;
            return oid == static_cast<i32>(jo::as_unit(leader_of(this))->id);
        }
        if (c == 'O') return jo::view<u32>(jo::as_unit(o)->formation_slot) != 0;
        return true;
    }

    i32 find_target(const char* mode, i32 range, i32 use_leader, i32 a14, i32 a18, i32 a1c) {
        c_find_54b480.hit();
        if (jo::self<jo::Unit>(this)->world == nullptr) return 0;
        if (range == 0) range = 0x640;
        if (static_cast<i8>(mode[0]) == 'T') {
            if (unit_invalid(jo::self<jo::Unit>(this)->target_building) == 0) return jo::self<jo::Unit>(this)->target_building;
            return 0;
        }
        u8* const leader = leader_of(this);
        Vec3 pos;
        if (use_leader != 0 && leader != nullptr && leader != self()) {
            pos = jo::view<Vec3>(jo::as_unit(leader)->position_);
        } else {
            pos = jo::view<Vec3>(jo::self<jo::Unit>(this)->position_);
            use_leader = 0;
        }
        u8* const type = type_of(jo::self<jo::Unit>(this)->type_id);
        bool special_b;
        {
            const i32 k = jo::as_unit_type(type)->unit_class;
            if (k == 5 || k == 6) {
                special_b = true;
            } else if (k == 8) {
                const i32 s = jo::as_unit_type(type)->kind;
                special_b = s == 4 || s == 5 || s == 6 || s == 9 || s == 0xa || s == 0xc;
            } else {
                special_b = false;
            }
        }
        const i8 special = special_b ? 1 : 0;

        // Keep the current target (+0x2d7) in mode 'E' when still valid.
        if (unit_invalid(jo::self<jo::Unit>(this)->target_building) == 0 && static_cast<i8>(mode[0]) == 'E') {
            u8* const t = unit_get(jo::self<jo::Unit>(this)->target_building);
            if (unit_dead(t) == 0 && jo::as_unit(t)->markers != -1) {
                Vec3 tmp;
                const Vec3 d = *vsub(&tmp, &pos, jo::as_unit(t)->position_);
                const double ls = length_sq(&d);
                if (static_cast<double>(sq(range)) > ls && random(0x64) <= 0x5f) {
                    i8 keep = 1;
                    const i32 army = same_army(self(), t, special);
                    if (army >= 0) keep = static_cast<i8>(army);
                    if (carrier_blocks(t)) keep = 0;
                    if (keep != 0) {
                        bool check = true;
                        if (static_cast<i8>(mode[1]) == 'P') {
                            check = joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_place_array))->is_missing_ca0(joan::orig::arg::i32(a14))) == 0 && zone_ok(a14, t);
                        }
                        if (check) {
                            bool far = false;
                            if (a1c != 0) {
                                const double v = joan::orig::arg::as<double>(joan::orig::as_unit(this)->value_of_type(joan::orig::arg::u32(static_cast<i32>(jo::as_unit(t)->id))));  // 0x00505b50
                                far = crt_abs(ftol(v)) > a1c;
                            }
                            if (!far && jo::self<jo::Unit>(this)->cell != nullptr && !line_blocked(self(), t, type)) {
                                return jo::self<jo::Unit>(this)->target_building;
                            }
                        }
                    }
                }
            }
            jo::self<jo::Unit>(this)->target_building = 0;
        }

        if (static_cast<i8>(mode[0]) == 'L') {
            if (leader_of(this) == nullptr) return -1;
            if (unit_dead(leader_of(this)) != 0) return -1;
            if (jo::as_unit(leader_of(this))->markers == -1) return -1;
            u8* const l = leader_of(this);
            Vec3 tmp;
            const Vec3 d = *vsub(&tmp, &pos, jo::as_unit(l)->position_);
            const double ls = length_sq(&d);
            if (static_cast<double>(sq(range)) > ls) return jo::as_unit(l)->id;
            return 0;
        }

        float best = static_cast<float>(sq(range));
        i32 best2 = -1;    // [ebp-0x68]
        i32 best3 = -1;    // [ebp-0x70]
        i32 target = 0;    // [ebp-0x74]
        i32 prio = 0;      // [ebp-0x6c] (see header)
        float box[6];
        box[0] = static_cast<float>(kFpu.sub(pos.x, static_cast<double>(range)));
        box[1] = static_cast<float>(kFpu.sub(pos.y, static_cast<double>(range)));
        box[2] = static_cast<float>(kFpu.sub(pos.z, static_cast<double>(range)));
        box[3] = static_cast<float>(kFpu.add(static_cast<double>(range), pos.x));
        box[4] = static_cast<float>(kFpu.add(static_cast<double>(range), pos.y));
        box[5] = static_cast<float>(kFpu.add(static_cast<double>(range), pos.z));
        i32 count = 0;
        void** const cells = joan::orig::arg::as<void**>(joan::orig::as_scene(jo::as_world(jo::self<jo::Unit>(this)->world)->scene)->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x005ef610
        i32 picked = 0;
        i32 limit;
        if (range < 0xc8) {
            limit = 2;
        } else if (range < 0x3e8) {
            limit = 5;
        } else if (range < 0x7d0) {
            limit = 0xa;
        } else if (range < 0xbb8) {
            limit = 0x14;
        } else {
            limit = 0x64;
        }
        float dist = 0.0f;  // [ebp-0xa8], always written before it is read
        // One unit candidate of a sector's list (the other actors are skipped by the kind test).
        auto visit = [&](u8* node) {
            if (node == jo::view<u8*>(jo::self<jo::Unit>(this)->actor) || jo::as_actor(node)->kind != 0xb) return;
            u8* const o = static_cast<u8*>(jo::as_actor(node)->owner);
            if (!(jo::as_unit(o)->hit_points > 0 || jo::as_unit(o)->state_120 != 0)) return;
            if (jo::as_unit(o)->markers == -1) return;
            // The original tests the army (same_army) before the distance; both are pure lookups / maths and
            // either one failing skips the unit, so the cheap distance goes first. Each difference is
            // evaluated twice by the original (x87 reloads): the same value, computed once here.
            {
                const double dx = kFpu.sub(pos.x, reinterpret_cast<const float*>(jo::as_unit(o)->position_)[0]);
                double s = kFpu.mul(dx, dx);
                const double dy = kFpu.sub(pos.y, reinterpret_cast<const float*>(jo::as_unit(o)->position_)[1]);
                s = kFpu.add(s, kFpu.mul(dy, dy));
                const double dz = kFpu.sub(pos.z, reinterpret_cast<const float*>(jo::as_unit(o)->position_)[2]);
                s = kFpu.add(s, kFpu.mul(dz, dz));
                dist = static_cast<float>(s);
            }
            if (dist > best) return;
            if (same_army(self(), o, special) == 0) return;
            if (!mode_filter(mode, o, type, a14)) return;
            consider(o, range, use_leader, a18, a1c, dist, best, best2, best3, target, prio, picked, type);
        };
        for (i32 i = 0; i < count; ++i) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_scene_s(cells[i])->first_actor());  // 0x005effa0
            u8* node = first;
            if (node == nullptr) continue;
            // [Debug] UnitIndex (en3d/scene_lists.hpp): the sector's units in walk order (the `picked > limit` stop
            // falls on a unit or on an actor the kind test skips - the same candidates either way). When a list is
            // edited while a candidate is looked at, the walk goes on from its neighbour as the original.
            if (const std::vector<u8*>* const units = joan::en3d::sector_units(cells[i])) {
                const std::uint32_t edits = joan::en3d::g_unit_list_edits;
                bool walk_on = false, stop = false;
                for (u8* const n : *units) {
                    if (picked > limit) {
                        stop = true;
                        break;
                    }
                    visit(n);
                    if (joan::en3d::g_unit_list_edits != edits) {
                        node = reinterpret_cast<u8*>(jo::as_actor(n)->scene_prev);
                        walk_on = true;
                        break;
                    }
                }
                if (stop || !walk_on) continue;
                while (node != nullptr && node != first) {
                    if (picked > limit) break;
                    visit(node);
                    node = reinterpret_cast<u8*>(jo::as_actor(node)->scene_prev);
                }
                continue;
            }
            do {
                if (picked > limit) break;
                visit(node);
                node = reinterpret_cast<u8*>(jo::as_actor(node)->scene_prev);
            } while (node != nullptr && node != first);
        }
        if (static_cast<i8>(mode[0]) == 'E' && random(0x64) <= 0x32) {
            if (unit_invalid(jo::self<jo::Unit>(this)->target_building) != 0 || unit_dead(unit_get(jo::self<jo::Unit>(this)->target_building)) != 0 ||
                jo::as_unit(unit_get(jo::self<jo::Unit>(this)->target_building))->markers == -1) {
                jo::self<jo::Unit>(this)->target_building = static_cast<i16>(target);
            } else if (target != 0) {
                jo::self<jo::Unit>(this)->target_building = static_cast<i16>(target);
            }
        }
        return target;
    }

    // 0x0054c3a8 .. 0x0054c916 for one candidate (range restored by the caller's copy).
    void consider(u8* o, i32 range, i32 use_leader, i32 a18, i32 a1c, float& dist, float& best, i32& best2,
                  i32& best3, i32& target, i32& prio, i32& picked, u8* type) {
        const i32 id = jo::as_unit(o)->id;
        u8* const oty = type_of(jo::as_unit(o)->type_id);
        if (jo::as_unit_type(oty)->unit_class == 0 && range >= 0x190) {
            range = ftol(kFpu.div(static_cast<double>(range), global<double>(d::dbl_1_2)));
        }
        const i32 nn = sq(range);
        const double dnn = static_cast<double>(nn);
        if (!(dnn >= static_cast<double>(dist))) return;
        if (a18 != 0) {
            const double v = joan::orig::arg::as<double>(joan::orig::as_unit(this)->heading_to_unit(joan::orig::arg::i32(id)));  // 0x00505960
            if (crt_abs(ftol(v)) > a18) return;
        }
        if (a1c != 0) {
            const double v = joan::orig::arg::as<double>(joan::orig::as_unit(this)->value_of_type(joan::orig::arg::u32(id)));  // 0x00505b50
            if (crt_abs(ftol(v)) > a1c) return;
        }
        ++picked;
        const i32 sort = jo::self<jo::Unit>(this)->target_select;
        if (sort == 1) {
            if (use_leader != 0) {
                Vec3 tmp;
                const Vec3 d = *vsub(&tmp, jo::self<jo::Unit>(this)->position_, jo::as_unit(o)->position_);
                dist = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(&d)));  // length_sq
            }
            if (!(dnn > static_cast<double>(best))) {
                best = dist;
                target = jo::as_unit(o)->id;
            }
            if (best > dist && random(0x64) <= 0x32) {
                best = dist;
                target = jo::as_unit(o)->id;
            }
        } else if (sort == 2) {
            const i32 hp = joan::orig::arg::as<i32>(joan::orig::as_unit(o)->hit_points_max());  // 0x00523320
            const i32 v = static_cast<i32>(jo::view<u32>(jo::as_unit(o)->state_120) * static_cast<u32>(hp) + jo::view<u32>(jo::as_unit(o)->hit_points));
            if (best2 == -1) {
                best2 = v;
                target = jo::as_unit(o)->id;
            }
            if (best2 > v && random(0x64) <= 0x32) {
                best2 = v;
                target = jo::as_unit(o)->id;
            }
        } else if (sort == 3) {
            if (best3 == -1) {
                best3 = jo::as_unit(o)->rank;
                target = jo::as_unit(o)->id;
            }
            if (best3 < jo::as_unit(o)->rank && random(0x64) <= 0x32) {
                best3 = jo::as_unit(o)->rank;
                target = jo::as_unit(o)->id;
            }
        } else if (sort == 5) {
            if (!(dnn > static_cast<double>(best)) || target == 0) {
                best = dist;
                target = jo::as_unit(o)->id;
                prio = joan::orig::arg::as<i32>(joan::orig::as_unit(type)->rank_527790(joan::orig::arg::u32(static_cast<i32>(jo::as_unit(o)->type_id))));  // 0x00527790
            } else {
                unit_get(target);  // result unused ([ebp-0xc4])
                const i32 p = joan::orig::arg::as<i32>(joan::orig::as_unit(type)->rank_527790(joan::orig::arg::u32(static_cast<i32>(jo::as_unit(o)->type_id))));  // 0x00527790
                if (p > prio) {
                    best = dist;
                    target = jo::as_unit(o)->id;
                    prio = p;
                } else if (p == prio) {
                    if (best > dist && random(0x64) <= 0x50) {
                        best = dist;
                        target = jo::as_unit(o)->id;
                        prio = p;
                    }
                }
            }
        } else if (sort == 4) {
            if (!(dnn > static_cast<double>(best)) || target == 0) {
                best = dist;
                target = jo::as_unit(o)->id;
                return;
            }
            u8* const cur = unit_get(target);
            bool o_ranged = jo::as_unit_type(type_of(jo::as_unit(o)->type_id))->unit_class == 5;
            if (!o_ranged) o_ranged = jo::as_unit_type(type_of(jo::as_unit(o)->type_id))->unit_class == 6;
            bool cur_ranged = jo::as_unit_type(type_of(jo::as_unit(cur)->type_id))->unit_class == 5;
            if (!cur_ranged) cur_ranged = jo::as_unit_type(type_of(jo::as_unit(cur)->type_id))->unit_class == 6;
            if (o_ranged) {
                if (cur_ranged) {
                    if (best > dist && random(0x64) <= 0x50) {
                        best = dist;
                        target = jo::as_unit(o)->id;
                    }
                } else {
                    best = dist;
                    target = jo::as_unit(o)->id;
                }
            } else if (cur_ranged) {
                const double half = kFpu.div(static_cast<double>(best), static_cast<double>(global<float>(d::flt_2)));
                if (half > static_cast<double>(dist)) {
                    best = dist;
                    target = jo::as_unit(o)->id;
                }
            } else {
                if (best > dist && random(0x64) <= 0x50) {
                    best = dist;
                    target = jo::as_unit(o)->id;
                }
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_03", name, addr, fn)
B("Unit::find_target_54b480", 0x0054b480, &H::find_target);
#undef B

}  // namespace game_b106_03
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

using joan::call::cdecl_call;

using joan::hook::global;

// 0x0054d820
void JOAN_CDECL init_pi_a3ce08() {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a3ce08) = static_cast<float>(t * 4.0);  // qword 0x9cf788 = 4.0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("init_pi_a3ce08", 0x0054d820, &init_pi_a3ce08);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {

using joan::hook::global;

void JOAN_CDECL div_a3ce10() {  // 0x0054d9c0
    global<float>(d::g_a3ce10) = global<float>(d::g_a3ce08) / global<float>(d::flt_2);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("div_a3ce10", 0x0054d9c0, &div_a3ce10);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

using joan::hook::global;

void JOAN_CDECL div_a3ce08() {  // 0x0054da70
    global<float>(d::g_a3ce14) = global<float>(d::g_a3ce08) / global<float>(d::flt_3);
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("Globals::div_a3ce08", 0x0054da70, &div_a3ce08);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

u32 JOAN_CDECL init_54d7d0() { joan::orig::init_pi_a3ce08(); return 0u; }  // eax dead: _initterm reloads it
u32 JOAN_CDECL init_54da10() { joan::orig::div_a3ce08(); return 0u; }  // eax dead: _initterm reloads it

JOAN_REPLACE(M, "init_54d7d0", 0x0054d7d0, &init_54d7d0);
JOAN_REPLACE(M, "init_54da10", 0x0054da10, &init_54da10);

}  // namespace game_b103_06
}  // namespace
#undef M

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::World::add_entry() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_03::H::world_add>(this)); }  // 0x42ce10
auto joan::orig::World::cleanup() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b87_05::H::cleanup>(this)); }  // 0x440320
auto joan::orig::World::build_tree() -> void { joan::bind::member<&::game_world4::H::build_tree>(this); }  // 0x44a260
auto joan::orig::Map::create_territories(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_20::H::create_territories>(this, a0)); }  // 0x547590
auto joan::orig::World::stream_cells(void* a0, void* a1, std::int32_t a2) -> void { joan::bind::member<&::game_world::H::stream_cells>(this, a0, a1, a2); }  // 0x547630
auto joan::orig::World::cells_pending(void* a0, void* a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_world::H::cells_pending>(this, a0, a1, a2)); }  // 0x5479a0
auto joan::orig::Map::add_cells(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_02::H::node_add>(this, a0)); }  // 0x547bf0
auto joan::orig::WorldCell::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::scalar_delete>(this, a0)); }  // 0x548430
auto joan::orig::Map::cell(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_02_2::H::get_548600>(this, a0)); }  // 0x548600
auto joan::orig::Map::build_tree(void* a0, std::int32_t a1, std::int32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_units5::H::build_tree>(this, a0, a1, a2)); }  // 0x548880
auto joan::orig::Map::release_tree() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::release_7e>(this)); }  // 0x54a040
auto joan::orig::Map::find_cell(void* a0, void* a1, void* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_units5::H::find_cell>(this, a0, a1, a2)); }  // 0x54a2c0
auto joan::orig::lerp_54a8a0(float a0, float a1, float a2) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_b84_02_2::lerp_54a8a0>(a0, a1, a2)); }  // 0x54a8a0
auto joan::orig::Map::sector(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_02_2::H::get_54aa00>(this, a0)); }  // 0x54aa00
auto joan::orig::BoxTree::reset_leaf() -> void { joan::bind::member<&::game_b87_06::H::clear_34_4>(this); }  // 0x54ac60
auto joan::orig::BoxTree::collect_54ad90(void* a0, void* a1, void* a2) -> void { joan::bind::member<&::game_leaves80b::H::tree_collect>(this, a0, a1, a2); }  // 0x54ad90
auto joan::orig::BmpRes::free_thunk() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_06::H::forward_620ab0>(this)); }  // 0x54b460
auto joan::orig::Unit::find_target_54b480(const char* a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b106_03::H::find_target>(this, a0, a1, a2, a3, a4, a5)); }  // 0x54b480
auto joan::orig::init_pi_a3ce08() -> void { joan::bind::free<&::game_b96_04::init_pi_a3ce08>(); }  // 0x54d820
auto joan::orig::div_a3ce10() -> void { joan::bind::free<&::game_b100_08::div_a3ce10>(); }  // 0x54d9c0
auto joan::orig::div_a3ce08() -> void { joan::bind::free<&::game_b94_04::div_a3ce08>(); }  // 0x54da70
