// en3d/bonetree.cpp: functions of the original d:\ww\en3d\bonetree.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
//
//      0x005b9830 BoneTree::BoneTree(const BoneTree&) (ret 4) - (n + 1) * 0x1c records (bonetree.cpp,
//                 line base 0xa16eac + 6), +8 = +4 + 0x1c
//
//      0x005ba110 BoneTable::index_of(name) (ret 4) - 0xa56a68 lookup, -999 when unknown
namespace {
namespace en3d_b85_04 {

void* const kMemory = obj(d::g_mem);  // allocator

void* game_memset(void* p, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

const char* const kBonetreeCpp = str(d::s_d_ww_en3d_bonetree_cpp);  // "d:\ww\en3d\bonetree.cpp"

JOAN_COUNTER(c_bone_copy, "BoneTree::BoneTree(copy)");

struct H {
    // 0x005b9830
    H* bone_copy(const std::uint8_t* src) {
        c_bone_copy.hit();
        game_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::BoneTree)));  // 0xc (x86)
        jo::self<jo::BoneTree>(this)->count = jo::as_bone_tree(src)->count;
        if (jo::as_bone_tree(src)->block != nullptr) {
            jo::self<jo::BoneTree>(this)->block = joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32((jo::self<jo::BoneTree>(this)->count + 1) * 0x1c), joan::orig::arg::str(kBonetreeCpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16eac)) + 6));  // 0x0058e7b0
            game_memset(jo::self<jo::BoneTree>(this)->block, 0, static_cast<std::uint32_t>((jo::self<jo::BoneTree>(this)->count + 1) * 0x1c));
            jo::view<std::uint8_t*>(jo::self<jo::BoneTree>(this)->nodes) = jo::view<std::uint8_t*>(jo::self<jo::BoneTree>(this)->block) + 0x1c;
            std::memcpy(jo::view<std::uint8_t*>(jo::self<jo::BoneTree>(this)->nodes) - 0x1c, jo::view<const std::uint8_t*>(jo::as_bone_tree(src)->nodes) - 0x1c, 0x1c);
            for (std::int32_t i = 0; i < jo::self<jo::BoneTree>(this)->count; ++i)
                std::memcpy(jo::view<std::uint8_t*>(jo::self<jo::BoneTree>(this)->nodes) + i * 0x1c, jo::view<const std::uint8_t*>(jo::as_bone_tree(src)->nodes) + i * 0x1c, 0x1c);
        }
        return this;
    }

    // 0x005ba110
    std::int32_t bone_index_of(const void* name) {
        const std::int32_t p = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(obj(d::g_bone_name_res))->find_5ba6e0(joan::orig::arg::str(name)));  // 0x005ba6e0
        if (p == 0) return -999;
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_table(this)->find(p));  // 0x005ba160
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("BoneHierarchy::BoneHierarchy_5b9830", 0x005b9830, &H::bone_copy);
E3D("BoneHierarchy::find_bone_5ba110", 0x005ba110, &H::bone_index_of);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04.cpp (en3d/b86_04)
//
//      0x005b9960 BoneTree::~BoneTree() wrapper - 0x005b9a80.
namespace {
namespace en3d_b86_04 {

struct H {
    // 0x005b9960
    void bonetree_dtor() { joan::orig::as_bone_tree(this)->free(); }  // 0x005b9a80
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b86_04", name, addr, fn)
EN3D("BoneHierarchy::~BoneHierarchy", 0x005b9960, &H::bonetree_dtor);
#undef EN3D

}  // namespace en3d_b86_04
}  // namespace

// ---------------------------------------- from modules/en3d_helpers37.cpp (en3d/helpers37)
//
//      0x005b9990 BoneTree::allocate(n)          - (n + 1) * 0x1c zeroed nodes, +8 = node 0
//                 (node -1 is the root sentinel), 0x005b9790 init on every node
//
//      0x005b9df0 BoneTree::set_parent(i, parent) - unlinks node i from its parent's child list,
//                 then appends it to the new parent's list (-999 = none)
//
//      0x005ba040 BoneTree::first_child(i)       - node[i].+0x18
//
//      0x005ba0a0 BoneTree::next_child(i, child) - node[child].+0x10
namespace {
namespace en3d_helpers37 {

JOAN_COUNTER(c_tree_allocate, "BoneTree::allocate");
JOAN_COUNTER(c_tree_set_parent, "BoneTree::set_parent");

constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kBonetreeFile(d::s_d_ww_en3d_bonetree_cpp);  // "d:\ww\en3d\bonetree.cpp"
constexpr std::int32_t kNone = -999;

struct H {
    std::uint8_t* node(std::int32_t i) { return jo::view<std::uint8_t*>(jo::self<jo::BoneTree>(this)->nodes) + i * 0x1c; }

