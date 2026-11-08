// en3d/morph.cpp: functions of the original d:\ww\en3d\morph.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../lib/crt/crt.hpp"
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

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::import_call;
using joan::img::obj;
using joan::img::str;

void* const kFiles = obj(d::g_misc);   // file system helper (0x00590b80 exists)
void* const kLog = obj(d::g_err);

using joan::orig::helpers::game_new;  // operator new
using joan::orig::helpers::game_delete;                      // operator delete
using joan::orig::helpers::memset_;

struct alignas(4) Path {
    std::uint8_t bytes[0x194];
};

const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90

const char* const kMorphCpp = str(d::s_d_ww_en3d_morph_cpp);  // "d:\ww\en3d\morph.cpp"

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
//
//      0x005c6820 Morph::load(name, dir, file, matrix) (ret 0x10) - <dir><file>.MDA: per source
//                 file vertex indices / positions / normals (+0x40 / +0x44 / +0x68 / +0x6c), then
//                 +0x34 targets of 0x44 bytes (name, 4 arrays), optional point / vector transform
namespace {
namespace en3d_b83_03 {


void* const kMemory = obj(d::g_mem);  // allocator (0x0058e7b0 / 0x0058eb30 / 0x0058ed70)

using joan::orig::helpers::memcpy_;

struct alignas(4) File {
    std::uint8_t bytes[0x210];  // 0x110 used (0x004179c0); room as in the larger frame
};

std::int32_t read_int(File& file) { return joan::orig::as_file(&file)->file_get_long(); }            // 0x00587e20
void read_bytes(File& file, void* p, std::int32_t n) { joan::orig::as_file(&file)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n)); }  // 0x005873a0

std::int32_t morph_line(std::int32_t add) { return static_cast<std::int32_t>(global<std::int16_t>(d::g_a16ec4)) + add; }
void* mem_realloc(void* p, std::int32_t size, std::int32_t line) {
    return joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(kMorphCpp), joan::orig::arg::i32(morph_line(line)));  // 0x0058eb30
}
void* mem_alloc(std::int32_t size, std::int32_t line) {
    return joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(size), joan::orig::arg::str(kMorphCpp), joan::orig::arg::i32(morph_line(line)));  // 0x0058e7b0
}
void mem_free(void* p, const char* file, std::int32_t line) { joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

JOAN_COUNTER(c_morph_load, "Morph::load");

struct H {
    // 0x005c6820
    void morph_load(const char* name, const char* dir, const char* file_name, void* matrix) {
        c_morph_load.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Morph>(this)->loaded) != 0) return;
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(this) + 1)), joan::orig::arg::str(name));  // strcpy
        Path path;
        joan::orig::as_string(&path)->construct(joan::orig::arg::str(dir));  // 0x005a11a0
        joan::orig::as_string(&path)->append(joan::orig::arg::str(file_name));  // 0x005a1550 append
        joan::orig::as_string(&path)->append(str(d::s_MDA));                // ".MDA"
        if (joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(path))) == 0) {  // 0x00590b80
            joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_Error_loading_Morph_Data_1s), c_str(path));  // 0x00586cb0
            return;
        }
        File file;
        joan::orig::as_file(&file)->construct();                             // 0x004179c0
        joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(path)), 1, static_cast<char>(0));  // 0x00586da0 open
        const std::int32_t sources = read_int(file);
        std::int32_t* const base = static_cast<std::int32_t*>(game_new(static_cast<std::uint32_t>(sources << 2)));
        for (std::int32_t i = 0; i < sources; ++i) {
            const std::int32_t advance = read_int(file);
            const std::int32_t n = read_int(file);
            base[i] = jo::self<jo::Morph>(this)->vertex_base;
            if (n != 0) {
                jo::view<void*>(jo::self<jo::Morph>(this)->vertex_indices) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->vertex_indices), (jo::self<jo::Morph>(this)->vertex_count + n) << 2, 0x23);
                jo::view<void*>(jo::self<jo::Morph>(this)->base_positions) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->base_positions), (jo::self<jo::Morph>(this)->vertex_count + n) * 0xc, 0x24);
                jo::view<void*>(jo::self<jo::Morph>(this)->base_normals) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->base_normals), (jo::self<jo::Morph>(this)->vertex_count + n) * 0xc, 0x25);
                for (std::int32_t k = 0; k < n; ++k) {
                    const std::int32_t index = read_int(file);
                    jo::view<std::int32_t*>(jo::self<jo::Morph>(this)->vertex_indices)[jo::self<jo::Morph>(this)->vertex_count] = index + base[i];
                    read_bytes(file, jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_positions) + jo::self<jo::Morph>(this)->vertex_count * 0xc, 0xc);
                    read_bytes(file, jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_normals) + jo::self<jo::Morph>(this)->vertex_count * 0xc, 0xc);
                    if (matrix != nullptr) {
                        import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_positions) + jo::self<jo::Morph>(this)->vertex_count * 0xc),
                                           matrix);  // point_transform
                        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_normals) + jo::self<jo::Morph>(this)->vertex_count * 0xc),
                                           matrix);  // vector_transform
                    }
                    jo::self<jo::Morph>(this)->vertex_count = jo::self<jo::Morph>(this)->vertex_count + 1;
                }
            }
            jo::self<jo::Morph>(this)->vertex_base = jo::self<jo::Morph>(this)->vertex_base + advance;
        }
        jo::self<jo::Morph>(this)->target_count = read_int(file);
        if (jo::self<jo::Morph>(this)->target_count == 0) {
            game_delete(base);
            mem_free(jo::view<void*>(jo::self<jo::Morph>(this)->base_positions), kMorphCpp, morph_line(0x3e));
            mem_free(jo::view<void*>(jo::self<jo::Morph>(this)->base_normals), kMorphCpp, morph_line(0x3f));
        }
        jo::view<void*>(jo::self<jo::Morph>(this)->targets) = game_new(static_cast<std::uint32_t>(jo::self<jo::Morph>(this)->target_count * static_cast<std::int32_t>(sizeof(jo::MorphTarget))));
        for (std::int32_t t = 0; t < jo::self<jo::Morph>(this)->target_count; ++t) {
            joan::orig::as_file(&file)->file_read_array(static_cast<void*>(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->targets) + t * static_cast<std::int32_t>(sizeof(jo::MorphTarget))), 0x32u, 1u);                         // 0x00587650
            auto target = [&]() { return jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->targets) + t * static_cast<std::int32_t>(sizeof(jo::MorphTarget)); };
            jo::view<void*>(jo::as_morph_target(target())->positions) = mem_alloc(jo::self<jo::Morph>(this)->vertex_count * 0xc, 0x49);
            memcpy_(jo::view<void*>(jo::as_morph_target(target())->positions), jo::view<void*>(jo::self<jo::Morph>(this)->base_positions), static_cast<std::uint32_t>(jo::self<jo::Morph>(this)->vertex_count * 0xc));
            jo::view<void*>(jo::as_morph_target(target())->normals) = mem_alloc(jo::self<jo::Morph>(this)->vertex_count * 0xc, 0x4d);
            memcpy_(jo::view<void*>(jo::as_morph_target(target())->normals), jo::view<void*>(jo::self<jo::Morph>(this)->base_normals), static_cast<std::uint32_t>(jo::self<jo::Morph>(this)->vertex_count * 0xc));
            jo::view<void*>(jo::as_morph_target(target())->position_deltas) = mem_alloc(jo::self<jo::Morph>(this)->vertex_count * 0xc, 0x51);
            memset_(jo::view<void*>(jo::as_morph_target(target())->position_deltas), 0, static_cast<std::uint32_t>(jo::self<jo::Morph>(this)->vertex_count * 0xc));
            jo::view<void*>(jo::as_morph_target(target())->normal_deltas) = mem_alloc(jo::self<jo::Morph>(this)->vertex_count * 0xc, 0x55);
            memset_(jo::view<void*>(jo::as_morph_target(target())->normal_deltas), 0, static_cast<std::uint32_t>(jo::self<jo::Morph>(this)->vertex_count * 0xc));
            for (std::int32_t i = 0; i < sources; ++i) {
                const std::int32_t count = read_int(file);
                std::int32_t j = 0;
                for (std::int32_t v = 0; v < count; ++v) {
                    const std::int32_t index = read_int(file) + base[i];
                    // Original order: element compare first, then the bound (reads [+0x40]).
                    while (jo::view<std::int32_t*>(jo::self<jo::Morph>(this)->vertex_indices)[j] != index && j < jo::self<jo::Morph>(this)->vertex_count) ++j;
                    read_bytes(file, jo::view<std::uint8_t*>(jo::as_morph_target(target())->positions) + j * 0xc, 0xc);
                    read_bytes(file, jo::view<std::uint8_t*>(jo::as_morph_target(target())->normals) + j * 0xc, 0xc);
                    read_bytes(file, jo::view<std::uint8_t*>(jo::as_morph_target(target())->position_deltas) + j * 0xc, 0xc);
                    read_bytes(file, jo::view<std::uint8_t*>(jo::as_morph_target(target())->normal_deltas) + j * 0xc, 0xc);
                    if (matrix != nullptr) {
                        import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::as_morph_target(target())->positions) + j * 0xc), matrix);
                        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::as_morph_target(target())->normals) + j * 0xc), matrix);
                        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::as_morph_target(target())->position_deltas) + j * 0xc), matrix);
                        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::as_morph_target(target())->normal_deltas) + j * 0xc), matrix);
                    }
                }
            }
        }
        joan::orig::as_file(&file)->file_close();  // 0x00587260 close
        game_delete(base);
        jo::view<void*>(jo::self<jo::Morph>(this)->positions) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->positions), jo::self<jo::Morph>(this)->vertex_count * 0xc, 0x82);
        jo::view<void*>(jo::self<jo::Morph>(this)->normals) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->normals), jo::self<jo::Morph>(this)->vertex_count * 0xc, 0x83);
        jo::view<void*>(jo::self<jo::Morph>(this)->position_step) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->position_step), jo::self<jo::Morph>(this)->vertex_count * 0xc, 0x84);
        jo::view<void*>(jo::self<jo::Morph>(this)->normal_step) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->normal_step), jo::self<jo::Morph>(this)->vertex_count * 0xc, 0x85);
        jo::self<jo::Morph>(this)->loaded = 1;
        joan::orig::as_morph(this)->select(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Morph>(this)->targets)));  // 0x005c7ff0
        joan::orig::as_file(&file)->destruct();         // 0x005872b0 ~File
    }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("MorphControl::init_5c6820", 0x005c6820, &H::morph_load);
#undef B83_03

}  // namespace en3d_b83_03
}  // namespace

