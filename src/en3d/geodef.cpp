// SPDX-License-Identifier: GPL-3.0-or-later
// en3d/geodef.cpp: functions of the original d:\ww\en3d\geodef.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (en3d/GEODEF.CPP, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;

using joan::orig::helpers::game_new;  // operator new

using joan::orig::helpers::at;

using joan::img::str;

using joan::img::obj;

void* const kFiles = obj(d::g_misc);     // file system helper (0x00590b80 exists)

}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01 {

struct Vec3 {
    std::uint32_t v[3];
};
struct Box {  // min / max (0x0060d430 / 0x0060d490)
    std::uint32_t v[6];
};

JOAN_COUNTER(c_sphere, "Model::bound_sphere");

struct H {
    // 0x005c51b0
    void bound_sphere() {
        c_sphere.hit();
        jo::Plane* const mesh = jo::self<jo::Model>(this)->mesh;
        if (mesh->vertex_count <= 0) {
            jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[0]) = global<std::uint32_t>(d::g_pos);
            jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[1]) = global<std::uint32_t>(d::g_a69cdc);
            jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[2]) = global<std::uint32_t>(d::g_a69ce0);
            jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[3]) = 0;
            return;
        }
        std::uint32_t stride = 0;
        // The vertex buffer as a byte pointer, stepped by the vertex stride (x64: kept pointer-wide).
        const std::uint8_t* p = static_cast<const std::uint8_t*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->buffer(&stride));  // 0x005cdae0
        Box box;
        joan::orig::as_aabb(&box)->set_point(p);  // 0x0060d430
        p += stride;
        for (std::int32_t i = 1; i < jo::self<jo::Model>(this)->mesh->vertex_count; ++i, p += stride)
            joan::orig::as_aabb(&box)->extend(p);  // 0x0060d490
        Vec3 centre;
        const Vec3* const c = joan::orig::arg::as<const Vec3*>(joan::orig::as_box(&box)->centre(&centre));  // 0x0060d3a0
        jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[0]) = c->v[0];
        jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[1]) = c->v[1];
        jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[2]) = c->v[2];
        jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[3]) = 0;
        float best = 0.0f;
        p = static_cast<const std::uint8_t*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->buffer(&stride));  // 0x005cdae0
        for (std::int32_t i = 0; i < jo::self<jo::Model>(this)->mesh->vertex_count; ++i, p += stride) {
            const float* const v = reinterpret_cast<const float*>(p);
            const float dx = v[0] - jo::self<jo::Model>(this)->sphere[0];
            const float dy = v[1] - jo::self<jo::Model>(this)->sphere[1];
            const float dz = v[2] - jo::self<jo::Model>(this)->sphere[2];
            const float d = dx * dx + dy * dy + dz * dz;
            if (d > best) best = d;
        }
        jo::self<jo::Model>(this)->sphere[3] = joan::orig::arg::as<float>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(best))));  // sqrt
    }

    // 0x005c5560
    std::int32_t load_bones_file(const char* file) {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::BoneTree)));  // 0xc on i686
        void* const tree = mem != nullptr ? joan::orig::as_bone_tree(mem)->construct() : nullptr;  // 0x005b9800
        jo::view<void*>(jo::self<jo::Model>(this)->bones) = tree;
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_tree(jo::view<void*>(jo::self<jo::Model>(this)->bones))->read(joan::orig::arg::mptr(file)));  // 0x005b9af0
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("GeometryDef::plane_bounding_sphere", 0x005c51b0, &H::bound_sphere);
E3D("GeometryDef::read_hierarchy", 0x005c5560, &H::load_bones_file);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/en3d_b85_03.cpp (en3d/b85_03)
namespace {
namespace en3d_b85_03 {

struct alignas(4) Path {  // 0x194 bytes in the frames (0x005a1170 writes byte 0)
    std::uint8_t bytes[0x194];
};
static_assert(sizeof(Path) == 0x194, "Path");
void path_ctor(Path& p) { joan::orig::as_string(&p)->construct(); }               // 0x005a1170
void path_assign(Path& p, const char* s) { joan::orig::as_string(&p)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void path_append(Path& p, const char* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }  // 0x005a1550
void path_copy(Path& dst, Path& src) { joan::orig::as_string(&dst)->assign(joan::orig::as_string(&src)); }  // 0x005a1400
void path_append_path(Path& dst, Path& src) { joan::orig::as_string(&dst)->append_str(joan::orig::as_string(&src)); }  // 0x005a14f0
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
bool file_exists(Path& p) { return joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(p))) != 0; }  // 0x00590b80

// p1 = file, base (before the last '.') in p1, extension in p2.
void split_name(Path& p1, Path& p2, const char* file) {
    path_assign(p1, file);
    const std::int32_t pos = joan::orig::as_string(&p1)->at(str(d::s_dot_9d36e8));  // 0x005a13a0 "."
    const std::int32_t len = joan::orig::arg::as<std::int32_t>(joan::orig::as_name(&p1)->length());                    // 0x0045e110
    path_assign(p2, joan::orig::arg::as<const char*>(joan::orig::as_string(&p1)->right_5c57c0(joan::orig::arg::i32(len - pos))));                  // 0x005c57c0
    path_assign(p1, joan::orig::arg::as<const char*>(joan::orig::as_string(&p1)->left_5c5800(joan::orig::arg::i32(pos))));                        // 0x005c5800
}

// new 0x164 stream (0x00577be0), opened on name (0x00577c70) and started (0x00577d10).
void* new_stream() {
    void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::FileLoader)));  // 0x164 on i686
    return mem != nullptr ? joan::orig::as_file_loader(mem)->construct() : nullptr;  // 0x00577be0
}
void start_stream(void* stream, const char* name) {
    joan::orig::as_file_loader(stream)->set_path(joan::orig::arg::str(name));             // 0x00577c70
    joan::orig::as_file_loader(stream)->start(joan::orig::arg::mptr(0), 0u, joan::orig::arg::u32(-1), 0u, 0u);  // 0x00577d10
}