    // 0x005b9990 BoneTree::allocate(n).
    void* tree_allocate(std::int32_t n) {
        c_tree_allocate.hit();
        jo::self<jo::BoneTree>(this)->count = n;
        jo::self<jo::BoneTree>(this)->block = joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32((jo::self<jo::BoneTree>(this)->count + 1) * 0x1c), joan::orig::arg::str(img::str(kBonetreeFile)), joan::orig::arg::i32(global<std::int16_t>(d::g_a16eb0) + 4));  // 0x0058e7b0
        std::memset(joan::orig::arg::mptr(jo::self<jo::BoneTree>(this)->block), 0, joan::orig::arg::u32((jo::self<jo::BoneTree>(this)->count + 1) * 0x1c));  // memset
        jo::view<std::uint8_t*>(jo::self<jo::BoneTree>(this)->nodes) = jo::view<std::uint8_t*>(jo::self<jo::BoneTree>(this)->block) + 0x1c;
        joan::orig::as_bone_node(jo::view<std::uint8_t*>(jo::self<jo::BoneTree>(this)->nodes) - 0x1c)->init(joan::orig::arg::u32(-1));  // 0x005b9790
        for (std::int32_t i = 0; i < jo::self<jo::BoneTree>(this)->count; ++i) joan::orig::as_bone_node(node(i))->init(joan::orig::arg::u32(i));
        return this;  // eax = this (loop test reload)
    }

    // 0x005b9df0 BoneTree::set_parent(i, parent). Node: +8 parent, +0xc prev, +0x10 next sibling,
    // +0x14 last child, +0x18 first child. eax is unused by the callers (BoneTree::read).
    void tree_set_parent(std::int32_t i, std::int32_t parent) {
        c_tree_set_parent.hit();
        std::uint8_t* const n = node(i);
        if (jo::as_bone_node(n)->parent == parent) return;
        if (jo::as_bone_node(n)->parent != kNone) {
            std::uint8_t* const old = node(jo::as_bone_node(n)->parent);
            const std::int32_t next = jo::as_bone_node(n)->next_sibling;
            const std::int32_t prev = jo::as_bone_node(n)->prev_sibling;
            if (next != kNone)
                jo::as_bone_node(node(next))->prev_sibling = prev;
            else
                jo::as_bone_node(old)->last_child = prev;
            if (prev != kNone)
                jo::as_bone_node(node(prev))->next_sibling = next;
            else
                jo::as_bone_node(old)->first_child = next;
            jo::as_bone_node(n)->next_sibling = kNone;
            jo::as_bone_node(n)->prev_sibling = kNone;
            jo::as_bone_node(n)->parent = kNone;
        }
        if (parent == kNone) return;
        std::uint8_t* const p = node(parent);
        const std::int32_t first = jo::as_bone_node(p)->first_child;
        std::int32_t second = kNone;
        if (first == kNone) {
            jo::as_bone_node(p)->last_child = i;
        } else {
            second = jo::as_bone_node(node(first))->prev_sibling;
            jo::as_bone_node(node(first))->prev_sibling = i;
        }
        if (second == kNone)
            jo::as_bone_node(p)->first_child = i;
        else
            jo::as_bone_node(node(second))->next_sibling = i;
        jo::as_bone_node(n)->next_sibling = first;
        jo::as_bone_node(n)->prev_sibling = second;
        jo::as_bone_node(n)->parent = parent;
    }

    // 0x005ba040 BoneTree::first_child(i).
    std::int32_t tree_first_child(std::int32_t i) { return jo::as_bone_node(node(i))->first_child; }

    // 0x005ba0a0 BoneTree::next_child(i, child).
    std::int32_t tree_next_child(std::int32_t, std::int32_t child) { return jo::as_bone_node(node(child))->next_sibling; }
};

#define HELPER37(name, addr, fn) JOAN_REPLACE("en3d/helpers37", name, addr, fn)
HELPER37("BoneHierarchy::init", 0x005b9990, &H::tree_allocate);
HELPER37("BoneHierarchy::set_parent", 0x005b9df0, &H::tree_set_parent);
HELPER37("BoneTree::first_child", 0x005ba040, &H::tree_first_child);
HELPER37("BoneTree::next_child", 0x005ba0a0, &H::tree_next_child);
#undef HELPER37

}  // namespace en3d_helpers37
}  // namespace