// ---------------------------------------- from modules/game_b106_07.cpp (game/b106_07)
//
//      0x005c7370 Morph::copy_from(src, matrix) (thiscall, ret 8) - en3d/morph.cpp: deep copy
//                 of vertex / normal arrays (transformed by matrix when non-null) and the
//                 0x44-byte target records; then 0x10-byte sub-object (0x005cae20 / 0x005cb1d0)
//                 or 0x005c7ff0(+0x38).
//
//  - 0x005c7370 / 0x00640db0 SEH frames only guard operator new + ctor (delete on throw);
//    normal path kept. No caller of 0x005c7370 reads eax (returns void here).
namespace {
namespace game_b106_07 {


void strcpy_(void* d, const void* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }  // strcpy
void memcpy_(void* d, const void* s, u32 n) { std::memcpy(joan::orig::arg::mptr(d), joan::orig::arg::ptr(s), joan::orig::arg::u32(n)); }  // memcpy

JOAN_COUNTER(c_morph_copy, "game/b106_07 Morph::copy_from");

// 0x005c7370 helpers
constexpr img::Addr kHeap = d::g_mem;
const char* morph_file() { return str(d::s_d_ww_en3d_morph_cpp); }  // 'd:\\ww\\en3d\\morph.cpp'
i32 line_base() { return global<i16>(d::g_line_base_a16ec8); }
jo::Vec3f* alloc_vec(i32 n, i32 line) {
    return joan::orig::arg::as<jo::Vec3f*>(joan::orig::as_mem(obj(kHeap))->add(joan::orig::arg::u32(n * 0xc), joan::orig::arg::str(morph_file()), joan::orig::arg::i32(line)));  // 0x0058e7b0
}

struct H {
    // 0x005c7370
    void morph_copy_from(u8* src, void* m) {
        u8* const self = reinterpret_cast<u8*>(this);
        if (static_cast<std::int8_t>(self[0]) != 0) return;
        c_morph_copy.hit();
        strcpy_(self + 1, src + 1);
        jo::view<u32>(jo::as_morph(self)->vertex_base) = jo::view<u32>(jo::as_morph(src)->vertex_base);
        jo::view<u32>(jo::as_morph(self)->vertex_count) = jo::view<u32>(jo::as_morph(src)->vertex_count);
        if (jo::view<u32>(jo::as_morph(self)->vertex_count) != 0) {
            {
                const i32 line = line_base() + 0xd;
                jo::as_morph(self)->vertex_indices = joan::orig::arg::as<jo::ArrI32*>(joan::orig::as_mem(obj(kHeap))->resize(joan::orig::arg::mptr(jo::as_morph(self)->vertex_indices), joan::orig::arg::u32(jo::as_morph(self)->vertex_count << 2), joan::orig::arg::str(morph_file()), joan::orig::arg::i32(line)));  // 0x0058eb30
            }
            memcpy_(jo::as_morph(self)->vertex_indices, jo::as_morph(src)->vertex_indices, jo::view<u32>(jo::as_morph(self)->vertex_count) << 2);
            jo::as_morph(self)->base_positions = alloc_vec(jo::as_morph(self)->vertex_count, line_base() + 0x10);
            jo::as_morph(self)->base_normals = alloc_vec(jo::as_morph(self)->vertex_count, line_base() + 0x11);
            jo::as_morph(self)->positions = alloc_vec(jo::as_morph(self)->vertex_count, line_base() + 0x12);
            jo::as_morph(self)->normals = alloc_vec(jo::as_morph(self)->vertex_count, line_base() + 0x13);
            jo::as_morph(self)->position_step = alloc_vec(jo::as_morph(self)->vertex_count, line_base() + 0x14);
            jo::as_morph(self)->normal_step = alloc_vec(jo::as_morph(self)->vertex_count, line_base() + 0x15);
            if (m != nullptr) {
                for (i32 i = 0; i < jo::as_morph(self)->vertex_count; ++i) {
                    import_call<void*>(d::iat_v3d_87_point_transform_3, jo::as_morph(self)->base_positions + i,
                                       jo::as_morph(src)->base_positions + i, m);  // point_transform
                    import_call<void*>(d::iat_v3d_87_vector_transform_3, jo::as_morph(self)->base_normals + i,
                                       jo::as_morph(src)->base_normals + i, m);  // vector_transform
                }
            } else {
                memcpy_(jo::as_morph(self)->base_positions, jo::as_morph(src)->base_positions, jo::as_morph(self)->vertex_count * 0xc);
                memcpy_(jo::as_morph(self)->base_normals, jo::as_morph(src)->base_normals, jo::as_morph(self)->vertex_count * 0xc);
            }
        }
        jo::view<u32>(jo::as_morph(self)->target_count) = jo::view<u32>(jo::as_morph(src)->target_count);
        jo::as_morph(self)->targets = joan::orig::arg::as<jo::MorphTarget*>(joan::crt::operator_new(joan::orig::arg::u32(jo::as_morph(self)->target_count * static_cast<std::int32_t>(sizeof(jo::MorphTarget)))));  // operator new
        for (i32 j = 0; j < jo::as_morph(self)->target_count; ++j) {
            strcpy_(jo::as_morph(self)->targets[j].name, jo::as_morph(src)->targets[j].name);
            {
                jo::Vec3f* const p = alloc_vec(jo::as_morph(self)->vertex_count, line_base() + 0x2d);
                jo::as_morph(self)->targets[j].positions = p;
            }
            {
                jo::Vec3f* const p = alloc_vec(jo::as_morph(self)->vertex_count, line_base() + 0x2e);
                jo::as_morph(self)->targets[j].normals = p;
            }
            {
                jo::Vec3f* const p = alloc_vec(jo::as_morph(self)->vertex_count, line_base() + 0x2f);
                jo::as_morph(self)->targets[j].position_deltas = p;
            }
            {
                jo::Vec3f* const p = alloc_vec(jo::as_morph(self)->vertex_count, line_base() + 0x30);
                jo::as_morph(self)->targets[j].normal_deltas = p;
            }
            // field `f` of target record j of `o`, element k
            auto el = [j](const u8* o, jo::Vec3f* jo::MorphTarget::*f, i32 k) {
                return jo::as_morph(o)->targets[j].*f + k;
            };
            if (m != nullptr) {
                for (i32 k = 0; k < jo::as_morph(self)->vertex_count; ++k) {
                    import_call<void*>(d::iat_v3d_87_point_transform_3, el(self, &jo::MorphTarget::positions, k), el(src, &jo::MorphTarget::positions, k), m);  // point_transform
                    import_call<void*>(d::iat_v3d_87_vector_transform_3, el(self, &jo::MorphTarget::normals, k), el(src, &jo::MorphTarget::normals, k), m);  // vector_transform
                    import_call<void*>(d::iat_v3d_87_vector_transform_3, el(self, &jo::MorphTarget::position_deltas, k), el(src, &jo::MorphTarget::position_deltas, k), m);
                    import_call<void*>(d::iat_v3d_87_vector_transform_3, el(self, &jo::MorphTarget::normal_deltas, k), el(src, &jo::MorphTarget::normal_deltas, k), m);
                }
            } else {
                memcpy_(el(self, &jo::MorphTarget::positions, 0), el(src, &jo::MorphTarget::positions, 0), jo::as_morph(self)->vertex_count * 0xc);
                memcpy_(el(self, &jo::MorphTarget::normals, 0), el(src, &jo::MorphTarget::normals, 0), jo::as_morph(self)->vertex_count * 0xc);
                memcpy_(el(self, &jo::MorphTarget::position_deltas, 0), el(src, &jo::MorphTarget::position_deltas, 0), jo::as_morph(self)->vertex_count * 0xc);
                memcpy_(el(self, &jo::MorphTarget::normal_deltas, 0), el(src, &jo::MorphTarget::normal_deltas, 0), jo::as_morph(self)->vertex_count * 0xc);
            }
        }
        self[0] = 1;
        if (jo::as_morph(src)->anim != nullptr) {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::MorphAnim)));  // operator new (0x10 on x86)
            jo::MorphAnim* const o = mem != nullptr ? joan::orig::arg::as<jo::MorphAnim*>(joan::orig::as_morph_anim(mem)->construct()) : nullptr;  // 0x005cae20
            jo::as_morph(self)->anim = o;
            joan::orig::as_morph_anim(jo::as_morph(self)->anim)->copy_from(joan::orig::arg::mptr(jo::as_morph(src)->anim), joan::orig::arg::mptr(self));  // 0x005cb1d0
            jo::as_morph(self)->anim_start = global<u32>(d::g_now);
            jo::as_morph(self)->anim_playing = 1;
        } else {
            joan::orig::as_morph(self)->select(joan::orig::arg::str(jo::as_morph(self)->targets));  // 0x005c7ff0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_07", name, addr, fn)
B("MorphControl::init_5c7370", 0x005c7370, &H::morph_copy_from);
#undef B

}  // namespace game_b106_07
}  // namespace

// ---------------------------------------- from modules/en3d_helpers37.cpp (en3d/helpers37)
//
//      0x005c7b80 Morph::release()               - frees the morph buffers and per-target arrays
namespace {
namespace en3d_helpers37 {


JOAN_COUNTER(c_morph_release, "Morph::release");

constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kMorphFile(d::s_d_ww_en3d_morph_cpp);     // "d:\ww\en3d\morph.cpp"

void mem_free(void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

struct H {

    // 0x005c7b80 Morph::release().
    void* morph_release() {
        c_morph_release.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Morph>(this)->loaded) == 0) return this;
        jo::self<jo::Morph>(this)->loaded = 0;
        auto release = [&](void*& field, std::int32_t line) {
            if (field != nullptr) {
                mem_free(field, img::str(kMorphFile), global<std::int16_t>(d::g_a16ecc) + line);
                field = nullptr;
            }
        };
        release(jo::view<void*>(jo::self<jo::Morph>(this)->positions), 0x8);
        release(jo::view<void*>(jo::self<jo::Morph>(this)->normals), 0xe);
        release(jo::view<void*>(jo::self<jo::Morph>(this)->position_step), 0x14);
        release(jo::view<void*>(jo::self<jo::Morph>(this)->normal_step), 0x1a);
        release(jo::view<void*>(jo::self<jo::Morph>(this)->vertex_indices), 0x20);
        release(jo::view<void*>(jo::self<jo::Morph>(this)->base_positions), 0x26);
        release(jo::view<void*>(jo::self<jo::Morph>(this)->base_normals), 0x2c);
        if (jo::view<void*>(jo::self<jo::Morph>(this)->targets) != nullptr) {
            for (std::int32_t i = 0; i < jo::self<jo::Morph>(this)->target_count; ++i) {
                if (jo::view<void*>(jo::self<jo::Morph>(this)->targets[i].positions) != nullptr)
                    mem_free(jo::view<void*>(jo::self<jo::Morph>(this)->targets[i].positions), img::str(kMorphFile),
                             global<std::int16_t>(d::g_a16ecc) + 0x36);
                if (jo::view<void*>(jo::self<jo::Morph>(this)->targets[i].normals) != nullptr)
                    mem_free(jo::view<void*>(jo::self<jo::Morph>(this)->targets[i].normals), img::str(kMorphFile),
                             global<std::int16_t>(d::g_a16ecc) + 0x3a);
                if (jo::view<void*>(jo::self<jo::Morph>(this)->targets[i].position_deltas) != nullptr)
                    mem_free(jo::view<void*>(jo::self<jo::Morph>(this)->targets[i].position_deltas), img::str(kMorphFile),
                             global<std::int16_t>(d::g_a16ecc) + 0x3e);
                if (jo::view<void*>(jo::self<jo::Morph>(this)->targets[i].normal_deltas) != nullptr)
                    mem_free(jo::view<void*>(jo::self<jo::Morph>(this)->targets[i].normal_deltas), img::str(kMorphFile),
                             global<std::int16_t>(d::g_a16ecc) + 0x42);
            }
            joan::crt::operator_delete(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Morph>(this)->targets)));  // operator delete
            jo::view<void*>(jo::self<jo::Morph>(this)->targets) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Morph>(this)->anim) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Morph>(this)->anim);
            if (p != nullptr) joan::orig::as_morph(p)->scalar_deleting_destructor(1u);  // 0x005c7fa0 scalar deleting dtor
            jo::view<void*>(jo::self<jo::Morph>(this)->anim) = nullptr;
        }
        jo::self<jo::Morph>(this)->anim_playing = 0;
        jo::view<std::uint32_t>(jo::self<jo::Morph>(this)->vertex_base) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Morph>(this)->vertex_count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Morph>(this)->target_count) = 0;
        return this;  // eax = this on both paths
    }
};