JOAN_COUNTER(c_begin_load, "ShadowModel::begin_load");

struct H {

    // 0x005c2370
    void begin_load(const char* file, std::uint32_t anim, std::uint32_t /*unused*/) {
        c_begin_load.hit();
        if (static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[0])) != 0) return;
        jo::view<std::uint16_t>(jo::self<jo::Model>(this)->kind) = static_cast<std::uint16_t>(anim);
        char fname[0x100];
        joan::orig::libc::_splitpath(joan::orig::arg::mptr(file), joan::orig::arg::mptr(static_cast<char*>(nullptr)), joan::orig::arg::mptr(static_cast<char*>(nullptr)), joan::orig::arg::mptr(static_cast<char*>(fname)), joan::orig::arg::mptr(static_cast<char*>(nullptr)));                                              // _splitpath
        // strncpy without a terminator into the fixed-size field, as the original does
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(&jo::view<char>(jo::self<jo::ShadowModel>(this)->bytes_0[4]))), joan::orig::arg::str(static_cast<const char*>(fname)), 0x20u);  // strncpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
        jo::self<jo::Model>(this)->u8_24 = 0;
        // Both branches of the original (word anim == 0 or not) are identical code.
        jo::self<jo::Model>(this)->stream = static_cast<jo::FileLoader*>(new_stream());
        start_stream(jo::self<jo::Model>(this)->stream, file);
        Path p1, p2, p3;
        path_ctor(p1);
        path_ctor(p2);
        path_ctor(p3);
        for (std::int32_t i = 1; i <= 4; ++i) {
            split_name(p1, p2, file);
            path_copy(p3, p1);
            path_append(p3, str(d::s_hash));         // "#"
            joan::orig::as_string(&p3)->append(joan::orig::arg::i32(i + 1));  // 0x005a15d0 append number
            path_append_path(p3, p2);
            if (file_exists(p3)) {
                jo::self<jo::Model>(this)->lod_streams[i - 1] = static_cast<jo::FileLoader*>(new_stream());
                start_stream(jo::self<jo::Model>(this)->lod_streams[i - 1], c_str(p3));
            } else {
                jo::self<jo::Model>(this)->lod_streams[i - 1] = nullptr;
            }
        }
        Path p4;
        path_ctor(p4);
        split_name(p1, p2, file);
        path_copy(p4, p1);
        path_append(p4, str(d::s_S_9d95b4));  // "#S"
        path_append_path(p4, p2);
        if (file_exists(p4)) {
            jo::self<jo::Model>(this)->shadow_stream = static_cast<jo::FileLoader*>(new_stream());
            start_stream(jo::self<jo::Model>(this)->shadow_stream, c_str(p4));
        } else {
            jo::self<jo::Model>(this)->shadow_stream = nullptr;
        }
        Path p5;
        path_ctor(p5);
        split_name(p1, p2, file);
        path_copy(p5, p1);
        path_append(p5, str(d::s_V_9d95b0));  // "#V"
        path_append(p5, str(d::s_SMO));  // ".SMO"
        if (file_exists(p5)) {
            jo::self<jo::Model>(this)->volume_stream = static_cast<jo::FileLoader*>(new_stream());
            start_stream(jo::self<jo::Model>(this)->volume_stream, c_str(p5));
        } else {
            jo::self<jo::Model>(this)->volume_stream = nullptr;
        }
        jo::self<jo::ShadowModel>(this)->bytes_0[1] = 1;
    }
};