// ---------------------------------------- from modules/en3d_b87_02.cpp (en3d/b87_02)
//
//      0x005b9a80 BoneTree::free() - frees +4 (0x0058ed70, bonetree.cpp line word 0xa16eb4 + 3),
//                 zeroes +4 / +8.
namespace {
namespace en3d_b87_02 {

void* const kMemory = obj(d::g_mem);  // allocator (0x0058ed70 free)

struct H {
    // 0x005b9a80
    void bonetree_free() {
        if (jo::self<jo::BoneTree>(this)->block == nullptr) return;
        joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::self<jo::BoneTree>(this)->block), str(d::s_d_ww_en3d_bonetree_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16eb4)) + 3));  // 0x0058ed70
        jo::self<jo::BoneTree>(this)->block = nullptr;
        jo::self<jo::BoneTree>(this)->nodes = nullptr;
    }
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b87_02", name, addr, fn)
EN3D("BoneHierarchy::deinit", 0x005b9a80, &H::bonetree_free);
#undef EN3D

}  // namespace en3d_b87_02
}  // namespace

// ---------------------------------------- from modules/en3d_helpers32.cpp (en3d/helpers32)
//
//      0x005b9af0 BoneTree::read(reader)         - bones (id, name) and links, bone-tree table
//
//      0x005b9fe0 BoneTree::parent(i)            - +8[i * 0x1c + 8]
namespace {
namespace en3d_helpers32 {

JOAN_COUNTER(c_tree_read, "BoneTree::read");

std::int32_t read_int(void* reader) { return joan::orig::as_file_mem(reader)->file_get_long(); }  // 0x005893c0
void read_bytes(void* reader, void* out, std::int32_t size) {
    joan::orig::as_file_mem(reader)->file_read(joan::orig::arg::mptr(out), joan::orig::arg::u32(size));  // 0x00588a80
}

// Stack dynamic array (ilib/dynarr.cpp, 0x1d bytes used on x86, 0x21 on x64).
struct alignas(alignof(void*)) DynArr {
    std::uint8_t bytes[((sizeof(joan::orig::DynArray) > 0x20 ? sizeof(joan::orig::DynArray) : 0x20) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};
// BoneTree::read records.
struct BoneRec {
    std::int32_t id;
    char name[0x32];
    char nul;
    char pad;  // uninitialised in the original; only copied into the temporary array
};
static_assert(sizeof(BoneRec) == 0x38, "BoneRec");
struct Link {
    std::int32_t child;
    std::int32_t parent;
};

struct H {
    // 0x005b9af0 BoneTree::read(reader).
    std::int32_t tree_read(void* reader) {
        c_tree_read.hit();
        const std::int32_t count = read_int(reader);
        std::int32_t size = count;
        DynArr bones, links;
        joan::orig::dynarray(&bones)->construct(0x38, 0x1e, 0);  // 0x005843d0
        joan::orig::dynarray(&links)->construct(8, 0x1e, 0);
        BoneRec rec;
        rec.pad = 0;
        Link link;
        for (std::int32_t i = 0; i < count; ++i) {
            rec.id = read_int(reader);
            if (rec.id >= size) size = rec.id + 1;
            read_bytes(reader, rec.name, 0x32);
            rec.nul = 0;
            joan::orig::dynarray(&bones)->linkin(&rec);               // 0x00584620 add
            const std::int32_t children = read_int(reader);
            for (std::int32_t k = 0; k < children; ++k) {
                link.parent = rec.id;
                link.child = read_int(reader);
                if (link.child >= size) size = link.child + 1;
                joan::orig::dynarray(&links)->linkin(&link);
            }
        }
        joan::orig::as_bone_tree(this)->allocate(joan::orig::arg::i32(size));  // 0x005b9990
        for (std::int32_t i = 1; i <= joan::orig::as_dynarray(&bones)->size(); ++i) {  // 0x00417ad0 count
            std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(&bones)->get(joan::orig::arg::i32(i)));  // 0x00417200 get
            const std::int32_t name_id = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(img::obj(d::g_bone_name_res))->id(joan::orig::arg::str(static_cast<void*>(e + 4))));  // 0x005ba8b0
            jo::self<jo::BoneTree>(this)->nodes[jo::as_name_entry(e)->index].name_id = name_id;
        }
        for (std::int32_t i = 0; i < size; ++i) joan::orig::as_bone_tree(this)->set_parent(joan::orig::arg::i32(i), joan::orig::arg::i32(-1));  // 0x005b9df0
        for (std::int32_t i = 1; i <= joan::orig::as_dynarray(&links)->size(); ++i) {
            std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(&links)->get(joan::orig::arg::i32(i)));
            joan::orig::as_bone_tree(this)->set_parent(joan::orig::arg::i32(reinterpret_cast<const Link*>(e)->child), joan::orig::arg::i32(reinterpret_cast<const Link*>(e)->parent));
        }
        joan::orig::dynarray(&links)->destruct();  // 0x005844b0
        joan::orig::dynarray(&bones)->destruct();
        return 1;
    }