#define HELPER37(name, addr, fn) JOAN_REPLACE("en3d/helpers37", name, addr, fn)
HELPER37("MorphControl::deinit", 0x005c7b80, &H::morph_release);
#undef HELPER37

}  // namespace en3d_helpers37
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (en3d/b82_17)
//
//      0x005c7fa0 Morph scalar deleting destructor (0x005cae50)
namespace {
namespace en3d_b82_17 {

struct H {
    // 0x005c7fa0
    void* morph_delete(std::uint32_t flags) {
        joan::orig::as_morph_anim(this)->destruct();  // 0x005cae50
        if ((flags & 1u) != 0) game_delete(this);
        return this;
    }
};

#define B82_17E(name, addr, fn) JOAN_REPLACE("en3d/b82_17", name, addr, fn)
B82_17E("Morph::scalar_deleting_destructor", 0x005c7fa0, &H::morph_delete);
#undef B82_17E

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
//
//      0x005c7ff0 Morph::select(name) (ret 4) - target 0x005c90a0, copies its +0x34 / +0x3c arrays
//
//      0x005c90a0 Morph::find(name) (ret 4)  - 1-based _stricmp search of the 0x44 entries
namespace {
namespace en3d_b84_01 {


void game_memcpy(void* d, const void* s, std::uint32_t n) { std::memcpy(joan::orig::arg::mptr(d), joan::orig::arg::ptr(s), joan::orig::arg::u32(n)); }

JOAN_COUNTER(c_morph_select, "Morph::select");

struct H {
    // 0x005c90a0
    std::int32_t morph_find(const char* name) {
        for (std::int32_t i = 1; i <= jo::self<jo::Morph>(this)->target_count; ++i) {
            if (joan::crt::stricmp(joan::orig::arg::str(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->targets) + (i - 1) * static_cast<std::int32_t>(sizeof(jo::MorphTarget))), joan::orig::arg::str(name)) == 0) return i;  // _stricmp
        }
        return 0;
    }