#define B85_03(name, addr, fn) JOAN_REPLACE("en3d/b85_03", name, addr, fn)
B85_03("GeometryDef::async_init", 0x005c2370, &H::begin_load);
#undef B85_03

}  // namespace en3d_b85_03
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
namespace {
namespace en3d_b83_03 {

struct H {
    // 0x005c4e30
    std::int32_t shadow_loaded() { return jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) == 0 ? 1 : 0; }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("GeometryDef::is_async_init_finish", 0x005c4e30, &H::shadow_loaded);
#undef B83_03

}  // namespace en3d_b83_03
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::str;

using i32 = std::int32_t;

using joan::img::obj;

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
// KERNEL32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;
using joan::img::obj;
using joan::img::str;

constexpr img::Addr kSleepEx = d::iat_KERNEL32_SleepEx;

void* const kFiles = obj(d::g_misc);      // file system helper (0x00590b80 exists)
void* const kLog = obj(d::g_err);        // error log

using joan::orig::helpers::game_new;  // operator new

// String / path object (0x194 bytes on the stack, no destructor).
struct alignas(4) Path {
    std::uint8_t bytes[0x194];
};
static_assert(sizeof(Path) == 0x194);
void path_ctor(Path& p) { joan::orig::as_string(&p)->construct(); }                             // 0x005a1170
void path_assign(Path& p, const char* s) { joan::orig::as_string(&p)->assign(joan::orig::arg::str(s)); }    // 0x005a1440
void path_append(Path& p, const char* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }        // 0x005a1550
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
bool file_exists(Path& p) { return joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(p))) != 0; }  // 0x00590b80

// Stream file object (a FileMem on the stack, 0x11c bytes on i686).
struct alignas(4) File {
    std::uint8_t bytes[sizeof(jo::FileMem)];
};
static_assert(sizeof(File) == sizeof(jo::FileMem));
void file_ctor(File& f) { joan::orig::as_file_mem(&f)->construct(); }  // 0x00587f80
void file_open(File& f, const char* name) {
    joan::orig::as_file_mem(&f)->open_588000(joan::orig::arg::str(name), 1, static_cast<char>(joan::orig::arg::i32(0)));  // 0x00588000
}
void file_close(File& f) { joan::orig::as_file_mem(&f)->close(); }  // 0x005886d0
void file_dtor(File& f) { joan::orig::as_file_mem(&f)->destruct(); }                 // 0x00587fd0

JOAN_COUNTER(c_model_load, "Model::load");

struct H {

