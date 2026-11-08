// game/unknown_mdl.cpp: functions whose original source file is unknown (formerly game/_unplaced/mdl.cpp;
// re/attribution.csv: between ww/en3d/geodef.cpp and ww/en3d/geodef.cpp). They move to their original file when
// evidence names it (Phase 3).
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

// KERNEL32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;

constexpr img::Addr kSleepEx = d::iat_KERNEL32_SleepEx;

void game_memset(void* p, std::int32_t v, std::uint32_t n) { std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

JOAN_COUNTER(c_unload, "Model::unload");

struct H {
    // Shared wait for the background loader (0x005c4e30 / SleepEx(5, 1) / 0x005c3110).
    void wait_loader() {
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_shadow_model(this)->loaded()) == 0) {  // 0x005c4e30
            win_call<std::uint32_t>(kSleepEx, 5u, 1);
            joan::orig::as_shadow_model(this)->poll_load();  // 0x005c3110
        }
    }

    // 0x005c4f10 (eax = this)
    void* unload() {
        c_unload.hit();
        wait_loader();
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Model>(this)->loaded)) == 0) return this;
        jo::self<jo::Model>(this)->u32_48 = 0;
        if (jo::view<void*>(jo::self<jo::Model>(this)->bones) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Model>(this)->bones);
            if (p != nullptr) joan::orig::as_bone_tree(p)->destroy(1u);  // 0x005b46f0
            jo::view<void*>(jo::self<jo::Model>(this)->bones) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Model>(this)->skin) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Model>(this)->skin);
            if (p != nullptr) joan::orig::as_skin(p)->destroy(1u);  // 0x005c5060
            jo::view<void*>(jo::self<jo::Model>(this)->skin) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Model>(this)->mesh) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Model>(this)->mesh);
            if (p != nullptr) joan::orig::as_plane(p)->destroy(1u);  // 0x00459c40
            jo::view<void*>(jo::self<jo::Model>(this)->mesh) = nullptr;
        }
        jo::view<std::uint8_t>(jo::self<jo::Model>(this)->loaded) = 0;
        return this;
    }

    // 0x005c0ec0 / 0x005e5540 / 0x005f45f0
    void* ctor_69() {
        game_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::Model)));  // 0x69 bytes on i686
        return this;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Model::unload", 0x005c4f10, &H::unload);
E3D("Model::Model", 0x005c0ec0, &H::ctor_69);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
namespace {
namespace en3d_b83_03 {

void* const kLog = obj(d::g_err);

struct alignas(4) Reader {  // a FileMem on the stack (0x11c bytes on i686)
    std::uint8_t bytes[sizeof(jo::FileMem)];
};

JOAN_COUNTER(c_poll_load, "ShadowModel::poll_load");

struct H {