    // 0x005c7ff0
    void morph_select(const char* name) {
        c_morph_select.hit();
        const std::int32_t index = joan::orig::arg::as<std::int32_t>(joan::orig::as_morph(this)->find(joan::orig::arg::str(name)));  // 0x005c90a0
        if (jo::self<jo::Morph>(this)->current_target != index) {
            jo::self<jo::Morph>(this)->current_target = index;
            jo::MorphTarget* const entry = jo::self<jo::Morph>(this)->targets + (index - 1);
            game_memcpy(jo::view<void*>(jo::self<jo::Morph>(this)->positions), jo::view<void*>(jo::as_morph_target(entry)->positions), jo::view<std::uint32_t>(jo::self<jo::Morph>(this)->vertex_count) * 0xcu);
            game_memcpy(jo::view<void*>(jo::self<jo::Morph>(this)->normals), jo::view<void*>(jo::as_morph_target(entry)->normals), jo::view<std::uint32_t>(jo::self<jo::Morph>(this)->vertex_count) * 0xcu);
        }
        jo::self<jo::Morph>(this)->blend_start = global<std::uint32_t>(d::g_now);
        jo::self<jo::Morph>(this)->blend_end = global<std::uint32_t>(d::g_now);
        jo::view<std::uint32_t>(jo::self<jo::Morph>(this)->blend_duration) = 0;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("MorphControl::set_morph_state", 0x005c7ff0, &H::morph_select);
E3D("MorphControl::get_morph_target_id", 0x005c90a0, &H::morph_find);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
//
//    - Morph::set_frame 0x005c80d0 (frame, k), ret 8: k == 0 -> 0x5c7ff0(frame); else target =
//      0x5c90a0(frame), early outs, per-vertex deltas (+0x60 / +0x64) from the target key's
//      +0x34 array minus +0x58 / +0x5c, times +0x48 / +0x4c / +0x50 from [0xa500ba].
//
//  - Return values: callers of 0x5acfa0, 0x5b8a50, 0x5c80d0, 0x5e7ce0, 0x601140, 0x616dd0,
//    0x623bd0 ignore eax (checked); 0x5d5fe0's callers use al only.
namespace {
namespace game_b100_11 {


JOAN_COUNTER(c_morph_frame, "Morph::set_frame");

struct H {
    // ------------------------------------------------------------------ 0x005c80d0
    void morph_set_frame(u32 frame, i32 k) {
        c_morph_frame.hit();
        if (k == 0) {
            joan::orig::as_morph(this)->select(joan::orig::arg::str(frame));  // 0x005c7ff0
            return;
        }
        const i32 target = joan::orig::arg::as<i32>(joan::orig::as_morph(this)->find(joan::orig::arg::str(frame)));  // 0x005c90a0
        if (jo::self<jo::Morph>(this)->current_target == target) return;
        if (jo::self<jo::Morph>(this)->current_target == 0 && jo::view<i32>(jo::self<jo::Morph>(this)->blend_start) == global<i32>(d::g_now) && jo::self<jo::Morph>(this)->blend_duration == k &&
            jo::self<jo::Morph>(this)->next_target == target)
            return;
        jo::self<jo::Morph>(this)->current_target = 0;
        jo::self<jo::Morph>(this)->next_target = target;
        u8* const a = jo::view<u8*>(jo::self<jo::Morph>(this)->targets[target - 1].positions);
        u8* const b = jo::view<u8*>(jo::self<jo::Morph>(this)->targets[target - 1].positions);
        for (i32 i = 0; i < jo::self<jo::Morph>(this)->vertex_count; ++i) {
            jo::self<jo::Morph>(this)->position_step[i].x = jo::as_vec3f(a)[i].x - jo::self<jo::Morph>(this)->positions[i].x;
            jo::self<jo::Morph>(this)->position_step[i].y = jo::as_vec3f(a)[i].y - jo::self<jo::Morph>(this)->positions[i].y;
            jo::self<jo::Morph>(this)->position_step[i].z = jo::as_vec3f(a)[i].z - jo::self<jo::Morph>(this)->positions[i].z;
            jo::self<jo::Morph>(this)->normal_step[i].x = jo::as_vec3f(b)[i].x - jo::self<jo::Morph>(this)->normals[i].x;
            jo::self<jo::Morph>(this)->normal_step[i].y = jo::as_vec3f(b)[i].y - jo::self<jo::Morph>(this)->normals[i].y;
            jo::self<jo::Morph>(this)->normal_step[i].z = jo::as_vec3f(b)[i].z - jo::self<jo::Morph>(this)->normals[i].z;
        }
        jo::view<i32>(jo::self<jo::Morph>(this)->blend_start) = global<i32>(d::g_now);
        jo::view<i32>(jo::self<jo::Morph>(this)->blend_end) = global<i32>(d::g_now) + k;
        jo::self<jo::Morph>(this)->blend_duration = k;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("MorphControl::morph_to_state_5c80d0", 0x005c80d0, &H::morph_set_frame);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b104_10.cpp (game/b104_10)
//
//    - Morph::blend_to 0x005c8360 (name, time, duration), ret 0xc; eax replicated (callee's,
//      this, or duration on the "already running" exit).
namespace {
namespace game_b104_10 {


JOAN_COUNTER(c_morph, "Morph::blend_to");

// ---------------------------------------------------------------------------------------
// Morph::blend_to 0x005c8360
// ---------------------------------------------------------------------------------------
float morph_frac(u32 num, u32 dur) {
    // fild qword (num zero-extended) / fidiv dword (dur as signed int32), PC24
    return static_cast<float>(joan::hook::x87::Fpu{true}.div(static_cast<double>(num),
                                                             static_cast<double>(static_cast<i32>(dur))));
}

std::uintptr_t morph_blend_to(void* self, u32 name, u32 time, u32 dur) {
    c_morph.hit();
    const u32 limit = global<u32>(d::g_now);
    if (dur == 0 || !(time + dur > limit)) {
        joan::orig::as_morph(self)->select(joan::orig::arg::str(name));  // 0x005c7ff0
        return 0;  // void callee; the only caller chain (0x005ae360) is unreferenced
    }
    const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_morph(self)->find(joan::orig::arg::str(name)));  // 0x005c90a0
    if (jo::as_morph(self)->current_target == idx) return reinterpret_cast<std::uintptr_t>(self);
    if (jo::as_morph(self)->current_target == 0 && jo::as_morph(self)->blend_start == time && jo::view<u32>(jo::as_morph(self)->blend_duration) == dur &&
        jo::as_morph(self)->next_target == idx) {
        return dur;  // eax = [this+0x50]
    }
    // Target idx of the MorphTarget array (0x44-byte records on x86; the array is allocated with sizeof, so x64's stride is sizeof).
    float* const target = jo::view<float*>(jo::as_morph(self)->targets[idx - 1].positions);

    if (jo::view<u32>(jo::as_morph(self)->blend_duration) != 0 && jo::as_morph(self)->last_update < jo::as_morph(self)->blend_end) {
        float f;
        const u32 s48 = jo::as_morph(self)->blend_start, s4c = jo::as_morph(self)->blend_end, s54 = jo::as_morph(self)->last_update;
        if (time <= s48) {
            f = 0.0f;
        } else if (time < s4c) {
            f = s54 < s48 ? morph_frac(time - s48, dur) : morph_frac(time - s54, dur);
        } else if (s54 <= s4c) {
            f = s54 < s48 ? morph_frac(s4c - s48, dur) : morph_frac(s4c - s54, dur);
        } else {
            f = 0.0f;
        }
        if (f < 0.0f || f > 0.0f) {  // fcomp 0.0 / test ah,0x40: equal or NaN skips
            for (i32 i = 0; i < jo::as_morph(self)->vertex_count; ++i) {
                for (i32 k = 0; k < 3; ++k) {
                    float* const cur = jo::view<float*>(jo::as_morph(self)->positions) + i * 3;
                    const float m = f * (jo::view<float*>(jo::as_morph(self)->position_step) + i * 3)[k];
                    cur[k] = m + cur[k];
                }
                for (i32 k = 0; k < 3; ++k) {
                    float* const cur = jo::view<float*>(jo::as_morph(self)->normals) + i * 3;
                    const float m = f * (jo::view<float*>(jo::as_morph(self)->normal_step) + i * 3)[k];
                    cur[k] = m + cur[k];
                }
            }
        }
    }
    jo::as_morph(self)->last_update = time < limit ? time : limit - 1;
    for (i32 i = 0; i < jo::as_morph(self)->vertex_count; ++i) {
        for (i32 k = 0; k < 3; ++k) {
            (jo::view<float*>(jo::as_morph(self)->position_step) + i * 3)[k] = target[i * 3 + k] - (jo::view<float*>(jo::as_morph(self)->positions) + i * 3)[k];
        }
        for (i32 k = 0; k < 3; ++k) {
            (jo::view<float*>(jo::as_morph(self)->normal_step) + i * 3)[k] = target[i * 3 + k] - (jo::view<float*>(jo::as_morph(self)->normals) + i * 3)[k];
        }
    }
    jo::as_morph(self)->current_target = 0;
    jo::as_morph(self)->next_target = idx;
    jo::as_morph(self)->blend_start = time;
    jo::as_morph(self)->blend_end = time + dur;
    jo::view<u32>(jo::as_morph(self)->blend_duration) = dur;
    return reinterpret_cast<std::uintptr_t>(self);
}

struct H {
    std::uintptr_t morph_blend_to(u32 name, u32 time, u32 dur) { return game_b104_10::morph_blend_to(this, name, time, dur); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("MorphControl::morph_to_state_5c8360", 0x005c8360, &H::morph_blend_to);
#undef B

}  // namespace game_b104_10
}  // namespace

// ---------------------------------------- from modules/en3d_helpers14.cpp (en3d/helpers15)
namespace {
namespace en3d_helpers14 {


std::uint32_t now() { return global<std::uint32_t>(d::g_now); }  // game clock (ms)

JOAN_COUNTER(c_morph, "Morph::update");
JOAN_COUNTER(c_morph_weights, "Morph::set_weights");

struct H {
    // 0x005c9120: positions / normals = base + sum(weight * target delta), restarts the blend.
    void morph_set_weights(const float* weights) {
        c_morph_weights.hit();
        jo::self<jo::Morph>(this)->current_target = 0;
        for (std::int32_t i = 0; i < jo::self<jo::Morph>(this)->vertex_count; ++i) {
            float p[3], nrm[3];
            std::memcpy(p, jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_positions) + i * 0xc, 12);
            std::memcpy(nrm, jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_normals) + i * 0xc, 12);
            for (std::int32_t j = 0; j < jo::self<jo::Morph>(this)->target_count; ++j) {
                const float w = weights[j];
                if (!(w < 0.0f || w > 0.0f)) continue;  // test ah,0x40: zero or NaN skips
                const float* const dp = reinterpret_cast<const float*>(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->targets[j].position_deltas) + i * 0xc);
                p[0] = w * dp[0] + p[0];
                p[1] = w * dp[1] + p[1];
                p[2] = w * dp[2] + p[2];
                const float* const dn = reinterpret_cast<const float*>(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->targets[j].normal_deltas) + i * 0xc);
                nrm[0] = w * dn[0] + nrm[0];
                nrm[1] = w * dn[1] + nrm[1];
                nrm[2] = w * dn[2] + nrm[2];
            }
            std::memcpy(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->positions) + i * 0xc, p, 12);
            std::memcpy(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->normals) + i * 0xc, nrm, 12);
        }
        jo::self<jo::Morph>(this)->blend_start = now();
        jo::self<jo::Morph>(this)->blend_end = now();
        jo::self<jo::Morph>(this)->blend_duration = 0;
    }

    // 0x005c8960: advance the morph to now and write positions / normals into the plane's
    // vertex buffer (type 7: 0x48-byte vertices, normal at +0x18; with a skeleton: transformed
    // by the vertex's bone).
    void morph_update(void* plane, void* skeleton) {
        c_morph.hit();
        if (jo::as_plane(plane)->data_pending != 0) joan::orig::as_plane(plane)->load_5d9be0();  // 0x005d9be0
        if (jo::self<jo::Morph>(this)->last_update == now()) return;
        if (jo::view<std::int32_t>(jo::self<jo::Morph>(this)->anim_playing) != 0) {
            const float elapsed = static_cast<float>(static_cast<double>(static_cast<std::uint32_t>(now() - jo::self<jo::Morph>(this)->anim_start)));
            joan::orig::as_morph_anim(jo::view<void*>(jo::self<jo::Morph>(this)->anim))->evaluate(joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::f32(elapsed), 1);  // 0x005cb3d0
        }
        if (jo::self<jo::Morph>(this)->current_target == 0) {
            if (!(now() < jo::self<jo::Morph>(this)->blend_end)) {
                if (jo::self<jo::Morph>(this)->last_update >= jo::self<jo::Morph>(this)->blend_end) return;
                jo::self<jo::Morph>(this)->current_target = jo::self<jo::Morph>(this)->next_target;
                if (jo::self<jo::Morph>(this)->current_target != 0) {
                    std::uint8_t* const target = reinterpret_cast<std::uint8_t*>(&jo::self<jo::Morph>(this)->targets[jo::self<jo::Morph>(this)->current_target - 1]);  // 0x44-byte records on x86
                    std::memmove(jo::view<void*>(jo::self<jo::Morph>(this)->positions), jo::view<void*>(jo::as_morph_target(target)->positions), static_cast<std::size_t>(jo::self<jo::Morph>(this)->vertex_count * 0xc));
                    std::memmove(jo::view<void*>(jo::self<jo::Morph>(this)->normals), jo::view<void*>(jo::as_morph_target(target)->normals), static_cast<std::size_t>(jo::self<jo::Morph>(this)->vertex_count * 0xc));
                }
            } else if (!(now() < jo::self<jo::Morph>(this)->blend_start)) {
                const joan::hook::x87::Fpu fpu{true};
                const std::uint32_t from = jo::self<jo::Morph>(this)->blend_start > jo::self<jo::Morph>(this)->last_update ? jo::self<jo::Morph>(this)->blend_start : jo::self<jo::Morph>(this)->last_update;
                const float w = static_cast<float>(fpu.div(static_cast<double>(static_cast<std::uint32_t>(now() - from)),
                                                           static_cast<double>(jo::self<jo::Morph>(this)->blend_duration)));
                for (std::int32_t i = 0; i < jo::self<jo::Morph>(this)->vertex_count; ++i) {
                    float* const p = reinterpret_cast<float*>(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->positions) + i * 0xc);
                    const float* const vp = reinterpret_cast<const float*>(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->position_step) + i * 0xc);
                    p[0] = w * vp[0] + p[0];
                    p[1] = w * vp[1] + p[1];
                    p[2] = w * vp[2] + p[2];
                    float* const nn = reinterpret_cast<float*>(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->normals) + i * 0xc);
                    const float* const vn = reinterpret_cast<const float*>(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->normal_step) + i * 0xc);
                    nn[0] = w * vn[0] + nn[0];
                    nn[1] = w * vn[1] + nn[1];
                    nn[2] = w * vn[2] + nn[2];
                }
            }
        }
        const std::int8_t type = jo::as_plane(plane)->vertex_format;
        if (type == 7) {
            std::uint8_t* const vb = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(plane)->buffer(joan::orig::arg::mptr(static_cast<std::int32_t*>(nullptr))));  // 0x005cdae0
            for (std::int32_t i = 0; i < jo::self<jo::Morph>(this)->vertex_count; ++i) {
                const std::int32_t index = jo::view<std::int32_t*>(jo::self<jo::Morph>(this)->vertex_indices)[i];
                std::uint8_t* const v = vb + index * 0x48;
                std::memcpy(jo::as_vertex_f7(v)->influence_pos, jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->positions) + i * 0xc, 12);
                import_call<void*>(d::iat_v3d_87_vector_normalize_2, jo::as_vertex_f7(v)->influence_normal, jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->positions) + i * 0xc);  // vector_normalize
            }
        } else if (skeleton != nullptr) {
            std::int32_t stride = 0;
            std::uint8_t* const vb = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(plane)->buffer(&stride));
            const std::int32_t normal_offset =
                jo::as_vertex_format(joan::orig::arg::as<void*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<std::int32_t>(type))))))->normal_offset;  // 0x00600200 format
            for (std::int32_t i = 0; i < jo::self<jo::Morph>(this)->vertex_count; ++i) {
                const std::uint8_t* const entry = jo::view<std::uint8_t*>(jo::as_skin(skeleton)->vertices) + jo::view<std::int32_t*>(jo::self<jo::Morph>(this)->vertex_indices)[i] * static_cast<std::int32_t>(sizeof(jo::SkinVertex));
                std::uint8_t* const v = vb + stride * jo::view<std::int32_t*>(jo::self<jo::Morph>(this)->vertex_indices)[i];
                std::uint8_t* const nrm = vb + stride * jo::view<std::int32_t*>(jo::self<jo::Morph>(this)->vertex_indices)[i] + normal_offset;
                void* const bone = joan::orig::arg::as<void*>(joan::orig::as_plane(plane)->bone_5cf8f0(joan::orig::arg::i32(jo::as_skin_vertex(entry)->bone0.bone)));  // 0x005cf8f0
                void* const matrix = joan::orig::arg::as<void*>(joan::orig::as_bone(bone)->matrix());                             // 0x0060ef80
                import_call<void*>(d::iat_v3d_87_point_transform_3, v, jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->positions) + i * 0xc, matrix);       // point_transform
                import_call<void*>(d::iat_v3d_87_vector_normalize_2, nrm, v);
            }
        } else {
            std::int32_t stride = 0;
            std::uint8_t* const vb = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(plane)->buffer(&stride));
            const std::int32_t has_normal =
                jo::as_vertex_format(joan::orig::arg::as<void*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<std::int32_t>(type))))))->prelit == 0 ? 1 : 0;
            const std::int32_t normal_offset = jo::as_vertex_format(joan::orig::arg::as<void*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<std::int32_t>(type))))))->normal_offset;
            for (std::int32_t i = 0; i < jo::self<jo::Morph>(this)->vertex_count; ++i) {
                std::uint8_t* const v = vb + jo::view<std::int32_t*>(jo::self<jo::Morph>(this)->vertex_indices)[i] * stride;
                std::memcpy(v, jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->positions) + i * 0xc, 12);
                if (has_normal != 0) import_call<void*>(d::iat_v3d_87_vector_normalize_2, v + normal_offset, jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->normals) + i * 0xc);
            }
        }
        jo::self<jo::Morph>(this)->last_update = now();
        joan::orig::as_plane(plane)->invalidate(2u);  // 0x005cf920
    }
};