    // 0x005b9fe0 BoneTree::parent(i).
    std::int32_t tree_parent(std::int32_t i) { return jo::self<jo::BoneTree>(this)->nodes[i].parent; }
};

#define HELPER32(name, addr, fn) JOAN_REPLACE("en3d/helpers32", name, addr, fn)
HELPER32("BoneHierarchy::read_file", 0x005b9af0, &H::tree_read);
HELPER32("BoneHierarchy::get_parent", 0x005b9fe0, &H::tree_parent);
#undef HELPER32

}  // namespace en3d_helpers32
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
//
//      0x005ba010 Rec::field_14(i)                (thiscall, ret 4) - [[+8] + i*0x1c + 0x14].
//
//      0x005ba070 Rec::field_c(unused, i)         (thiscall, ret 8) - [[+8] + i*0x1c + 0xc].
//
//      0x005ba0d0 Rec::name(i)                    (thiscall, ret 4) - 0x005ba910 on 0xa56a68.
namespace {
namespace game_b103_11 {

struct H {
    // 0x005ba010
    u32 rec_14(i32 i) { return jo::view<u32>(jo::self<jo::BoneTree>(this)->nodes[i].last_child); }
    // 0x005ba070
    u32 rec_c(u32 /*unused*/, i32 i) { return jo::view<u32>(jo::self<jo::BoneTree>(this)->nodes[i].prev_sibling); }
    // 0x005ba0d0
    std::uintptr_t rec_name(i32 i) {  // the name string (a pointer)
        const u32 id = jo::view<u32>(jo::self<jo::BoneTree>(this)->nodes[i].name_id);
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_names(obj(d::g_bone_name_res))->get(joan::orig::arg::u32(id)));  // 0x005ba910
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Rec::field_14", 0x005ba010, &H::rec_14);
B103("Rec::field_c", 0x005ba070, &H::rec_c);
B103("Rec::name", 0x005ba0d0, &H::rec_name);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/en3d_b83_08.cpp (en3d/b83_08)
//
//      0x005ba160 BoneTable::find(id) (ret 4)          - index of the 0x1c entry with +4 == id, -999
namespace {
namespace en3d_b83_08 {

struct H {
    // 0x005ba160
    std::int32_t bone_find(std::int32_t id) {
        for (std::int32_t i = 0; i < jo::self<jo::BoneTree>(this)->count; ++i) {
            if (jo::view<std::uint32_t>(jo::self<jo::BoneTree>(this)->nodes[i].name_id) == static_cast<std::uint32_t>(id)) return i;
        }
        return -999;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b83_08", name, addr, fn)
E3D("BoneHierarchy::find_bone_5ba160", 0x005ba160, &H::bone_find);
#undef E3D

}  // namespace en3d_b83_08
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
//
//      0x005b0620 ActorDef::load_bones(dir, name, rec) (ret 0xc) - <dir><name>.AM3 (0x005c5400) or
//                 the record's bone table (0x005c5610), 0x005c56d0, bone map +0x14 (0x005ba1d0 /
//                 0x0058e7b0 / 0x005ba300); 0 / 1
//
//      0x005ba300 BoneTable::map(out, other) (ret 8) - per bone of other: index here by id (-999)
//
//      0x005ba1d0 BoneTable::differs(other) (ret 4)  - 0 when either table is all-zero or ids match
namespace {
namespace en3d_b84_01 {

JOAN_COUNTER(c_bone_map, "BoneTable::map");
JOAN_COUNTER(c_bone_differs, "BoneTable::differs");

struct H {
    std::uint32_t bone_id(std::int32_t i) { return jo::view<std::uint32_t>(jo::self<jo::BoneTree>(this)->nodes[i].name_id); }

    // 0x005ba300
    void bone_map(std::int32_t* out, H* other) {
        c_bone_map.hit();
        std::int32_t other_zero = 1;
        std::int32_t self_zero = 1;
        for (std::int32_t i = 0; i < jo::self<jo::BoneTree>(other)->count; ++i)
            if (other->bone_id(i) != 0) other_zero = 0;
        for (std::int32_t i = 0; i < jo::self<jo::BoneTree>(this)->count; ++i)
            if (bone_id(i) != 0) self_zero = 0;
        if (other_zero != 0 || self_zero != 0) {
            for (std::int32_t i = 0; i < jo::self<jo::BoneTree>(other)->count; ++i) out[i] = i < jo::self<jo::BoneTree>(this)->count ? i : -999;
            return;
        }
        for (std::int32_t i = 0; i < jo::self<jo::BoneTree>(other)->count; ++i) {
            const std::uint32_t id = other->bone_id(i);
            out[i] = -999;
            if (id != 0) {
                const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_table(this)->find(static_cast<std::int32_t>(id)));  // 0x005ba160
                if (r != -999) out[i] = r;
            }
        }
    }