    // Stream slot s: 0x00578110 then scalar delete (0x005afd50) of the re-read pointer, = 0.
    static void release_stream(jo::FileLoader*& s) {
        joan::orig::as_file_loader(s)->stop();  // 0x00578110
        void* const p = s;
        if (p != nullptr) joan::orig::as_file_loader(p)->scalar_deleting_destructor(1u);  // 0x005afd50
        s = nullptr;
    }
    void release_all() {
        if (jo::self<jo::Model>(this)->volume_stream != nullptr) release_stream(jo::self<jo::Model>(this)->volume_stream);
        if (jo::self<jo::Model>(this)->shadow_stream != nullptr) release_stream(jo::self<jo::Model>(this)->shadow_stream);
        for (std::int32_t i = 1; i <= 4; ++i)
            if (jo::self<jo::Model>(this)->lod_streams[i - 1] != nullptr) release_stream(jo::self<jo::Model>(this)->lod_streams[i - 1]);
    }
    static std::int32_t stream_ready(jo::FileLoader* s) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(s)->done()); }  // 0x00578250
    static std::int32_t stream_has_data(jo::FileLoader* s) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(s)->ok()); }  // 0x005b0a80
    static void open_reader(Reader& reader, jo::FileLoader* s) {
        joan::orig::as_file_mem(&reader)->construct();  // 0x00587f80
        joan::orig::as_file_mem(&reader)->open_mem_5881c0(joan::orig::arg::str(jo::as_file_loader(s)->buffer), joan::orig::arg::i32(jo::as_file_loader(s)->bytes_read), static_cast<char>(joan::orig::arg::i32(1u)), static_cast<char>(joan::orig::arg::i32(0u)));  // 0x005881c0
    }
    static void* new_model(std::uint32_t a, std::uint32_t b) {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on i686
        return mem != nullptr ? joan::orig::as_plane(mem)->construct(joan::orig::arg::u32(a), joan::orig::arg::u32(b)) : nullptr;  // 0x005cbaf0
    }
    static void* new_shadow() {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Skin)));  // 0x3e on i686
        return mem != nullptr ? joan::orig::as_skin(mem)->construct() : nullptr;  // 0x005f45f0
    }
    // fld [+0x40] / fcomp [0x9cf778] / test ah,0x41 / jne -> 1000.0f (0x447a0000).
    std::uint32_t lod_distance_bits() {
        return jo::self<jo::Model>(this)->sphere[3] > global<float>(d::flt_1000) ? jo::view<std::uint32_t>(jo::self<jo::Model>(this)->sphere[3]) : 0x447a0000u;
    }
    std::int32_t count_lods() {
        std::int32_t n = 0;
        for (std::int32_t i = 1; i <= 4; ++i)
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->level(joan::orig::arg::mptr(static_cast<void*>(&i)), 0u)) != 0) ++n;  // 0x005d45e0
        return n;
    }
    // +0x60 / +0x64 states: absent or finished without data -> next state.
    static bool stream_skip(jo::FileLoader* s) {
        if (s == nullptr) return true;
        return stream_ready(s) != 0 && stream_has_data(s) == 0;
    }
    static bool stream_loadable(jo::FileLoader* s) { return stream_ready(s) != 0 && stream_has_data(s) != 0; }

    // State 4 (both variants): the shadow volume on +0x64.
    void load_shadow_volume(std::uint8_t flag14) {
        if (stream_skip(jo::self<jo::Model>(this)->volume_stream)) {
            jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 5;
            return;
        }
        if (!stream_loadable(jo::self<jo::Model>(this)->volume_stream)) return;
        void* const model = new_model(1u, 0u);
        Reader reader;
        open_reader(reader, jo::self<jo::Model>(this)->volume_stream);
        joan::orig::as_plane(model)->load_5ddd70(joan::orig::arg::mptr(static_cast<void*>(&reader)), static_cast<std::uint8_t>(joan::orig::arg::u32(flag14)));  // 0x005ddd70
        if (jo::as_plane(model)->material_count != 1)
            joan::orig::warning_box(joan::orig::arg::mptr(kLog), str(d::s_Shadow_volume_model_1_does_n), static_cast<void*>(jo::self<jo::ShadowModel>(this)->bytes_0 + 4));  // 0x00586bc0
        for (std::int32_t m = 0; m < jo::as_plane(model)->material_count; ++m) {
            jo::as_plane(model)->materials[m].blend = 0xd;
            jo::as_plane(model)->materials[m].texture_mode = 1;
            for (std::int32_t k = 0; k < 4; ++k)
                joan::orig::as_material(jo::as_plane(model)->materials + m)->set_texture(joan::orig::arg::i32(k), joan::orig::arg::mptr(0u));  // 0x005d5650
        }
        joan::orig::as_plane(model)->invalidate(1u);   // 0x005cf920
        joan::orig::as_plane(model)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        jo::as_plane(model)->load_state = 0;
        joan::orig::as_plane(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->link(joan::orig::arg::mptr(model));  // 0x005d73b0
        joan::orig::as_file_mem(&reader)->close();   // 0x005886d0
        jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 5;
        joan::orig::as_file_mem(&reader)->destruct();  // 0x00587fd0 ~Reader
    }

    // 0x005c3110
    void poll_load() {
        c_poll_load.hit();
        if (jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) == 0) return;
        const std::uint8_t flag14 = 0;  // [ebp-0x14], never changed
        const std::int32_t state = jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]);
        if (jo::self<jo::Model>(this)->kind == 0) {
            if (state == 1) {
                if (stream_ready(jo::self<jo::Model>(this)->stream) == 0) return;
                if (stream_has_data(jo::self<jo::Model>(this)->stream) == 0) {
                    release_all();
                    jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 0;
                    return;
                }
                jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28) = new_model(1u, 0u);
                Reader reader;
                open_reader(reader, jo::self<jo::Model>(this)->stream);
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->load_5ddd70(joan::orig::arg::mptr(static_cast<void*>(&reader)), static_cast<std::uint8_t>(joan::orig::arg::u32(flag14)));  // 0x005ddd70
                jo::self<jo::ShadowModel>(this)->ptr_28->load_state = 0;
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->upload();  // 0x005cfa10
                joan::orig::as_file_mem(&reader)->close();  // 0x005886d0
                jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 2;
                joan::orig::as_file_mem(&reader)->destruct();  // ~Reader
            } else if (state == 2) {
                std::int32_t pending = 0;
                for (std::int32_t i = 1; i <= 4; ++i) {
                    jo::FileLoader*& off = jo::self<jo::Model>(this)->lod_streams[i - 1];
                    if (off == nullptr) continue;
                    if (stream_ready(off) == 0) {
                        ++pending;
                        continue;
                    }
                    if (stream_has_data(off) == 0) {
                        release_stream(off);
                        continue;
                    }
                    Reader reader;
                    open_reader(reader, off);
                    void* const lod = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->add_child(joan::orig::arg::i32(i), joan::orig::arg::mptr(0u)));  // 0x005d4470
                    joan::orig::as_plane(lod)->load_5ddd70(joan::orig::arg::mptr(static_cast<void*>(&reader)), static_cast<std::uint8_t>(joan::orig::arg::u32(flag14)));         // 0x005ddd70
                    joan::orig::as_plane(lod)->share_bones(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28)));                              // 0x005d4a00
                    jo::as_plane(lod)->load_state = 0;
                    joan::orig::as_plane(lod)->upload();      // 0x005cfa10
                    joan::orig::as_file_mem(&reader)->close();  // 0x005886d0
                    release_stream(off);
                    joan::orig::as_file_mem(&reader)->destruct();  // ~Reader
                }
                if (pending != 0) return;
                joan::orig::as_model(this)->bound_sphere();  // 0x005c51b0
                jo::self<jo::ShadowModel>(this)->u32_30 = 1;
                const std::int32_t n = count_lods();
                const std::uint32_t distance = lod_distance_bits();
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->set_lod(joan::orig::arg::u32(n), joan::orig::arg::u32(distance));  // 0x005d4720
                jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 3;
            } else if (state == 3) {
                if (stream_skip(jo::self<jo::Model>(this)->shadow_stream)) {
                    jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 4;
                    return;
                }
                if (!stream_loadable(jo::self<jo::Model>(this)->shadow_stream)) return;
                void* const model = new_model(1u, 1u);
                Reader reader;
                open_reader(reader, jo::self<jo::Model>(this)->shadow_stream);
                joan::orig::as_plane(model)->load_5ddd70(joan::orig::arg::mptr(static_cast<void*>(&reader)), static_cast<std::uint8_t>(joan::orig::arg::u32(flag14)));  // 0x005ddd70
                joan::orig::as_plane(model)->upload();                                      // 0x005cfa10
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->set_ec(joan::orig::arg::mptr(model));                      // 0x005d7310
                joan::orig::as_plane(model)->share_bones(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28)));                      // 0x005d4a00
                joan::orig::as_file_mem(&reader)->close();                       // 0x005886d0
                jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 4;
                joan::orig::as_file_mem(&reader)->destruct();  // ~Reader
            } else if (state == 4) {
                load_shadow_volume(flag14);
            } else if (state == 5) {
                release_all();
                jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 0;
                jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[0]) = 1;
            }
            return;
        }
        if (state == 1) {
            if (stream_ready(jo::self<jo::Model>(this)->stream) == 0) return;
            if (stream_has_data(jo::self<jo::Model>(this)->stream) == 0) {
                release_all();
                jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 0;
                return;
            }
            jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28) = new_model(1u, 0u);
            jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_2c) = new_shadow();
            Reader reader;
            open_reader(reader, jo::self<jo::Model>(this)->stream);
            joan::orig::as_mesh(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->load_any(joan::orig::arg::mptr(static_cast<void*>(&reader)), joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_2c)), joan::orig::arg::u32(jo::self<jo::Model>(this)->load_flag), joan::orig::arg::u32(static_cast<std::uint8_t>(1)));  // 0x005da280
            jo::self<jo::ShadowModel>(this)->ptr_28->load_state = 0;
            if (jo::self<jo::ShadowModel>(this)->ptr_28->vertex_format != 7) jo::self<jo::ShadowModel>(this)->ptr_28->buffer_mode = 1;
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->upload();  // 0x005cfa10
            joan::orig::as_file_mem(&reader)->close();  // 0x005886d0
            jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 2;
            joan::orig::as_file_mem(&reader)->destruct();  // ~Reader
        } else if (state == 2) {
            std::int32_t pending = 0;
            for (std::int32_t i = 1; i <= 4; ++i) {
                jo::FileLoader*& off = jo::self<jo::Model>(this)->lod_streams[i - 1];
                if (off == nullptr) continue;
                if (stream_ready(off) == 0) {
                    ++pending;
                    continue;
                }
                if (stream_has_data(off) == 0) {
                    release_stream(off);
                    continue;
                }
                Reader reader;
                open_reader(reader, off);
                void* const lod = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->add_child(joan::orig::arg::i32(i), joan::orig::arg::mptr(0u)));     // 0x005d4470
                void* const shadow = joan::orig::arg::as<void*>(joan::orig::as_skin(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_2c))->add_lod(joan::orig::arg::i32(i), joan::orig::arg::mptr(0u)));  // 0x005f4dc0
                joan::orig::as_mesh(lod)->load_any(joan::orig::arg::mptr(static_cast<void*>(&reader)), joan::orig::arg::mptr(shadow), joan::orig::arg::u32(jo::self<jo::Model>(this)->load_flag), joan::orig::arg::u32(static_cast<std::uint8_t>(1)));  // 0x005da280
                if (jo::as_plane(lod)->vertex_format != 7) jo::as_plane(lod)->buffer_mode = 1;
                joan::orig::as_plane(lod)->share_bones(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28)));  // 0x005d4a00
                jo::as_plane(lod)->load_state = 0;
                joan::orig::as_plane(lod)->upload();      // 0x005cfa10
                joan::orig::as_file_mem(&reader)->close();  // 0x005886d0
                release_stream(off);
                joan::orig::as_file_mem(&reader)->destruct();  // ~Reader
            }
            if (pending != 0) return;
            joan::orig::as_model(this)->copy_sphere();  // 0x005c53b0
            jo::self<jo::ShadowModel>(this)->u32_30 = 0;
            const std::int32_t n = count_lods();
            const std::uint32_t distance = lod_distance_bits();
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->set_lod(joan::orig::arg::u32(n), joan::orig::arg::u32(distance));  // 0x005d4720
            joan::orig::as_skin(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_2c))->set_lod_count(static_cast<std::uint8_t>(joan::orig::arg::u32(n)));            // 0x005f4fe0
            jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 3;
        } else if (state == 3) {
            if (stream_skip(jo::self<jo::Model>(this)->shadow_stream)) {
                jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 4;
                return;
            }
            if (!stream_loadable(jo::self<jo::Model>(this)->shadow_stream)) return;
            void* const model = new_model(1u, 1u);
            void* const shadow = new_shadow();
            Reader reader;
            open_reader(reader, jo::self<jo::Model>(this)->shadow_stream);
            joan::orig::as_mesh(model)->load_any(joan::orig::arg::mptr(static_cast<void*>(&reader)), joan::orig::arg::mptr(shadow), joan::orig::arg::u32(static_cast<std::uint8_t>(0)), joan::orig::arg::u32(static_cast<std::uint8_t>(1)));  // 0x005da280
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28))->set_ec(joan::orig::arg::mptr(model));   // 0x005d7310
            joan::orig::as_skin(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_2c))->link_36(joan::orig::arg::mptr(shadow));  // 0x005f5090
            if (jo::as_plane(model)->vertex_format != 7) jo::as_plane(model)->buffer_mode = 1;
            joan::orig::as_plane(model)->share_bones(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ShadowModel>(this)->ptr_28)));  // 0x005d4a00
            joan::orig::as_plane(model)->upload();                  // 0x005cfa10
            jo::as_plane(model)->load_state = 0;
            joan::orig::as_file_mem(&reader)->close();  // 0x005886d0
            jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 4;
            joan::orig::as_file_mem(&reader)->destruct();  // ~Reader
        } else if (state == 4) {
            load_shadow_volume(flag14);
        } else if (state == 5) {
            release_all();
            jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[1]) = 0;
            jo::view<std::int8_t>(jo::self<jo::ShadowModel>(this)->bytes_0[0]) = 1;
        }
    }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("ShadowModel::poll_load", 0x005c3110, &H::poll_load);