#define HELPER15(name, addr, fn) JOAN_REPLACE("en3d/helpers15", name, addr, fn)
HELPER15("Morph::set_weights", 0x005c9120, &H::morph_set_weights);
HELPER15("MorphControl::update_plane", 0x005c8960, &H::morph_update);
#undef HELPER15

}  // namespace en3d_helpers14
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
//
//      0x005c9350 MorphAnim::blend_to(w*, start, dur) (thiscall, ret 0xc) - advance the running
//                 blend (+0x58 / +0x5c += f * +0x60 / +0x64), new deltas from +0x68 / +0x6c and the
//                 weighted target table +0x38 (0x44 records), +0x48 / +0x4c / +0x50 / +0x54.
//
//  - Direct callers / references: 0x005b2aa0 (0x00417845, 0x0047d41e) and 0x005c9350 (0x004d2519)
//
//  - 0x005c9350: fild of a zero-extended qword (exact uint32) / fidiv of the signed dword dur ->
//    Fpu{true}.div; the rest is float fmul / fadd / fsub -> C++ float in the original order.
//    `fcomp 0.0; test ah,0x40; jne skip` = skip when equal OR unordered.
namespace {
namespace game_b103_11 {

using joan::hook::x87::Fpu;


JOAN_COUNTER(c_blend_to, "MorphAnim::blend_to");

// Component c (x, y, z) of element i of a Vec3f array.
float& comp(jo::Vec3f* v, i32 i, i32 c) { return reinterpret_cast<float*>(v + i)[c]; }
const float& comp(const jo::Vec3f* v, i32 i, i32 c) { return reinterpret_cast<const float*>(v + i)[c]; }

struct H {
    // 0x005c9350
    void blend_to(const float* w, u32 start, u32 dur) {
        c_blend_to.hit();
        if (dur == 0 || start + dur <= global<u32>(d::g_now)) {
            joan::orig::as_morph(this)->set_weights(joan::orig::arg::mptr(w));  // 0x005c9120
            return;
        }
        const Fpu fpu{true};
        if (jo::view<u32>(jo::self<jo::Morph>(this)->blend_duration) != 0 && jo::self<jo::Morph>(this)->last_update < jo::self<jo::Morph>(this)->blend_end) {
            float t;
            const double d = static_cast<double>(static_cast<i32>(dur));  // fidiv dword (signed)
            if (start <= jo::self<jo::Morph>(this)->blend_start) {
                t = 0.0f;
            } else if (start < jo::self<jo::Morph>(this)->blend_end) {
                if (jo::self<jo::Morph>(this)->last_update < jo::self<jo::Morph>(this)->blend_start)
                    t = static_cast<float>(fpu.div(static_cast<double>(start - jo::self<jo::Morph>(this)->blend_start), d));
                else
                    t = static_cast<float>(fpu.div(static_cast<double>(start - jo::self<jo::Morph>(this)->last_update), d));
            } else if (jo::self<jo::Morph>(this)->last_update <= jo::self<jo::Morph>(this)->blend_end) {
                if (jo::self<jo::Morph>(this)->last_update < jo::self<jo::Morph>(this)->blend_start)
                    t = static_cast<float>(fpu.div(static_cast<double>(jo::self<jo::Morph>(this)->blend_end - jo::self<jo::Morph>(this)->blend_start), d));
                else
                    t = static_cast<float>(fpu.div(static_cast<double>(jo::self<jo::Morph>(this)->blend_end - jo::self<jo::Morph>(this)->last_update), d));
            } else {
                t = 0.0f;
            }
            const float zero = global<float>(d::k_9cf5c4);
            if (!(t == zero) && t == t) {  // jne skip: equal or unordered
                for (i32 i = 0; i < jo::self<jo::Morph>(this)->vertex_count; ++i) {
                    for (i32 c = 0; c < 3; ++c) {
                        float& p = comp(jo::self<jo::Morph>(this)->positions, i, c);
                        p = t * comp(jo::self<jo::Morph>(this)->position_step, i, c) + p;
                    }
                    for (i32 c = 0; c < 3; ++c) {
                        float& p = comp(jo::self<jo::Morph>(this)->normals, i, c);
                        p = t * comp(jo::self<jo::Morph>(this)->normal_step, i, c) + p;
                    }
                }
            }
        }
        const float zero = global<float>(d::k_9cf5c4);
        for (i32 j = 0; j < jo::self<jo::Morph>(this)->vertex_count; ++j) {
            for (i32 c = 0; c < 3; ++c)
                comp(jo::self<jo::Morph>(this)->position_step, j, c) =
                    comp(jo::self<jo::Morph>(this)->base_positions, j, c) - comp(jo::self<jo::Morph>(this)->positions, j, c);
            for (i32 c = 0; c < 3; ++c)
                comp(jo::self<jo::Morph>(this)->normal_step, j, c) =
                    comp(jo::self<jo::Morph>(this)->base_normals, j, c) - comp(jo::self<jo::Morph>(this)->normals, j, c);
            for (i32 k = 0; k < jo::self<jo::Morph>(this)->target_count; ++k) {
                const float wk = w[k];
                if (wk == zero || wk != wk) continue;  // jne skip: equal or unordered
                for (i32 c = 0; c < 3; ++c) {
                    const jo::Vec3f* tv = jo::self<jo::Morph>(this)->targets[k].position_deltas;
                    float& p = comp(jo::self<jo::Morph>(this)->position_step, j, c);
                    p = comp(tv, j, c) * w[k] + p;
                }
                for (i32 c = 0; c < 3; ++c) {
                    const jo::Vec3f* tv = jo::self<jo::Morph>(this)->targets[k].normal_deltas;
                    float& p = comp(jo::self<jo::Morph>(this)->normal_step, j, c);
                    p = comp(tv, j, c) * w[k] + p;
                }
            }
        }
        const u32 g = global<u32>(d::g_now);
        jo::self<jo::Morph>(this)->last_update = start < g ? start : g - 1;
        jo::view<u32>(jo::self<jo::Morph>(this)->current_target) = 0;
        jo::view<u32>(jo::self<jo::Morph>(this)->next_target) = 0;
        jo::self<jo::Morph>(this)->blend_start = start;
        jo::self<jo::Morph>(this)->blend_end = start + dur;
        jo::view<u32>(jo::self<jo::Morph>(this)->blend_duration) = dur;
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("MorphAnim::blend_to", 0x005c9350, &H::blend_to);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/en3d_b85_03.cpp (en3d/b85_03)
//
//      0x005c9b10 Morph::add(dir, name, matrix) (ret 0xc) - <dir><name>.MDA appended to the
//                 existing data: per source vertex indices / positions / normals (+0x40 / +0x44 /
//                 +0x68 / +0x6c), then per named target (0x005c90a0) its four arrays grown from
//                 the old count and filled, optional point / vector transform; +0x58..+0x64
//                 resized; 0x005c7ff0(+0x38) when +0x80 == 0
namespace {
namespace en3d_b85_03 {


using joan::orig::helpers::mul;
std::int32_t add(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

void* const kMemory = obj(d::g_mem);    // allocator (0x0058eb30 realloc)

using joan::orig::helpers::memcpy_;

static_assert(sizeof(Path) == 0x194, "Path");
void path_append(Path& p, const char* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }  // 0x005a1550
bool file_exists(Path& p) { return joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(p))) != 0; }  // 0x00590b80

struct alignas(4) Reader {  // 0x110 used (0x004179c0 writes +0x105, flags up to +0x10d); room
    std::uint8_t bytes[0x210];
};
std::int32_t mread_int(Reader& r) { return joan::orig::as_file(&r)->file_get_long(); }            // 0x00587e20
void mread(Reader& r, void* p, std::int32_t n) { joan::orig::as_file(&r)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n)); }  // 0x005873a0

std::int32_t morph_line(std::int32_t a) { return static_cast<std::int32_t>(global<std::int16_t>(d::g_a16ed0)) + a; }
void* mem_realloc(void* p, std::int32_t size, std::int32_t line) {
    return joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(kMorphCpp), joan::orig::arg::i32(morph_line(line)));  // 0x0058eb30
}

// 50-byte target name read followed by the target count, as in the original frame.
struct TargetName {
    char name[0x34];
    std::int32_t count;
};
static_assert(sizeof(TargetName) == 0x38, "name + count");

JOAN_COUNTER(c_morph_add, "Morph::add");

struct H {
    // 0x005c9b10
    void morph_add(const char* dir, const char* name, void* matrix) {
        c_morph_add.hit();
        const std::int32_t old_count = jo::self<jo::Morph>(this)->vertex_count;
        Path path;
        joan::orig::as_string(&path)->construct(joan::orig::arg::str(dir));  // 0x005a11a0
        path_append(path, name);
        path_append(path, str(d::s_MDA));                   // ".MDA"
        if (!file_exists(path)) {
            joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_Error_loading_Morph_Data_1s), c_str(path));  // 0x00586cb0 "Error loading Morph Data : %1s"
            return;
        }
        Reader file;
        joan::orig::as_file(&file)->construct();                            // 0x004179c0
        joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(path)), 1, static_cast<char>(0));  // 0x00586da0 open
        const std::int32_t sources = mread_int(file);
        std::int32_t* const base = static_cast<std::int32_t*>(game_new(static_cast<std::uint32_t>(sources << 2)));
        for (std::int32_t i = 0; i < sources; ++i) {
            const std::int32_t advance = mread_int(file);
            const std::int32_t n = mread_int(file);
            base[i] = jo::self<jo::Morph>(this)->vertex_base;
            if (n != 0) {
                jo::view<void*>(jo::self<jo::Morph>(this)->vertex_indices) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->vertex_indices), add(jo::self<jo::Morph>(this)->vertex_count, n) << 2, 0x21);
                jo::view<void*>(jo::self<jo::Morph>(this)->base_positions) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->base_positions), mul(add(jo::self<jo::Morph>(this)->vertex_count, n), 0xc), 0x22);
                jo::view<void*>(jo::self<jo::Morph>(this)->base_normals) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->base_normals), mul(add(jo::self<jo::Morph>(this)->vertex_count, n), 0xc), 0x23);
                for (std::int32_t k = 0; k < n; ++k) {
                    const std::int32_t index = mread_int(file);
                    jo::view<std::int32_t*>(jo::self<jo::Morph>(this)->vertex_indices)[jo::self<jo::Morph>(this)->vertex_count] = add(index, base[i]);
                    mread(file, jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_positions) + mul(jo::self<jo::Morph>(this)->vertex_count, 0xc), 0xc);
                    mread(file, jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_normals) + mul(jo::self<jo::Morph>(this)->vertex_count, 0xc), 0xc);
                    if (matrix != nullptr) {
                        import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_positions) + mul(jo::self<jo::Morph>(this)->vertex_count, 0xc)),
                                           matrix);  // point_transform
                        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_normals) + mul(jo::self<jo::Morph>(this)->vertex_count, 0xc)),
                                           matrix);  // vector_transform
                    }
                    jo::self<jo::Morph>(this)->vertex_count = jo::self<jo::Morph>(this)->vertex_count + 1;
                }
            }
            jo::self<jo::Morph>(this)->vertex_base = add(jo::self<jo::Morph>(this)->vertex_base, advance);
        }
        TargetName target_name;
        target_name.name[0x32] = 0;  // never written by the 50-byte read (stale stack bytes in the original)
        target_name.name[0x33] = 0;
        target_name.count = mread_int(file);
        if (target_name.count == 0) game_delete(base);
        for (std::int32_t i = 0; i < target_name.count; ++i) {
            joan::orig::as_file(&file)->file_read_array(static_cast<void*>(target_name.name), 0x32u, 1u);                         // 0x00587650
            const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::as_morph(this)->find(joan::orig::arg::str(static_cast<void*>(target_name.name))));  // 0x005c90a0
            std::uint8_t* const t = jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->targets) + mul(r - 1, static_cast<std::int32_t>(sizeof(jo::MorphTarget)));
            jo::view<void*>(jo::as_morph_target(t)->positions) = mem_realloc(jo::view<void*>(jo::as_morph_target(jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->targets) + mul(i, static_cast<std::int32_t>(sizeof(jo::MorphTarget))))->positions), mul(jo::self<jo::Morph>(this)->vertex_count, 0xc), 0x4a);
            memcpy_(jo::view<std::uint8_t*>(jo::as_morph_target(t)->positions) + mul(old_count, 0xc), jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_positions) + mul(old_count, 0xc),
                    static_cast<std::uint32_t>(mul(jo::self<jo::Morph>(this)->vertex_count - old_count, 0xc)));
            jo::view<void*>(jo::as_morph_target(t)->normals) = mem_realloc(jo::view<void*>(jo::as_morph_target(t)->normals), mul(jo::self<jo::Morph>(this)->vertex_count, 0xc), 0x4e);
            memcpy_(jo::view<std::uint8_t*>(jo::as_morph_target(t)->normals) + mul(old_count, 0xc), jo::view<std::uint8_t*>(jo::self<jo::Morph>(this)->base_normals) + mul(old_count, 0xc),
                    static_cast<std::uint32_t>(mul(jo::self<jo::Morph>(this)->vertex_count - old_count, 0xc)));
            jo::view<void*>(jo::as_morph_target(t)->position_deltas) = mem_realloc(jo::view<void*>(jo::as_morph_target(t)->position_deltas), mul(jo::self<jo::Morph>(this)->vertex_count, 0xc), 0x52);
            memset_(jo::view<std::uint8_t*>(jo::as_morph_target(t)->position_deltas) + mul(old_count, 0xc), 0, static_cast<std::uint32_t>(mul(jo::self<jo::Morph>(this)->vertex_count - old_count, 0xc)));
            jo::view<void*>(jo::as_morph_target(t)->normal_deltas) = mem_realloc(jo::view<void*>(jo::as_morph_target(t)->normal_deltas), mul(jo::self<jo::Morph>(this)->vertex_count, 0xc), 0x56);
            memset_(jo::view<std::uint8_t*>(jo::as_morph_target(t)->normal_deltas) + mul(old_count, 0xc), 0, static_cast<std::uint32_t>(mul(jo::self<jo::Morph>(this)->vertex_count - old_count, 0xc)));
            for (std::int32_t k = 0; k < sources; ++k) {
                const std::int32_t count = mread_int(file);
                std::int32_t j = 0;
                for (std::int32_t v = 0; v < count; ++v) {
                    const std::int32_t index = add(mread_int(file), base[k]);
                    // Original order: element compare first, then the bound (reads [+0x40]).
                    while (jo::view<std::int32_t*>(jo::self<jo::Morph>(this)->vertex_indices)[j] != index && j < jo::self<jo::Morph>(this)->vertex_count) ++j;
                    mread(file, jo::view<std::uint8_t*>(jo::as_morph_target(t)->positions) + mul(j, 0xc), 0xc);
                    mread(file, jo::view<std::uint8_t*>(jo::as_morph_target(t)->normals) + mul(j, 0xc), 0xc);
                    mread(file, jo::view<std::uint8_t*>(jo::as_morph_target(t)->position_deltas) + mul(j, 0xc), 0xc);
                    mread(file, jo::view<std::uint8_t*>(jo::as_morph_target(t)->normal_deltas) + mul(j, 0xc), 0xc);
                    if (matrix != nullptr) {
                        import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::as_morph_target(t)->positions) + mul(j, 0xc)), matrix);
                        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::as_morph_target(t)->normals) + mul(j, 0xc)), matrix);
                        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::as_morph_target(t)->position_deltas) + mul(j, 0xc)), matrix);
                        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(jo::view<std::uint8_t*>(jo::as_morph_target(t)->normal_deltas) + mul(j, 0xc)), matrix);
                    }
                }
            }
        }
        joan::orig::as_file(&file)->file_close();  // 0x00587260 close
        game_delete(base);
        jo::view<void*>(jo::self<jo::Morph>(this)->positions) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->positions), mul(jo::self<jo::Morph>(this)->vertex_count, 0xc), 0x83);
        jo::view<void*>(jo::self<jo::Morph>(this)->normals) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->normals), mul(jo::self<jo::Morph>(this)->vertex_count, 0xc), 0x84);
        jo::view<void*>(jo::self<jo::Morph>(this)->position_step) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->position_step), mul(jo::self<jo::Morph>(this)->vertex_count, 0xc), 0x85);
        jo::view<void*>(jo::self<jo::Morph>(this)->normal_step) = mem_realloc(jo::view<void*>(jo::self<jo::Morph>(this)->normal_step), mul(jo::self<jo::Morph>(this)->vertex_count, 0xc), 0x86);
        if (jo::self<jo::Morph>(this)->anim == nullptr) joan::orig::as_morph(this)->select(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Morph>(this)->targets)));  // 0x005c7ff0
        joan::orig::as_file(&file)->destruct();  // 0x005872b0 ~Reader
    }
};