    // 0x005ba1d0
    std::int32_t bone_differs(H* other) {
        c_bone_differs.hit();
        if (jo::self<jo::BoneTree>(other)->count > jo::self<jo::BoneTree>(this)->count) return 1;
        std::int32_t other_zero = 1;
        std::int32_t same = 1;
        for (std::int32_t i = 0; i < jo::self<jo::BoneTree>(other)->count; ++i) {
            const std::uint32_t id = other->bone_id(i);
            if (id != 0) {
                other_zero = 0;
                if (bone_id(i) != id) same = 0;
            }
        }
        std::int32_t self_zero = 1;
        for (std::int32_t i = 0; i < jo::self<jo::BoneTree>(this)->count; ++i)
            if (bone_id(i) != 0) self_zero = 0;
        if (other_zero != 0 || same != 0 || self_zero != 0) return 0;
        return 1;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("BoneHierarchy::fill_mapping_table", 0x005ba300, &H::bone_map);
E3D("BoneHierarchy::need_mapping_table", 0x005ba1d0, &H::bone_differs);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - Obj::ctor_5ba4b0 0x005ba4b0 (ret 0): 0x5843d0(0x38, 0x1e, 0), memset(+0x1d, 0, 0xc).
namespace {
namespace game_b99_07 {

using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ 0x005ba4b0
    H* ctor_5ba4b0() {
        joan::orig::dynarray(this)->construct(0x38, 0x1e, 0);  // 0x005843d0
        // The object is really a NameTable (BoneNameTable's 12-byte `header` is its index_cap, sorted and sorted_valid:
        // 12 bytes on x86, 16 on x64 where `sorted` is 8 bytes).
        jo::NameTable* const nt = jo::self<jo::NameTable>(this);
        crt_memset(&nt->index_cap, 0, sizeof(nt->index_cap) + sizeof(nt->sorted) + sizeof(nt->sorted_valid));  // memset(+0x1d, 0, 0xc)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("BoneNameRes::BoneNameRes", 0x005ba4b0, &H::ctor_5ba4b0);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
//
//      0x005ba500 Obj5ba::~Obj5ba            (thiscall) - 0x005ba660, then base 0x005844b0 (SEH
//                 frame: normal path only).
namespace {
namespace game_b95_06 {

struct H {
    // ------------------------------------------------------------------------ 0x005ba500
    u32 dtor_5ba500() {
        joan::orig::as_bone_tree(this)->reset();                  // 0x005ba660
        joan::orig::as_dynarray(this)->destruct_5844b0();  // 0x005844b0
        return 0;  // only caller: the atexit stub 0x005673f0 (result discarded); ~DynArray returns nothing
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("BoneNameRes::~BoneNameRes", 0x005ba500, &H::dtor_5ba500);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
//
//  * BoneTree::build 0x005ba570 (reset 0x005ba660, bone array, index table sorted by qsort
//    with 0x005ba990, [0xa67698] = this).
namespace {
namespace game_b94_06 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_bone_build, "BoneTree::build");

struct H {
    // 0x005ba570 BoneTree::build(bones (0x38 each), count)
    void bone_build(std::uint8_t* bones, std::int32_t count) {
        c_bone_build.hit();
        joan::orig::as_bone_tree(this)->reset();  // 0x005ba660
        jo::self<jo::NameTable>(this)->index_cap = count;
        jo::view<void*>(jo::self<jo::NameTable>(this)->sorted) = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(static_cast<std::uint32_t>(jo::self<jo::NameTable>(this)->index_cap) << 2), str(d::s_d_ww_en3d_bonetree_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16eb8)) + 4));  // 0x0058e7b0
        for (std::int32_t i = 0; i < count; ++i) {
            joan::orig::dynarray(this)->linkin(static_cast<void*>(bones + i * 0x38));  // 0x00584620
            jo::view<std::int32_t*>(jo::self<jo::NameTable>(this)->sorted)[i] = i + 1;
        }
        jo::self<jo::NameTable>(this)->sorted_valid = 1;
        global<void*>(d::g_bone_name_registry) = this;
        joan::orig::libc::qsort(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::NameTable>(this)->sorted)), joan::orig::arg::u32(count), 4u, reinterpret_cast<void*>(fn::code_value(fn::cmp_names)));  // qsort, 0x005ba990
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("BoneNameRes::init", 0x005ba570, &H::bone_build);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
//
//      0x005ba660 BoneTree::reset      (thiscall, ret 0)
namespace {
namespace game_b92_04 {

using i16 = std::int16_t;

void* const kMem = obj(d::g_mem);

void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

struct H {
    // ------------------------------------------------------------------------ 0x005ba660
    void bonetree_reset() {
        if (jo::self<jo::NameTable>(this)->sorted != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::NameTable>(this)->sorted), str(d::s_d_ww_en3d_bonetree_cpp), static_cast<i32>(global<i16>(d::g_a16ebc)) + 3);  // bonetree.cpp
            jo::self<jo::NameTable>(this)->sorted = nullptr;
        }
        jo::view<u32>(jo::self<jo::NameTable>(this)->index_cap) = 0;
        joan::orig::dynarray(this)->zap(i32{1});  // 0x00584580
        jo::view<u32>(jo::self<jo::NameTable>(this)->sorted_valid) = 0;
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("BoneNameRes::deinit", 0x005ba660, &H::bonetree_reset);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (en3d/b82_17)
//
//      0x005ba6e0 NameTable::find(name) (ret 4)  - binary search (0x005911a0) of +0x21
//
//      0x005ba760 NameTable::add(name) (ret 4)   - new entry (0x00417520 / 0x00417200), name
//                 strncpy 0x32, index array +0x21 grown / sorted insert (0x005910f0)
namespace {
namespace en3d_b82_17 {

void* const kFiles = obj(d::g_misc);   // array helper object (0x005911a0 / 0x005910f0)
void* const kMemory = obj(d::g_mem);  // allocator (0x0058eb30 realloc / 0x0058ed70 free)

JOAN_COUNTER(c_name_add, "NameTable::add");
JOAN_COUNTER(c_name_find, "NameTable::find");

struct H {
    // 0x005ba6e0
    std::int32_t name_find(const char* name) {
        c_name_find.hit();
        global<void*>(d::g_bone_name_registry) = this;
        const std::int32_t count = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
        const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(kFiles)->bin_search_5911a0(joan::orig::arg::mptr(name), joan::orig::arg::str(jo::view<void*>(jo::self<jo::NameTable>(this)->sorted)), joan::orig::arg::i32(count), 4, fn::code_value(fn::cmp_key_name), joan::orig::arg::mptr(static_cast<void*>(nullptr))));  // 0x005911a0
        if (r == 0) return 0;
        return static_cast<std::int32_t>(jo::view<std::uint32_t*>(jo::self<jo::NameTable>(this)->sorted)[r - 1]);
    }

    // 0x005ba760
    std::int32_t name_add(const char* name) {
        c_name_add.hit();
        const std::int32_t count = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
        joan::orig::dynarray(this)->linkin(nullptr);                               // 0x00584620
        std::int32_t index = joan::orig::as_dynarray(this)->cursor();  // 0x00417520
        std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(index)));  // 0x00417200
        jo::as_name_entry(e)->index = index;
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(e + 4))), joan::orig::arg::str(name), 0x32u);  // strncpy
        jo::as_name_entry(e)->u8_36 = 0;
        if (joan::orig::as_dynarray(this)->size() > jo::self<jo::NameTable>(this)->index_cap) {  // 0x00417ad0
            jo::self<jo::NameTable>(this)->index_cap = joan::orig::as_dynarray(this)->size() + jo::self<jo::NameTable>(this)->block_num;
            jo::view<void*>(jo::self<jo::NameTable>(this)->sorted) = joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::NameTable>(this)->sorted)), joan::orig::arg::u32(jo::self<jo::NameTable>(this)->index_cap << 2), str(d::s_d_ww_en3d_bonetree_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16ec0)) + 0x11));  // 0x0058eb30
        }
        global<void*>(d::g_bone_name_registry) = this;
        std::int32_t pos;
        joan::orig::as_misc(kFiles)->bin_search_5911a0(joan::orig::arg::mptr(static_cast<void*>(&index)), joan::orig::arg::str(jo::view<void*>(jo::self<jo::NameTable>(this)->sorted)), joan::orig::arg::i32(count), 4, fn::code_value(fn::cmp_names), joan::orig::arg::mptr(static_cast<void*>(&pos)));  // 0x005911a0
        joan::orig::as_misc(kFiles)->insert_array_rec_5910f0(joan::orig::arg::str(jo::view<void*>(jo::self<jo::NameTable>(this)->sorted)), joan::orig::arg::i32(count), 4, joan::orig::arg::i32(pos), joan::orig::arg::mptr(static_cast<void*>(&index)));  // 0x005910f0
        return index;
    }
};

#define B82_17E(name, addr, fn) JOAN_REPLACE("en3d/b82_17", name, addr, fn)
B82_17E("BoneNameRes::search_name", 0x005ba6e0, &H::name_find);
B82_17E("BoneNameRes::add_name", 0x005ba760, &H::name_add);
#undef B82_17E

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (en3d/b82_17)
//
//      0x005b9790 BoneNode::init(v) (ret 4)      - +0 = v, +4 = 0, links = -999
namespace {
namespace en3d_b82_17_2 {


struct H {
    // 0x005b9790 (eax = this)
    void* node_init(std::uint32_t v) {
        jo::view<std::uint32_t>(jo::self<jo::BoneNode>(this)->index) = v;
        jo::view<std::uint32_t>(jo::self<jo::BoneNode>(this)->name_id) = 0;
        jo::self<jo::BoneNode>(this)->parent = -999;
        jo::self<jo::BoneNode>(this)->next_sibling = -999;
        jo::self<jo::BoneNode>(this)->prev_sibling = -999;
        jo::self<jo::BoneNode>(this)->first_child = -999;
        jo::self<jo::BoneNode>(this)->last_child = -999;
        return this;
    }
};

#define B82_17E(name, addr, fn) JOAN_REPLACE("en3d/b82_17", name, addr, fn)
B82_17E("BoneNode::init", 0x005b9790, &H::node_init);
#undef B82_17E

}  // namespace en3d_b82_17_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers32.cpp (en3d/helpers32)
//
//      0x005b9800 BoneTree::BoneTree()           - zeroed 12 bytes
namespace {
namespace en3d_helpers32_2 {

struct H {
    // 0x005b9800 BoneTree::BoneTree().
    void* tree_ctor() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::BoneTree));  // memset 0xc (x86)
        return this;
    }
};

#define HELPER32(name, addr, fn) JOAN_REPLACE("en3d/helpers32", name, addr, fn)
HELPER32("BoneHierarchy::BoneHierarchy_5b9800", 0x005b9800, &H::tree_ctor);
#undef HELPER32

}  // namespace en3d_helpers32_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers37.cpp (en3d/helpers37)
//
//      0x005ba8b0 NameTable::id(name)            - 0x005ba6e0 lookup, else 0x005ba760 add; 0 for null
namespace {
namespace en3d_helpers37_2 {

using joan::call::thiscall;

struct H {
    // 0x005ba8b0 NameTable::id(name).
    std::int32_t name_id(const char* name) {
        if (name == nullptr) return 0;
        std::int32_t id = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(this)->find_5ba6e0(joan::orig::arg::str(name)));  // 0x005ba6e0 find
        if (id == 0) id = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(this)->add(joan::orig::arg::str(name)));  // 0x005ba760 add
        return id;
    }
};

#define HELPER37(name, addr, fn) JOAN_REPLACE("en3d/helpers37", name, addr, fn)
HELPER37("BoneNameRes::get_name_id", 0x005ba8b0, &H::name_id);
#undef HELPER37

}  // namespace en3d_helpers37_2
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
//
//      0x005b0620 ActorDef::load_bones(dir, name, rec) (ret 0xc) - <dir><name>.AM3 (0x005c5400) or
//                 the record's bone table (0x005c5610), 0x005c56d0, bone map +0x14 (0x005ba1d0 /
//                 0x0058e7b0 / 0x005ba300); 0 / 1
namespace {
namespace en3d_b84_01_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::game_new;  // operator new

struct H {
    // 0x005c5610
    std::int32_t load_bones_table(const std::uint32_t* table) {
        if (*table != static_cast<std::uint32_t>(jo::self<jo::Model>(this)->mesh->bone_count)) return 0;
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::BoneTree)));  // 0xc on x86
        void* const tree = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_bone_tree(mem)->BoneTree_copy(joan::orig::arg::mptr(table))) : nullptr;  // 0x005b9830
        jo::view<void*>(jo::self<jo::Model>(this)->bones) = tree;
        return 1;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("GeometryDef::share_hierarchy", 0x005c5610, &H::load_bones_table);
#undef E3D

}  // namespace en3d_b84_01_2
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

using i32 = std::int32_t;

struct H {
    // 0x005ba910
    const char* names_get(std::uint32_t key) {
        if (key == 0) return img::str(d::kEmpty);
        return reinterpret_cast<const char*>(joan::orig::arg::as<std::uintptr_t>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(key))) + 4);  // 0x00417200 (null entry -> 4)
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("Names::get", 0x005ba910, &H::names_get);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
namespace {
namespace game_b103_11_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::orig::helpers::stricmp_;  // _stricmp

// 0x005ba990: qsort comparator over name ids.
i32 JOAN_CDECL cmp_names(const u32* a, const u32* b) {
    const char* nb = joan::orig::arg::as<const char*>(joan::orig::as_names(global<void*>(d::g_bone_name_registry))->get(joan::orig::arg::u32(*b)));  // 0x005ba910
    const char* na = joan::orig::arg::as<const char*>(joan::orig::as_names(global<void*>(d::g_bone_name_registry))->get(joan::orig::arg::u32(*a)));  // 0x005ba910
    return stricmp_(na, nb);
}

// 0x005ba9e0: search comparator, key string vs name id.
i32 JOAN_CDECL cmp_key_name(const char* key, const u32* b) {
    const char* nb = joan::orig::arg::as<const char*>(joan::orig::as_names(global<void*>(d::g_bone_name_registry))->get(joan::orig::arg::u32(*b)));  // 0x005ba910
    return stricmp_(key, nb);
}

struct H {
    // 0x005ba960
    std::uintptr_t call_417200(u32 a) { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(a))); }  // 0x00417200
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Obj::call_417200", 0x005ba960, &H::call_417200);
B103("cmp_names", 0x005ba990, &cmp_names);
B103("cmp_key_name", 0x005ba9e0, &cmp_key_name);
#undef B103

}  // namespace game_b103_11_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::BoneNode::init(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17_2::H::node_init>(this, a0)); }  // 0x5b9790
auto joan::orig::BoneTree::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers32_2::H::tree_ctor>(this)); }  // 0x5b9800
auto joan::orig::BoneTree::BoneTree_copy(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b85_04::H::bone_copy>(this, a0)); }  // 0x5b9830
auto joan::orig::BoneTree::dtor_wrapper() -> void { joan::bind::member<&::en3d_b86_04::H::bonetree_dtor>(this); }  // 0x5b9960
auto joan::orig::BoneTree::allocate(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers37::H::tree_allocate>(this, a0)); }  // 0x5b9990
auto joan::orig::BoneTree::free() -> void { joan::bind::member<&::en3d_b87_02::H::bonetree_free>(this); }  // 0x5b9a80
auto joan::orig::BoneTree::read(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers32::H::tree_read>(this, a0)); }  // 0x5b9af0
auto joan::orig::BoneTree::set_parent(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::en3d_helpers37::H::tree_set_parent>(this, a0, a1); }  // 0x5b9df0
auto joan::orig::BoneTree::parent(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers32::H::tree_parent>(this, a0)); }  // 0x5b9fe0
auto joan::orig::BoneTree::first_child(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers37::H::tree_first_child>(this, a0)); }  // 0x5ba040
auto joan::orig::BoneTree::next_child(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers37::H::tree_next_child>(this, a0, a1)); }  // 0x5ba0a0
auto joan::orig::BoneTable::index_of(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b85_04::H::bone_index_of>(this, a0)); }  // 0x5ba110
auto joan::orig::BoneTable::find(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b83_08::H::bone_find>(this, a0)); }  // 0x5ba160
auto joan::orig::BoneTable::differs(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b84_01::H::bone_differs>(this, a0)); }  // 0x5ba1d0
auto joan::orig::BoneTable::map(void* a0, void* a1) -> void { joan::bind::member<&::en3d_b84_01::H::bone_map>(this, a0, a1); }  // 0x5ba300
auto joan::orig::BoneNameTable::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_07::H::ctor_5ba4b0>(this)); }  // 0x5ba4b0
auto joan::orig::BoneTree::build(void* a0, std::int32_t a1) -> void { joan::bind::member<&::game_b94_06::H::bone_build>(this, a0, a1); }  // 0x5ba570
auto joan::orig::BoneTree::reset() -> void { joan::bind::member<&::game_b92_04::H::bonetree_reset>(this); }  // 0x5ba660
auto joan::orig::NameTable::find_5ba6e0(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b82_17::H::name_find>(this, a0)); }  // 0x5ba6e0
auto joan::orig::NameTable::add(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b82_17::H::name_add>(this, a0)); }  // 0x5ba760
auto joan::orig::NameTable::id(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers37_2::H::name_id>(this, a0)); }  // 0x5ba8b0
auto joan::orig::Names::get(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b91_03::H::names_get>(this, a0)); }  // 0x5ba910
auto joan::orig::Model::load_bones_table(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b84_01_2::H::load_bones_table>(this, a0)); }  // 0x5c5610