    // Shared wait for the background loader (0x005c4e30 / SleepEx(5, 1) / 0x005c3110).
    void wait_loader() {
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_shadow_model(this)->loaded()) == 0) {  // 0x005c4e30
            win_call<std::uint32_t>(kSleepEx, 5u, 1);
            joan::orig::as_shadow_model(this)->poll_load();  // 0x005c3110
        }
    }

    static void* new_mesh(std::int32_t a, std::int32_t b) {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on i686
        return mem != nullptr ? joan::orig::as_plane(mem)->construct(joan::orig::arg::u32(a), joan::orig::arg::u32(b)) : nullptr;  // 0x005cbaf0
    }
    static void* new_shadow() {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Skin)));  // 0x3e on i686
        return mem != nullptr ? joan::orig::as_skin(mem)->construct() : nullptr;  // 0x005f45f0
    }

    // p1 = file, base (before the last '.') in p1, extension in p2.
    static void split_name(Path& p1, Path& p2, const char* file) {
        path_assign(p1, file);
        const std::int32_t pos = joan::orig::as_string(&p1)->at(str(d::s_dot_9d36e8));  // 0x005a13a0 "."
        const std::int32_t len = joan::orig::arg::as<std::int32_t>(joan::orig::as_name(&p1)->length());                    // 0x0045e110
        path_assign(p2, joan::orig::arg::as<const char*>(joan::orig::as_string(&p1)->right_5c57c0(joan::orig::arg::i32(len - pos))));                  // 0x005c57c0
        path_assign(p1, joan::orig::arg::as<const char*>(joan::orig::as_string(&p1)->left_5c5800(joan::orig::arg::i32(pos))));                        // 0x005c5800
    }
    static void path_copy(Path& dst, Path& src) { joan::orig::as_string(&dst)->assign(joan::orig::as_string(&src)); }  // 0x005a1400
    static void path_append_path(Path& dst, Path& src) { joan::orig::as_string(&dst)->append_str(joan::orig::as_string(&src)); }  // 0x005a14f0

    // +0x40 when > 1000.0f (0x9cf778), else 1000.0f (bits copied).
    std::uint32_t lod_range() {
        if (jo::self<jo::Model>(this)->sphere[3] > 1000.0f) return jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[3]);
        return 0x447a0000u;
    }

    static void shadow_warning(Path& p) {
        using Fn = void(JOAN_CDECL*)(void*, const char*, Path);
        reinterpret_cast<Fn>(fn::code_value(fn::Error_msg))(kLog, str(d::s_Shadow_volume_model_1_does_n), p);  // 0x00586bc0 "Shadow volume model %1 does not have exactly 1 material"
    }
    static void shadow_materials(void* s) {
        for (std::int32_t j = 0; j < jo::as_plane(s)->material_count; ++j) {
            jo::as_plane(s)->materials[j].blend = 0xd;
            jo::as_plane(s)->materials[j].texture_mode = 1;
            for (std::int32_t k = 0; k < 4; ++k)
                joan::orig::as_material(jo::as_plane(s)->materials + j)->set_texture(joan::orig::arg::i32(k), joan::orig::arg::mptr(0));  // 0x005d5650
        }
    }

    // 0x005c0ef0
    void model_load(const char* file, std::uint32_t anim, std::uint32_t flag) {
        c_model_load.hit();
        wait_loader();
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Model>(this)->loaded)) != 0) return;
        jo::view<std::uint16_t>(jo::self<jo::Model>(this)->kind) = static_cast<std::uint16_t>(anim);
        char fname[0x100];
        joan::orig::libc::_splitpath(joan::orig::arg::mptr(file), joan::orig::arg::mptr(static_cast<char*>(nullptr)), joan::orig::arg::mptr(static_cast<char*>(nullptr)), joan::orig::arg::mptr(static_cast<char*>(fname)), joan::orig::arg::mptr(static_cast<char*>(nullptr)));                                    // _splitpath
        // false positive: Model::u8_4 is the first byte of the 32-byte name field at +4 in the original layout
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(&jo::view<char>(jo::self<jo::Model>(this)->u8_4))), joan::orig::arg::str(static_cast<const char*>(fname)), 0x20u);  // strncpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
        jo::self<jo::Model>(this)->u8_24 = 0;
        const std::uint8_t b_flag = static_cast<std::uint8_t>(flag);
        if (static_cast<std::int16_t>(anim) == 0) {
            jo::view<void*>(jo::self<jo::Model>(this)->mesh) = new_mesh(1, 0);
            File file1;
            file_ctor(file1);
            file_open(file1, file);
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->load_5ddd70(&file1, static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x005ddd70
            jo::self<jo::Model>(this)->mesh->load_state = 0;
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->upload();  // 0x005cfa10
            file_close(file1);
            Path p1, p2, p3;
            path_ctor(p1);
            path_ctor(p2);
            path_ctor(p3);
            std::int32_t count = 0;
            for (std::int32_t i = 1; i <= 4; ++i) {
                split_name(p1, p2, file);
                path_copy(p3, p1);
                path_append(p3, str(d::s_hash));                    // "#"
                joan::orig::as_string(&p3)->append(joan::orig::arg::i32(i + 1));  // 0x005a15d0
                path_append_path(p3, p2);
                if (file_exists(p3)) {
                    file_open(file1, c_str(p3));
                    void* const lod = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->add_child(joan::orig::arg::i32(i), joan::orig::arg::mptr(0)));  // 0x005d4470
                    joan::orig::as_plane(lod)->load_5ddd70(&file1, static_cast<std::uint8_t>(joan::orig::arg::u32(0)));                                 // 0x005ddd70
                    joan::orig::as_plane(lod)->share_bones(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Model>(this)->mesh)));                            // 0x005d4a00
                    jo::as_plane(lod)->load_state = 0;
                    joan::orig::as_plane(lod)->upload();  // 0x005cfa10
                    file_close(file1);
                    ++count;
                }
            }
            joan::orig::as_model(this)->bound_sphere();  // 0x005c51b0
            jo::self<jo::Model>(this)->has_sphere = 1;
            const std::uint32_t range = lod_range();
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->set_lod(joan::orig::arg::u32(count), joan::orig::arg::u32(range));  // 0x005d4720
            Path p4;
            path_ctor(p4);
            split_name(p1, p2, file);
            path_copy(p4, p1);
            path_append(p4, str(d::s_S_9d95b4));  // "#S"
            path_append_path(p4, p2);
            if (file_exists(p4)) {
                void* const m = new_mesh(1, 1);
                File file2;
                file_ctor(file2);
                file_open(file2, c_str(p4));
                joan::orig::as_plane(m)->load_5ddd70(&file2, static_cast<std::uint8_t>(joan::orig::arg::u32(0)));       // 0x005ddd70
                joan::orig::as_plane(m)->upload();                  // 0x005cfa10
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->set_ec(joan::orig::arg::mptr(m));  // 0x005d7310
                joan::orig::as_plane(m)->share_bones(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Model>(this)->mesh)));  // 0x005d4a00
                file_close(file2);
                file_dtor(file2);
            }
            Path p5;
            path_ctor(p5);
            split_name(p1, p2, file);
            path_copy(p5, p1);
            path_append(p5, str(d::s_V_9d95b0));  // "#V"
            path_append(p5, str(d::s_SMO));  // ".SMO"
            if (file_exists(p5)) {
                void* const s = new_mesh(1, 0);
                File file3;
                file_ctor(file3);
                file_open(file3, c_str(p5));
                joan::orig::as_plane(s)->load_5ddd70(&file3, static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x005ddd70
                if (jo::view<std::uint32_t>(jo::as_plane(s)->material_count) != 1) shadow_warning(p5);
                shadow_materials(s);
                joan::orig::as_plane(s)->invalidate(1u);   // 0x005cf920
                joan::orig::as_plane(s)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
                jo::as_plane(s)->load_state = 0;
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->link(joan::orig::arg::mptr(s));  // 0x005d73b0
                file_close(file3);
                file_dtor(file3);
            }
            file_dtor(file1);
        } else {
            jo::view<void*>(jo::self<jo::Model>(this)->mesh) = new_mesh(1, 0);
            jo::view<void*>(jo::self<jo::Model>(this)->skin) = new_shadow();
            File file1;
            file_ctor(file1);
            file_open(file1, file);
            joan::orig::as_mesh(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->load_any(&file1, joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Model>(this)->skin)), joan::orig::arg::u32(b_flag), joan::orig::arg::u32(static_cast<std::uint8_t>(0)));  // 0x005da280
            jo::self<jo::Model>(this)->mesh->load_state = 0;
            if (static_cast<std::int32_t>(jo::self<jo::Model>(this)->mesh->vertex_format) != 7) jo::self<jo::Model>(this)->mesh->buffer_mode = 1;
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->upload();  // 0x005cfa10
            file_close(file1);
            Path p1, p2, p3;
            path_ctor(p1);
            path_ctor(p2);
            path_ctor(p3);
            std::int32_t count = 0;
            for (std::int32_t i = 1; i <= 4; ++i) {
                split_name(p1, p2, file);
                path_copy(p3, p1);
                path_append(p3, str(d::s_hash));         // "#"
                joan::orig::as_string(&p3)->append(joan::orig::arg::i32(i + 1));  // 0x005a15d0
                path_append_path(p3, p2);
                if (file_exists(p3)) {
                    file_open(file1, c_str(p3));
                    void* const lod = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->add_child(joan::orig::arg::i32(i), joan::orig::arg::mptr(0)));     // 0x005d4470
                    void* const shadow = joan::orig::arg::as<void*>(joan::orig::as_skin(jo::view<void*>(jo::self<jo::Model>(this)->skin))->add_lod(joan::orig::arg::i32(i), joan::orig::arg::mptr(0)));  // 0x005f4dc0
                    joan::orig::as_mesh(lod)->load_any(&file1, joan::orig::arg::mptr(shadow), joan::orig::arg::u32(b_flag), joan::orig::arg::u32(static_cast<std::uint8_t>(0)));  // 0x005da280
                    if (static_cast<std::int32_t>(jo::as_plane(lod)->vertex_format) != 7) jo::as_plane(lod)->buffer_mode = 1;
                    joan::orig::as_plane(lod)->share_bones(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Model>(this)->mesh)));  // 0x005d4a00
                    joan::orig::as_plane(lod)->upload();                  // 0x005cfa10
                    jo::as_plane(lod)->load_state = 0;
                    file_close(file1);
                    ++count;
                }
            }
            joan::orig::as_model(this)->copy_sphere();  // 0x005c53b0
            jo::self<jo::Model>(this)->has_sphere = 0;
            const std::uint32_t range = lod_range();
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->set_lod(joan::orig::arg::u32(count), joan::orig::arg::u32(range));  // 0x005d4720
            joan::orig::as_skin(jo::view<void*>(jo::self<jo::Model>(this)->skin))->set_lod_count(static_cast<std::uint8_t>(joan::orig::arg::u32(count)));         // 0x005f4fe0
            Path p4;
            path_ctor(p4);
            split_name(p1, p2, file);
            path_copy(p4, p1);
            path_append(p4, str(d::s_S_9d95b4));  // "#S"
            path_append_path(p4, p2);
            if (file_exists(p4)) {
                void* const m = new_mesh(1, 1);
                void* const shadow = new_shadow();
                File file2;
                file_ctor(file2);
                file_open(file2, c_str(p4));
                joan::orig::as_mesh(m)->load_any(&file2, joan::orig::arg::mptr(shadow), joan::orig::arg::u32(static_cast<std::uint8_t>(0)), joan::orig::arg::u32(static_cast<std::uint8_t>(0)));  // 0x005da280
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->set_ec(joan::orig::arg::mptr(m));       // 0x005d7310
                joan::orig::as_skin(jo::view<void*>(jo::self<jo::Model>(this)->skin))->link_36(joan::orig::arg::mptr(shadow));  // 0x005f5090
                if (static_cast<std::int32_t>(jo::as_plane(m)->vertex_format) != 7) jo::as_plane(m)->buffer_mode = 1;
                joan::orig::as_plane(m)->share_bones(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Model>(this)->mesh)));  // 0x005d4a00
                joan::orig::as_plane(m)->upload();                  // 0x005cfa10
                jo::as_plane(m)->load_state = 0;
                file_close(file2);
                file_dtor(file2);
            }
            Path p5;
            path_ctor(p5);
            split_name(p1, p2, file);
            path_copy(p5, p1);
            path_append(p5, str(d::s_V_9d95b0));  // "#V"
            path_append(p5, str(d::s_SMO));  // ".SMO"
            if (file_exists(p5)) {
                void* const s = new_mesh(1, 0);
                File file3;
                file_ctor(file3);
                file_open(file3, c_str(p5));
                joan::orig::as_plane(s)->load_5ddd70(&file3, static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x005ddd70
                if (jo::view<std::uint32_t>(jo::as_plane(s)->material_count) != 1) shadow_warning(p5);
                shadow_materials(s);
                joan::orig::as_plane(s)->invalidate(1u);  // 0x005cf920
                joan::orig::as_plane(s)->upload();     // 0x005cfa10
                jo::as_plane(s)->load_state = 0;
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->link(joan::orig::arg::mptr(s));  // 0x005d73b0
                file_close(file3);
                file_dtor(file3);
            }
            file_dtor(file1);
        }
        jo::view<std::uint8_t>(jo::self<jo::Model>(this)->loaded) = 1;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("GeometryDef::init", 0x005c0ef0, &H::model_load);