#define B85_03(name, addr, fn) JOAN_REPLACE("en3d/b85_03", name, addr, fn)
B85_03("MorphControl::append_morphable_vertex_5c9b10", 0x005c9b10, &H::morph_add);
#undef B85_03

}  // namespace en3d_b85_03
}  // namespace

// ---------------------------------------- from modules/game_b107_19.cpp (game/b107_19)
//
//    - Morph::merge_5ca5d0 0x005ca5d0 (other, matrix), ret 8 (en3d/morph.cpp; no references:
//      dead code, eax not observable -> void).
//
//  - 0x005ca5d0: when matrix == 0 the per-target memcpys (0x7b2a20) use this->+0x40 (the NEW total
//    count) * 12 as size while the vertex-list copy uses other->+0x40 * 12; kept as in the
//    original (would overrun when this already had vertices; the function is unreferenced).
namespace {
namespace game_b107_19 {


JOAN_COUNTER(c_merge, "Morph::merge_5ca5d0");

void* realloc_(void* p, i32 size, i32 line_delta) {
    const i32 line = static_cast<i32>(global<i16>(d::g_a16ed4)) + line_delta;
    return joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), str(d::s_d_ww_en3d_morph_cpp), joan::orig::arg::i32(line));  // 0x0058eb30
}
void* memcpy_(void* d, const void* s, i32 n) { return std::memcpy(joan::orig::arg::mptr(d), joan::orig::arg::ptr(s), joan::orig::arg::u32(n)); }
void point_transform(void* out, void* in, void* m) { import_call<void*>(d::iat_v3d_87_point_transform_3, out, in, m); }
void vector_transform(void* out, void* in, void* m) { import_call<void*>(d::iat_v3d_87_vector_transform_3, out, in, m); }
// The four vertex arrays of a morph target in the order the original walks them (+0x34, +0x3c, +0x38, +0x40).
void*& target_array(jo::MorphTarget& t, i32 k) {
    switch (k) {
        case 0: return jo::view<void*>(t.positions);
        case 1: return jo::view<void*>(t.normals);
        case 2: return jo::view<void*>(t.position_deltas);
        default: return jo::view<void*>(t.normal_deltas);
    }
}