#undef B83_03

}  // namespace en3d_b83_03
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04 {

using joan::orig::helpers::game_delete;                      // operator delete

// Stream file object (a FileMem on the stack, 0x11c bytes on i686).
struct alignas(4) File {
    std::uint8_t bytes[sizeof(jo::FileMem)];
};
static_assert(sizeof(File) == sizeof(jo::FileMem));

JOAN_COUNTER(c_load_bones, "Model::load_bones(path)");

struct H {
    // 0x005c5400
    std::int32_t load_bones_path(const char* path) {
        c_load_bones.hit();
        if (joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(path)) == 0) return 0;  // 0x00590b80
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::BoneTree)));  // 0xc on i686
        void* const tree = mem != nullptr ? joan::orig::as_bone_tree(mem)->construct() : nullptr;  // 0x005b9800
        jo::view<void*>(jo::self<jo::Model>(this)->bones) = tree;
        File file;
        joan::orig::as_file_mem(&file)->construct();                                         // 0x00587f80
        joan::orig::as_file_mem(&file)->open_588000(joan::orig::arg::str(path), 1, static_cast<char>(joan::orig::arg::i32(0)));                     // 0x00588000
        const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_tree(jo::view<void*>(jo::self<jo::Model>(this)->bones))->read(&file));  // 0x005b9af0
        joan::orig::as_file_mem(&file)->close();                                  // 0x005886d0
        joan::orig::as_file_mem(&file)->destruct();                                                // 0x00587fd0
        return r;
    }

    // 0x005c5060
    H* delete_5c5060(std::uint32_t flags) {
        joan::orig::as_skin(this)->reset();  // 0x005f4620
        if ((flags & 1u) != 0) game_delete(this);
        return this;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("Model::load_bones(path)", 0x005c5400, &H::load_bones_path);
E3D("ShadowEntry::scalar_delete", 0x005c5060, &H::delete_5c5060);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16 {

using u8 = std::uint8_t;

using u32 = std::uint32_t;

struct H {
    // ---------------------------------------------------------------- stubs / getters
    void stub0() {}
    u32 zero0() { return 0; }
    u32 zero1(u32) { return 0; }
    u8 true0() { return 1; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Obj::true_5c50b0", 0x005c50b0, &H::true0);
B("Obj::stub_5c50d0", 0x005c50d0, &H::stub0);
B("Obj::false_5c50f0", 0x005c50f0, &H::zero0);
B("Obj::false_5c5110", 0x005c5110, &H::zero0);
B("Obj::false_5c5130", 0x005c5130, &H::zero1);
B("Obj::false_5c5150", 0x005c5150, &H::zero1);
B("Obj::false_5c5170", 0x005c5170, &H::zero1);
B("Obj::false_5c5190", 0x005c5190, &H::zero1);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01_3 {


JOAN_COUNTER(c_link_bones, "Model::link_bones");

struct H {
    // 0x005c56d0
    std::int32_t link_bones() {
        c_link_bones.hit();
        if (jo::self<jo::Model>(this)->u32_48 != 0) return 1;
        for (std::int32_t i = 0; i < jo::self<jo::Model>(this)->bones->count; ++i) {
            const std::int32_t parent = joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_tree(jo::view<void*>(jo::self<jo::Model>(this)->bones))->parent(joan::orig::arg::i32(i)));  // 0x005b9fe0
            if (parent == -1 || parent == -999) {
                void* const bone = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->bone_5cf8f0(joan::orig::arg::i32(i)));  // 0x005cf8f0
                joan::orig::as_bone(bone)->set_parent(joan::orig::arg::mptr(static_cast<void*>(nullptr)));            // 0x0060ec70
            } else {
                void* const pb = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->bone_5cf8f0(joan::orig::arg::i32(parent)));  // 0x005cf8f0
                void* const bone = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Model>(this)->mesh))->bone_5cf8f0(joan::orig::arg::i32(i)));    // 0x005cf8f0
                joan::orig::as_bone(bone)->set_parent(joan::orig::arg::mptr(pb));                                        // 0x0060ec70
            }
        }
        jo::self<jo::Model>(this)->u32_48 = 1;
        return 1;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Model::link_bones", 0x005c56d0, &H::link_bones);
#undef E3D

}  // namespace en3d_b84_01_3
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04_2 {

using i32 = std::int32_t;

struct H {
    // 0x005c57c0
    const char* str_right(std::int32_t n) {
        const std::int32_t len = joan::orig::arg::as<std::int32_t>(joan::orig::as_name(this)->length());  // 0x0045e110
        return joan::orig::arg::as<const char*>(joan::orig::as_string(this)->substr(joan::orig::arg::i32(len - n), 0));  // 0x005a1230
    }

    // 0x005c5800
    const char* str_left(std::int32_t n) {
        return joan::orig::arg::as<const char*>(joan::orig::as_string(this)->substr(0, joan::orig::arg::i32(n)));  // 0x005a1230
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("String::right", 0x005c57c0, &H::str_right);
E3D("String::left", 0x005c5800, &H::str_left);
#undef E3D

}  // namespace en3d_b85_04_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Model::Model_5c0ec0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b84_01::H::ctor_69>(this)); }  // 0x5c0ec0
auto joan::orig::ShadowModel::poll_load() -> void { joan::bind::member<&::en3d_b83_03::H::poll_load>(this); }  // 0x5c3110
auto joan::orig::Model::unload() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b84_01::H::unload>(this)); }  // 0x5c4f10
auto joan::orig::Skin::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b85_04::H::delete_5c5060>(this, a0)); }  // 0x5c5060
auto joan::orig::Model::load_bones_path(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b85_04::H::load_bones_path>(this, a0)); }  // 0x5c5400
auto joan::orig::Model::link_bones() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b84_01_3::H::link_bones>(this)); }  // 0x5c56d0
auto joan::orig::String::right_5c57c0(std::int32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::en3d_b85_04_2::H::str_right>(this, a0)); }  // 0x5c57c0
auto joan::orig::String::left_5c5800(std::int32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::en3d_b85_04_2::H::str_left>(this, a0)); }  // 0x5c5800