#undef E3D

}  // namespace en3d_b84_01_2
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01_3 {

struct H {
    // 0x005c4e60 (eax = this when loaded, else +0x28)
    void* model_create(void* mesh, const std::uint32_t* sphere) {
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Model>(this)->loaded)) != 0) return this;
        jo::view<std::uint16_t>(jo::self<jo::Model>(this)->kind) = 0;
        jo::self<jo::Model>(this)->u8_4 = 0;
        jo::view<void*>(jo::self<jo::Model>(this)->mesh) = mesh;
        if (sphere != nullptr) {
            jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[0]) = sphere[0];
            jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[1]) = sphere[1];
            jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[2]) = sphere[2];
            jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[3]) = sphere[3];
        } else {
            joan::orig::as_model(this)->bound_sphere();  // 0x005c51b0
        }
        jo::self<jo::Model>(this)->has_sphere = 1;
        jo::self<jo::Model>(this)->mesh->load_state = 0;
        jo::view<std::uint8_t>(jo::self<jo::Model>(this)->loaded) = 1;
        return jo::view<void*>(jo::self<jo::Model>(this)->mesh);
    }

    // 0x005c53b0
    void copy_sphere() {
        const jo::Skin* const skin = jo::self<jo::Model>(this)->skin;
        jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[0]) = skin->center[0];
        jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[1]) = skin->center[1];
        jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[2]) = skin->center[2];
        jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[3]) = jo::view<std::uint32_t>(skin->radius);
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("GeometryDef::init_5c4e60", 0x005c4e60, &H::model_create);
E3D("GeometryDef::skin_bounding_sphere", 0x005c53b0, &H::copy_sphere);
#undef E3D

}  // namespace en3d_b84_01_3
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Model::load(const char* a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::en3d_b84_01_2::H::model_load>(this, a0, a1, a2); }  // 0x5c0ef0
auto joan::orig::ShadowModel::begin_load(const char* a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::en3d_b85_03::H::begin_load>(this, a0, a1, a2); }  // 0x5c2370
auto joan::orig::ShadowModel::loaded() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b83_03::H::shadow_loaded>(this)); }  // 0x5c4e30
auto joan::orig::Model::bound_sphere() -> void { joan::bind::member<&::en3d_b84_01::H::bound_sphere>(this); }  // 0x5c51b0
auto joan::orig::Model::load_bones_file(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b84_01::H::load_bones_file>(this, a0)); }  // 0x5c5560
auto joan::orig::Model::create(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b84_01_3::H::model_create>(this, a0, a1)); }  // 0x5c4e60
auto joan::orig::Model::copy_sphere() -> void { joan::bind::member<&::en3d_b84_01_3::H::copy_sphere>(this); }  // 0x5c53b0