struct H {
    // ------------------------------------------------------------------ 0x005ca5d0
    void merge_5ca5d0(void* other, void* matrix) {
        c_merge.hit();
        const i32 base_a = jo::self<jo::Morph>(this)->vertex_base;  // [ebp-0x8]
        const i32 base_v = jo::self<jo::Morph>(this)->vertex_count;  // [ebp-0xc]
        jo::self<jo::Morph>(this)->vertex_base = jo::self<jo::Morph>(this)->vertex_base + jo::as_morph(other)->vertex_base;
        jo::self<jo::Morph>(this)->vertex_count = jo::self<jo::Morph>(this)->vertex_count + jo::as_morph(other)->vertex_count;
        if (jo::as_morph(other)->vertex_count != 0) {
            jo::view<void*>(jo::self<jo::Morph>(this)->vertex_indices) = realloc_(jo::view<void*>(jo::self<jo::Morph>(this)->vertex_indices), jo::self<jo::Morph>(this)->vertex_count << 2, 0x9);
            jo::view<void*>(jo::self<jo::Morph>(this)->base_positions) = realloc_(jo::view<void*>(jo::self<jo::Morph>(this)->base_positions), jo::self<jo::Morph>(this)->vertex_count * 0xc, 0xb);
            jo::view<void*>(jo::self<jo::Morph>(this)->base_normals) = realloc_(jo::view<void*>(jo::self<jo::Morph>(this)->base_normals), jo::self<jo::Morph>(this)->vertex_count * 0xc, 0xc);
            jo::view<void*>(jo::self<jo::Morph>(this)->positions) = realloc_(jo::view<void*>(jo::self<jo::Morph>(this)->positions), jo::self<jo::Morph>(this)->vertex_count * 0xc, 0xd);
            jo::view<void*>(jo::self<jo::Morph>(this)->normals) = realloc_(jo::view<void*>(jo::self<jo::Morph>(this)->normals), jo::self<jo::Morph>(this)->vertex_count * 0xc, 0xe);
            jo::view<void*>(jo::self<jo::Morph>(this)->position_step) = realloc_(jo::view<void*>(jo::self<jo::Morph>(this)->position_step), jo::self<jo::Morph>(this)->vertex_count * 0xc, 0xf);
            jo::view<void*>(jo::self<jo::Morph>(this)->normal_step) = realloc_(jo::view<void*>(jo::self<jo::Morph>(this)->normal_step), jo::self<jo::Morph>(this)->vertex_count * 0xc, 0x10);
            for (i32 i = 0; i < jo::as_morph(other)->vertex_count; ++i) {
                jo::self<jo::Morph>(this)->vertex_indices[i + base_v].value = jo::as_morph(other)->vertex_indices[i].value + base_a;
                if (matrix != nullptr) {
                    point_transform(jo::view<u8*>(jo::self<jo::Morph>(this)->base_positions) + (i + base_v) * 0xc, jo::view<u8*>(jo::as_morph(other)->base_positions) + i * 0xc, matrix);
                    vector_transform(jo::view<u8*>(jo::self<jo::Morph>(this)->base_normals) + (i + base_v) * 0xc, jo::view<u8*>(jo::as_morph(other)->base_normals) + i * 0xc, matrix);
                }
            }
            if (matrix == nullptr) {
                memcpy_(jo::view<u8*>(jo::self<jo::Morph>(this)->base_positions) + base_v * 0xc, jo::view<void*>(jo::as_morph(other)->base_positions), jo::as_morph(other)->vertex_count * 0xc);
                memcpy_(jo::view<u8*>(jo::self<jo::Morph>(this)->base_normals) + base_v * 0xc, jo::view<void*>(jo::as_morph(other)->base_normals), jo::as_morph(other)->vertex_count * 0xc);
            }
        }
        for (i32 i = 0; i < jo::as_morph(other)->target_count; ++i) {
            const i32 j = joan::orig::arg::as<i32>(joan::orig::as_morph(this)->find(joan::orig::arg::str(jo::view<u8*>(jo::as_morph(other)->targets) + i * static_cast<std::int32_t>(sizeof(jo::MorphTarget)))));  // 0x005c90a0
            if (j == 0) continue;
            for (i32 k = 0; k < 4; ++k) {
                void*& field = target_array(jo::self<jo::Morph>(this)->targets[j - 1], k);
                field = realloc_(field, jo::self<jo::Morph>(this)->vertex_count * 0xc, 0x2b + k);
            }
            if (matrix != nullptr) {
                for (i32 v = 0; v < jo::as_morph(other)->vertex_count; ++v) {
                    u8* src = jo::view<u8*>(jo::as_morph(other)->targets) + i * static_cast<std::int32_t>(sizeof(jo::MorphTarget));
                    u8* dst = jo::view<u8*>(jo::self<jo::Morph>(this)->targets) + (j - 1) * static_cast<std::int32_t>(sizeof(jo::MorphTarget));
                    point_transform(jo::view<u8*>(jo::as_morph_target(dst)->positions) + (v + base_v) * 0xc, jo::view<u8*>(jo::as_morph_target(src)->positions) + v * 0xc, matrix);
                    src = jo::view<u8*>(jo::as_morph(other)->targets) + i * static_cast<std::int32_t>(sizeof(jo::MorphTarget));
                    dst = jo::view<u8*>(jo::self<jo::Morph>(this)->targets) + (j - 1) * static_cast<std::int32_t>(sizeof(jo::MorphTarget));
                    vector_transform(jo::view<u8*>(jo::as_morph_target(dst)->normals) + (v + base_v) * 0xc, jo::view<u8*>(jo::as_morph_target(src)->normals) + v * 0xc, matrix);
                    src = jo::view<u8*>(jo::as_morph(other)->targets) + i * static_cast<std::int32_t>(sizeof(jo::MorphTarget));
                    dst = jo::view<u8*>(jo::self<jo::Morph>(this)->targets) + (j - 1) * static_cast<std::int32_t>(sizeof(jo::MorphTarget));
                    vector_transform(jo::view<u8*>(jo::as_morph_target(dst)->position_deltas) + (v + base_v) * 0xc, jo::view<u8*>(jo::as_morph_target(src)->position_deltas) + v * 0xc, matrix);
                    src = jo::view<u8*>(jo::as_morph(other)->targets) + i * static_cast<std::int32_t>(sizeof(jo::MorphTarget));
                    dst = jo::view<u8*>(jo::self<jo::Morph>(this)->targets) + (j - 1) * static_cast<std::int32_t>(sizeof(jo::MorphTarget));
                    vector_transform(jo::view<u8*>(jo::as_morph_target(dst)->normal_deltas) + (v + base_v) * 0xc, jo::view<u8*>(jo::as_morph_target(src)->normal_deltas) + v * 0xc, matrix);
                }
            } else {
                for (i32 k = 0; k < 4; ++k) {
                    const i32 n = jo::self<jo::Morph>(this)->vertex_count * 0xc;  // this->+0x40 (new total), as in the original
                    memcpy_(static_cast<u8*>(target_array(jo::self<jo::Morph>(this)->targets[j - 1], k)) + base_v * 0xc, target_array(jo::as_morph(other)->targets[i], k), n);
                }
            }
        }
        if (jo::as_morph(other)->anim == nullptr) joan::orig::as_morph(this)->select(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Morph>(this)->targets)));  // 0x005c7ff0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_19", name, addr, fn)
B("MorphControl::append_morphable_vertex_5ca5d0", 0x005ca5d0, &H::merge_5ca5d0);
#undef B

}  // namespace game_b107_19
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
//
//      0x005cae20 MorphAnim::MorphAnim()            - memset 0x10
//
//      0x005c67b0 Morph::Morph()                    - memset 0x84
namespace {
namespace en3d_b83_03_2 {

using joan::orig::helpers::memset_;

struct H {
    // 0x005cae20 (eax = this)
    void* anim_ctor() {
        memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::MorphAnim)));  // 0x10 on x86
        return this;
    }

    // 0x005c67b0 (eax = this)
    void* morph_ctor() {
        memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::Morph)));  // 0x84 on x86
        return this;
    }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("MorphAnimation::MorphAnimation", 0x005cae20, &H::anim_ctor);
B83_03("MorphControl::MorphControl", 0x005c67b0, &H::morph_ctor);
#undef B83_03

}  // namespace en3d_b83_03_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers32.cpp (en3d/helpers32)
//
//      0x005c67f0 thin wrapper of 0x005c7b80
namespace {
namespace en3d_helpers32 {

using joan::call::thiscall;

struct H {
    // 0x005c67f0: wrapper of 0x005c7b80.
    std::uintptr_t wrap_5c7b80() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_morph(this)->release()); }
};

#define HELPER32(name, addr, fn) JOAN_REPLACE("en3d/helpers32", name, addr, fn)
HELPER32("MorphControl::~MorphControl", 0x005c67f0, &H::wrap_5c7b80);
#undef HELPER32

}  // namespace en3d_helpers32
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
//
//      0x005cb1d0 Arr::copy_from(src, desc)         (thiscall, ret 8) - deep copy of 8-byte
//                 {count, block} entries, once (+0 flag).
namespace {
namespace game_b96_06 {

using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::op_new;

JOAN_COUNTER(c_arr_copy, "Arr::copy_from");

struct H {
    // ------------------------------------------------------------------ 0x005cb1d0
    void arr_copy_from(u8* src, u8* desc) {
        c_arr_copy.hit();
        if (jo::self<jo::Arr>(this)->loaded != 0) return;
        jo::self<jo::Arr>(this)->loaded = 1;
        jo::view<u32>(jo::self<jo::Arr>(this)->track_count) = jo::view<u32>(jo::as_morph(desc)->target_count);
        jo::view<u32>(jo::self<jo::Arr>(this)->length) = jo::view<u32>(jo::as_morph_anim(src)->length);
        jo::view<void*>(jo::self<jo::Arr>(this)->tracks) = op_new(jo::view<u32>(jo::self<jo::Arr>(this)->track_count) * static_cast<u32>(sizeof(jo::MorphTrack)));  // << 3 (x86)
        for (i32 i = 0; i < jo::as_morph_anim(src)->track_count; ++i) {
            u8* const d = jo::view<u8*>(jo::self<jo::Arr>(this)->tracks);
            jo::view<u32>(jo::as_morph_track(d)[i].key_count) = jo::view<u32>(jo::as_morph_anim(src)->tracks[i].key_count);
            jo::self<jo::Arr>(this)->tracks[i].keys = op_new(jo::view<u32>(jo::self<jo::Arr>(this)->tracks[i].key_count) << 3);
            std::memcpy(joan::orig::arg::mptr(jo::self<jo::Arr>(this)->tracks[i].keys), static_cast<const void*>(jo::as_morph_anim(src)->tracks[i].keys), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Arr>(this)->tracks[i].key_count) << 3));  // memcpy
        }
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Arr::copy_from", 0x005cb1d0, &H::arr_copy_from);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b103_11_2.cpp (game/b103_11)
namespace {
namespace game_b103_11_2 {


using u32 = std::uint32_t;

struct H {
    // 0x005cadf0
    H* add_3c(u32 v) {
        jo::view<u32>(jo::self<jo::Morph>(this)->vertex_base) = jo::view<u32>(jo::self<jo::Morph>(this)->vertex_base) + v;
        return this;
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Obj::add_3c", 0x005cadf0, &H::add_3c);
#undef B103

}  // namespace game_b103_11_2
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
namespace {
namespace en3d_b83_03_3 {


using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using joan::img::str;

void* const kFiles = obj(d::g_misc);   // file system helper (0x00590b80 exists)
void* const kLog = obj(d::g_err);

using joan::orig::helpers::game_new;  // operator new
using joan::orig::helpers::memset_;

struct alignas(4) Path {
    std::uint8_t bytes[0x194];
};
struct alignas(4) File {
    std::uint8_t bytes[0x210];  // 0x110 used (0x004179c0); room as in the larger frame
};

const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
std::int32_t read_int(File& file) { return joan::orig::as_file(&file)->file_get_long(); }            // 0x00587e20
void read_bytes(File& file, void* p, std::int32_t n) { joan::orig::as_file(&file)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n)); }  // 0x005873a0

JOAN_COUNTER(c_anim_load, "MorphAnim::load");

struct H {
    // 0x005cae80
    void anim_load(const char* dir, const char* name, void* part) {
        c_anim_load.hit();
        if (jo::self<jo::MorphAnim>(this)->loaded != 0) return;
        jo::self<jo::MorphAnim>(this)->loaded = 1;
        Path path;
        joan::orig::as_string(&path)->construct(joan::orig::arg::str(dir));  // 0x005a11a0
        joan::orig::as_string(&path)->append(joan::orig::arg::str(name));     // 0x005a1550 append
        joan::orig::as_string(&path)->append(str(d::s_MA));                // ".MA"
        if (joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(path))) == 0) {  // 0x00590b80
            joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_Error_loading_Morph_Animatio), c_str(path));  // 0x00586cb0
            return;
        }
        File file;
        joan::orig::as_file(&file)->construct();                            // 0x004179c0
        joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(path)), 1, static_cast<char>(0));  // 0x00586da0 open
        const std::int32_t count = read_int(file);
        jo::view<std::uint32_t>(jo::self<jo::MorphAnim>(this)->track_count) = jo::view<std::uint32_t>(jo::as_morph(part)->target_count);
        read_bytes(file, &jo::self<jo::MorphAnim>(this)->length, 4);
        jo::view<void*>(jo::self<jo::MorphAnim>(this)->tracks) = game_new(jo::view<std::uint32_t>(jo::as_morph(part)->target_count) * static_cast<std::uint32_t>(sizeof(jo::MorphTrack)));  // << 3 (x86)
        memset_(jo::view<void*>(jo::self<jo::MorphAnim>(this)->tracks), 0, jo::view<std::uint32_t>(jo::as_morph(part)->target_count) * static_cast<std::uint32_t>(sizeof(jo::MorphTrack)));
        for (std::int32_t i = 0; i < count; ++i) {
            char target_name[0x34];
            read_bytes(file, target_name, 0x32);
            const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::as_morph(part)->find(joan::orig::arg::str(static_cast<void*>(target_name))));  // 0x005c90a0
            if (r == 0) continue;
            std::uint8_t* const entry = jo::view<std::uint8_t*>(jo::self<jo::MorphAnim>(this)->tracks) + r * static_cast<std::int32_t>(sizeof(jo::MorphTrack)) - static_cast<std::int32_t>(sizeof(jo::MorphTrack));
            jo::as_morph_track(entry)->key_count = read_int(file);
            jo::as_morph_track(jo::view<std::uint8_t*>(jo::self<jo::MorphAnim>(this)->tracks) + r * static_cast<std::int32_t>(sizeof(jo::MorphTrack)) - static_cast<std::int32_t>(sizeof(jo::MorphTrack)))->keys =
                game_new(static_cast<std::uint32_t>(jo::as_morph_track(jo::view<std::uint8_t*>(jo::self<jo::MorphAnim>(this)->tracks) + r * static_cast<std::int32_t>(sizeof(jo::MorphTrack)) - static_cast<std::int32_t>(sizeof(jo::MorphTrack)))->key_count << 3));
            for (std::int32_t k = 0; k < jo::as_morph_track(jo::view<std::uint8_t*>(jo::self<jo::MorphAnim>(this)->tracks) + r * static_cast<std::int32_t>(sizeof(jo::MorphTrack)) - static_cast<std::int32_t>(sizeof(jo::MorphTrack)))->key_count; ++k) {
                read_bytes(file, jo::view<std::uint8_t*>(jo::as_morph_track(jo::view<std::uint8_t*>(jo::self<jo::MorphAnim>(this)->tracks) + r * static_cast<std::int32_t>(sizeof(jo::MorphTrack)) - static_cast<std::int32_t>(sizeof(jo::MorphTrack)))->keys) + k * 8, 4);
                read_bytes(file, jo::view<std::uint8_t*>(jo::as_morph_track(jo::view<std::uint8_t*>(jo::self<jo::MorphAnim>(this)->tracks) + r * static_cast<std::int32_t>(sizeof(jo::MorphTrack)) - static_cast<std::int32_t>(sizeof(jo::MorphTrack)))->keys) + k * 8 + 4, 4);
            }
        }
        joan::orig::as_file(&file)->destruct();  // 0x005872b0 ~File
    }

    // 0x005cae50 (eax from 0x005cb310)
    std::uint32_t anim_dtor() { joan::orig::as_table(this)->clear(); return 0; }  // 0x005cb310 (void; result unused by callers)
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("MorphAnimation::init", 0x005cae80, &H::anim_load);
B83_03("MorphAnimation::~MorphAnimation", 0x005cae50, &H::anim_dtor);
#undef B83_03

}  // namespace en3d_b83_03_3
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01_2.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01_2 {


using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;                      // operator delete

struct H {
    // 0x005cb310
    void table_clear() {
        if (jo::view<void*>(jo::self<jo::MorphAnim>(this)->tracks) != nullptr) {
            for (std::int32_t i = 0; i < jo::self<jo::MorphAnim>(this)->track_count; ++i) {
                void* const p = jo::self<jo::MorphAnim>(this)->tracks[i].keys;
                if (p != nullptr) game_delete(p);
            }
            game_delete(jo::view<void*>(jo::self<jo::MorphAnim>(this)->tracks));
            jo::view<void*>(jo::self<jo::MorphAnim>(this)->tracks) = nullptr;
        }
        jo::view<std::uint32_t>(jo::self<jo::MorphAnim>(this)->track_count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::MorphAnim>(this)->length) = 0;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Table::clear", 0x005cb310, &H::table_clear);
#undef E3D

}  // namespace en3d_b84_01_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers14.cpp (en3d/helpers15)
namespace {
namespace en3d_helpers14_2 {


using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;


std::uint32_t now() { return global<std::uint32_t>(d::g_now); }  // game clock (ms)

JOAN_COUNTER(c_morph_eval, "MorphController::evaluate");

struct H {
    // 0x005cb3d0 MorphController (+4 track count, +8 tracks {key count, keys {time, weight}},
    // +0xc length): weight per target at time t (clamped or looped), then Morph::set_weights.
    void morph_evaluate(void* morph, float t, std::int32_t looping) {
        c_morph_eval.hit();
        if (jo::as_morph(morph)->last_update == now()) return;
        const std::int32_t n = jo::self<jo::MorphController>(this)->track_count;
        std::uint8_t* const tracks = jo::view<std::uint8_t*>(jo::self<jo::MorphController>(this)->tracks);
        const float length = jo::self<jo::MorphController>(this)->length;
        float local[0x100];
        float tt;
        if (looping == 0) {
            if (t >= length) {  // past the end: every track's last key
                float* const buf = n <= 0x100 ? local : joan::orig::arg::as<float*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(n << 2))));
                for (std::int32_t i = 0; i < n; ++i) {
                    const std::uint8_t* const keys = static_cast<std::uint8_t*>(jo::as_morph_track(tracks)[i].keys);
                    std::memcpy(&buf[i], keys + jo::as_morph_track(tracks)[i].key_count * 8 - 4, 4);
                }
                joan::orig::as_morph(morph)->set_weights(joan::orig::arg::mptr(static_cast<const float*>(buf)));  // 0x005c9120
                if (buf != local) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(buf)));
                return;
            }
            tt = t;
        } else if (t >= length) {
            tt = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::fmod(joan::orig::arg::f64(static_cast<double>(t)), joan::orig::arg::f64(static_cast<double>(length)))));  // fmod
        } else {
            tt = t;
        }
        float* const buf = n <= 0x100 ? local : joan::orig::arg::as<float*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(n << 2))));
        for (std::int32_t i = 0; i < n; ++i) {
            const std::int32_t count = jo::as_morph_track(tracks)[i].key_count;
            const float* const keys = static_cast<float*>(jo::as_morph_track(tracks)[i].keys);  // {time, weight}
            std::int32_t last = -1;
            for (std::int32_t j = 0; j < count; ++j) {
                if (!(tt >= keys[j * 2])) break;
                last = j;
            }
            if (last < 0) {
                jo::view<std::uint32_t>(buf[i]) = 0;
            } else if (last == count - 1) {
                std::memcpy(&buf[i], &keys[last * 2 + 1], 4);
            } else {
                const float w = (keys[last * 2 + 2] - tt) / (keys[last * 2 + 2] - keys[last * 2]);
                buf[i] = w * keys[last * 2 + 1] + (1.0f - w) * keys[last * 2 + 3];
            }
        }
        joan::orig::as_morph(morph)->set_weights(joan::orig::arg::mptr(static_cast<const float*>(buf)));
        if (buf != local) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(buf)));
    }
};

#define HELPER15(name, addr, fn) JOAN_REPLACE("en3d/helpers15", name, addr, fn)
HELPER15("MorphController::evaluate", 0x005cb3d0, &H::morph_evaluate);
#undef HELPER15

}  // namespace en3d_helpers14_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Morph::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_03_2::H::morph_ctor>(this)); }  // 0x5c67b0
auto joan::orig::Camera::wrap_5c7b80() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers32::H::wrap_5c7b80>(this)); }  // 0x5c67f0
auto joan::orig::Morph::load(const char* a0, const char* a1, const char* a2, void* a3) -> void { joan::bind::member<&::en3d_b83_03::H::morph_load>(this, a0, a1, a2, a3); }  // 0x5c6820
auto joan::orig::Morph::copy_from(void* a0, void* a1) -> void { joan::bind::member<&::game_b106_07::H::morph_copy_from>(this, a0, a1); }  // 0x5c7370
auto joan::orig::Morph::release() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers37::H::morph_release>(this)); }  // 0x5c7b80
auto joan::orig::Morph::scalar_deleting_destructor(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17::H::morph_delete>(this, a0)); }  // 0x5c7fa0
auto joan::orig::Morph::select(const char* a0) -> void { joan::bind::member<&::en3d_b84_01::H::morph_select>(this, a0); }  // 0x5c7ff0
auto joan::orig::Morph::set_frame(std::uint32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b100_11::H::morph_set_frame>(this, a0, a1); }  // 0x5c80d0
auto joan::orig::Morph::blend_to(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_10::H::morph_blend_to>(this, a0, a1, a2)); }  // 0x5c8360
auto joan::orig::Morph::update(void* a0, void* a1) -> void { joan::bind::member<&::en3d_helpers14::H::morph_update>(this, a0, a1); }  // 0x5c8960
auto joan::orig::Morph::find(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b84_01::H::morph_find>(this, a0)); }  // 0x5c90a0
auto joan::orig::Morph::set_weights(void* a0) -> void { joan::bind::member<&::en3d_helpers14::H::morph_set_weights>(this, a0); }  // 0x5c9120
auto joan::orig::Morph::blend_to_5c9350(void* a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b103_11::H::blend_to>(this, a0, a1, a2); }  // 0x5c9350
auto joan::orig::Morph::add(const char* a0, const char* a1, void* a2) -> void { joan::bind::member<&::en3d_b85_03::H::morph_add>(this, a0, a1, a2); }  // 0x5c9b10
auto joan::orig::MorphAnim::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_03_2::H::anim_ctor>(this)); }  // 0x5cae20
auto joan::orig::MorphAnim::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b83_03_3::H::anim_dtor>(this)); }  // 0x5cae50
auto joan::orig::MorphAnim::load(const char* a0, const char* a1, void* a2) -> void { joan::bind::member<&::en3d_b83_03_3::H::anim_load>(this, a0, a1, a2); }  // 0x5cae80
auto joan::orig::MorphAnim::copy_from(void* a0, void* a1) -> void { joan::bind::member<&::game_b96_06::H::arr_copy_from>(this, a0, a1); }  // 0x5cb1d0
auto joan::orig::Table::clear() -> void { joan::bind::member<&::en3d_b84_01_2::H::table_clear>(this); }  // 0x5cb310
auto joan::orig::MorphAnim::evaluate(void* a0, float a1, std::int32_t a2) -> void { joan::bind::member<&::en3d_helpers14_2::H::morph_evaluate>(this, a0, a1, a2); }  // 0x5cb3d0
